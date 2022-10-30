#include "it2spc.h"
#include "itloader.h"
#include "math.h"

namespace IT2SPC
{

    enum
    {
        MaxUnrollThreshold = 2000,
        LoopLossTolerance = 30000
    };

    enum
    {
        SAMPHEAD_END = 1,
        SAMPHEAD_LOOP = 2,
        SAMPHEAD_FILTER = 12,
        SAMPHEAD_RANGE = 240
    };

    typedef struct tcresult
    {
        int samp_loss;
        int range;
        int filter;
        int peak;
        int samp[16];
        bool overflow;
    } cresult;

    int GenerateBRR(const s16 *cdata, u8 *output, int srclength, int srcloop, int srclooplen, int ffixed, int loopfixed, double amp);
    void BRR_CompressBlock(int *source, int *dest, cresult *presult, int ffixed);
    int ComputeFilter(int x_2, int x_1, int filter);
    int ClampNibble(int n);
    int ClampWord(int n);

    static int UnrollCount(int value, int bound)
    {
        int test = value;
        int uc = 0;
        while (test % bound != 0)
        {
            test += value;
            uc++;
        }
        return uc;
    }

    static double ResampleLoop(s16 **Data, int &length,
                               int &loopstart, int &looplength,
                               int amount)
    {

        int new_looplength = looplength + amount;
        int new_length = length + amount;
        int new_loopstart = new_length - new_looplength;

        double factor = (double)length / (double)new_length;

        s16 *newdata = new s16[new_length];

        // resample with linear interpolation
        for (int t = 0; t < new_length; t++)
        {

            // get samples
            int a = (int)floor((double)t * factor);
            double sa = a < length ? (*Data)[a] : (*Data)[a - looplength];

            a = ((int)floor((double)t * factor)) + 1;
            double sb = a < length ? (*Data)[a] : (*Data)[a - looplength];

            double ratio = (double)t * factor;
            ratio -= floor(ratio);

            int c = (int)(floor(sa * (1 - ratio) + sb * (ratio) + 0.5));
            if (c < -32768)
                c = -32768;
            if (c > 32767)
                c = 32767;
            newdata[t] = c;
        }
        delete[](*Data);
        (*Data) = newdata;

        length = new_length;
        loopstart = new_loopstart;
        looplength = new_looplength;

        return 1.0 / factor; // must scale center freq by this factor!!
    }

    static void PadData(s16 **Data, int &length, int &loopstart,
                        int &looplength, int begin, int end)
    {
        // oof
        s16 *newdata = new s16[length + begin + end];

        s16 *out = newdata;
        for (int i = 0; i < begin; i++)
            *out++ = 0;
        for (int i = 0; i < length; i++)
            *out++ = (*Data)[i];
        for (int i = 0; i < end; i++)
            *out++ = 0;

        delete[](*Data);
        (*Data) = newdata;

        length += begin + end;
        loopstart += begin;
    }

    void UnrollLoop(s16 **Data, int &length, int &loopstart, int &looplength, int unroll)
    {
        s16 *newdata = new s16[length + looplength * unroll];
        s16 *out = newdata;
        for (int i = 0; i < loopstart; i++)
        {
            *out++ = (*Data)[i];
        }

        for (int uc = 0; uc < (unroll + 1); uc++)
            for (int i = loopstart; i < loopstart + looplength; i++)
                *out++ = (*Data)[i];
        delete[](*Data);
        (*Data) = newdata;

        length += unroll * looplength;
        looplength += unroll * looplength;
    }

    void UnrollBidiLoop(s16 **Data, int &length, int &loopstart, int &looplength)
    {
        s16 *newdata = new s16[length + looplength];
        s16 *out = newdata;
        for (int i = 0; i < length; i++)
        {
            *out++ = (*Data)[i];
        }

        for (int i = 0; i < looplength; i++)
        {
            *out++ = (*Data)[length - 1 - i];
        }

        delete[](*Data);
        (*Data) = newdata;

        length += looplength;
        looplength += looplength;
    }

    Source::~Source()
    {
        if (Data)
            delete[] Data;
    }

