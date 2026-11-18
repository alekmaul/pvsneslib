#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "it2spc.h"
#include "itloader.h"

#include "spc_program.h"

static u8 CurrentInstrument = 0;
static u8 patch_byte = 0x3C;
static u8 max_instruments = 64; // changing these values breaks
static u8 max_length = 200;     // everything even if the values
static u8 max_patterns = 64;    // of MOD_SEQU, MOD_PTABLE_L, etc,
static u8 max_samples = 64;     // in the sound driver are changed...
static u16 header_size = 616;
static u16 driver_base = 0x400;
static u16 module_base = 0x18ca;
static u16 sample_table_offset = 0x200;
static u16 spc_ram_size = 0;
static u16 totalsizem1 = 0;
static u32 totalitsize = 0;
static u32 totabanksize = 0;

static bool ChkSfx;

static int spc_program_size = sizeof(spc_program);

extern bool VERBOSE;
extern int BANKNUM;

enum
{
    SPC_RAM_SIZE = 58000,
    MODULE_BASE = 0x1A00
};

static void bank_add_module(Bank *bank, const ITModule *mod);
static int bank_add_source(Bank *bank, Source *s);
static void bank_add_source_data(Bank *bank, const ITSampleData *src);
static void bank_export_asm(const Bank *bank, const char *inputfile, const char *outputfile);
static void bank_export_inc(const Bank *bank, const char *output);

static Module *module_new(const ITModule *mod, const u16 *source_list, int source_count,
                          const u8 *directory, Source *const *sources);
static void module_free(Module *m);
static void module_parse_sm_option(Module *m, const char *text);
static void module_parse_sm_options(Module *m, const ITModule *mod);
static void module_export(const Module *m, File *file, bool write_header);

static Pattern *pattern_new(const ITPattern *source);
static void pattern_free(Pattern *p);
static int pattern_export_size(const Pattern *p);
static void pattern_export(const Pattern *p, File *file);

static Instrument *instrument_new(const ITInstrument *source);
static void instrument_free(Instrument *ins);
static int instrument_export_size(const Instrument *ins);
static void instrument_export(const Instrument *ins, File *file);

static Sample *sample_new(const ITSample *source, int directory, double tuning);
static void sample_export(const Sample *s, File *file);

/***********************************************************************************************
 *
 * Small dynamic byte buffer
 *
 ***********************************************************************************************/

typedef struct
{
    u8 *data;
    int size;
    int capacity;
} ByteBuffer;

static void bytebuf_push(ByteBuffer *b, u8 value)
{
    if (b->size == b->capacity)
    {
        b->capacity = b->capacity ? b->capacity * 2 : 64;
        b->data = (u8 *)xrealloc(b->data, b->capacity);
    }
    b->data[b->size++] = value;
}

/***********************************************************************************************
 *
 * Bank
 *
 ***********************************************************************************************/

void bank_init(Bank *bank, const ITBank *itbank, bool HIROM, bool CHKSFX)
{
    bank->Modules = NULL;
    bank->ModuleCount = 0;
    bank->Sources = NULL;
    bank->SourceCount = 0;

    bank->HiROM = HIROM;
    ChkSfx = CHKSFX;

    for (int i = 0, n = itbank->module_count; i < n; i++)
    {
        bank_add_module(bank, itbank->modules[i]);
    }

    for (int i = 0, n = itbank->sound_count; i < n; i++)
    {
        bank_add_source_data(bank, itbank->sounds[i]);
    }

    if (VERBOSE)
    {
        printf("-----------------------------------------------------------------------\n");
        printf("  Total Modules Size: [%6i bytes]\n", totabanksize);
        printf("       Total IT Size: [%6i bytes]\n", totalitsize);
        fflush(stdout);
    }
}

static void bank_push_source(Bank *bank, Source *s)
{
    bank->Sources = (Source **)xrealloc(bank->Sources, (bank->SourceCount + 1) * sizeof(Source *));
    bank->Sources[bank->SourceCount++] = s;
}

static void bank_add_source_data(Bank *bank, const ITSampleData *src)
{
    bank_push_source(bank, source_new(src));
}

static void bank_add_module(Bank *bank, const ITModule *mod)
{
    int size = file_size(mod->Filename);

    if (VERBOSE)
    {
        printf("-----------------------------------------------------------------------\n");
        printf("Adding module, Filename: <%s>\n", mod->Filename);
        printf("             Title: <%.26s>\n", mod->Title);
        printf("             IT Size: [%5i bytes]\n", size);
        fflush(stdout);
    }
    totalitsize += size;

    u16 *source_list = (u16 *)xcalloc(mod->SampleCount, sizeof(u16));
    int source_count = 0;
    u8 *directory = (u8 *)xcalloc(mod->SampleCount, sizeof(u8));

    for (int i = 0; i < mod->SampleCount; i++)
    {
        Source *s = source_new(&mod->Samples[i]->Data);

        int index = bank_add_source(bank, s);
        bool exists = false;

        for (int j = 0; j < source_count; j++)
        {
            if (source_list[j] == index)
            {
                directory[i] = j;
                exists = true;
                break;
            }
        }

        if (!exists)
        {
            directory[i] = source_count;
            source_list[source_count++] = index;
        }
    }

    bank->Modules = (Module **)xrealloc(bank->Modules, (bank->ModuleCount + 1) * sizeof(Module *));
    bank->Modules[bank->ModuleCount++] = module_new(mod, source_list, source_count, directory, bank->Sources);

    free(source_list);
    free(directory);
}

