/*---------------------------------------------------------------------------------

	background.h -- definitions for SNES backgrounds

	Copyright (C) 2012
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
/*! \file background.h
    \brief snes background defines and functionality.


*/
#ifndef SNES_BACKGROUND_INCLUDE
#define SNES_BACKGROUND_INCLUDE

#include <snes/snestypes.h>

#include <snes/dma.h>
#include <snes/interrupts.h>
#include <snes/video.h>

/*! \brief The shift to apply to map base when storing it in a tile map location register */
#define SC_BASE_SHIFT 2

/*! \brief Macro to set the tile map adress in tile map location */
#define SC_TILE_BASE(base)  ((base) << SC_BASE_SHIFT)

/*!	\brief Bit defines for the background control registers */
typedef enum
{
   SC_32x32    =  (0 << 0), /*!< \brief 32 x 32 tile size */
   SC_64x32    =  (1 << 0), /*!< \brief 64 x 32 tile size */
   SC_32x64    =  (2 << 0), /*!< \brief 32 x 64 tile size */
   SC_64x64    =  (3 << 0), /*!< \brief 64 x 64 tile size */

   BG_MODE0 = (0 << 0), /*!< \brief 4-color     4-color     4-color     4-color   ;Normal */
   BG_MODE1 = (1 << 0), /*!< \brief 16-color    16-color    4-color     -         ;Normal */
   BG_MODE2 = (2 << 0), /*!< \brief 16-color    16-color    (o.p.t)     -         ;Offset-per-tile */
   BG_MODE3 = (3 << 0), /*!< \brief 256-color   16-color    -           -         ;Normal */
   BG_MODE4 = (4 << 0), /*!< \brief 256-color   4-color     (o.p.t)     -         ;Offset-per-tile */
   BG_MODE5 = (5 << 0), /*!< \brief 16-color    4-color     -           -         ;512-pix-hires */
   BG_MODE6 = (6 << 0), /*!< \brief 16-color    -           (o.p.t)     -         ;512-pix plus Offs-p-t */
   BG_MODE7 = (7 << 0), /*!< \brief 256-color   EXTBG       -           -         ;Rotation/Scaling */

   BG1_TSIZE8x8 = (0 << 4),
   BG2_TSIZE8x8 = (0 << 5),
   BG3_TSIZE8x8 = (0 << 6),
   BG4_TSIZE8x8 = (0 << 7),

   BG1_TSIZE16x16 = (1 << 4),
   BG2_TSIZE16x16 = (1 << 5),
   BG3_TSIZE16x16 = (1 << 6),
   BG4_TSIZE16x16 = (1 << 7),
   
   BG1_ENABLE = (1 << 0),
   BG2_ENABLE = (1 << 1),
   BG3_ENABLE = (1 << 2),
   BG4_ENABLE = (1 << 3),
   OBJ_ENABLE = (1 << 4),
}BackgroundControl;

#define    BG_4COLORS 4
#define    BG_16COLORS 16
#define    BG_256COLORS 256
#define    BG3_MODE1_PRORITY_HIGH (1<<3)

/*! \def REG_BGMODE
    \brief BG Mode and BG Character Size (W)
	7    BG4 Tile Size (0=8x8, 1=16x16)  ;\(BgMode0..4: variable 8x8 or 16x16)
	6    BG3 Tile Size (0=8x8, 1=16x16)  ; (BgMode5: 8x8 acts as 16x8)
	5    BG2 Tile Size (0=8x8, 1=16x16)  ; (BgMode6: fixed 16x8?)
	4    BG1 Tile Size (0=8x8, 1=16x16)  ;/(BgMode7: fixed 8x8)
	3    BG3 Priority in Mode 1 (0=Normal, 1=High)
	2-0  BG Screen Mode (0..7 = see below)

	The BG Screen Modes are:

	Mode   BG1         BG2         BG3         BG4
	0      4-color     4-color     4-color     4-color   ;Normal
	1      16-color    16-color    4-color     -         ;Normal
	2      16-color    16-color    (o.p.t)     -         ;Offset-per-tile
	3      256-color   16-color    -           -         ;Normal
	4      256-color   4-color     (o.p.t)     -         ;Offset-per-tile
	5      16-color    4-color     -           -         ;512-pix-hires
	6      16-color    -           (o.p.t)     -         ;512-pix plus Offs-p-t
	7      256-color   EXTBG       -           -         ;Rotation/Scaling

	Mode 7 supports rotation/scaling and extbg (but doesn't support hv-flip).
	Mode 5/6 don't support screen addition/subtraction.
	CG Direct Select is support on BG1 of Mode 3/4, and on BG1/BG2? of Mode 7.
*/
#define REG_BGMODE	(*(vuint8*)0x2105)

