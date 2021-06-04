/* The contents of this file are subject to the terms of the
   Common Development and Distribution License, Version 1.0 only. */

#include "BrrCodec.h"
#include "base/basemath.h"
#include <string.h>

using namespace base::math;


static const int16 gauss_table[512] =
{
0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001,
0x001, 0x001, 0x001, 0x002, 0x002, 0x002, 0x002, 0x002,
0x002, 0x002, 0x003, 0x003, 0x003, 0x003, 0x003, 0x004,
0x004, 0x004, 0x004, 0x004, 0x005, 0x005, 0x005, 0x005,
0x006, 0x006, 0x006, 0x006, 0x007, 0x007, 0x007, 0x008,
0x008, 0x008, 0x009, 0x009, 0x009, 0x00A, 0x00A, 0x00A,
0x00B, 0x00B, 0x00B, 0x00C, 0x00C, 0x00D, 0x00D, 0x00E,
0x00E, 0x00F, 0x00F, 0x00F, 0x010, 0x010, 0x011, 0x011,
0x012, 0x013, 0x013, 0x014, 0x014, 0x015, 0x015, 0x016,
0x017, 0x017, 0x018, 0x018, 0x019, 0x01A, 0x01B, 0x01B,
0x01C, 0x01D, 0x01D, 0x01E, 0x01F, 0x020, 0x020, 0x021,
0x022, 0x023, 0x024, 0x024, 0x025, 0x026, 0x027, 0x028,
0x029, 0x02A, 0x02B, 0x02C, 0x02D, 0x02E, 0x02F, 0x030,
0x031, 0x032, 0x033, 0x034, 0x035, 0x036, 0x037, 0x038,
0x03A, 0x03B, 0x03C, 0x03D, 0x03E, 0x040, 0x041, 0x042,
0x043, 0x045, 0x046, 0x047, 0x049, 0x04A, 0x04C, 0x04D,
0x04E, 0x050, 0x051, 0x053, 0x054, 0x056, 0x057, 0x059,
0x05A, 0x05C, 0x05E, 0x05F, 0x061, 0x063, 0x064, 0x066,
0x068, 0x06A, 0x06B, 0x06D, 0x06F, 0x071, 0x073, 0x075,
0x076, 0x078, 0x07A, 0x07C, 0x07E, 0x080, 0x082, 0x084,
0x086, 0x089, 0x08B, 0x08D, 0x08F, 0x091, 0x093, 0x096,
0x098, 0x09A, 0x09C, 0x09F, 0x0A1, 0x0A3, 0x0A6, 0x0A8,
0x0AB, 0x0AD, 0x0AF, 0x0B2, 0x0B4, 0x0B7, 0x0BA, 0x0BC,
0x0BF, 0x0C1, 0x0C4, 0x0C7, 0x0C9, 0x0CC, 0x0CF, 0x0D2,
0x0D4, 0x0D7, 0x0DA, 0x0DD, 0x0E0, 0x0E3, 0x0E6, 0x0E9,
0x0EC, 0x0EF, 0x0F2, 0x0F5, 0x0F8, 0x0FB, 0x0FE, 0x101,
0x104, 0x107, 0x10B, 0x10E, 0x111, 0x114, 0x118, 0x11B,
0x11E, 0x122, 0x125, 0x129, 0x12C, 0x130, 0x133, 0x137,
0x13A, 0x13E, 0x141, 0x145, 0x148, 0x14C, 0x150, 0x153,
0x157, 0x15B, 0x15F, 0x162, 0x166, 0x16A, 0x16E, 0x172,
0x176, 0x17A, 0x17D, 0x181, 0x185, 0x189, 0x18D, 0x191,
0x195, 0x19A, 0x19E, 0x1A2, 0x1A6, 0x1AA, 0x1AE, 0x1B2,
0x1B7, 0x1BB, 0x1BF, 0x1C3, 0x1C8, 0x1CC, 0x1D0, 0x1D5,
0x1D9, 0x1DD, 0x1E2, 0x1E6, 0x1EB, 0x1EF, 0x1F3, 0x1F8,
0x1FC, 0x201, 0x205, 0x20A, 0x20F, 0x213, 0x218, 0x21C,
0x221, 0x226, 0x22A, 0x22F, 0x233, 0x238, 0x23D, 0x241,
0x246, 0x24B, 0x250, 0x254, 0x259, 0x25E, 0x263, 0x267,
0x26C, 0x271, 0x276, 0x27B, 0x280, 0x284, 0x289, 0x28E,
0x293, 0x298, 0x29D, 0x2A2, 0x2A6, 0x2AB, 0x2B0, 0x2B5,
0x2BA, 0x2BF, 0x2C4, 0x2C9, 0x2CE, 0x2D3, 0x2D8, 0x2DC,
0x2E1, 0x2E6, 0x2EB, 0x2F0, 0x2F5, 0x2FA, 0x2FF, 0x304,
0x309, 0x30E, 0x313, 0x318, 0x31D, 0x322, 0x326, 0x32B,
0x330, 0x335, 0x33A, 0x33F, 0x344, 0x349, 0x34E, 0x353,
0x357, 0x35C, 0x361, 0x366, 0x36B, 0x370, 0x374, 0x379,
0x37E, 0x383, 0x388, 0x38C, 0x391, 0x396, 0x39B, 0x39F,
0x3A4, 0x3A9, 0x3AD, 0x3B2, 0x3B7, 0x3BB, 0x3C0, 0x3C5,
0x3C9, 0x3CE, 0x3D2, 0x3D7, 0x3DC, 0x3E0, 0x3E5, 0x3E9,
0x3ED, 0x3F2, 0x3F6, 0x3FB, 0x3FF, 0x403, 0x408, 0x40C,
0x410, 0x415, 0x419, 0x41D, 0x421, 0x425, 0x42A, 0x42E,
0x432, 0x436, 0x43A, 0x43E, 0x442, 0x446, 0x44A, 0x44E,
0x452, 0x455, 0x459, 0x45D, 0x461, 0x465, 0x468, 0x46C,
0x470, 0x473, 0x477, 0x47A, 0x47E, 0x481, 0x485, 0x488,
0x48C, 0x48F, 0x492, 0x496, 0x499, 0x49C, 0x49F, 0x4A2,
0x4A6, 0x4A9, 0x4AC, 0x4AF, 0x4B2, 0x4B5, 0x4B7, 0x4BA,
0x4BD, 0x4C0, 0x4C3, 0x4C5, 0x4C8, 0x4CB, 0x4CD, 0x4D0,
0x4D2, 0x4D5, 0x4D7, 0x4D9, 0x4DC, 0x4DE, 0x4E0, 0x4E3,
0x4E5, 0x4E7, 0x4E9, 0x4EB, 0x4ED, 0x4EF, 0x4F1, 0x4F3,
0x4F5, 0x4F6, 0x4F8, 0x4FA, 0x4FB, 0x4FD, 0x4FF, 0x500,
0x502, 0x503, 0x504, 0x506, 0x507, 0x508, 0x50A, 0x50B,
0x50C, 0x50D, 0x50E, 0x50F, 0x510, 0x511, 0x511, 0x512,
0x513, 0x514, 0x514, 0x515, 0x516, 0x516, 0x517, 0x517,
0x517, 0x518, 0x518, 0x518, 0x518, 0x518, 0x519, 0x519
};