// returns the index of the source in the bank; s is freed if an identical source exists
static int bank_add_source(Bank *bank, Source *s)
{
    for (int i = 0; i < bank->SourceCount; i++)
    {
        if (source_compare(bank->Sources[i], s))
        {
            source_free(s);
            return i;
        }
    }
    bank_push_source(bank, s);
    return bank->SourceCount - 1;
}

void bank_free(Bank *bank)
{
    for (int i = 0; i < bank->ModuleCount; i++)
    {
        module_free(bank->Modules[i]);
    }
    free(bank->Modules);
    bank->Modules = NULL;
    bank->ModuleCount = 0;

    for (int i = 0; i < bank->SourceCount; i++)
    {
        source_free(bank->Sources[i]);
    }
    free(bank->Sources);
    bank->Sources = NULL;
    bank->SourceCount = 0;
}

/***********************************************************************************************
 *
 * Module
 *
 ***********************************************************************************************/

static void ValidateID(char *id)
{
    for (size_t i = 0; id[i]; i++)
    {
        char c = id[i];
        if ((c >= 1 && c <= 47) ||
            (c >= 58 && c <= 64) ||
            (c >= 91 && c <= 96) ||
            (c >= 123))
        {
            id[i] = '_';
        }
    }
}

// "MOD_" + file name in upper case, without directory and extension (from the first dot)
static char *Path2ID(const char *prefix, const char *source)
{
    char *s = xstrdup(source);
    char *start, *dot, *res;

    for (size_t i = 0; s[i]; i++)
    {
        s[i] = toupper((unsigned char)s[i]);
        if (s[i] == '\\')
            s[i] = '/';
    }

    start = strrchr(s, '/');
    start = start ? start + 1 : s;

    dot = strchr(start, '.');
    if (dot)
        *dot = 0;

    res = (char *)xmalloc(strlen(prefix) + strlen(start) + 1);
    strcpy(res, prefix);
    strcat(res, start);
    free(s);

    ValidateID(res);

    return res;
}

static Module *module_new(const ITModule *mod, const u16 *source_list, int source_count,
                          const u8 *directory, Source *const *sources)
{
    Module *m = (Module *)xcalloc(1, sizeof(Module));

    m->id = Path2ID("MOD_", mod->Filename);

    m->InitialVolume = mod->GlobalVolume;
    m->InitialTempo = mod->InitialTempo;
    m->InitialSpeed = mod->InitialSpeed;

    m->SourceList = (u16 *)xcalloc(source_count, sizeof(u16));
    memcpy(m->SourceList, source_list, source_count * sizeof(u16));
    m->SourceCount = source_count;

    for (int i = 0; i < 8; i++)
        m->InitialChannelVolume[i] = mod->ChannelVolume[i];

    for (int i = 0; i < 8; i++)
        m->InitialChannelPanning[i] = mod->ChannelPan[i];

    module_parse_sm_options(m, mod);

    for (int i = 0; i < max_length; i++)
        m->Sequence[i] = i < mod->Length ? mod->Orders[i] : 255;

    m->Patterns = (Pattern **)xcalloc(mod->PatternCount, sizeof(Pattern *));
    for (int i = 0; i < mod->PatternCount; i++)
    {
        m->Patterns[m->PatternCount++] = pattern_new(mod->Patterns[i]);
    }

    m->Instruments = (Instrument **)xcalloc(mod->InstrumentCount, sizeof(Instrument *));
    for (int i = 0; i < mod->InstrumentCount; i++)
    {
        m->Instruments[m->InstrumentCount++] = instrument_new(mod->Instruments[i]);
    }

    m->Samples = (Sample **)xcalloc(mod->SampleCount, sizeof(Sample *));
    for (int i = 0; i < mod->SampleCount; i++)
    {
        m->Samples[m->SampleCount++] =
            sample_new(mod->Samples[i],
                       directory[i],
                       sources[source_list[directory[i]]]->TuningFactor);
    }

    if (VERBOSE)
    {
        int pattsize = 0;
        for (int i = 0; i < mod->PatternCount; i++)
        {
            pattsize += pattern_export_size(m->Patterns[i]);
        }
        int sampsize = 0;
        for (int i = 0; i < source_count; i++)
        {
            sampsize += sources[source_list[i]]->Length;
        }
        int instrsize = 0;
        for (int i = 0; i < m->InstrumentCount; i++)
        {
            instrsize += instrument_export_size(m->Instruments[i]);
        }

        u32 echosize = m->EchoDelay * 2048;
        m->totalsize = pattsize + sampsize + instrsize + echosize;
        spc_ram_size = (64 * 1024) - module_base - header_size;
        u32 bytesfree = spc_ram_size - m->totalsize;
        totabanksize = totabanksize + m->totalsize;
        if (ChkSfx && (totalsizem1 != 0))
        {
            printf(
                "Conversion report:\n"
                "    Pattern data: [%5i bytes]   Module Length: [%i/%i]\n"
                "     Sample data: [%5i bytes]        Patterns: [%i/%i]\n"
                " Instrument data: [%5i bytes]     Instruments: [%i/%i]\n"
                "     Echo region: [%5i bytes]         Samples: [%i/%i]\n"
                "           Total: [%5i bytes]   *%i bytes free* *%i bytes free with 1st module*\n",
                pattsize, mod->Length, max_length,
                sampsize, mod->PatternCount, max_patterns,
                instrsize, mod->InstrumentCount, max_instruments,
                (int)echosize, mod->SampleCount, max_samples,
                (int)m->totalsize, (int)bytesfree, (int)(bytesfree - totalsizem1));
        }
        else
        {
            printf(
                "Conversion report:\n"
                "    Pattern data: [%5i bytes]   Module Length: [%i/%i]\n"
                "     Sample data: [%5i bytes]        Patterns: [%i/%i]\n"
                " Instrument data: [%5i bytes]     Instruments: [%i/%i]\n"
                "     Echo region: [%5i bytes]         Samples: [%i/%i]\n"
                "           Total: [%5i bytes]   *%i bytes free*\n",
                pattsize, mod->Length, max_length,
                sampsize, mod->PatternCount, max_patterns,
                instrsize, mod->InstrumentCount, max_instruments,
                (int)echosize, mod->SampleCount, max_samples,
                (int)m->totalsize, (int)bytesfree);
        }
        if (totalsizem1 == 0)
            totalsizem1 = m->totalsize;

        if (m->totalsize > spc_ram_size)
        {
            printf("%s: " ERRORRED("error") ": Module is too big. Maximum is %i bytes\n", ERRORBRIGHT("smconv"), spc_ram_size);
        }
    }

    return m;
}