    Source::Source(const ITLoader::SampleData &source)
    {
        int length = source.Length;
        int loopstart;
        int looplength;

        //		if( cinput ) {
        //			id = cinput->id;
        //		}

        if (source.Length == 0)
        {
            Length = 0;
            Loop = 0;
            TuningFactor = 1.0;
            Data = 0;
            return;
        }
        loopstart = source.LoopStart;
        looplength = source.LoopEnd - source.LoopStart;
        if (!source.Loop)
            looplength = 0;

        if ((length > loopstart + looplength) && looplength)
            length = loopstart + looplength;

        s16 *cdata = new s16[length];

        if (source.Bits16)
        {
            for (int i = 0; i < length; i++)
            {
                cdata[i] = source.Data16[i];
            }
        }
        else
        {
            for (int i = 0; i < length; i++)
            {
                cdata[i] = ((source.Data8[i] * 32767) / 128);
            }
        }

        if (source.BidiLoop)
        {
            UnrollBidiLoop(&cdata, length, loopstart, looplength);
        }

        double tuning_factor = 1.0;

        if (looplength)
        {
            if (looplength & 0xF)
            {
                // must be modified
                int unroll = UnrollCount(looplength & 0xF, 16);
                if ((looplength * (1 + unroll)) < MaxUnrollThreshold)
                {
                    UnrollLoop(&cdata, length, loopstart, looplength, unroll);
                }
                else
                {
                    tuning_factor = ResampleLoop(&cdata, length, loopstart, looplength, 16 - (looplength & 15));
                }
            }
            if (loopstart & 15)
            {
                PadData(&cdata, length, loopstart, looplength, 16 - (loopstart & 15), 0);
            }
        }
        else
        {
            if (length & 0xF)
                PadData(&cdata, length, loopstart, looplength, 0, 16 - (length & 0xF));
        }

        Length = length / 16 * 9;
        Loop = (loopstart / 16) * 9;
        Data = new u8[Length];

        double amp = 1.0;
        u8 c_redo = 1;

        while (c_redo)
        {

            //			if( cinput )
            //				c_redo=GenerateBRR( cdata, Data, length, loopstart, looplength, cinput->force_filter == -1 ? 4 : cinput->force_filter, cinput->force_loop_filter == -1 ? 4 : cinput->force_loop_filter, amp );
            //			else
            c_redo = GenerateBRR(cdata, Data, length, loopstart, looplength, 4, 4, amp);
            amp -= 0.01;
        }

        TuningFactor = tuning_factor;
    }

    inline static unsigned char test_overflow(const int *ls);
    inline static double roundf(double d) { return floor(d + 0.5); }

