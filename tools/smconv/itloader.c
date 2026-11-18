#include <stdlib.h>
#include <string.h>

#include "itloader.h"
#include "io.h"

// IT file format found here : https://fileformats.fandom.com/wiki/Impulse_tracker

static ITModule *itmodule_new(const char *filename);
static void itmodule_free(ITModule *m);
static ITInstrument *itinstrument_new(File *file);
static void itenvelope_load(ITEnvelope *e, File *file);
static ITSample *itsample_new(File *file, const char *filename, int index);
static void itsample_free(ITSample *s);
static void itsample_load_data(ITSample *s, File *file, const char *filename, int index);
static ITPattern *itpattern_new(File *file);
static ITPattern *itpattern_new_empty(void);
static void itpattern_free(ITPattern *p);
static ITSampleData *itsampledata_new_wav(const char *filename);
static void itsampledata_free(ITSampleData *d);

void itbank_init(ITBank *bank, const char **files, int file_count)
{
    bank->modules = NULL;
    bank->module_count = 0;
    bank->sounds = NULL;
    bank->sound_count = 0;

    for (int i = 0; i < file_count; i++)
    {
        itbank_add_module(bank, files[i]);
    }
}

void itbank_free(ITBank *bank)
{
    for (int i = 0; i < bank->module_count; i++)
        itmodule_free(bank->modules[i]);
    free(bank->modules);
    bank->modules = NULL;
    bank->module_count = 0;

    for (int i = 0; i < bank->sound_count; i++)
        itsampledata_free(bank->sounds[i]);
    free(bank->sounds);
    bank->sounds = NULL;
    bank->sound_count = 0;
}

void itbank_add_sound(ITBank *bank, const char *filename)
{
    bank->sounds = (ITSampleData **)xrealloc(bank->sounds, (bank->sound_count + 1) * sizeof(ITSampleData *));
    bank->sounds[bank->sound_count++] = itsampledata_new_wav(filename);
}

void itbank_add_module(ITBank *bank, const char *filename)
{
    bank->modules = (ITModule **)xrealloc(bank->modules, (bank->module_count + 1) * sizeof(ITModule *));
    bank->modules[bank->module_count++] = itmodule_new(filename);
}

static ITModule *itmodule_new(const char *filename)
{
    // zeroed: a file that is not an IT module gives an empty module
    ITModule *m = (ITModule *)xcalloc(1, sizeof(ITModule));
    File file;

    file_open(&file, filename, MODE_READ);

    m->Filename = xstrdup(filename);

    if (file_read8(&file) != 'I' || file_read8(&file) != 'M' ||
        file_read8(&file) != 'P' || file_read8(&file) != 'M')
    {
        file_close(&file);
        return m;
    }

    for (int i = 0; i < 26; i++)
        m->Title[i] = file_read8(&file);

    m->PatternHighlight = file_read16(&file);

    m->Length = file_read16(&file);
    m->InstrumentCount = file_read16(&file);
    m->SampleCount = file_read16(&file);
    m->PatternCount = file_read16(&file);
    m->Cwtv = file_read16(&file);
    m->Cmwt = file_read16(&file);
    m->Flags = file_read16(&file);
    m->Special = file_read16(&file);
    m->GlobalVolume = file_read8(&file);
    m->MixingVolume = file_read8(&file);
    m->InitialSpeed = file_read8(&file);
    m->InitialTempo = file_read8(&file);

    m->Sep = file_read8(&file);
    m->PWD = file_read8(&file);
    m->MessageLength = file_read16(&file);

    u32 MessageOffset = file_read32(&file);

    file_skip(&file, 4); // reserved

    for (int i = 0; i < 64; i++)
        m->ChannelPan[i] = file_read8(&file);

    for (int i = 0; i < 64; i++)
        m->ChannelVolume[i] = file_read8(&file);

    bool foundend = false;
    int ActualLength = m->Length;
    for (int i = 0; i < 256; i++)
    {
        m->Orders[i] = i < m->Length ? file_read8(&file) : 255;
        if (m->Orders[i] == 255 && !foundend)
        {
            foundend = true;
            ActualLength = i + 1;
        }
    }

    m->Length = ActualLength;

    m->Instruments = (ITInstrument **)xcalloc(m->InstrumentCount, sizeof(ITInstrument *));
    m->Samples = (ITSample **)xcalloc(m->SampleCount, sizeof(ITSample *));
    m->Patterns = (ITPattern **)xcalloc(m->PatternCount, sizeof(ITPattern *));

    u32 *InstrTable = (u32 *)xcalloc(m->InstrumentCount, sizeof(u32));
    u32 *SampleTable = (u32 *)xcalloc(m->SampleCount, sizeof(u32));
    u32 *PatternTable = (u32 *)xcalloc(m->PatternCount, sizeof(u32));

    for (int i = 0; i < m->InstrumentCount; i++)
        InstrTable[i] = file_read32(&file);
    for (int i = 0; i < m->SampleCount; i++)
        SampleTable[i] = file_read32(&file);
    for (int i = 0; i < m->PatternCount; i++)
        PatternTable[i] = file_read32(&file);

    for (int i = 0; i < m->InstrumentCount; i++)
    {
        file_seek(&file, InstrTable[i]);
        m->Instruments[i] = itinstrument_new(&file);
    }

    for (int i = 0; i < m->SampleCount; i++)
    {
        file_seek(&file, SampleTable[i]);
        m->Samples[i] = itsample_new(&file, filename, i + 1);
    }

    for (int i = 0; i < m->PatternCount; i++)
    {
        if (PatternTable[i])
        {
            file_seek(&file, PatternTable[i]);
            m->Patterns[i] = itpattern_new(&file);
        }
        else
        {
            m->Patterns[i] = itpattern_new_empty();
        }
    }

    if (m->MessageLength)
    {
        m->Message = (char *)xmalloc(m->MessageLength + 1);
        file_seek(&file, MessageOffset);
        m->Message[m->MessageLength] = 0;
        for (int i = 0; i < m->MessageLength; i++)
            m->Message[i] = file_read8(&file);
    }
    else
    {
        m->Message = 0;
    }

    free(InstrTable);
    free(SampleTable);
    free(PatternTable);

    file_close(&file);
    return m;
}