static int SearchForSNESMODtag(const char *message)
{
    const char *test = "[[SNESMOD]]";
    int matches = 0;

    for (int i = 0; message[i]; i++)
    {
        if (message[i] == test[matches])
            matches++;
        else
            matches = 0;
        if (matches == 11)
            return i - 10;
    }
    return -1;
}

static int SeekNextLine(const char *text, int offset)
{
    while (text[offset] != 0 &&
           text[offset] != '\n' &&
           text[offset] != '\r')

        offset++;

    while (text[offset] == '\n' || text[offset] == '\r')
        offset++;

    return offset;
}

static bool is_whitespace(char c)
{
    return c == ' ' || c == '\t';
}

static bool not_term(char c)
{
    return c != 0 && c != '\r' && c != '\n';
}

static int minmax(int value, int min, int max)
{
    value = value < min ? min : value;
    return value > max ? max : value;
}

static void module_parse_sm_option(Module *m, const char *text)
{
    char **args = NULL;
    int argcount = 0;

    if (!text)
        return;
    if (!text[0])
        return;

    int offs = 0;

    // skip whitespace
    while (is_whitespace(text[offs]))
    {
        offs++;
    }

    while (not_term(text[offs]))
    {
        int len;
        for (len = 0; not_term(text[offs + len]); len++)
        {
            if (is_whitespace(text[offs + len]))
            {
                break;
            }
        }
        if (len == 0)
        {
            offs++;
        }
        else
        {
            args = (char **)xrealloc(args, (argcount + 1) * sizeof(char *));
            args[argcount] = (char *)xmalloc(len + 1);
            memcpy(args[argcount], text + offs, len);
            args[argcount][len] = 0;
            argcount++;
            offs += len;
        }

        // skip whitespace
        while (is_whitespace(text[offs]))
        {
            offs++;
        }
    }

    if (argcount == 0)
    {
        // no args?
        return;
    }

    // args is filled with arguments
    // determine command type

    for (int i = 0; args[0][i]; i++)
    {
        if (args[0][i] >= 'A' && args[0][i] <= 'Z')
        {
            args[0][i] += 'a' - 'A';
        }
    }

    if (strcmp(args[0], "edl") == 0)
    {
        if (argcount >= 2)
            m->EchoDelay = minmax(atoi(args[1]), 0, 15);
    }
    else if (strcmp(args[0], "efb") == 0)
    {
        if (argcount >= 2)
            m->EchoFeedback = minmax(atoi(args[1]), -128, 127);
    }
    else if (strcmp(args[0], "evol") == 0)
    {
        if (argcount < 2)
        {
            // nothing
        }
        else if (argcount < 3)
        {
            m->EchoVolumeL = minmax(atoi(args[1]), -128, 127);
            m->EchoVolumeR = m->EchoVolumeL;
        }
        else
        {
            m->EchoVolumeL = minmax(atoi(args[1]), -128, 127);
            m->EchoVolumeR = minmax(atoi(args[2]), -128, 127);
        }
    }
    else if (strcmp(args[0], "efir") == 0)
    {
        for (int i = 0; i < 8; i++)
        {
            if (argcount <= (1 + i))
            {
                break;
            }
            m->EchoFIR[i] = minmax(atoi(args[1 + i]), -128, 127);
        }
    }
    else if (strcmp(args[0], "eon") == 0)
    {
        for (int i = 1; i < argcount; i++)
        {
            int c = atoi(args[i]);
            if (c >= 1 && c <= 8)
            {
                m->EchoEnable |= (1 << (c - 1));
            }
        }
    }

    for (int i = 0; i < argcount; i++)
        free(args[i]);
    free(args);
}