/*! \def REG_MOSAIC
    \brief Mosaic Size and Mosaic Enable (W)
	Allows to divide the BG layer into NxN pixel blocks, in each block, 
	the hardware picks the upper-left pixel of each block, and fills the 
	whole block by the color - thus effectively reducing the screen resolution.

	7-4  Mosaic Size        (0=Smallest/1x1, 0Fh=Largest/16x16)
	3    BG4 Mosaic Enable  (0=Off, 1=On)
	2    BG3 Mosaic Enable  (0=Off, 1=On)
	1    BG2 Mosaic Enable  (0=Off, 1=On)
	0    BG1 Mosaic Enable  (0=Off, 1=On)

	Horizontally, the first block is always located on the left edge of the 
	TV screen. Vertically, the first block is located on the top of the TV screen.
	When changing the mosaic size mid-frame, the hardware does first finish current 
	block (using the old vertical size) before applying the new vertical size. 
	Technically, vertical mosaic is implemented as so: subtract the veritical 
	index (within the current block) from the vertical scroll register (BGnVOFS).
*/
#define REG_MOSAIC	(*(vuint8*)0x2106)

/*! \def REG_BG1SC
	\def REG_BG2SC	(*(vuint8*)0x2108)
	\def REG_BG3SC	(*(vuint8*)0x2109)
	\def REG_BG4SC	(*(vuint8*)0x210A)
    \brief BGx Screen Base and Screen Size (W)
	2107h - BG1SC - BG1 Screen Base and Screen Size (W)
	2108h - BG2SC - BG2 Screen Base and Screen Size (W)
	2109h - BG3SC - BG3 Screen Base and Screen Size (W)
	210Ah - BG4SC - BG4 Screen Base and Screen Size (W)

	7-2  SC Base Address in VRAM (in 1K-word steps, aka 2K-byte steps)
	1-0  SC Size (0=One-Screen, 1=V-Mirror, 2=H-Mirror, 3=Four-Screen)
					(0=32x32, 1=64x32, 2=32x64, 3=64x64 tiles)
				(0: SC0 SC0    1: SC0 SC1  2: SC0 SC0  3: SC0 SC1   )
				(   SC0 SC0       SC0 SC1     SC1 SC1     SC2 SC3   )
			   
*/
#define BG1SC_ADDR	0x2107
#define REG_BG1SC	(*(vuint8*)0x2107)
#define REG_BG2SC	(*(vuint8*)0x2108)
#define REG_BG3SC	(*(vuint8*)0x2109)
#define REG_BG4SC	(*(vuint8*)0x210A)

/*! \def REG_BG12NBA
    \brief BG Character Data Area Designation (W)
	15-12 BG4 Tile Base Address (in 4K-word steps)
	11-8  BG3 Tile Base Address (in 4K-word steps)
	7-4   BG2 Tile Base Address (in 4K-word steps)
	3-0   BG1 Tile Base Address (in 4K-word steps)

Ignored in Mode 7 (Base is always zero)
*/
#define REG_BG12NBA	(*(vuint8*)0x210B)
#define REG_BG34NBA	(*(vuint8*)0x210C)

/*! \brief BGx Horizontal Scroll (X) (W) and M7HOFS*/
#define	REG_BGxHOFS		0x210D
/*! \brief BGx Vertical Scroll (Y) (W) and M7VOFS*/
#define	REG_BGxVOFS		0x210E