InvalidFileException::~InvalidFileException () throw()
{
}



 BrrCodec::BrrCodec ()
{
reset();
}



 BrrCodec::~BrrCodec ()
{
}



void  BrrCodec::reset ()
{
loop_start = 0;
loop_enabled = false;
gauss_enabled = false;
user_pitch_enabled = false;

pitch_step_base = 0x1000;

input_sample_rate = 32000;
output_sample_rate = 32000;

wav_data.clear();
brr_data.clear();

cb_func = 0;
cb_data = 0;

cur_progress = 0;
last_progress = 0;
cb_progress = 0;

total_blocks = 0;
total_error = 0;
avg_error = 0;
min_error = 0;
max_error = 0;
}



void  BrrCodec::decode ()
{
if (!gauss_enabled)
 {
 // 7.8125 = 32000 / 0x1000
 output_sample_rate = uint32(double(pitch_step_base) * 7.8125 + 0.5);
 }

wav_data.clear();

if (brr_data.empty())
 return;

brr_data.resize(((brr_data.size() + 8) / 9) * 9, 0);
brr_data.end()[-9] |= 1;

std::vector<uint8>::const_iterator data = brr_data.begin();
int16 sample[8];  // 4 samples stored twice
int16 last_sample[2];
uint8 header = 0;
uint samp_i = 0;
uint brr_counter = 1;  // --1 == 0
int32 pitch = 0x3000;  // decode 4 samples

last_sample[0] = 0;
last_sample[1] = 0;

while (true)
 {
 while (pitch >= 0)
  {
  pitch -= 0x1000;

  if (--brr_counter == 0)
   {
   if (header & 1)
    return;

   header = *data;
   ++data;
   brr_counter = 16;

   if ((header & 3) == 1)
    return;
   }

  uint8 range = header >> 4;
  uint8 filter = (header >> 2) & 3;

  uint8 samp = *data;
  int32 s;

  // the high nybble is decoded before the low nybble
  if (brr_counter & 1)
   {
   ++data;
   s = int32((samp & 0x0F) ^ 8) - 8;
   }
  else
   s = int32((samp >> 4) ^ 8) - 8;

  if (range > 12)
   s &= ~0x07FF;  // -2048 or 0
  else
   s = (s << range) >> 1;  // -16384 to 16383

  // BRR accumulates to 17 bits, saturates to 16 bits, and then wraps to 15 bits

  switch (filter)
   {
   // last[1] * 15 / 16
   case 1:
    s += last_sample[1];  // add 16/16
    s += -last_sample[1] >> 4;  // add (-1)/16
    // don't clamp - result does not overflow 16 bits
   break;

   // last[1] * 61 / 32 - last[0] * 15 / 16
   case 2:
    s += last_sample[1] << 1;  // add 64/32
    s += -(last_sample[1] + (last_sample[1] << 1)) >> 5;  // add (-3)/32
    s += -last_sample[0];  // add (-16)/16
    s += last_sample[0] >> 4;  // add 1/16
    s = clamp<16>(s);
   break;

   // last[1] * 115 / 64 - last[0] * 13 / 16
   case 3:
    s += last_sample[1] << 1;  // add 128/64
    s += -(last_sample[1] + (last_sample[1] << 2) + (last_sample[1] << 3)) >> 6;  // add (-13)/64
    s += -last_sample[0];  // add (-16)/16
    s += (last_sample[0] + (last_sample[0] << 1)) >> 4;  // add 3/16
    s = clamp<16>(s);
   break;
   }

  s = int16(s << 1) >> 1;  // wrap to 15 bits, sign-extend to 16 bits
  last_sample[0] = last_sample[1];
  last_sample[1] = s;  // 15-bit
  samp_i = (samp_i - 1) & 3;  // do this before storing the sample
  sample[samp_i] = s;  // 15-bit
  sample[samp_i + 4] = s;  // store twice
  }  // pitch loop

 const int16* samp = &sample[samp_i];
 int32 s;

 if (gauss_enabled)
  {
  const int32 p = pitch >> 4;
  const int32 np = -p;
  const int16* G4 = &gauss_table[-1 + np];
  const int16* G3 = &gauss_table[255 + np];
  const int16* G2 = &gauss_table[512 + p];
  const int16* G1 = &gauss_table[256 + p];

  // p is always < 0 and >= -256
  // the first 3 steps wrap using a 15-bit accumulator
  // the last step accumulates to 16-bits then saturates to 15-bits

  s =  int32(G4[0] * samp[3]) >> 11;
  s += int32(G3[0] * samp[2]) >> 11;
  s += int32(G2[0] * samp[1]) >> 11;
  s =  int16(s << 1) >> 1;
  s += int32(G1[0] * samp[0]) >> 11;
  s = clamp<15>(s);

  s = (s * 0x07FF) >> 11;  // envx
  s = (s * 0x7F) >> 7;  // volume

  pitch += pitch_step_base;
  }
 else
  {
  s = samp[3];
  pitch += 0x1000;
  }

 s <<= 1;
 wav_data.push_back(s);
 }  // main loop
}



