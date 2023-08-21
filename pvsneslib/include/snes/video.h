/*---------------------------------------------------------------------------------

    Video registers and defines

    Copyright (C) 2012-2017
        Alekmaul

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any
    damages arising from the use of this software.

    Permission is granted to anyone to use this software for any
    purpose, including commercial applications, and to alter it and
    redistribute it freely, subject to the following restrictions:

    1.	The origin of this software must not be misrepresented; you
        must not claim that you wrote the original software. If you use
        this software in a product, an acknowledgment in the product
        documentation would be appreciated but is not required.
    2.	Altered source versions must be plainly marked as such, and
        must not be misrepresented as being the original software.
    3.	This notice may not be removed or altered from any source
        distribution.

---------------------------------------------------------------------------------*/

/*!	\file video.h
    \brief contains the basic definitions for controlling the video hardware.

*/
#ifndef SNES_VIDEO_INCLUDE
#define SNES_VIDEO_INCLUDE

#include <snes/snestypes.h>

/*!	\brief Bit defines for the video control registers */
typedef enum
{
    DSP_FORCEVBL = BIT(7), /*!< \brief Forced Blanking (0=Normal, 1=Screen Black) */

    VRAM_INCLOW = (0 << 7), /*!< \brief Increment VRAM Address after accessing High/Low byte (0=Low, 1=High) */
    VRAM_INCHIGH = (1 << 7),
    VRAM_ADRTR_0B = (0 << 2),     /*!< \brief Address Translation    (0..3 = 0bit/None, 8bit, 9bit, 10bit) */
    VRAM_ADRTR_8B = (1 << 2),     /*!< \brief Address Translation    (0..3 = 0bit/None, 8bit, 9bit, 10bit) */
    VRAM_ADRTR_9B = (2 << 2),     /*!< \brief Address Translation    (0..3 = 0bit/None, 8bit, 9bit, 10bit) */
    VRAM_ADRTR_10B = (3 << 2),    /*!< \brief Address Translation    (0..3 = 0bit/None, 8bit, 9bit, 10bit) */
    VRAM_ADRSTINC_1 = (0 << 0),   /*!< \brief Address Increment Step (0..3 = Increment Word-Address by 1,32,128,128) */
    VRAM_ADRSTINC_32 = (1 << 0),  /*!< \brief Address Increment Step (0..3 = Increment Word-Address by 1,32,128,128) */
    VRAM_ADRSTINC_128 = (2 << 0) /*!< \brief Address Increment Step (0..3 = Increment Word-Address by 1,32,128,128) */
} VideoControl;

#define FADE_IN 2
#define FADE_OUT 1
#define MOSAIC_IN 2
#define MOSAIC_OUT 1
#define MOSAIC_BG1 (1 << 0)
#define MOSAIC_BG2 (1 << 1)
#define MOSAIC_BG3 (1 << 2)
#define MOSAIC_BG4 (1 << 3)

#define CM_DIRCOLOR (1 << 0) /*!< \brief  Color Math Control Register A & B bits */
#define CM_SUBBGOBJ_ENABLE (1 << 1)
#define CM_ALWAYS_ENABLE (0 << 4)
#define CM_MATWIN_ENABLE (1 << 4)
#define CM_NOMATWIN_ENABLE (2 << 4)
#define CM_NEVER_ENABLE (3 << 4)
#define CM_SCBLK_ALWAYS_ENABLE (0 << 6)
#define CM_SCBLK_MATWIN_ENABLE (1 << 6)
#define CM_SCBLK_NOMATWIN_ENABLE (2 << 6)
#define CM_SCBLK_NEVER_ENABLE (3 << 6)

#define CM_SUB_MAINSUB (1 << 7)
#define CM_DIV2_DIVIDE (1 << 6)
#define CM_MSCR_BACK (1 << 5)
#define CM_MSCR_PAL47 (1 << 4)
#define CM_MSCR_BG4 (1 << 3)
#define CM_MSCR_BG3 (1 << 2)
#define CM_MSCR_BG2 (1 << 1)
#define CM_MSCR_BG1 (1 << 0)