/*! \brief BG1 Horizontal Scroll (X) (W) and M7HOFS*/
#define	REG_BG1HOFS 		(*(vuint8*)0x210D)
/*! \brief BG1 Vertical Scroll (Y) (W) and M7VOFS*/
#define	REG_BG1VOFS  		(*(vuint8*)0x210E)
/*! \brief BG2 Horizontal Scroll (X) (W)*/
#define	REG_BG2HOFS  		(*(vuint8*)0x210F)
/*! \brief BG2 Vertical Scroll (Y) (W)*/
#define	REG_BG2VOFS  		(*(vuint8*)0x2110)
/*! \brief BG3 Horizontal Scroll (X) (W)*/
#define	REG_BG3HOFS  		(*(vuint8*)0x2111)
/*! \brief BG3 Vertical Scroll (Y) (W)*/
#define	REG_BG3VOFS  		(*(vuint8*)0x2112)
/*! \brief BG4 Horizontal Scroll (X) (W)*/
#define	REG_BG4HOFS   		(*(vuint8*)0x2113)
/*! \brief BG4 Vertical Scroll (Y) (W)*/
#define	REG_BG4VOFS   		(*(vuint8*)0x2114)

/*! \def REG_TM 
	\def REG_TS
    \brief  TM - Main Screen Designation (W)
			TS - Sub Screen Designation (W)

  7-5  Not used
  4    OBJ (0=Disable, 1=Enable)
  3    BG4 (0=Disable, 1=Enable)
  2    BG3 (0=Disable, 1=Enable)
  1    BG2 (0=Disable, 1=Enable)
  0    BG1 (0=Disable, 1=Enable)
  -    Backdrop (Always enabled)

	Allows to enable/disable video layers. The Main screen is the "normal" display. 
	The Sub screen is used only for Color Math and for 512-pixel Hires Mode.
*/
#define REG_TM	(*(vuint8*)0x212C)
#define REG_TS	(*(vuint8*)0x212D)


#define COLWIN_OB13W1ENABLEIN	(1<<0) /*!< \brief BG1   BG3   OBJ   Window-1 Area Inside */
#define COLWIN_OB13W1ENABLEOUT	(2<<0) /*!< \brief BG1   BG3   OBJ   Window-1 Area Outside */
#define COLWIN_OB13W2ENABLEIN	(1<<2) /*!< \brief BG1   BG3   OBJ   Window-2 Area Inside */
#define COLWIN_OB13W2ENABLEOUT	(2<<2) /*!< \brief BG1   BG3   OBJ   Window-2 Area Outside */
#define COLWIN_MB24W1ENABLEIN	(1<<4) /*!< \brief BG2   BG4   MATH  Window-1 Area Inside */
#define COLWIN_MB24W1ENABLEOUT	(2<<4) /*!< \brief BG2   BG4   MATH  Window-1 Area Outside */
#define COLWIN_MB24W2ENABLEIN	(1<<6) /*!< \brief BG2   BG4   MATH  Window-2 Area Inside */
#define COLWIN_MB24W2ENABLEOUT	(2<<6) /*!< \brief BG2   BG4   MATH  Window-2 Area Outside */

/*! \def REG_W12SEL
    \brief Window BG1/BG2 Mask Settings (W)
	2123h - W12SEL - Window BG1/BG2 Mask Settings (W)
	2124h - W34SEL - Window BG3/BG4 Mask Settings (W)
	2125h - WOBJSEL - Window OBJ/MATH Mask Settings (W)
	Bit  2123h 2124h 2125h
	7-6  BG2   BG4   MATH  Window-2 Area (0..1=Disable, 1=Inside, 2=Outside)
	5-4  BG2   BG4   MATH  Window-1 Area (0..1=Disable, 1=Inside, 2=Outside)
	3-2  BG1   BG3   OBJ   Window-2 Area (0..1=Disable, 1=Inside, 2=Outside)
	1-0  BG1   BG3   OBJ   Window-1 Area (0..1=Disable, 1=Inside, 2=Outside)
*/
#define REG_W12SEL		(*(vuint8*)0x2123)
#define REG_W34SEL		(*(vuint8*)0x2124)
#define REG_WOBJSEL		(*(vuint8*)0x2125)