static void itmodule_free(ITModule *m)
{
    if (m->Instruments)
        for (int i = 0; i < m->InstrumentCount; i++)
            free(m->Instruments[i]);
    free(m->Instruments);
    if (m->Samples)
        for (int i = 0; i < m->SampleCount; i++)
            itsample_free(m->Samples[i]);
    free(m->Samples);
    if (m->Patterns)
        for (int i = 0; i < m->PatternCount; i++)
            itpattern_free(m->Patterns[i]);
    free(m->Patterns);
    free(m->Message);
    free(m->Filename);
    free(m);
}

static ITInstrument *itinstrument_new(File *file)
{
    ITInstrument *ins = (ITInstrument *)xcalloc(1, sizeof(ITInstrument));

    file_skip(file, 4); // IMPI
    ins->DOSFilename[12] = 0;
    for (int i = 0; i < 12; i++)
        ins->DOSFilename[i] = file_read8(file);
    file_skip(file, 1); // 00h
    ins->NewNoteAction = file_read8(file);
    ins->DuplicateCheckType = file_read8(file);
    ins->DuplicateCheckAction = file_read8(file);
    ins->Fadeout = file_read16(file);
    ins->PPS = file_read8(file);
    ins->PPC = file_read8(file);
    ins->GlobalVolume = file_read8(file);
    ins->DefaultPan = file_read8(file);
    ins->RandomVolume = file_read8(file);
    ins->RandomPanning = file_read8(file);
    ins->TrackerVersion = file_read16(file);
    ins->NumberOfSamples = file_read8(file);

    ins->Name[26] = 0;
    for (int i = 0; i < 26; i++)
        ins->Name[i] = file_read8(file);

    ins->InitialFilterCutoff = file_read8(file);
    ins->InitialFilterResonance = file_read8(file);

    ins->MidiChannel = file_read8(file);
    ins->MidiProgram = file_read8(file);
    ins->MidiBank = file_read16(file);

    file_read8(file); // reserved

    for (int i = 0; i < 120; i++)
    {
        ins->Notemap[i].Note = file_read8(file);
        ins->Notemap[i].Sample = file_read8(file);
    }

    itenvelope_load(&ins->VolumeEnvelope, file);
    itenvelope_load(&ins->PanningEnvelope, file);
    itenvelope_load(&ins->PitchEnvelope, file);
    return ins;
}

