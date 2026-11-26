#ifndef ITLOADER_H
#define ITLOADER_H

#include <stdio.h>
#include <stdlib.h>

#include "basetypes.h"
#include "fileio.h"

typedef struct
{
    u16 DataLength;
    u16 Rows;
    u8 *Data;
} ITPattern;

typedef struct
{
    bool Bits16;
    int Length;
    int LoopStart;
    int LoopEnd;
    int C5Speed;
    int SustainStart;
    int SustainEnd;
    bool Loop;
    bool Sustain;
    bool BidiLoop;
    bool BidiSustain;

    union {
        s8 *Data8;
        s16 *Data16;
    } d;
} ITSampleData;

typedef struct
{
    char Name[27];
    char DOSFilename[13];

    u8 GlobalVolume;

    bool HasSample;
    bool Stereo;
    bool Compressed;

    u8 DefaultVolume;
    u8 DefaultPanning;
    u8 Convert;

    u8 VibratoSpeed;
    u8 VibratoDepth;
    u8 VibratoForm;
    u8 VibratoRate;

    ITSampleData Data;
} ITSample;

typedef struct
{
    u8 Note;
    u8 Sample;
} NotemapEntry;

typedef struct
{
    u8 y;
    u16 x;
} EnvelopeEntry;

typedef struct
{
    bool Enabled;
    bool Loop;
    bool Sustain;
    bool IsFilter;

    int Length;

    int LoopStart;
    int LoopEnd;
    int SustainStart;
    int SustainEnd;

    EnvelopeEntry Nodes[25];
} ITEnvelope;

typedef struct
{
    char Name[27];
    char DOSFilename[13];

    u8 NewNoteAction;
    u8 DuplicateCheckType;
    u8 DuplicateCheckAction;

    u16 Fadeout;
    u8 PPS;
    u8 PPC;
    u8 GlobalVolume;
    u8 DefaultPan;
    u8 RandomVolume;
    u8 RandomPanning;
    u16 TrackerVersion;
    u8 NumberOfSamples;
    u8 InitialFilterCutoff;
    u8 InitialFilterResonance;

    u8 MidiChannel;
    u8 MidiProgram;
    u16 MidiBank;

    NotemapEntry Notemap[120];

    ITEnvelope VolumeEnvelope;
    ITEnvelope PanningEnvelope;
    ITEnvelope PitchEnvelope;
} ITInstrument;

typedef struct
{
    char *Filename;

    char Title[26];

    u16 PatternHighlight;
    u16 Length;
    u16 InstrumentCount;
    u16 SampleCount;
    u16 PatternCount;
    u16 Cwtv;
    u16 Cmwt;
    int Flags;
    int Special;
    u8 GlobalVolume;
    u8 MixingVolume;
    u8 InitialSpeed;
    u8 InitialTempo;
    u8 Sep;
    u8 PWD;

    u16 MessageLength;
    char *Message;

    int ChannelPan[64];
    int ChannelVolume[64];

    int Orders[256];

    ITInstrument **Instruments;
    ITSample **Samples;
    ITPattern **Patterns;
} ITModule;

// conversion data handler
typedef struct
{
    ITModule **modules;
    int module_count;
    ITSampleData **sounds;
    int sound_count;
} ITBank;

void itbank_init(ITBank *bank, const char **files, int file_count);
void itbank_free(ITBank *bank);
void itbank_add_module(ITBank *bank, const char *filename);
void itbank_add_sound(ITBank *bank, const char *filename);

#endif