template <int16 G4, int16 G3, int16 G2>
inline static uint8 test_gauss (const int16* ls)
{
int32 s;
s =  int32(G4 * ls[0]) >> 11;
s += int32(G3 * ls[1]) >> 11;
s += int32(G2 * ls[2]) >> 11;
return (s > 0x3FFF) || (s < -0x4000);
}



/*
There are 3 pitch values that can cause sign inversion in the gaussian
filtering by overflowing the 15-bit accumulator if the input samples are
too close to the min/max value.

The sum of the first 3 gauss_table values for each of these 3 pitch values
is 2049 while all other pitch values are 2048 or less.
*/

inline static uint8 test_overflow (const int16* ls)
{
uint8 r;

// p = -256; gauss_table[255, 511, 256]
r =  test_gauss<370, 1305, 374>(ls);

// p = -255; gauss_table[254, 510, 257]
r |= test_gauss<366, 1305, 378>(ls);

// p = -247; gauss_table[246, 502, 265]
r |= test_gauss<336, 1303, 410>(ls);

return r;
}



void  BrrCodec::encode ()
{
reset_progress();

brr_data.clear();

if (loop_start >= wav_data.size())
 {
 loop_start = 0;
 loop_enabled = false;
 }

if (loop_enabled)
 {
 uint32 start_align = (16 - (loop_start & 15)) & 15;
 uint32 loop_size = wav_data.size() - loop_start;
 uint32 end_align = loop_size;

 while (end_align & 15)
  end_align <<= 1;

 // remove the existing loop block from the alignment
 end_align -= loop_size;

 // also include the loop_start alignment
 end_align += start_align;

 if (end_align != 0)
  {
  wav_data.resize(wav_data.size() + end_align, 0);

  const int16* src = &wav_data[loop_start];
  int16* dst = &wav_data[loop_start + loop_size];
  const int16* end = dst + end_align;

  while (dst != end)
   *dst++ = *src++;

  // 16-sample align loop_start
  loop_start += start_align;
  }
 }
else
 wav_data.resize(((wav_data.size() + 15) / 16) * 16, 0);

const float base_adjust_rate = 0.0004;
float adjust_rate = base_adjust_rate;
const uint32 loop_block = loop_start / 16u;
const uint32 wimax = wav_data.size() / 16u;
uint32 wi = 0;
int16 best_samp[18];

best_samp[0] = 0;
best_samp[1] = 0;

total_blocks = wimax;
total_error = 0;
avg_error = 0;
min_error = 1e20;
max_error = 0;

while (wi != wimax)
 {
 int16* p = &wav_data[wi * 16u];
 double best_err = 1e20;
 int16 blk_samp[18];
 uint8 best_data[9];

 blk_samp[0] = best_samp[0];
 blk_samp[1] = best_samp[1];

 for (uint8 filter = 0; filter <= 3; ++filter)
  {
  if (filter != 0)
   {
   if ((wi == 0) || (wi == loop_block))
    continue;
   }

  // Ranges 0, 13, 14, 15 are "invalid", so they are not used for encoding.
  // The values produced by these ranges are fully covered by the other
  // range values, so there will be no loss in quality.
  for (uint8 range = 12; range >= 1; --range)
   {
   const int32 rhalf = (1 << range) >> 1;
   double blk_err = 0;
   uint8 blk_data[16];

   for (uint n = 0; n < 16; ++n)
    {
    int16* blk_ls = blk_samp + n;
    int32 filter_s;

    switch (filter)
     {
     default:
     case 0:
      filter_s = 0;
     break;

     case 1:
      filter_s  = blk_ls[1];  // add 16/16
      filter_s += -blk_ls[1] >> 4;  // add (-1)/16
     break;

     case 2:
      filter_s  = blk_ls[1] << 1;  // add 64/32
      filter_s += -(blk_ls[1] + (blk_ls[1] << 1)) >> 5;  // add (-3)/32
      filter_s += -blk_ls[0];  // add (-16)/16
      filter_s += blk_ls[0] >> 4;  // add 1/16
     break;

     case 3:
      filter_s  = blk_ls[1] << 1;  // add 128/64
      filter_s += -(blk_ls[1] + (blk_ls[1] << 2) + (blk_ls[1] << 3)) >> 6;  // add (-13)/64
      filter_s += -blk_ls[0];  // add (-16)/16
      filter_s += (blk_ls[0] + (blk_ls[0] << 1)) >> 4;  // add 3/16
     break;
     }

    // undo 15 -> 16 bit conversion
    int32 xs = p[n] >> 1;

    // undo 16 -> 15 bit wrapping
    // check both possible 16-bit values
    int32 s1 = int16(xs & 0x7FFF);
    int32 s2 = int16(xs | 0x8000);

    // undo filtering
    s1 -= filter_s;
    s2 -= filter_s;

    // restore low bit lost during range decoding
    s1 <<= 1;
    s2 <<= 1;

    // reduce s to range with nearest value rounding
    // range = 2, rhalf = 2
    // s(-6, -5, -4, -3) = -1
    // s(-2, -1,  0,  1) =  0
    // s( 2,  3,  4,  5) =  1
    s1 = (s1 + rhalf) >> range;
    s2 = (s2 + rhalf) >> range;

    s1 = clamp<4>(s1);
    s2 = clamp<4>(s2);

    uint8 rs1 = s1 & 0x0F;
    uint8 rs2 = s2 & 0x0F;

    // -16384 to 16383
    s1 = (s1 << range) >> 1;
    s2 = (s2 << range) >> 1;

    // BRR accumulates to 17 bits, saturates to 16 bits, and then wraps to 15 bits

    if (filter >= 2)
     {
     s1 = clamp<16>(s1 + filter_s);
     s2 = clamp<16>(s2 + filter_s);
     }
    else
     {
     // don't clamp - result does not overflow 16 bits
     s1 += filter_s;
     s2 += filter_s;
     }

    // wrap to 15 bits, sign-extend to 16 bits
    s1 = int16(s1 << 1) >> 1;
    s2 = int16(s2 << 1) >> 1;

    double d1 = xs - s1;
    double d2 = xs - s2;

    d1 *= d1;
    d2 *= d2;

    // If d1 == d2, prefer s2 over s1.
    if (d1 < d2)
     {
     blk_err += d1;
     blk_ls[2] = s1;
     blk_data[n] = rs1;
     }
    else
     {
     blk_err += d2;
     blk_ls[2] = s2;
     blk_data[n] = rs2;
     }
    }  // block loop

   // Use < for comparison. This will cause the encoder to prefer
   // less complex filters and higher ranges when error rates are equal.
   // This will then result in a slightly lower average error rate.
   if (blk_err < best_err)
    {
    best_err = blk_err;

    for (uint n = 0; n < 16; ++n)
     best_samp[n + 2] = blk_samp[n + 2];

    best_data[0] = (range << 4) | (filter << 2);

    for (uint n = 0; n < 8; ++n)
     best_data[n + 1] = (blk_data[n * 2] << 4) | blk_data[n * 2 + 1];
    }
   }  // range loop
  }  // filter loop

 uint16 overflow = 0;

 for (uint n = 0; n < 16; ++n)
  {
  uint8 b = test_overflow(best_samp + n);
  overflow = (overflow << 1) | b;
  }

 if (overflow)
  {
  float f[16];

  for (uint n = 0; n < 16; ++n)
   f[n] = adjust_rate;

  for (int n = 0; n < 16; ++n, overflow <<= 1)
   if (overflow & 0x8000)
    {
    float t = 0.05;

    for (int i = n; i >= 0; --i, t *= 0.1)
     f[i] *= 1.0 + t;

    t = 0.05 * 0.1;
    for (int i = n + 1; i < 16; ++i, t *= 0.1)
     f[i] *= 1.0 + t;
    }

  for (uint n = 0; n < 16; ++n)
   p[n] = int16(p[n] * (1.0 - f[n]));

  adjust_rate *= 1.1;
  }
 else
  {
  adjust_rate = base_adjust_rate;
  best_samp[0] = best_samp[16];
  best_samp[1] = best_samp[17];

  total_error += best_err;

  if (best_err < min_error)
   min_error = best_err;

  if (best_err > max_error)
   max_error = best_err;

  brr_data.insert(brr_data.end(), best_data, best_data + 9);

  wi += 1;
  set_progress(wi * 100u / wimax);
  }
 }  // wave loop

if (wimax == 0)
 min_error = 0;
else
 avg_error = total_error / wimax;

if (brr_data.empty() || !loop_enabled)
 brr_data.insert(brr_data.end(), 9, 0);

brr_data.end()[-9] |= (uint8(loop_enabled) << 1) | 1;

set_progress(100);

if (!user_pitch_enabled)
 {
 // 0.128 = 0x1000 / 32000
 uint x = uint(double(input_sample_rate) * 0.128 + 0.5);

 if (x < 1)
  x = 1;
 else if (x > 0x3FFF)
  x = 0x3FFF;

 pitch_step_base = x;
 }
}