#define CM_APPLY_BLUE (1 << 7)
#define CM_APPLY_GREEN (1 << 6)
#define CM_APPLY_RED (1 << 5)

#define M7_HFLIP (1 << 0)    /*!< \brief Mode7 screen H-Flip (0=Normal, 1=Flipped) flip 256x256 "screen"*/
#define M7_VFLIP (1 << 1)    /*!< \brief Mode7 screen V-Flip (0=Normal, 1=Flipped) flip 256x256 "screen"*/
#define M7_WRAP (1 << 6)     /*!< \brief Mode7 Wrap within 128x128 tile area */
#define M7_OUTTRANS (2 << 6) /*!< \brief Outside 128x128 tile area is Transparent */
#define M7_OUTTILE (3 << 6)  /*!< \brief Outside 128x128 tile area is filled by Tile 00h */

#define PPU_50HZ (1 << 4) /*!< \brief Frame Rate (PPU2.Pin30)  (0=NTSC/60Hz, 1=PAL/50Hz) */

/*! \def REG_INIDISP
    \brief Display Control 1 (W)
    7     Forced Blanking (0=Normal, 1=Screen Black)
    6-4   Not used
    3-0   Master Brightness (0=Screen Black, or N=1..15: Brightness*(N+1)/16)

    In Forced Blank, VRAM, OAM and CGRAM can be freely accessed (otherwise it's
    accessible only during Vblank). Even when in forced blank, the TV Set keeps
    receiving Vsync/Hsync signals (thus producing a stable black picture). And,
    the CPU keeps receiving Hblank/Vblank signals (so any enabled video NMIs, IRQs,
    HDMAs are kept generated).

    Forced blank doesn't apply immediately... so one must wait whatever
    (maybe a scanline) before VRAM can be freely accessed... or is it only
    vice-versa: disabling forced blank doesn't apply immediately/shows garbage
    pixels?
*/
#define REG_INIDISP (*(vuint8 *)0x2100)

/*! \def REG_VMAIN
    \brief VRAM Address Increment Mode (W)
    7     Increment VRAM Address after accessing High/Low byte (0=Low, 1=High)
    6-4   Not used
    3-2   Address Translation    (0..3 = 0bit/None, 8bit, 9bit, 10bit)
    1-0   Address Increment Step (0..3 = Increment Word-Address by 1,32,128,128)

    The address translation is intended for bitmap graphics (where one would have filled the BG Map
    by increasing Tile numbers), technically it does thrice left-rotate the lower 8, 9, or 10 bits
    of the Word-address:
    Translation  Bitmap Type              Port [2116h/17h]    VRAM Word-Address
    8bit rotate  4-color; 1 word/plane    aaaaaaaaYYYxxxxx --> aaaaaaaaxxxxxYYY
    9bit rotate  16-color; 2 words/plane  aaaaaaaYYYxxxxxP --> aaaaaaaxxxxxPYYY
    10bit rotate 256-color; 4 words/plane aaaaaaYYYxxxxxPP --> aaaaaaxxxxxPPYYY

    Where "aaaaa" would be the normal address MSBs, "YYY" is the Y-index (within a 8x8 tile),
    "xxxxx" selects one of the 32 tiles per line, "PP" is the bit-plane index (for BGs with more
    than one Word per plane). For the intended result (writing rows of 256 pixels) the Translation
    should be combined with Increment Step=1.
    For Mode 7 bitmaps one could eventually combine step 32/128 with 8bit/10bit rotate:

  8bit-rotate/step32   aaaaaaaaXXXxxYYY --> aaaaaaaaxxYYYXXX
  10bit-rotate/step128 aaaaaaXXXxxxxYYY --> aaaaaaxxxxYYYXXX

    Though the SNES can't access enough VRAM for fullscreen Mode 7 bitmaps.
    Step 32 (without translation) is useful for updating BG Map columns (eg. after horizontal scrolling).
*/
#define REG_VMAIN (*(vuint8 *)0x2115)

