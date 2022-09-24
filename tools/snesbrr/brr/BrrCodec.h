/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#ifndef BrrCodec_h
#define BrrCodec_h

#include <vector>
#include <exception>
#include "base/basetypes.h"
#include "base/noncopyable.h"
#include "base/Stream.h"

using namespace base;
using namespace base::types;

class InvalidFileException : public std::exception
{
public:
    virtual ~InvalidFileException() throw();
};

class BrrCodec : noncopyable
{
public:
    uint32 loop_start;
    bool loop_enabled;
    bool gauss_enabled;
    bool user_pitch_enabled;

    uint16 pitch_step_base;

    uint32 input_sample_rate;
    uint32 output_sample_rate;

    std::vector<int16> wav_data;
    std::vector<uint8> brr_data;

    void (*cb_func)(BrrCodec &);
    void *cb_data;

    uint cur_progress;
    uint last_progress;
    uint cb_progress;

    uint32 total_blocks;
    double total_error;
    double avg_error;
    double min_error;
    double max_error;

private:
    void reset_progress()
    {
        cur_progress = 0;
        last_progress = 0;
        cb_progress = 0;
        if (cb_func)
            (*cb_func)(*this);
    }

    void set_progress(uint n)
    {
        if (cb_func)
        {
            cur_progress = n;
            if (cur_progress != last_progress)
            {
                (*cb_func)(*this);
                last_progress = cur_progress;
            }
        }
    }

public:
    BrrCodec();
    ~BrrCodec();

    void reset();

    void decode();
    void encode();

    void read_brr(Stream &is);
    void write_brr(Stream &os);

    void read_wav(Stream &is);
    void write_wav(Stream &os);
};

#endif