void  BrrCodec::read_brr (Stream& is)
{
brr_data.resize(((is.size() + 8) / 9) * 9, 0);
is.read(&brr_data[0], is.size());
}



void  BrrCodec::write_brr (Stream& os)
{
os.write(&brr_data[0], brr_data.size());
}



static uint16  read2 (Stream& is)
{
uint8 buf[2];

is.read(buf, 2);

return buf[0] | (buf[1] << 8);
}



static uint32  read4 (Stream& is)
{
uint8 buf[4];

is.read(buf, 4);

return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}



static void  write2 (Stream& os, uint16 s)
{
uint8 buf[2];

buf[0] = s;
buf[1] = s >> 8;

os.write(buf, 2);
}



static void  write4 (Stream& os, uint32 s)
{
uint8 buf[4];

buf[0] = s;
buf[1] = s >> 8;
buf[2] = s >> 16;
buf[3] = s >> 24;

os.write(buf, 4);
}



inline static uint32 word_align (uint32 x)
{
return (x + 1) & ~1;
}



void  BrrCodec::read_wav (Stream& is)
{
uint8 cid[4];

is.read(cid, 4);
if (memcmp(cid, "RIFF", 4) != 0)
 throw InvalidFileException();

is.seekoff(4);

is.read(cid, 4);
if (memcmp(cid, "WAVE", 4) != 0)
 throw InvalidFileException();

uint32 fmt_off = 0;
uint32 fmt_size = 0;
uint32 data_off = 0;
uint32 data_size = 0;

while (!is.eof())
 {
 is.read(cid, 4);
 uint32 size = read4(is);

 if (memcmp(cid, "fmt ", 4) == 0)
  {
  fmt_off = is.tell();
  fmt_size = size;
  }
 else if (memcmp(cid, "data", 4) == 0)
  {
  data_off = is.tell();
  data_size = size;
  }

 size = word_align(size);

 if ((is.tell() + size) > is.size())
  throw InvalidFileException();

 is.seekoff(size);
 }

if ((fmt_off == 0) || (data_off == 0))
 throw InvalidFileException();

if (fmt_size < 16)
 throw InvalidFileException();

is.seekpos(fmt_off);

// PCM only
if (read2(is) != 1)
 throw InvalidFileException();

uint16 channels = read2(is);

if ((channels != 1) && (channels != 2))
 throw InvalidFileException();

input_sample_rate = read4(is);

if (input_sample_rate == 0)
 throw InvalidFileException();

is.seekoff(6);

uint16 bit_rate = read2(is);

if ((bit_rate != 8) && (bit_rate != 16))
 throw InvalidFileException();

is.seekpos(data_off);

data_size /= (bit_rate >> 3) * channels;

if (data_size == 0)
 throw InvalidFileException();

wav_data.clear();

if (bit_rate == 16)
 {
 if (channels == 1)
  {
  do
   {
   int16 m = int16(read2(is));
   wav_data.push_back(m);
   }
  while (--data_size != 0);
  }
 else
  {
  do
   {
   int16 l = int16(read2(is));
   int16 r = int16(read2(is));
   int16 m = int32(l + r) >> 1;
   wav_data.push_back(m);
   }
  while (--data_size != 0);
  }
 }
else
 {
 if (channels == 1)
  {
  do
   {
   int16 m = int8(is.read() ^ 0x80) << 8;
   wav_data.push_back(m);
   }
  while (--data_size != 0);
  }
 else
  {
  do
   {
   int16 l = int8(is.read() ^ 0x80) << 8;
   int16 r = int8(is.read() ^ 0x80) << 8;
   int16 m = int32(l + r) >> 1;
   wav_data.push_back(m);
   }
  while (--data_size != 0);
  }
 }
}



void  BrrCodec::write_wav (Stream& os)
{
// 8 bytes
os.write("RIFF", 4);
write4(os, 36 + wav_data.size() * 2);

// 4 bytes
os.write("WAVE", 4);

// 8 bytes
os.write("fmt ", 4);
write4(os, 16);  // size

// 16 bytes
write2(os, 1);  // compression code (1 = PCM)
write2(os, 1);  // channels
write4(os, output_sample_rate);  // sample rate
write4(os, output_sample_rate * 2);  // average bytes per second
write2(os, 2);  // block align
write2(os, 16);  // bits per sample

// 8 bytes
os.write("data", 4);
write4(os, wav_data.size() * 2);

for (std::vector<int16>::const_iterator i = wav_data.begin(); i != wav_data.end(); ++i)
 write2(os, *i);
}