/*! \def REG_VMADDLH
    \brief  VMADDL - VRAM Address (lower 8bit) (W)
            VMADDH - VRAM Address (upper 8bit) (W)

    VRAM Address for reading/writing. This is a WORD address (2-byte steps),
    the PPU could theoretically address up to 64K-words (128K-bytes), in practice,
    only 32K-words (64K-bytes) are installed in SNES consoles (VRAM address bit15
    is not connected, so addresses 8000h-FFFFh are mirrors of 0-7FFFh).
    After reading/writing VRAM Data, the Word-address can be automatically incremented
    by 1,32,128 (depending on the Increment Mode in Port 2115h) (Note: the Address Translation
    feature is applied only "temporarily" upon memory accesses, it doesn't affect the
    value in Port 2116h-17h).
    Writing to 2116h/2117h does prefetch 16bit data from the new address (for later reading).
*/
#define REG_VMADDLH (*(vuint16 *)0x2116)

/*
    Writing to 2118h or 2119h does simply modify the LSB or MSB of the currently addressed VRAM
    word (with optional Address Translation applied). Depending on the Increment Mode the address
    does (or doesn't) get automatically incremented after the write.
*/
#define REG_VMDATAL (*(vuint8 *)0x2118)   /*!< \brief VRAM Data Write (lower 8bit) (W) */
#define REG_VMDATAH (*(vuint8 *)0x2119)   /*!< \brief VRAM Data Write (upper 8bit) (W) */
#define REG_VMDATALH (*(vuint16 *)0x2118) /*!< \brief VRAM Data Write (lower and upper 8bit) (W) */

/*! \def REG_CGADD

    \brief Palette CGRAM Address (Color Generator Memory) (W)
    Color index (0..255). This is a WORD-address (2-byte steps), allowing to access
    256 words (512 bytes). Writing to this register resets the 1st/2nd access
    flipflop (for 2122h/213Bh) to 1st access.
*/
#define REG_CGADD (*(vuint8 *)0x2121)

/*! \def CGRAM_PALETTE
    \brief Palette CGRAM Data Write (W)
    1st Access: Lower 8 bits (even address)
    2nd Access: Upper 7 bits (odd address) (upper 1bit = PPU2 open bus)

    Reads and Writes to EVEN and ODD byte-addresses work as follows:
    Write to EVEN address  -->  set Cgram_Lsb = Data    ;memorize value
    Write to ODD address   -->  set WORD[addr-1] = Data*256 + Cgram_Lsb
    Read from ANY address  -->  return BYTE[addr]

    The address is automatically incremented after every read or write access.

    256-Color Palette Entries
    15    Not used (should be zero) (read: PPU2 Open Bus)
    14-10 Blue
    9-5   Green
    4-0   Red

    Palette Indices
    00h      Main Backdrop color (used when all BG/OBJ pixels are transparent)
    01h-FFh  256-color BG palette (when not using direct-color mode)
    01h-7Fh  128-color BG palette (BG2 in Mode 7)
    01h-7Fh  Eight 16-color BG palettes
    01h-1Fh  Eight 4-color BG palettes (except BG2-4 in Mode 0)
    21h-3Fh  Eight 4-color BG palettes (BG2 in Mode 0 only)
    41h-5Fh  Eight 4-color BG palettes (BG3 in Mode 0 only)
    61h-7Fh  Eight 4-color BG palettes (BG4 in Mode 0 only)
    81h-FFh  Eight 16-color OBJ palettes (half of them with color-math disabled)
    N/A      Sub Backdrop color (not in CGRAM, set via COLDATA, Port 2132h)
*/
#define CGRAM_PALETTE ((u8 *)0x2122)

/*! \def REG_CGWSEL
    \brief Color Math Control Register A (W)
    7-6  Force Main Screen Black (3=Always, 2=MathWindow, 1=NotMathWin, 0=Never)
    5-4  Color Math Enable       (0=Always, 1=MathWindow, 2=NotMathWin, 3=Never)
    3-2  Not used
    1    Sub Screen BG/OBJ Enable    (0=No/Backdrop only, 1=Yes/Backdrop+BG+OBJ)
    0    Direct Color (for 256-color BGs)  (0=Use Palette, 1=Direct Color)
*/
#define REG_CGWSEL (*(vuint8 *)0x2130)