static void module_parse_sm_options(Module *m, const ITModule *mod)
{
    m->EchoVolumeL = 0;
    m->EchoVolumeR = 0;
    m->EchoDelay = 0;
    m->EchoFeedback = 0;
    m->EchoFIR[0] = 127;
    for (int i = 1; i < 8; i++)
        m->EchoFIR[i] = 0;

    m->EchoEnable = 0;

    if (mod->Message)
    {
        int offset = SearchForSNESMODtag(mod->Message);
        if (offset != -1)
        {
            offset = SeekNextLine(mod->Message, offset);
            while (mod->Message[offset] != 0)
            {
                module_parse_sm_option(m, mod->Message + offset);
                offset = SeekNextLine(mod->Message, offset);
            }
        }
    }
}

static void module_free(Module *m)
{
    for (int i = 0; i < m->PatternCount; i++)
        pattern_free(m->Patterns[i]);
    free(m->Patterns);

    for (int i = 0; i < m->InstrumentCount; i++)
        instrument_free(m->Instruments[i]);
    free(m->Instruments);

    for (int i = 0; i < m->SampleCount; i++)
        free(m->Samples[i]);
    free(m->Samples);

    free(m->SourceList);
    free(m->id);
    free(m);
}

/***********************************************************************************************
 *
 * Pattern
 *
 ***********************************************************************************************/

static Pattern *pattern_new(const ITPattern *source)
{
    Pattern *p = (Pattern *)xcalloc(1, sizeof(Pattern));
    ByteBuffer data = {NULL, 0, 0};

    p->Rows = (u8)(source->Rows - 1);

    int row;
    const u8 *read = source->Data;

    if (source->DataLength != 0)
    {
        ByteBuffer row_buffer = {NULL, 0, 0};

        u8 spc_hints;
        u8 data_bits;
        u8 p_maskvar[8] = {0};

        spc_hints = 0;
        (void)spc_hints; // computed but not exported (0xFF is written instead)
        data_bits = 0;

        for (row = 0; row < source->Rows;)
        {
            u8 chvar = *read++;

            if (chvar == 0)
            {
                bytebuf_push(&data, 0xFF); // spc_hints
                bytebuf_push(&data, data_bits);

                for (int i = 0, n = row_buffer.size; i < n; i++)
                    bytebuf_push(&data, row_buffer.data[i]);
                row_buffer.size = 0;

                data_bits = 0;
                spc_hints = 0;
                row++;
                continue;
            }

            int channel = (chvar - 1) & 63;
            data_bits |= 1 << channel;
            if (channel > 7)
            {
                printf("%s: " ERRORRED("error") ": More than 8 channels. Found channel %i\n", ERRORBRIGHT("smconv"), channel + 1);
                break;
            }

            u8 maskvar;
            if (chvar & 128)
            {
                maskvar = *read++;
                maskvar = ((maskvar >> 4) | (maskvar << 4)) & 0xFF;
                maskvar |= maskvar >> 4;
                p_maskvar[channel] = maskvar;
            }
            else
            {
                maskvar = p_maskvar[channel];
            }

            bytebuf_push(&row_buffer, maskvar);

            if (maskvar & 16)
            { // note
                bytebuf_push(&row_buffer, *read++);
            }
            if (maskvar & 32)
            { // instr
                bytebuf_push(&row_buffer, *read++);
            }
            if (maskvar & 64)
            { // vcmd
                bytebuf_push(&row_buffer, *read++);
            }

            u8 cmd = 0, param = 0;
            if (maskvar & 128)
            { // cmd+param
                bytebuf_push(&row_buffer, cmd = *read++);
                bytebuf_push(&row_buffer, param = *read++);
            }

            if ((maskvar & 1))
            {
                spc_hints |= 1 << channel;
                if (maskvar & 128)
                {
                    if ((cmd == 7) || (cmd == 19 && (param & 0xF0) == 0xD0))
                    {
                        // glissando or note delay:
                        // cancel hint
                        spc_hints &= ~(1 << channel);
                    }
                }
            }
        }
        free(row_buffer.data);
    }
    else
    {
        // empty pattern
        for (int i = 0; i < source->Rows; i++)
        {
            bytebuf_push(&data, 0);
            bytebuf_push(&data, 0);
        }
    }

    p->Data = data.data;
    p->DataSize = data.size;
    p->DataCapacity = data.capacity;
    return p;
}