/*! \def REG_WH0
    \brief Window 1 Left Position (X1) (W)
	2126h - WH0 - Window 1 Left Position (X1) (W)
	2127h - WH1 - Window 1 Right Position (X2) (W)
	2128h - WH2 - Window 2 Left Position (X1) (W)
	2129h - WH3 - Window 2 Right Position (X2) (W)
	7-0   Window Position (00h..0FFh; 0=leftmost, 255=rightmost)

	Specifies the horizontal boundaries of the windows. Note that there 
	are no vertical boundaries (these could be implemented by manipulating the window registers 
	via IRQ and/or HDMA).
	The "inside-window" region extends from X1 to X2 (that, including the X1 and X2 coordinates), so 
	the window width is X2-X1+1. If the width is zero (or negative), then the "inside-window" becomes 
	empty, and the whole screen will be treated "outside-window".
*/
#define REG_WH0		(*(vuint8*)0x2126)
#define REG_WH1		(*(vuint8*)0x2127)
#define REG_WH2		(*(vuint8*)0x2128)
#define REG_WH3		(*(vuint8*)0x2129)

/*! \def REG_WBGLOG
    \brief Window 1/2 Mask Logic (W)
	212Ah/212Bh - WBGLOG/WOBJLOG - Window 1/2 Mask Logic (W)
	Bit  212Ah 212Bh
	7-6  BG4   -     Window 1/2 Mask Logic (0=OR, 1=AND, 2=XOR, 3=XNOR)
	5-4  BG3   -     Window 1/2 Mask Logic (0=OR, 1=AND, 2=XOR, 3=XNOR)
	3-2  BG2   MATH  Window 1/2 Mask Logic (0=OR, 1=AND, 2=XOR, 3=XNOR)
	1-0  BG1   OBJ   Window 1/2 Mask Logic (0=OR, 1=AND, 2=XOR, 3=XNOR)
	Allows to merge the Window 1 and 2 areas into a single "final" window area 
	(which is then used by TMW, TSW, and CGWSEL). The OR/AND/XOR/XNOR logic is 
	applied ONLY if BOTH window 1 and 2 are enabled (in WxxSEL registers). 
	If only one window is enabled, then that window is used as is as "final" 
	area. If both are disabled, then the "final" area will be empty. Note: 
	"XNOR" means "1 XOR area1 XOR area2" (ie. the inverse of the normal XOR result).
*/
#define REG_WBGLOG		(*(vuint8*)0x212A)
#define REG_WOBJLOG		(*(vuint8*)0x212B)

/*! \def REG_TMW
    \brief Window Area Main Screen Disable (W)
	212Eh - TMW - Window Area Main Screen Disable (W)
	212Fh - TSW - Window Area Sub Screen Disable (W)
	7-5  Not used
	4    OBJ (0=Enable, 1=Disable)  ;\"Disable" forcefully disables the layer
	3    BG4 (0=Enable, 1=Disable)  ; within the window area (otherwise it is
	2    BG3 (0=Enable, 1=Disable)  ; enabled or disabled as selected in the
	1    BG2 (0=Enable, 1=Disable)  ; master enable bits in port 212Ch/212Dh)
	0    BG1 (0=Enable, 1=Disable)  ;/
	-    Backdrop (Always enabled)
	Allows to disable video layers within the window region.
*/
#define REG_TMW		(*(vuint8*)0x212E)
#define REG_TSW		(*(vuint8*)0x212F)

//background state
typedef struct BgState {
	unsigned char type;
	unsigned char active;
	unsigned short gfxaddr;
	unsigned short mapaddr;
} BgState;

extern BgState bgState[4]; /*!\brief Definition of each background */

/*!\brief Change Background Char address
	\param bgNumber	background number (0 to 3)
	\param address	address of tile graphics (4K aligned)
*/
void bgSetGfxPtr(u8 bgNumber, u16 address);

