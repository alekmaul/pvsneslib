#ifndef PM7_H
#define PM7_H

#include <snes.h>
extern u8 height_m7;
extern u8 angle_m7;

extern u16 posx_m7[3]; // dsb 6  position for some modes with subpixel precision
extern u16 posy_m7[3]; // dsb 6
extern u16 scale;      // dsb 2  for uniform scale
extern u32 scale2;     // dsb 4  for separate axis scale
extern u16 cosa;       // dsb 2  sincos result
extern u16 sina;       // dsb 2
// extern u32 math_a;      // dsb 4  multiply/divide/math input terms 16 or 32-bit
// extern u32 math_b;      // dsb 4
// extern u8 math_p[8];    // dsb 8  product/quotient
// extern u8 math_r[8];    // dsb 8  remainder
extern u8 temp[14];     // dsb 16
extern u32 det_r;       // dsb 4
extern u16 texelx;      // dsb 2
extern u16 texely;      // dsb 2
extern u16 screenx;     // dsb 2
extern u16 screeny;     // dsb 2
extern u8 nmi_ready;    // dsb 1
extern u8 nmi_count;    // dsb 1
extern u8 nmi_bgmode;   // dsb 1  hardware settings applied when nmi_ready
extern u16 nmi_hofs;    // dsb 2
extern u16 nmi_vofs;    // dsb 2
extern u8 nmi_m7t[8];   // dsb 8
extern u16 nmi_m7x;     // dsb 2
extern u16 nmi_m7y;     // dsb 2
extern u8 nmi_cgwsel;   // dsb 1
extern u8 nmi_cgadsub;  // dsb 1
extern u16 nmi_bg2hofs; // dsb 2
extern u16 nmi_bg2vofs; // dsb 2
extern u8 nmi_tm;
// dsb 1
extern u8 new_hdma_en; // dsb 1
extern u8 nmi_hdma_en; // dsb 1
extern u8 angle;       // dsb 1

//  ; perspective
//  ; inputs
extern u8 pv_buffer;   // dsb 1 ; 0/1 selects double buffer
extern u8 pv_l0;       // dsb 1 ; first scanline
extern u8 pv_l1;       // dsb 1 ; last scanline + 1
extern u16 pv_s0;      // dsb 2 ; horizontal texel distance at l0
extern u16 pv_s1;      // dsb 2 ; horizontal texel distance at l1
extern u16 pv_sh;      // dsb 2 ; vertical texel distance from l0 to l1, sh=0 to copy s0 scale for efficiency: (s0*(l1-l0)/256)
extern u8 pv_interp;   // dsb 1 ; interpolate every X lines, 0,1=1x (no interpolation, 2=2x, 4=4x, other values invalid
extern u8 pv_wrap;     // dsb 1 ; 0 if no wrapping, 1 if wrapping (does not affect PPU wrapping)
                       // ; temporaries
extern u16 pv_zr;      // dsb 2 ; interpolated 1/Z
extern u16 pv_zr_inc;  // dsb 2 ; zr increment per line
extern u16 pv_sh_;     // dsb 2 ; =pv_sh, or if pv_sh=0 then computed value
extern u32 pv_scale;   // dsb 4 ; 8-bit scale of a/b/c/d
extern u8 pv_negate;   // dsb 1 ; negation of a/b/c/d
extern u16 pv_interps; // dsb 2 ; interpolate * 4 for stride increment

extern s16 velx_m7;
extern s16 vely_m7;

void updateM7Vblank();
void initpm7();
void updatepm7();
void dptest();

#endif