static void pattern_free(Pattern *p)
{
    free(p->Data);
    free(p);
}

static int pattern_export_size(const Pattern *p)
{
    return 1 + p->DataSize;
}

/***********************************************************************************************
 *
 * Instrument
 *
 ***********************************************************************************************/

static Instrument *instrument_new(const ITInstrument *source)
{
    Instrument *ins = (Instrument *)xcalloc(1, sizeof(Instrument));

    int a = source->Fadeout / 4;
    ins->Fadeout = a > 255 ? 255 : a;
    ins->SampleIndex = source->Notemap[60].Sample - 1;
    ins->GlobalVolume = source->GlobalVolume;
    ins->SetPan = source->DefaultPan;

    // load envelope
    const ITEnvelope *e = &source->VolumeEnvelope;

    ins->EnvelopeLength = e->Enabled ? e->Length * 4 : 0;
    CurrentInstrument++;

    if (ins->EnvelopeLength)
    {
        ins->EnvelopeSustain = e->Sustain ? e->SustainStart * 4 : 0xFF;
        ins->EnvelopeLoopStart = e->Loop ? e->LoopStart * 4 : 0xFF;
        ins->EnvelopeLoopEnd = e->Loop ? e->LoopEnd * 4 : 0xFF;

        ins->EnvelopeData = (EnvelopeNode *)xcalloc(ins->EnvelopeLength / 4, sizeof(EnvelopeNode));
        for (int i = 0; i < ins->EnvelopeLength / 4; i++)
        {
            EnvelopeNode *ed = &ins->EnvelopeData[i];
            ed->y = e->Nodes[i].y;
            if (i != (ins->EnvelopeLength / 4) - 1)
            {
                int duration = e->Nodes[i + 1].x - e->Nodes[i].x;
                ed->duration = duration;
                if (duration > 0)
                {
                    ed->delta = ((e->Nodes[i + 1].y - e->Nodes[i].y) * 256 + duration / 2) / duration;
                }
                else
                {
                    ed->delta = 64;
                    ed->duration = 255;
                    printf("\nsmconv: warning 'Volume envelope for instrument %i must have more\n               than one node to play properly'\n\n", CurrentInstrument);
                }
            }
            else
            {
                ed->delta = 0;
                ed->duration = 0;
            }
        }
    }
    else
    {
        ins->EnvelopeData = 0;
    }
    return ins;
}

static void instrument_free(Instrument *ins)
{
    free(ins->EnvelopeData);
    free(ins);
}

static int instrument_export_size(const Instrument *ins)
{
    // The C++ version returned 5 + (!EnvelopeLength) ? 0 : (3 + (EnvelopeLength/4) * 4),
    // which is always 0 (the + is evaluated before ?:). Kept as is, so that the conversion
    // report and the MOD_xxx_SIZE definitions stay the same.
    (void)ins;
    return 0;
}

/***********************************************************************************************
 *
 * Sample
 *
 ***********************************************************************************************/

static Sample *sample_new(const ITSample *source, int directory, double tuning)
{
    Sample *s = (Sample *)xcalloc(1, sizeof(Sample));

    s->DefaultVolume = source->DefaultVolume;
    s->GlobalVolume = source->GlobalVolume;
    s->SetPan = source->DefaultPanning ^ 128;

    double a = ((double)source->Data.C5Speed * tuning);
    s->PitchBase = (int)floor(log(a / 8363.0) / log(2.0) * 768.0 + 0.5);

    s->DirectoryIndex = directory;
    return s;
}

/***********************************************************************************************
 *
 * Source
 *
 ***********************************************************************************************/

bool source_compare(const Source *a, const Source *b)
{
    if (a->Length != b->Length)
        return false;
    if (a->Loop != b->Loop)
        return false;
    for (int i = 0; i < a->Length; i++)
    {
        if (a->Data[i] != b->Data[i])
        {
            return false;
        }
    }
    return true;
}

/***********************************************************************************************
 *
 * Export
 *
 ***********************************************************************************************/