/*! \def REG_CGADSUB
    \brief Color Math Control Register B (W)
    7    Color Math Add/Subtract        (0=Add; Main+Sub, 1=Subtract; Main-Sub)
    6    Color Math "Div2" Half Result  (0=No divide, 1=Divide result by 2)
    5    Color Math when Main Screen = Backdrop        (0=Off, 1=On) ;\
    4    Color Math when Main Screen = OBJ/Palette4..7 (0=Off, 1=On) ; OFF: Show
    -    Color Math when Main Screen = OBJ/Palette0..3 (Always=Off)  ; Raw Main,
    3    Color Math when Main Screen = BG4             (0=Off, 1=On) ;   or
    2    Color Math when Main Screen = BG3             (0=Off, 1=On) ; ON: Show
    1    Color Math when Main Screen = BG2             (0=Off, 1=On) ; Main+/-Sub
    0    Color Math when Main Screen = BG1             (0=Off, 1=On) ;/
*/
#define REG_CGADSUB (*(vuint8 *)0x2131)

/*! \def REG_COLDATA
     \brief Color Math Sub Screen Backdrop Color (W)
    This 8bit port allows to manipulate some (or all) bits
    of a 15bit RGB value. Examples: Black: write E0h (R,G,B=0), Cyan: write 20h (R=0) and DFh (G,B=1Fh).
    7    Apply Blue  (0=No change, 1=Apply Intensity as Blue)
    6    Apply Green (0=No change, 1=Apply Intensity as Green)
    5    Apply Red   (0=No change, 1=Apply Intensity as Red)
    4-0  Intensity   (0..31)
*/
#define REG_COLDATA (*(vuint8 *)0x2132)

/*! \def REG_M7SEL
    \brief Rotation/Scaling Mode Settings (W)
    7-6   Screen Over (see below)
    5-2   Not used
    1     Screen V-Flip (0=Normal, 1=Flipped)     ; flip 256x256 "screen"
    0     Screen H-Flip (0=Normal, 1=Flipped)     ;

    Screen Over (when exceeding the 128x128 tile BG Map size):
    0=Wrap within 128x128 tile area
    1=Wrap within 128x128 tile area (same as 0)
    2=Outside 128x128 tile area is Transparent
    3=Outside 128x128 tile area is filled by Tile 00h
*/
#define REG_M7SEL (*(vuint8 *)0x211A)

/* 1st Write: Lower 8bit  ;\1st/2nd write mechanism uses "M7_old"
  2nd Write: Upper 8bit  ;/
*/
#define REG_M7A (*(vuint8 *)0x211B)    /*!< \brief Rotation/Scaling Parameter A (and Maths 16bit operand) (W) */
#define REG_M7B (*(vuint8 *)0x211C)    /*!< \brief Rotation/Scaling Parameter B (and Maths 16bit operand) (W) */
#define REG_M7C (*(vuint8 *)0x211D)    /*!< \brief Rotation/Scaling Parameter C (W) */
#define REG_M7D (*(vuint8 *)0x211E)    /*!< \brief Rotation/Scaling Parameter D (W) */
#define REG_M7HOFS (*(vuint8 *)0x210D) /*!< \brief BG1 Horizontal Scroll (X) (W) */
#define REG_M7VOFS (*(vuint8 *)0x210E) /*!< \brief BG1 Vertical Scroll (Y) (W) */
#define REG_M7X (*(vuint8 *)0x211F)    /*!< \brief Rotation/Scaling Center Coordinate X (W) */
#define REG_M7Y (*(vuint8 *)0x2120)    /*!< \brief Rotation/Scaling Center Coordinate Y (W) */

/* After writing to 211Bh or 211Ch, the result can be read immediately from 2134h-2136h (the 21xxh Ports
  are rapidly clocked by the PPU, there's no delay needed when reading via "MOV A,[211Ch]" or via
  "MOV A,[1Ch]" (with D=2100h), both works even when the CPU runs at 3.5MHz).
*/
#define REG_MPYL (*(vuint8 *)0x2134)   /*!< \brief Signed Multiply Result (lower 8bit) (R) */
#define REG_MPYM (*(vuint8 *)0x2135)   /*!< \brief Signed Multiply Result (middle 8bit) (R) */
#define REG_MPYH (*(vuint8 *)0x2136)   /*!< \brief Signed Multiply Result (upper 8bit) (R) */
#define REG_MPYMH (*(vuint16 *)0x2135) /*!< \brief Signed Multiply Result (middle +upper 8bit) (R) */