static void itenvelope_load(ITEnvelope *e, File *file)
{
    u8 FLG = file_read8(file);
    e->Enabled = !!(FLG & 1);
    e->Loop = !!(FLG & 2);
    e->Sustain = !!(FLG & 4);
    e->IsFilter = !!(FLG & 128);

    e->Length = file_read8(file);

    e->LoopStart = file_read8(file);
    e->LoopEnd = file_read8(file);

    e->SustainStart = file_read8(file);
    e->SustainEnd = file_read8(file);

    for (int i = 0; i < 25; i++)
    {
        e->Nodes[i].y = file_read8(file);
        e->Nodes[i].x = file_read16(file);
    }
}

static ITSample *itsample_new(File *file, const char *filename, int index)
{
    ITSample *s = (ITSample *)xcalloc(1, sizeof(ITSample));

    file_skip(file, 4); // IMPS
    s->DOSFilename[12] = 0;
    for (int i = 0; i < 12; i++)
        s->DOSFilename[i] = file_read8(file);
    file_skip(file, 1); // 00h
    s->GlobalVolume = file_read8(file);
    u8 Flags = file_read8(file);

    s->HasSample = !!(Flags & 1);
    s->Data.Bits16 = !!(Flags & 2);
    s->Stereo = !!(Flags & 4);
    s->Compressed = !!(Flags & 8);
    s->Data.Loop = !!(Flags & 16);
    s->Data.Sustain = !!(Flags & 32);
    s->Data.BidiLoop = !!(Flags & 64);
    s->Data.BidiSustain = !!(Flags & 128);

    s->DefaultVolume = file_read8(file);

    s->Name[26] = 0;
    for (int i = 0; i < 26; i++)
        s->Name[i] = file_read8(file);

    s->Convert = file_read8(file);
    s->DefaultPanning = file_read8(file);

    s->Data.Length = file_read32(file);
    s->Data.LoopStart = file_read32(file);
    s->Data.LoopEnd = file_read32(file);
    s->Data.C5Speed = file_read32(file);
    s->Data.SustainStart = file_read32(file);
    s->Data.SustainEnd = file_read32(file);

    u32 SamplePointer = file_read32(file);

    s->VibratoSpeed = file_read8(file);
    s->VibratoDepth = file_read8(file);
    s->VibratoRate = file_read8(file);
    s->VibratoForm = file_read8(file);

    file_seek(file, SamplePointer);
    itsample_load_data(s, file, filename, index);
    return s;
}

static void itsample_free(ITSample *s)
{
    if (!s)
        return;
    if (s->Data.Bits16)
        free(s->Data.d.Data16);
    else
        free(s->Data.d.Data8);
    free(s);
}

static void itsample_load_data(ITSample *s, File *file, const char *filename, int index)
{
    if (!s->Compressed || s->Data.Length == 0)
    {
        // subtract offset for unsigned samples
        int offset = (s->Convert & 1) ? 0 : (s->Data.Bits16 ? -32768 : -128);

        // signed samples
        if (s->Data.Bits16)
        {
            s->Data.d.Data16 = (s16 *)xmalloc(s->Data.Length * sizeof(s16));
            for (int i = 0; i < s->Data.Length; i++)
            {
                s->Data.d.Data16[i] = file_read16(file) + offset;
            }
        }
        else
        {
            s->Data.d.Data8 = (s8 *)xmalloc(s->Data.Length);
            for (int i = 0; i < s->Data.Length; i++)
            {
                s->Data.d.Data8[i] = file_read8(file) + offset;
            }
        }
    }
    else
    {
        // TODO : accept compressed samples.
        printf("%s: " ERRORRED("fatal error") ": samples must not be compressed (sample %i '%s' in %s)\n",
               ERRORBRIGHT("smconv"), index, s->Name, filename);
        exit(EXIT_FAILURE);
    }
}

static ITPattern *itpattern_new(File *file)
{
    ITPattern *p = (ITPattern *)xcalloc(1, sizeof(ITPattern));

    p->DataLength = file_read16(file);
    p->Rows = file_read16(file);
    file_skip(file, 4); // reserved
    p->Data = (u8 *)xmalloc(p->DataLength);
    for (int i = 0; i < p->DataLength; i++)
    {
        p->Data[i] = file_read8(file);
    }
    return p;
}

static ITPattern *itpattern_new_empty(void)
{
    ITPattern *p = (ITPattern *)xcalloc(1, sizeof(ITPattern));

    p->DataLength = 0;
    p->Rows = 64;
    p->Data = (u8 *)xcalloc(p->Rows, 1);
    return p;
}

static void itpattern_free(ITPattern *p)
{
    if (!p)
        return;
    free(p->Data);
    free(p);
}