    int GenerateBRR(const s16 *cdata, u8 *output, int srclength,
                    int srcloop, int srclooplen, int ffixed,
                    int loopfixed, double amp)
    {
        int a; // general variables
        int b; //
        int w; // write position

        int w_loop = 0;  // loop start
        int loop_v1 = 0; // loop start values
        bool use_filter0 = false;
        bool redo_loopf = true;

        bool got_loop = false;

        if (srclength == 0)
            return 0;

        int sbuffer[18]; // uncompressed data, includes 2 previous entries
        int cbuffer[16]; // compressed data
        cresult cres;    // compression results

        sbuffer[0] = 0; // reset previous data
        sbuffer[1] = 0; //

        //		stat->fc[0] = 0;	// reset status
        //		stat->fc[1] = 0;	//
        //		stat->fc[2] = 0;	//
        //		stat->fc[3] = 0;	//
        //		stat->overloss = 0;	//

        // Loop through sample data 16 bytes at a time
        a = 0;
        w = 0;

        while (redo_loopf)
        {
            redo_loopf = false;

            for (; a < srclength; a += 16)
            {
                // Load buffer with 16 samples
                for (b = 0; b < 16; b++)
                {
                    if ((a + b) >= srclength && (a + b) >= 0)
                        sbuffer[b + 2] = 0;
                    else
                        sbuffer[b + 2] = ClampWord((int)roundf((double)cdata[a + b] * amp));
                }

                // Compress block, use filter0 on first sample

                if (a != 0 && !use_filter0)
                    BRR_CompressBlock(sbuffer + 2, cbuffer, &cres, ffixed);
                else
                    BRR_CompressBlock(sbuffer + 2, cbuffer, &cres, 0);

                use_filter0 = false;

                // Save loop start(write-pos) and data
                if (a == srcloop)
                {
                    w_loop = w;
                    loop_v1 = cres.samp[0];
                }

                if (cres.overflow)
                {
                    return 1;
                }

                // increment counter for filter selected
                //				stat->fc[cres.filter]++;

                // add to sample loss
                //				stat->overloss += cres.samp_loss;

                // set previous data for next loop
                sbuffer[1] = cres.samp[15];
                sbuffer[0] = cres.samp[14];

                // write header
                output[w] = (cres.range << 4) + (cres.filter << 2);
                for (b = 0; b < 8; b++)
                    output[w + 1 + b] = cbuffer[b * 2] * 16 + cbuffer[b * 2 + 1]; // write data (big endian)

                // check for loop
                if (srclooplen)
                {
                    // set loop flag
                    output[w] |= SAMPHEAD_LOOP;
                    // check if read counter is past loop_start
                    if (!got_loop && a >= srcloop && a < (srcloop + srclooplen))
                    {
                        // set loop offset
                        got_loop = true;
                    }
                }
                // increment write position
                w += 9;
            }

            // set sample end flag
            w -= 9;
            output[w] = output[w] | SAMPHEAD_END;
            if (srclooplen)
                output[w] = output[w] | SAMPHEAD_LOOP;

            // calculate percentages
            //			double st;
            //			st = (double)( stat->fc[0] + stat->fc[1] + stat->fc[2] + stat->fc[3] );
            //			stat->fp[0] = roundf( (((double)stat->fc[0]) / st) *100 );
            //			stat->fp[1] = roundf( (((double)stat->fc[1]) / st) *100 );
            //			stat->fp[2] = roundf( (((double)stat->fc[2]) / st) *100 );
            //			stat->fp[3] = roundf( (((double)stat->fc[3]) / st) *100 );
            //			stat->overlp = ((double)stat->overloss) / st;

            // check loop loss...
            if (srclooplen)
            {
                int lc_range;
                int lc_filter;
                int lc_value;
                lc_filter = (output[w_loop] & SAMPHEAD_FILTER) >> 2;
                if (lc_filter == 0) // check filter setting
                {
                    return 0; // skip if zero (no loss)
                }
                else
                {
                    lc_range = (output[w_loop] & SAMPHEAD_RANGE) >> 4;
                    lc_value = output[w_loop + 1] >> 4;
                    lc_value <<= lc_range;
                    lc_value += ComputeFilter(sbuffer[15], sbuffer[14], lc_filter);
                    if (abs(lc_value - loop_v1) > LoopLossTolerance || (loopfixed != 4))
                    {
                        // redo compression...
                        a = srcloop;
                        w = w_loop;
                        redo_loopf = true;
                        use_filter0 = true;
                    }
                }
            }
        }
        return 0;
    }

