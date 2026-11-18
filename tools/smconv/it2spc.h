#ifndef IT2SPC_H
#define IT2SPC_H

#include "itloader.h"

//---------------------------------------------
// BRR sample data
typedef struct
{
    //---------------------------------------------
    u16 Length;
    u16 Loop;
    u8 *Data;

    double TuningFactor;

    const char *id; // never set, kept for the soundbank definitions
} Source;

Source *source_new(const ITSampleData *source);
void source_free(Source *s);
bool source_compare(const Source *a, const Source *b);
void source_export(const Source *s, File *file, bool spc_direct);

//---------------------------------------------
typedef struct
{
    //---------------------------------------------
    u8 DefaultVolume;
    u8 GlobalVolume;
    u16 PitchBase;
    u8 DirectoryIndex;
    u8 SetPan;
} Sample;

typedef struct
{
    u8 y;
    u8 duration;
    s16 delta;
} EnvelopeNode;

//---------------------------------------------
typedef struct
{
    //---------------------------------------------
    u8 Fadeout;
    u8 SampleIndex;
    u8 GlobalVolume;
    u8 SetPan;
    u8 EnvelopeLength;
    u8 EnvelopeSustain;
    u8 EnvelopeLoopStart;
    u8 EnvelopeLoopEnd;
    EnvelopeNode *EnvelopeData;
} Instrument;

//---------------------------------------------
typedef struct
{
    //---------------------------------------------
    u8 Rows;

    u8 *Data;
    int DataSize;
    int DataCapacity;
} Pattern;

//---------------------------------------------
typedef struct
{
    //---------------------------------------------
    u8 InitialVolume;
    u8 InitialTempo;
    u8 InitialSpeed;
    u8 InitialChannelVolume[8];
    u8 InitialChannelPanning[8];

    u8 EchoVolumeL;
    u8 EchoVolumeR;
    u8 EchoDelay;
    u8 EchoFeedback;
    u8 EchoFIR[8];
    u8 EchoEnable;

    u8 Sequence[200];

    // <pointers>
    Pattern **Patterns;
    int PatternCount;
    Instrument **Instruments;
    int InstrumentCount;
    Sample **Samples;
    int SampleCount;

    char *id;
    u16 *SourceList;
    int SourceCount;
    u32 totalsize;
} Module;

//---------------------------------------------
typedef struct
{
    //---------------------------------------------
    Module **Modules;
    int ModuleCount;
    Source **Sources;
    int SourceCount;

    bool HiROM;
} Bank;

void bank_init(Bank *bank, const ITBank *itbank, bool hirom, bool chksfx);
void bank_free(Bank *bank);

void bank_export(const Bank *bank, const char *output);
void bank_make_spc(const Bank *bank, const char *spcfile);

#endif // IT2SPC_H