void bank_make_spc(const Bank *bank, const char *spcfile)
{
    File file;

    file_open(&file, spcfile, MODE_WRITE);

    file_write_ascii(&file, "SNES-SPC700 Sound File Data v0.30");
    file_write8(&file, 26); // 26,26
    file_write8(&file, 26);
    file_write8(&file, 26); // header contins id666 tag
    file_write8(&file, 30); // version minor

    // SPC700 registers
    file_write16(&file, driver_base);
    file_write8(&file, 0);    // A
    file_write8(&file, 0);    // X
    file_write8(&file, 0);    // Y
    file_write8(&file, 0);    // PSW
    file_write8(&file, 0xEF); // SP
    file_write16(&file, 0);   // reserved

    // ID666 tag
    file_write_asciif(&file, "<INSERT SONG TITLE>", 32);
    file_write_asciif(&file, "<INSERT GAME TITLE>", 32);
    file_write_asciif(&file, "NAME OF DUMPER", 16);
    file_write_asciif(&file, "comments...", 32);
    file_write_asciif(&file, "", 11);
    file_write_asciif(&file, "180", 3);
    file_write_asciif(&file, "5000", 5);
    file_write_asciif(&file, "<INSERT SONG ARTIST>", 32);
    file_write8(&file, 0);
    file_write8(&file, '0');
    file_zero_fill(&file, 45); // reserved

    //-------------------------------------------------------
    // SPC memory
    //-------------------------------------------------------

    // zero fill up to program block
    file_zero_fill(&file, driver_base);

    int SampleTableOffset = file_tell(&file) - sample_table_offset;

    // write spc program
    for (int i = 0; i < spc_program_size; i++)
    {
        if (i == patch_byte || i == patch_byte + 1) // PATCH
            file_write8(&file, 0);
        else
            file_write8(&file, spc_program[i]);
    }

    // zero fill up to module base
    file_zero_fill(&file, (module_base - driver_base - spc_program_size));

    int StartOfModule = file_tell(&file);

    const Module *mod = bank->Modules[0];
    module_export(mod, &file, false);

    // unused entries are 0 (they were not initialized in the C++ version)
    u16 source_table[2 * 64] = {0};

    //!!TODO export sample list and create sample table!
    for (int i = 0; i < mod->SourceCount && i < max_samples; i++)
    {
        const Source *src = bank->Sources[mod->SourceList[i]];
        source_table[i * 2 + 0] = file_tell(&file) - StartOfModule + MODULE_BASE;
        source_table[i * 2 + 1] = source_table[i * 2] + src->Loop;
        source_export(src, &file, true);
    }

    int EndOfData = file_tell(&file);
    file_seek(&file, SampleTableOffset);
    for (int i = 0; i < 128; i++)
        file_write16(&file, source_table[i]);

    file_seek(&file, EndOfData);

    file_zero_fill(&file, 65792 - EndOfData);

    // DSP registers
    for (int i = 0; i < 128; i++)
        file_write8(&file, 0);

    // unused/ipl rom
    for (int i = 0; i < 128; i++)
        file_write8(&file, 0);

    file_close(&file);
}

static char *concat(const char *a, const char *b)
{
    char *res = (char *)xmalloc(strlen(a) + strlen(b) + 1);
    strcpy(res, a);
    strcat(res, b);
    return res;
}

void bank_export(const Bank *bank, const char *output)
{
    char *bin_output = concat(output, ".bnk");
    File file;

    file_open(&file, bin_output, MODE_WRITE);

    file_write16(&file, bank->SourceCount);
    file_write16(&file, bank->ModuleCount);

    // reserve space for tables
    for (u32 i = 0; i < 3 * 128; i++)
        file_write8(&file, 0xAA);

    for (int i = 0; i < bank->SourceCount * 3; i++)
    {
        file_write8(&file, 0xAA);
    }

    u32 *module_ptr = (u32 *)xcalloc(bank->ModuleCount, sizeof(u32));
    u32 *source_ptr = (u32 *)xcalloc(bank->SourceCount, sizeof(u32));

    for (int i = 0; i < bank->ModuleCount; i++)
    {
        module_ptr[i] = file_tell(&file);
        module_export(bank->Modules[i], &file, true);
    }

    for (int i = 0; i < bank->SourceCount; i++)
    {
        source_ptr[i] = file_tell(&file);
        source_export(bank->Sources[i], &file, false);
    }

    // export module pointers
    file_seek(&file, 4);
    for (int i = 0; i < 128; i++)
    {
        int addr, bnk;
        if (i < bank->ModuleCount)
        {
            if (bank->HiROM)
            { // 64k banks
                addr = module_ptr[i] & 65535;
                bnk = module_ptr[i] >> 16;
            }
            else
            { // 32k banks
                addr = 0x8000 + (module_ptr[i] & 32767);
                bnk = module_ptr[i] >> 15;
            }
        }
        else
        {
            bnk = addr = 0;
        }
        file_write16(&file, addr);
        file_write8(&file, bnk);
    }

    // export source pointers
    for (int i = 0; i < bank->SourceCount; i++)
    {
        int addr, bnk;
        if (bank->HiROM)
        { // 64k banks
            addr = source_ptr[i] & 65535;
            bnk = source_ptr[i] >> 16;
        }
        else
        { // 32k banks
            addr = 0x8000 + (source_ptr[i] & 32767);
            bnk = source_ptr[i] >> 15;
        }
        file_write16(&file, addr);
        file_write8(&file, bnk);
    }

    file_close(&file);
    free(module_ptr);
    free(source_ptr);

    char *asm_out = concat(output, ".asm");
    bank_export_asm(bank, bin_output, asm_out);
    free(asm_out);

    char *inc_out = concat(output, ".h");
    bank_export_inc(bank, inc_out);
    free(inc_out);

    free(bin_output);
}

