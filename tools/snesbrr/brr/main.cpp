/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include <cstdio>
#include <cstring>
#include <exception>
#include "base/basetypes.h"
#include "base/FileStream.h"
#include "base/StreamException.h"
#include "base/OptionParser.h"
#include "BrrCodec.h"

using namespace base;
using namespace base::types;

enum
{
    OPT_Help = 1,
    OPT_Encode,
    OPT_Decode,
    OPT_Transcode,
    OPT_Loop,
    OPT_Pitch,
    OPT_Gauss
};

static const OptionParser::Option option_list[] =
    {
        {OPT_Help, "-?", "--help", "Display this help message"},
        {OPT_Encode, "-e", "--encode", "Encode WAV -> BRR"},
        {OPT_Decode, "-d", "--decode", "Decode BRR -> WAV"},
        {OPT_Transcode, "-t", "--transcode", "Transcode WAV -> (BRR) -> WAV"},
        {OPT_Loop, "-l", "--loop-start", "Set the loop start sample"},
        {OPT_Pitch, "-p", "--pitch", "Set the pitch rate for decoding"},
        {OPT_Gauss, "-g", "--enable-gauss", "Enable gaussian filtering during decoding"},
        {0, 0, 0, 0}};

static void cb_progress(BrrCodec &codec)
{
    if (codec.last_progress >= 100)
        return;

    if (codec.cur_progress == 0)
    {
        std::printf("\n|0          25          50          75        100|\n");
        //             ==================================================
    }
    else
    {
        uint n = codec.cur_progress - codec.last_progress + codec.cb_progress;
        uint i = n / 2;

        codec.cb_progress = n % 2;

        while (i--)
            std::printf("=");

        if (codec.cur_progress >= 100)
            std::printf("\n\n");
    }

    std::fflush(stdout);
}

static int run_main(int argc, const char *const *argv)
{
    OptionParser op(argc, argv, option_list);
    BrrCodec codec;
    const char *in_fn = 0;
    const char *out_fn = 0;
    uint mode = 0;
    bool err = false;

    while (op.next())
    {
        uint id = op.parse();

        switch (id)
        {
        case OPT_Help:
        case OPT_Encode:
        case OPT_Decode:
        case OPT_Transcode:
            if (mode == 0)
                mode = id;
            else
            {
                std::printf("Extra option: %s\n", op.get());
                err = true;
            }
            break;

        case OPT_Loop:
            if (op.next())
            {
                unsigned long x = 0;
                if (std::sscanf(op.get(), "%lu", &x) == 1)
                {
                    codec.loop_start = x;
                    codec.loop_enabled = true;
                }
                else
                {
                    std::printf("Invalid loop start: %s\n", op.get());
                    err = true;
                }
            }
            else
            {
                std::printf("Missing loop start argument.\n");
                err = true;
            }
            break;

        case OPT_Pitch:
            if (op.next())
            {
                uint x = 0;
                if ((std::sscanf(op.get(), "%x", &x) == 1) && (x > 0) && (x < 0x4000))
                {
                    codec.pitch_step_base = x;
                    codec.user_pitch_enabled = true;
                }
                else
                {
                    std::printf("Invalid pitch: %s\n", op.get());
                    err = true;
                }
            }
            else
            {
                std::printf("Missing pitch argument.\n");
                err = true;
            }
            break;

        case OPT_Gauss:
            codec.gauss_enabled = true;
            break;

        default:
            if (!in_fn)
                in_fn = op.get();
            else if (!out_fn)
                out_fn = op.get();
            else
            {
                std::printf("Invalid option: %s\n", op.get());
                err = true;
            }
            break;
        }

        if (err)
        {
            op.stop();
            mode = 0;
        }
    }

    if (mode == OPT_Help)
    {
        op.print_help("[options] input-file output-file");
        return 0;
    }

    if (mode == 0)
    {
        std::printf("Missing encoding/decoding mode.\n");
        err = true;
    }

    if (!in_fn)
    {
        std::printf("Missing input-file argument.\n");
        err = true;
    }

    if (!out_fn)
    {
        std::printf("Missing output-file argument.\n");
        err = true;
    }

    if (err)
    {
        std::printf("Try \'--help\' for more information.\n");
        return 1;
    }

    codec.cb_func = &cb_progress;

    try
    {
        FileStream file(in_fn, FileStream::in);

        switch (mode)
        {
        case OPT_Encode:
            std::printf("Encoding %s -> %s\n", in_fn, out_fn);
            std::fflush(stdout);
            codec.user_pitch_enabled = false;
            codec.read_wav(file);
            codec.encode();
            break;

        case OPT_Decode:
            std::printf("Decoding %s -> %s\n", in_fn, out_fn);
            std::fflush(stdout);
            codec.read_brr(file);
            codec.decode();
            break;

        case OPT_Transcode:
            std::printf("Transcoding %s -> (BRR) -> %s\n", in_fn, out_fn);
            std::fflush(stdout);
            codec.read_wav(file);
            codec.encode();
            codec.decode();
            break;
        }
    }
    catch (const StreamClosedException &e)
    {
        std::printf("Error opening file \"%s\"\n", in_fn);
        return 1;
    }
    catch (const StreamException &e)
    {
        std::printf("Error reading file \"%s\"\n", in_fn);
        return 1;
    }
    catch (const InvalidFileException &e)
    {
        std::printf("Invalid wave file \"%s\"\n", in_fn);
        return 1;
    }

    try
    {
        FileStream file(out_fn, FileStream::out);

        switch (mode)
        {
        case OPT_Encode:
            codec.write_brr(file);
            break;

        case OPT_Decode:
            codec.write_wav(file);
            break;

        case OPT_Transcode:
            codec.write_wav(file);
            break;
        }
    }
    catch (const StreamClosedException &e)
    {
        std::printf("Error opening file \"%s\"\n", out_fn);
        return 1;
    }
    catch (const StreamException &e)
    {
        std::printf("Error writing file \"%s\"\n", out_fn);
        return 1;
    }

    switch (mode)
    {
    case OPT_Encode:
    case OPT_Transcode:
        if (codec.loop_enabled)
            std::printf("BRR Loop Start Offset: 0x%02lX\n", (unsigned long)(codec.loop_start / 16u * 9u));

        std::printf("BRR Pitch Rate: 0x%04X\n\n", (uint)(codec.pitch_step_base));

        std::printf("Block Error Rates\n");
        std::printf("  Blocks: %15lu\n", (unsigned long)(codec.total_blocks));
        std::printf("   Total: %15.0f\n", codec.total_error);
        std::printf(" Average: %20.4f\n", codec.avg_error);
        std::printf(" Minimum: %15.0f\n", codec.min_error);
        std::printf(" Maximum: %15.0f\n", codec.max_error);
        break;
    }

    return 0;
}

#define SNESBRRVERSION __BUILD_VERSION
#define SNESBRRDATE __BUILD_DATE

int main(int argc, const char *const *argv)
{
    std::printf("\n===========================================================");
    std::printf("\n---                  snesbrr v%s %s            ---", SNESBRRVERSION, SNESBRRDATE);
    std::printf("\n-----------------------------------------------------------");
    std::printf("\n           (c) 2006-2021 DMV27 & Alekmaul ");
    std::printf("\nBased on SNESBRR (C) 2006 DMV27 ");
    std::printf("\n===========================================================\n");
    try
    {
        return run_main(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::printf("\nERROR: %s\n", e.what());
    }
    catch (...)
    {
        std::printf("\nERROR: Unknown exception.\n");
    }

    return 1;
}