    void BRR_CompressBlock(int *source, int *dest, cresult *presult, int ffixed)
    {
        // source   = source buffer
        // dest     = return buffer (compressed data)
        // presults = result return
        // ffixed   = filter selection
        int r_shift;
        int r_half;
        int c;
        int s1;
        int s2;
        int rs1;
        int rs2;
        int ra;
        int rb;
        int cp;
        int c_1;
        int c_2;
        int x;
        int block_data[16];
        int block_error;
        int block_errorb = 2147483647; // max
        int block_datab[16];
        int block_samp[16];
        int block_sampb[18];
        int block_rangeb = 0;
        int block_filterb = 0;
        int filter;
        int fmin;
        int fmax;
        // set filter ranges
        if (ffixed == 4)
        {
            fmin = 0;
            fmax = 3;
        }
        else
        {
            fmin = ffixed;
            fmax = ffixed;
        }
        // loop through filters
        for (filter = fmin; filter <= fmax; filter++)
        {
            // loop through ranges
            for (r_shift = 12; r_shift >= 0; r_shift--)
            {
                r_half = (1 << r_shift) >> 1; // half shift value (for rounding)
                c_1 = source[-1];             // previous samp 1
                c_2 = source[-2];             // previous samp 2
                block_error = 0;              // reset error
                // loop through samples
                for (x = 0; x < 16; x++)
                {
                    // calculate filter values
                    cp = ComputeFilter(c_2, c_1, filter);
                    c = source[x] >> 1;                  // load sample, /2
                    s1 = (signed short int)(c & 0x7FFF); // uhh? :)
                    s2 = (signed short int)(c | 0x8000); //
                    s1 -= cp;                            // undo filter
                    s2 -= cp;                            //

                    s1 <<= 1; // restore lost bit
                    s2 <<= 1; //

                    s1 += r_half;   // shift and round
                    s2 += r_half;   //
                    s1 >>= r_shift; //
                    s2 >>= r_shift; //

                    s1 = ClampNibble(s1); // clamp
                    s2 = ClampNibble(s2); //
                    rs1 = s1;             // save data
                    rs2 = s2;             //

                    s1 = (s1 << r_shift) >> 1; // undo shift
                    s2 = (s2 << r_shift) >> 1; //

                    if (filter >= 2)             // apply filter
                    {                            //
                        s1 = ClampWord(s1 + cp); //
                        s2 = ClampWord(s2 + cp); //
                    }                            //
                    else                         //
                    {                            //
                        s1 = s1 + cp;            //
                        s2 = s2 + cp;            //
                    }                            //

                    s1 = ((signed short int)(s1 << 1)) >> 1; // sign extend
                    s2 = ((signed short int)(s2 << 1)) >> 1; //

                    ra = (c)-s1; // check difference
                    rb = (c)-s2; //

                    if (ra < 0)
                        ra = -ra; // absolute value
                    if (rb < 0)
                        rb = -rb; //

                    if (ra < rb)                // pick lesser error value
                    {                           //
                        block_error += (int)ra; // add error value
                        block_data[x] = rs1;    // set data
                    }                           //
                    else                        //
                    {                           //
                        block_error += (int)rb; // add error value
                        block_data[x] = rs2;    // set data
                        s1 = s2;                //
                    }                           //

                    if (block_data[x] < 0) // unsign nibble
                        block_data[x] += 16;
                    c_2 = c_1; // set previous samples
                    c_1 = s1;  //

                    block_samp[x] = s1; // save sample
                }
                // check if error rate is lower than current
                if (block_error < block_errorb)
                {
                    // copy all data to "best" buffer
                    block_errorb = block_error;
                    block_rangeb = r_shift;
                    block_filterb = filter;
                    for (x = 0; x < 16; x++)
                        block_datab[x] = block_data[x];
                    for (x = 0; x < 16; x++)
                        block_sampb[x + 2] = block_samp[x];
                }
            }
        }

        unsigned int overflow = 0;

        block_sampb[0] = block_sampb[14 + 2];
        block_sampb[1] = block_sampb[15 + 2];
        for (x = 0; x < 16; x++)
        {
            overflow = (overflow << 1) | test_overflow(block_sampb + x);
        }

        // copy best buffer to output
        for (x = 0; x < 16; x++)
            dest[x] = block_datab[x];

        // copy affected sample data
        for (x = 0; x < 16; x++)
            presult->samp[x] = block_sampb[x + 2];

        // return results
        presult->range = block_rangeb;
        presult->filter = block_filterb;
        presult->samp_loss = block_errorb;

        if (overflow)
            presult->overflow = true;
        else
            presult->overflow = false;
    }

    int ComputeFilter(int x_2, int x_1, int filter)
    {
        int cp = 0;
        switch (filter)
        {
        case 0:     // 0, 0
            cp = 0; // add 0
            break;
        case 1:              // 15/16
            cp = x_1;        // add 16/16
            cp += -x_1 >> 4; // add -1/16
            break;
        case 2:                             // 61/32, -15/16
            cp = x_1 << 1;                  // add 64/32
            cp += -(x_1 + (x_1 << 1)) >> 5; // add -3/32
            cp += -x_2;                     // add -16/16
            cp += x_2 >> 4;                 // add 1/16
            break;
        case 3:                                          // 115/64, -13/16
            cp = x_1 << 1;                               // add 128/64
            cp += -(x_1 + (x_1 << 2) + (x_1 << 3)) >> 6; // add -13/64
            cp += -x_2;                                  // add -16/16
            cp += (x_2 + (x_2 << 1)) >> 4;               // add 3/16
        }
        return cp;
    }

    int ClampNibble(int n)
    {
        if (n < -8)
            n = -8;
        if (n > 7)
            n = 7;
        return n;
    }

    int ClampWord(int n)
    {
        if (n < -32768)
            n = -32768;
        if (n > 32767)
            n = 32767;
        return n;
    }

    template <int G4, int G3, int G2>
    inline static unsigned char test_gauss(const int *ls)
    {
        int s;
        s = int(G4 * ls[0]) >> 11;
        s += int(G3 * ls[1]) >> 11;
        s += int(G2 * ls[2]) >> 11;
        return (s > 0x3FFF) || (s < -0x4000);
    }

    inline static unsigned char test_overflow(const int *ls)
    {
        unsigned char r;

        // p = -256; gauss_table[255, 511, 256]
        r = test_gauss<370, 1305, 374>(ls);

        // p = -255; gauss_table[254, 510, 257]
        r |= test_gauss<366, 1305, 378>(ls);

        // p = -247; gauss_table[246, 502, 265]
        r |= test_gauss<336, 1303, 410>(ls);

        return r;
    }
};