/*!\brief Change Background Map address
	\param bgNumber	background number (0 to 3)
	\param address	address of tile map
	\param mapSize-> map size (0=32x32, 1=64x32, 2=32x64, 3=64x64 tiles)
*/
void bgSetMapPtr(u8 bgNumber, u16 address, u8 mapSize);

/*!\brief Initializes a Tile Set and Loads the Tile GFX into VRAM 
	\param bgNumber	background number (0 to 3)
	\param tileSource	address of tile graphics entry
	\param tilePalette	address of palette entry
	\param paletteEntry palette number (0..16 for 16 colors mode) of the begining of each colors
	\param tileSize	size of tile graphics
	\param paletteSize	size of palette
	\param colorMode	used for correct palette entry (BG_4COLORS, BG_16COLORS, BG_256COLORS)
	\param address	address of tile graphics (4K aligned)
*/
//void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 colorMode, u16 address);
void bgInitTileSet(u8 bgNumber, u8 *tileSource, u8 *tilePalette, u8 paletteEntry, u16 tileSize, u16 paletteSize, u16 colorMode, u16 address);

/*!\brief Add some Tile Set and Loads the Tile GFX into VRAM 
	\param bgNumber	background number (0 to 3, 0xff to not change gfx pointer)
	\param tileSource	address of tile graphics entry
	\param tileSize	size of tile graphics
	\param address	address of tile graphics (4K aligned)
*/
void bgInitTileSetData(u8 bgNumber, u8 *tileSource, u16 tileSize, u16 address);

/*!\brief Initializes a Tile & Map Set and Loads the Tile & Map GFX into VRAM for Mode 7
	\param tileSource	address of tile graphics entry
	\param mapSource	address of map entry
	\param tilePalette	address of palette entry
	\param tileSize	size of tile graphics
	\param address	address of tile graphics (4K aligned)
*/
void bgInitMapTileSet7(u8 *tileSource,  u8 *mapSource, u8 *tilePalette, u16 tileSize, u16 address);

/*!\brief Initializes a Map Set and loads it into SNES VRAM  
	\param bgNumber	background number (0 to 3, 0xff to not change map pointer)
	\param mapSource	address of map entry
	\param mapSize	size of map
	\param sizeMode	used for correct tile mapping (SC_32x32, SC_64x32, SC_32x64, SC_64x64)
	\param address	address of map
*/
void bgInitMapSet(u8 bgNumber, u8 *mapSource, u16 mapSize, u16 sizeMode, u16 address);

/*!	\brief Sets the scroll hardware to the specified location
	\param bgNumber	background number (0 to 3)
	\param x	the horizontal scroll
	\param y	the vertical scroll
*/
void bgSetScroll(u8 bgNumber, u16 x, u16 y);

/*!	\brief Enable a BG in the actual SNES mode
	\param bgNumber	background number (0 to 3 regarding current mode)
*/
void bgSetEnable(u8 bgNumber);

/*!	\brief Disable a BG in the actual SNES mode
	\param bgNumber	background number (0 to 3 regarding current mode)
*/
void bgSetDisable(u8 bgNumber);

/*!	\brief Enable a BG from sub screen (translucent) in the actual SNES mode
	\param bgNumber	background number (0 to 3 regarding current mode)
*/
void bgSetEnableSub(u8 bgNumber);

/*!	\brief Disable a BG from sub screen (translucent) in the actual SNES mode
	\param bgNumber	background number (0 to 3 regarding current mode)
*/
void bgSetDisableSub(u8 bgNumber);

/*!	\brief Define a region on screen where map / sprite can be disable
	\param bgNumber	background number (0 to 3 regarding current mode)
	\param winNumber	windows number (0 to 1 regarding current window)
	\param xLeft	left value where we begin to add map/sprites
	\param xRight	right value where we stop to add map/sprites
*/
void bgSetWindowsRegions(u8 bgNumber, u8 winNumber, u8 xLeft, u8 xRight);

#endif //SNES_BACKGROUND_INCLUDE