static void bank_export_inc(const Bank *bank, const char *output)
{
    FILE *f = fopen(output, "w");

    if (!f)
    {
        printf("%s: " ERRORRED("error") ": can't create %s\n", ERRORBRIGHT("smconv"), output);
        return;
    }

    fprintf(f,
            "// snesmod soundbank definitions\n\n"
            "#ifndef __SOUNDBANK_DEFINITIONS__\n"
            "#define __SOUNDBANK_DEFINITIONS__\n\n");

    for (int i = 0; i < bank->ModuleCount; i++)
    {
        const char *id = bank->Modules[i]->id;
        if (id && id[0])
        {
            char *size_id = concat(id, "_SIZE");
            fprintf(f, "#define %-32s\t%i\n", id, i);
            fprintf(f, "#define %-32s\t%i\n", size_id, (int)bank->Modules[i]->totalsize);
            free(size_id);
        }
    }
    fprintf(f, "\n");

    for (int i = 0; i < bank->SourceCount; i++)
    {
        const char *id = bank->Sources[i]->id;
        if (id && id[0])
        {
            fprintf(f, "#define %-32s\t%i\n", id, i);
        }
    }
    fprintf(f, "\n#endif // __SOUNDBANK_DEFINITIONS__\n");
    fclose(f);
}

static void bank_export_asm(const Bank *bank, const char *inputfile, const char *outputfile)
{
    FILE *f = fopen(outputfile, "w");

    if (!f)
    {
        printf("%s: " ERRORRED("error") ": can't create %s\n", ERRORBRIGHT("smconv"), outputfile);
        return;
    }

    int size = file_size(inputfile);

    fprintf(f,
            ";************************************************\n"
            "; snesmod soundbank data                        *\n"
            "; total size: %10i bytes                  *\n"
            ";************************************************\n"
            "\n"
            ".include \"hdr.asm\"\n"
            "\n",
            size);

    int banksize;

    if (bank->HiROM)
        banksize = 65536;
    else
        banksize = 32768;

    // the .bnk file name, with / as directory separator
    // (the directory is kept: it avoids to use a specific directory to generate sources)
    char *foo = xstrdup(inputfile);
    for (size_t i = 0; foo[i]; i++)
    {
        if (foo[i] == '\\')
            foo[i] = '/';
    }

    if (size <= banksize)
    {
        fprintf(f,
                ".BANK %i\n"
                ".SECTION \"SOUNDBANK\" ; need dedicated bank(s)\n\n"
                "SOUNDBANK__:\n",
                BANKNUM);

        fprintf(f, ".incbin \"%s\"\n", foo);

        fprintf(f, ".ENDS\n");
    }
    else
    {
        u32 lastbank = size / banksize;
        for (u32 j = 0; j <= lastbank; j++)
        {
            fprintf(f,
                    ".BANK %i\n"
                    ".SECTION \"SOUNDBANK%i\" ; need dedicated bank(s)\n\n"
                    "SOUNDBANK__%i:\n",
                    (int)(BANKNUM + j), (int)j, (int)j);

            if (bank->HiROM)
            {
                if (j == 0)
                    fprintf(f, ".incbin \"%s\" read $10000\n", foo);
                else if (j == lastbank)
                    fprintf(f, ".incbin \"%s\" skip $%x\n", foo, j * 0x10000);
                else
                    fprintf(f, ".incbin \"%s\" skip $%x read $10000\n", foo, j * 0x10000);
            }
            else
            {
                if (j == 0)
                    fprintf(f, ".incbin \"%s\" read $8000\n", foo);
                else if (j == lastbank)
                    fprintf(f, ".incbin \"%s\" skip $%x\n", foo, j * 0x8000);
                else
                    fprintf(f, ".incbin \"%s\" skip $%x read $8000\n", foo, j * 0x8000);
            }

            fprintf(f, ".ENDS\n\n");
        }
    }

    free(foo);
    fclose(f);
}

void source_export(const Source *s, File *file, bool spc_direct)
{
    if (!spc_direct)
    {
        file_write16(file, s->Length);
        file_write16(file, s->Loop);
    }

    for (int i = 0; i < s->Length; i++)
    {
        file_write8(file, s->Data[i]);
    }

    if (!spc_direct)
    {
        if (s->Length & 1)
            file_write8(file, 0); // align by 2
    }
}