static unsigned int str4(const char *str)
{
    return (unsigned int)str[0] |
           ((unsigned int)str[1] << 8) |
           ((unsigned int)str[2] << 16) |
           ((unsigned int)str[3] << 24);
}

static void itsampledata_free(ITSampleData *d)
{
    if (!d)
        return;
    if (d->Bits16)
        free(d->d.Data16);
    else
        free(d->d.Data8);
    free(d);
}

// load data from WAV file
static ITSampleData *itsampledata_new_wav(const char *filename)
{
    ITSampleData *d = (ITSampleData *)xcalloc(1, sizeof(ITSampleData));

    unsigned int file_size_;
    unsigned int bit_depth = 8;
    unsigned int hasformat = 0;
    // unsigned int hasdata = 0;
    unsigned int chunk_code;
    unsigned int chunk_size;
    unsigned int num_channels = 0;
    File file;

    (void)str4;

    // initialize data
    d->Bits16 = false;
    d->Length = 0;
    d->LoopStart = 0;
    d->LoopEnd = 0;
    d->C5Speed = 32000;
    d->SustainStart = 0;
    d->SustainEnd = 0;
    d->Loop = false;
    d->Sustain = false;
    d->BidiLoop = false;
    d->BidiSustain = false;

    file_size_ = file_size(filename);
    file_open(&file, filename, MODE_READ);

    file_read32(&file); // "RIFF"
    file_read32(&file); // filesize-8
    file_read32(&file); // "WAVE"

    while (1)
    {
        // break on end of file
        if (file_tell(&file) >= file_size_)
            break;

        // read chunk code and length
        chunk_code = file_read32(&file);
        chunk_size = file_read32(&file);

        // parse chunk code
        switch (chunk_code)
        {
        //---------------------------------------------------------------------
        case 0x20746D66: // format chunk
            //---------------------------------------------------------------------

            // check compression code (1 = PCM)
            if (file_read16(&file) != 1)
            {
                printf("%s: " ERRORRED("fatal error") ": Unsupported WAV format\n", ERRORBRIGHT("smconv"));
                file_close(&file);
                return d;
            }

            // read # of channels
            num_channels = file_read16(&file);

            // read sampling frequency
            d->C5Speed = file_read32(&file);

            // skip average something, wBlockAlign
            file_read32(&file);
            file_read16(&file);

            // get bit depth, catch unsupported values
            bit_depth = file_read16(&file);
            if (bit_depth != 8 && bit_depth != 16)
            {
                printf("%s: " ERRORRED("fatal error") ": WAV bit depth must be 8 or 16\n", ERRORBRIGHT("smconv"));
                file_close(&file);
                return d; // LOADWAV_UNSUPPORTED_BD;
            }

            if (bit_depth == 16)
                d->Bits16 = true;

            // skip the rest of the chunk (if any)
            if ((chunk_size - 0x10) > 0)
                file_skip(&file, (chunk_size - 0x10));

            hasformat = 1;
            break;

        //---------------------------------------------------------------------
        case 0x61746164: // data chunk
        //---------------------------------------------------------------------
        {
            int t, dat;
            u32 c;

            if (!hasformat)
            {
                printf("%s: " ERRORRED("fatal error") ": CORRUPT WAV FILE...\n", ERRORBRIGHT("smconv"));
                file_close(&file);
                return d; // LOADWAV_CORRUPT;
            }

            // clip chunk size against end of file (for some borked wavs...)
            {
                u32 br = file_size_ - file_tell(&file);
                chunk_size = chunk_size > br ? br : chunk_size;
            }

            d->Length = chunk_size / (bit_depth / 8) / num_channels;

            if (bit_depth == 16)
                d->d.Data16 = (s16 *)xmalloc((chunk_size / 2) * sizeof(s16));
            else
                d->d.Data8 = (s8 *)xmalloc(chunk_size);

            // read sample data
            for (t = 0; t < d->Length; t++)
            {
                dat = 0;

                // for multi-channel samples, get average value
                for (c = 0; c < num_channels; c++)
                {
                    dat += bit_depth == 8 ? ((int)file_read8(&file)) - 128 : ((short)file_read16(&file));
                }
                dat /= (int)num_channels;

                if (bit_depth == 8)
                {
                    d->d.Data8[t] = dat;
                }
                else
                {
                    d->d.Data16[t] = dat;
                }
            }

            // hasdata = 1;

            break;
        }
        default:
            file_skip(&file, chunk_size);
        }
    }

    file_close(&file);
    return d;
}