/*! \def REG_STAT77
    \brief PPU1 Status and Version Number (R)
    7    OBJ Time overflow  (0=Okay, 1=More than 8x34 OBJ pixels per scanline)
    6    OBJ Range overflow (0=Okay, 1=More than 32 OBJs per scanline)
    5    Master/Slave Mode (PPU1.Pin25) (0=Normal=Master)
    4    Not used (PPU1 open bus) (same as last value read from PPU1)
    3-0  PPU1 5C77 Version Number (only version 1 exists as far as I know)

The overflow flags are cleared at end of V-Blank, but NOT during forced blank!
The overflow flags are set (regardless of OBJ enable/disable in 212Ch), at following
    times: Bit6 when V=OBJ.YLOC/H=OAM.INDEX*2, bit7 when V=OBJ.YLOC+1/H=0.
*/
#define REG_STAT77 (*(vuint8 *)0x213E)

/*! \def REG_STAT78
    \brief PPU2 Status and Version Number (R)
    7    Current Interlace-Frame (0=1st, 1=2nd Frame)
    6    H/V-Counter/Lightgun/Joypad2.Pin6 Latch Flag (0=No, 1=New Data Latched)
    5    Not used (PPU2 open bus) (same as last value read from PPU2)
    4    Frame Rate (PPU2.Pin30)  (0=NTSC/60Hz, 1=PAL/50Hz)
    3-0  PPU2 5C78 Version Number (version 1..3 exist as far as I know)
*/
#define REG_STAT78 (*(vuint8 *)0x213F)

// macro creates a 15 bit color from 3x5 bit components
/** \brief  Macro to convert 5 bits or 8 bits r g b components into a single 15 bit RGB triplet */
#define RGB8(r, g, b) (((r) >> 3) | (((g) >> 3) << 5) | (((b) >> 3) << 10))
#define RGB5(r, g, b) ((r) | ((g) << 5) | ((b) << 10))

#define SCREEN_HEIGHT 224 /** \brief  Screen height in pixels */
#define SCREEN_WIDTH 256  /** \brief  Screen width in pixels */

extern u8 videoMode;    /*!< \brief Current value of REG_TM */
extern u8 videoModeSub; /*!< \brief Current value of REG_TS */

/*! \fn  setBrightness(u8 level)
    \brief sets the screens brightness.
    \param level	15 = full brightness, 0= black
*/
void setBrightness(u8 level);

/*! \fn  setMode(u8 mode, u8 size)
    \brief Set the SNES hardware to operate in new display mode.
    \param mode	0..7 for available Snes display mode
    \param size	Tile Size (8x8 or 16x16) of each BG in this mode (if available)

    Mode   BG1         BG2         BG3         BG4
    0      4-color     4-color     4-color     4-color   ;Normal
    1      16-color    16-color    4-color     -         ;Normal
    2      16-color    16-color    (o.p.t)     -         ;Offset-per-tile
    3      256-color   16-color    -           -         ;Normal
    4      256-color   4-color     (o.p.t)     -         ;Offset-per-tile
    5      16-color    4-color     -           -         ;512-pix-hires
    6      16-color    -           (o.p.t)     -         ;512-pix plus Offs-p-t
    7      256-color   EXTBG       -           -         ;Rotation/Scaling
*/
void setMode(u8 mode, u8 size);

/*! \fn  setScreenOn(void)
    \brief Put screen On.
*/
void setScreenOn(void);

/*! \fn  setScreenOff(void)
    \brief Put screen Off.
*/
void setScreenOff(void);

/*! \fn  setFadeEffect(u8 mode)
    \brief Do a fadein or fadeout effect.
    \param mode	(FADE_IN  = black to complete light, FADE_OUT = light to black)
*/
void setFadeEffect(u8 mode);