static void module_export(const Module *m, File *file, bool write_header)
{
    int HeaderStart = file_tell(file);

    if (write_header)
    {
        file_write16(file, 0xaaaa); // reserve for module size
        file_write16(file, m->SourceCount);

        for (int i = 0; i < m->SourceCount; i++)
        {
            file_write16(file, m->SourceList[i]);
        }
    }

    int ModuleStart = file_tell(file);

    file_write8(file, m->InitialVolume);
    file_write8(file, m->InitialTempo);
    file_write8(file, m->InitialSpeed);

    for (int i = 0; i < 8; i++)
        file_write8(file, m->InitialChannelVolume[i]);

    for (int i = 0; i < 8; i++)
        file_write8(file, m->InitialChannelPanning[i]);

    file_write8(file, m->EchoVolumeL);
    file_write8(file, m->EchoVolumeR);
    file_write8(file, m->EchoDelay);
    file_write8(file, m->EchoFeedback);

    for (int i = 0; i < 8; i++)
        file_write8(file, m->EchoFIR[i]);

    file_write8(file, m->EchoEnable);

    for (int i = 0; i < max_length; i++)
        file_write8(file, m->Sequence[i]);

    u16 *pattern_ptr = (u16 *)xcalloc(m->PatternCount, sizeof(u16));
    u16 *instrument_ptr = (u16 *)xcalloc(m->InstrumentCount, sizeof(u16));
    u16 *sample_ptr = (u16 *)xcalloc(m->SampleCount, sizeof(u16));

    u32 start_of_tables = file_tell(file);

    // reserve space for pointers
    for (int i = 0; i < (max_patterns + max_instruments + max_samples); i++)
    {
        file_write16(file, 0xBAAA);
    }

    for (int i = 0; i < m->PatternCount; i++)
    {
        u32 ptr = file_tell(file) - ModuleStart;

        if (ptr > SPC_RAM_SIZE)
            printf("\nERROR: Module is too big.");

        pattern_ptr[i] = (u16)ptr;
        pattern_export(m->Patterns[i], file);
    }

    for (int i = 0; i < m->InstrumentCount; i++)
    {
        u32 ptr = file_tell(file) - ModuleStart;

        if (ptr > SPC_RAM_SIZE)
            printf("\nERROR: Module is too big.");

        instrument_ptr[i] = (u16)ptr;
        instrument_export(m->Instruments[i], file);
    }

    for (int i = 0; i < m->SampleCount; i++)
    {
        u32 ptr = file_tell(file) - ModuleStart;

        if (ptr > SPC_RAM_SIZE)
            printf("\nERROR: Module is too big.");

        sample_ptr[i] = (u16)ptr;
        sample_export(m->Samples[i], file);
    }

    u32 end_of_mod = file_tell(file);

    if (write_header)
    {
        file_seek(file, HeaderStart);
        file_write16(file, (end_of_mod - ModuleStart + 1) >> 1);
    }

    file_seek(file, start_of_tables);

    for (int i = 0; i < max_patterns; i++)
        file_write8(file, i < m->PatternCount ? ((pattern_ptr[i] + MODULE_BASE) & 0xFF) : 0xFF);
    for (int i = 0; i < max_patterns; i++)
        file_write8(file, i < m->PatternCount ? ((pattern_ptr[i] + MODULE_BASE) >> 8) : 0xFF);

    for (int i = 0; i < max_instruments; i++)
        file_write8(file, i < m->InstrumentCount ? ((instrument_ptr[i] + MODULE_BASE) & 0xFF) : 0xFF);
    for (int i = 0; i < max_instruments; i++)
        file_write8(file, i < m->InstrumentCount ? ((instrument_ptr[i] + MODULE_BASE) >> 8) : 0xFF);

    for (int i = 0; i < max_samples; i++)
        file_write8(file, i < m->SampleCount ? ((sample_ptr[i] + MODULE_BASE) & 0xFF) : 0xFF);
    for (int i = 0; i < max_samples; i++)
        file_write8(file, i < m->SampleCount ? ((sample_ptr[i] + MODULE_BASE) >> 8) : 0xFF);

    free(pattern_ptr);
    free(instrument_ptr);
    free(sample_ptr);

    file_seek(file, end_of_mod);

    if (write_header)
    {
        if (file_tell(file) & 1)
            file_write8(file, 0); // align by 2
    }
}

static void pattern_export(const Pattern *p, File *file)
{
    file_write8(file, p->Rows);
    for (int i = 0; i < p->DataSize; i++)
    {
        file_write8(file, p->Data[i]);
    }
}

static void instrument_export(const Instrument *ins, File *file)
{
    file_write8(file, ins->Fadeout);
    file_write8(file, ins->SampleIndex);
    file_write8(file, ins->GlobalVolume);
    file_write8(file, ins->SetPan);
    file_write8(file, ins->EnvelopeLength);
    if (ins->EnvelopeLength)
    {
        file_write8(file, ins->EnvelopeSustain);
        file_write8(file, ins->EnvelopeLoopStart);
        file_write8(file, ins->EnvelopeLoopEnd);
        for (int i = 0; i < ins->EnvelopeLength / 4; i++)
        {
            file_write8(file, ins->EnvelopeData[i].y);
            file_write8(file, ins->EnvelopeData[i].duration);
            file_write16(file, ins->EnvelopeData[i].delta);
        }
    }
}

static void sample_export(const Sample *s, File *file)
{
    file_write8(file, s->DefaultVolume);
    file_write8(file, s->GlobalVolume);
    file_write16(file, s->PitchBase);
    file_write8(file, s->DirectoryIndex);
    file_write8(file, s->SetPan);
}