/*! \fn  setFadeEffectEx(u8 mode, u8 framesNumber)
    \brief Do a fadein or fadeout effect.
    \param mode	(FADE_IN  = black to complete light, FADE_OUT = light to black)
    \param framesNumber the number of frames
*/
void setFadeEffectEx(u8 mode, u8 framesNumber);

/*! \fn  setMosaicEffect(u8 mode, u8 bgNumbers)
    \brief Do a mosaic in or out effect.
    \param bgNumbers	(MOSAIC_BG1 to MOSAIC_BG4 depending of which background to use for effect)
    \param mode	(MOSAIC_IN  = normal to mosaic, MOSAIC_OUT = mosaic to normal)
*/
void setMosaicEffect(u8 mode, u8 bgNumbers);

/*! \fn  setColorEffect(u8 colorMathA, u8 colorMathB)
    \brief Do addition or other color effects.
    \param colorMathA	value for color math A register
    \param colorMathB	value for color math B register
*/
void setColorEffect(u8 colorMathA, u8 colorMathB);

/*! \fn  setColorIntensity(u8 colorApply, u8 intensity)
    \brief Change intensity for transparency
    \param colorApply	with component is affect (red, green, blue)
    \param intensity	value for intensity
*/
void setColorIntensity(u8 colorApply, u8 intensity);

/*! \fn  setPalette(palette, paletteEntry, paletteSize)
    \brief Change a palette in CGRAM.
    \param palette	address of palette
    \param paletteEntry palette entry (0..16 for 16 colors mode) of the beginning of each colors
    \param paletteSize	size of palette
*/
#define setPalette(palette, paletteEntry, paletteSize) dmaCopyCGram(palette, paletteEntry, paletteSize)

/*! \fn  setPaletteColor(paletteEntry, paletteColor)
    \brief Change a color palette in CGRAM.
    \param paletteEntry palette color number (0..255)
    \param paletteColor	RGB5 color
*/
#define setPaletteColor(paletteEntry, paletteColor) \
    REG_CGADD = paletteEntry;                       \
    *CGRAM_PALETTE = (paletteColor)&0xFF;           \
    *CGRAM_PALETTE = (paletteColor) >> 8;

/*! \fn  getPalette(u8 paletteEntry, u8 paletteSize, u16 *paletteColors)
    \brief Get a palette from CGRAM.
    \param paletteEntry 1st entry in palette (0..255 for 16 colors mode)
    \param paletteSize	size of palette to get
    \param paletteColors	RGB5 color to save all values
*/
void getPalette(u8 paletteEntry, u8 paletteSize, u16 *paletteColors);

/*! \fn  getPaletteColor(u8 paletteEntry, u16 *paletteColor)
    \brief Get a color palette from CGRAM.
    \param paletteEntry palette color number (0..255)
    \param paletteColor	RGB5 color to save value
*/
void getPaletteColor(u8 paletteEntry, u16 *paletteColor);

/*! \fn  setMode7(u8 mode)
    \brief Put screen in mode 7 with generic init.
    \param mode	Rotation/Scaling Mode Settings (see REG_M7SEL)
*/
void setMode7(u8 mode);

/*! \fn  setMode7Rot(u8 angle)
    \brief Change angle view in mode 7 with matrix transformation.
    \param angle : 0..255 value
*/
void setMode7Rot(u8 angle);

/*! \fn  setMode7MoveForwardBack(u8 z8)
    \brief Change perspective view forward/backward in mode 7 without changing matrix.
    \param z8 : 0..255 value
*/
void setMode7MoveForwardBack(u8 z8);

/*! \fn  setMode7MoveLeftRight(u8 z8)
    \brief Change perspective view left/right in mode 7 without changing matrix.
    \param z8 : 0..255 value
*/
void setMode7MoveLeftRight(u8 z8);

/*! \fn  setMode7Angle(u8 angle)
    \brief Change angle view in mode 7 without changing matrix.
    \param angle : 0..255 value
*/
void setMode7Angle(u8 angle);

/*! \fn  setMode7Scale(u16 xscale, u16 yscale)
    \brief Change scaling effect of screen in mode 7.
    \param xscale : scaling for x 
    \param yscale : scaling for y 
*/
void setMode7Scale(u16 xscale, u16 yscale);

#endif // SNES_VIDEO_INCLUDE
