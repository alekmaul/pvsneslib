/*---------------------------------------------------------------------------------

    sprite.h -- definitions for SNES sprites

    Copyright (C) 2012-2025
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
/*! \file sprite.h
    \brief snes sprites functionality.
*/

#ifndef SNES_SPRITES_INCLUDE
#define SNES_SPRITES_INCLUDE

#include <snes/snestypes.h>

#define ATTR2_DISABLED (0xe8)

#define OBJ_SIZE8_L16 (0 << 5)  /*!< \brief default OAM size 8x8 (SM) and 16x16 (LG) pix for OBJSEL register */
#define OBJ_SIZE8_L32 (1 << 5)  /*!< \brief default OAM size 8x8 (SM) and 32x32 (LG) pix for OBJSEL register */
#define OBJ_SIZE8_L64 (2 << 5)  /*!< \brief default OAM size 8x8 (SM) and 64x64 (LG) pix for OBJSEL register */
#define OBJ_SIZE16_L32 (3 << 5) /*!< \brief default OAM size 16x16 (SM) and 32x32 (LG) pix for OBJSEL register */
#define OBJ_SIZE16_L64 (4 << 5) /*!< \brief default OAM size 16x16 (SM) and 64x64 (LG) pix for OBJSEL register */
#define OBJ_SIZE32_L64 (5 << 5) /*!< \brief default OAM size 32x32 (SM) and 64x64 (LG) pix for OBJSEL register */

#define OBJ_SMALL (0)
#define OBJ_LARGE (1)
#define OBJ_SHOW (0)
#define OBJ_HIDE (1)

#define OBJ_SPRITE32 1 /*!< \brief sprite with 32x32 identifier */
#define OBJ_SPRITE16 2 /*!< \brief sprite with 16x16 identifier */
#define OBJ_SPRITE8 4  /*!< \brief sprite with 8x8 identifier */

/*!	\struct t_sprites
    \brief Dynamic sprite definition (16 bytes)
*/
typedef struct
{
    s16 oamx;        /*!< \brief 0 x position on the screen  */
    s16 oamy;        /*!< \brief 2 y position on the screen  */
    u16 oamframeid;  /*!< \brief 4 frame index in graphic file of the sprite  */
    u8 oamattribute; /*!< \brief 6 sprite attribute value (vhoopppc v : vertical flip h: horizontal flip o: priority bits p: palette num c : last byte of tile num)  */
    u8 oamrefresh;   /*!< \brief 7 =1 if we need to load graphics from graphic file  */
    u8 *oamgraphics; /*!< \brief 8..11 pointer to graphic file  */
    u16 dummy1;      /*!< \brief 12..15 to be 16 aligned */
    u16 dummy2;
} t_sprites __attribute__((__packed__)); /*!< seems to do nothing */

/*!	\struct t_metasprites
    \brief Dynamic metasprite definition (16 bytes)
*/
typedef struct
{
    s16 metsprofsx;                          /*!< 0 x offset of the current sprite in meta sprite, 0xFFFF if it is the end of meta sprite definition  */
    s16 metsprofsy;                          /*!< 2 y offset of the current sprite in meta sprite  */
    u16 metsprframeid;                       /*!< 4 frame index in graphic file of the sprite  */
    u8 metsprattribute;                      /*!< 6 sprite attribute value (vhoopppc v : vertical flip h: horizontal flip o: priority bits p: palette num c : last byte of tile num)  */
    u8 metsprtype;                           /*!< 7 1,2 or 4 for 32x32, 16x16, 8x8 sprite type */
    u8 *metsprgraphics;                      /*!< 8..11 pointer to graphic file  */
    //u16 metsprend;                           /*!< 12..13 0xFFFF if it is the end of meta sprite definition */
    //u16 dummy1;                              /*!< 14..15 to be 16 aligned */
} t_metasprites __attribute__((__packed__)); /*!< seems to do nothing */

/*!  \brief Sprite Table (from no$sns help file)<br>
Contains data for 128 OBJs. OAM Size is 512+32 Bytes. The first part (512<br>
bytes) contains 128 4-byte entries for each OBJ:<br>
  Byte 0 - X-Coordinate (lower 8bit) (upper 1bit at end of OAM)<br>
  Byte 1 - Y-Coordinate (all 8bits)<br>
  Byte 2 - Tile Number  (lower 8bit) (upper 1bit within Attributes)<br>
  Byte 3 - Attributes<br>
Attributes:<br>
  Bit7    Y-Flip (0=Normal, 1=Mirror Vertically)<br>
  Bit6    X-Flip (0=Normal, 1=Mirror Horizontally)<br>
  Bit5-4  Priority relative to BG (0=Low..3=High)<br>
  Bit3-1  Palette Number (0-7) (OBJ Palette 4-7 can use Color Math via CGADSUB)<br>
  Bit0    Tile Number (upper 1bit)<br>
After above 512 bytes, additional 32 bytes follow, containing 2-bits per OBJ:<br>
  Bit7    OBJ 3 OBJ Size     (0=Small, 1=Large)<br>
  Bit6    OBJ 3 X-Coordinate (upper 1bit)<br>
  Bit5    OBJ 2 OBJ Size     (0=Small, 1=Large)<br>
  Bit4    OBJ 2 X-Coordinate (upper 1bit)<br>
  Bit3    OBJ 1 OBJ Size     (0=Small, 1=Large)<br>
  Bit2    OBJ 1 X-Coordinate (upper 1bit)<br>
  Bit1    OBJ 0 OBJ Size     (0=Small, 1=Large)<br>
  Bit0    OBJ 0 X-Coordinate (upper 1bit)<br>
And so on, next 31 bytes with bits for OBJ4..127. Note: The meaning of the OBJ<br>
Size bit (Small/Large) can be defined in OBSEL Register (Port 2101h).<br>
*/
extern t_sprites oambuffer[128]; /*!< \brief current sprite buffer for dynamic engine */

/**
 * \brief to address oam table low and high
 *
 * This buffer is automatically transferred to the PPU OAM by the \ref VBlank-ISR on non lag-frames.
 */
extern u8 oamMemory[128 * 4 + 8 * 4];

/*! \def REG_OBSEL
    \brief Object Size and Object Base (W)
    7-5   OBJ Size Selection  (0-5, see below) (6-7=Reserved)
         Val Small  Large
         0 = 8x8    16x16    ;Caution:
         1 = 8x8    32x32    ;In 224-lines mode, OBJs with 64-pixel height
         2 = 8x8    64x64    ;may wrap from lower to upper screen border.
         3 = 16x16  32x32    ;In 239-lines mode, the same problem applies
         4 = 16x16  64x64    ;also for OBJs with 32-pixel height.
         5 = 32x32  64x64
         6 = 16x32  32x64 (undocumented)
         7 = 16x32  32x32 (undocumented)
        (Ie. a setting of 0 means Small OBJs=8x8, Large OBJs=16x16 pixels)
        (Whether an OBJ is "small" or "large" is selected by a bit in OAM)
    4-3   Gap between OBJ 0FFh and 100h (0=None) (4K-word steps) (8K-byte steps)
    2-0   Base Address for OBJ Tiles 000h..0FFh  (8K-word steps) (16K-byte steps)
*/
#define REG_OBSEL (*(vuint8 *)0x2101)

/*! \def REG_OAMADDL
    \def REG_OAMADDH
    \brief OAM Address and Priority Rotation (W)
    15    OAM Priority Rotation  (0=OBJ num 0, 1=OBJ num N) (OBJ with highest priority)
    9-14  Not used
    7-1   OBJ Number num N (for OBJ Priority)   ;bit7-1 are used for two purposes
    8-0   OAM Address   (for OAM read/write)    ;

    This register contains of a 9bit Reload value and a 10bit Address register (plus the
    priority flag). Writing to 2102h or 2103h does change the lower 8bit or upper 1bit of
    the Reload value, and does additionally copy the (whole) 9bit Reload value to the 10bit
    Address register (with address Bit0=0 so next access will be an even address).
    Caution: During rendering, the PPU is destroying the Address register (using it internally
    for whatever purposes), after rendering (at begin of Vblank, ie. at begin of line 225/240,
    but only if not in Forced Blank mode) it reinitializes the Address from the Reload value;
    the same reload occurs also when deactivating forced blank anytime during the first scanline
    of vblank (ie. during line 225/240).
*/
#define REG_OAMADDL (*(vuint8 *)0x2102)
#define REG_OAMADDH (*(vuint8 *)0x2103)

/*
  1st Access: Lower 8bit (even address)
  2nd Access: Upper 8bit (odd address)
*/
#define REG_OAMDATA (*(vuint8 *)0x2104) /*!< \brief OAM Data Write (W) */
#define REG_RDOAM (*(vuint8 *)0x2138)   /*!< \brief OAM Data Read (R) */

/*! \brief Initializes the 2D sprite engine
 */
void oamInit(void);

/*! \brief Write all OBJ descriptors to OAM
 */
void oamUpdate(void);

/*! \brief sets an oam entry to the supplied values
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
    \param xf x flipping [0 - 1]
    \param yf y flipping [0 - 1]
*/
void oamFlip(u16 id, u8 xf, u8 yf);

/*! \brief sets an oam entry to the supplied values
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
    \param xspr the x location of the sprite in pixels
    \param yspr the y location of the sprite in pixels
    \param gfxoffset tilenumber graphic offset
    \param attr (attributes with priority, flipping, palette)
*/
void oamSetAttr(u16 id, u16 xspr, u16 yspr, u16 gfxoffset, u8 attr);

/*! \brief defined attribute of a sprite
    \param priority The sprite priority (0 to 3)
    \param vflip flip the sprite vertically
    \param hflip flip the sprite horizontally
    \param gfxoffset tilenumber graphic offset
    \param paletteoffset palette default offset for sprite
*/
#define OAM_ATTR(priority, hflip, vflip, gfxoffset, paletteoffset) ((vflip << 7) | (hflip << 6) | (priority << 4) | (paletteoffset << 1) | ((gfxoffset >> 8) & 1))

/*! \brief sets an oam entry to the supplied values
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
    \param xspr the x location of the sprite in pixels
    \param yspr the y location of the sprite in pixels
    \param priority The sprite priority (0 to 3)
    \param vflip flip the sprite vertically
    \param hflip flip the sprite horizontally
    \param gfxoffset tilenumber graphic offset
    \param paletteoffset palette default offset for sprite
*/
void oamSet(u16 id, u16 xspr, u16 yspr, u8 priority, u8 hflip, u8 vflip, u16 gfxoffset, u8 paletteoffset);

/*! \brief sets an oam coordinate to the supplied values
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
    \param xspr the x location of the sprite in pixels
    \param yspr the y location of the sprite in pixels
*/
void oamSetXY(u16 id, u16 xspr, u16 yspr);

/*! \brief get the x oam coordinate to the supplied values
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
*/
#define oamGetX(id) (oamMemory[id + 0])

/*! \brief get the y oam coordinate to the supplied values
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
*/
#define oamGetY(id) (oamMemory[id + 1])

/*! \brief sets an oam graphic offset to the supplied values
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
    \param gfxoffset tilenumber graphic offset
*/
void oamSetGfxOffset(u16 id, u16 gfxoffset);

/*! \brief Put the correct size and hide or show a sprite
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
    \param size	normal or large (OBJ_SMALL or OBJ_LARGE)
    \param hide	1 (OBJ_HIDE) to to hide the sprite, 0 (OBJ_SHOW) to see it
*/
void oamSetEx(u16 id, u8 size, u8 hide);

/** \brief Hide or show a sprite
    \param id the oam number to be set [0 - 127] * 4 because of oam structure
    \param hide	1 (OBJ_HIDE) to to hide the sprite, 0 (OBJ_SHOW) to see it
    \warning When OBJ_HIDE is used, the sprite is set at these coordinates: x = 255, y 240. Therefore when a sprite is hidden and it is needed to show it again (use OBJ_SHOW), it is necessary to reset the sprite coordinates with oamSetXY().
    If the sprite needs to be hidden and keep its coordinates, please use oamSetGfxOffset instead by passing a transparent tile.
*/
void oamSetVisible(u16 id, u8 hide);

/*! \brief Hides the sprites in the supplied range: if count is zero all 128 sprites will be hidden
    \param first	number of 1st sprite to write * 4 because of oam structure
    \param numEntries	total number of sprites to write
*/
void oamClear(u16 first, u8 numEntries);

/*!\brief Initializes a sprites Gfx and Loads the GFX into VRAM
    \param tileSource	address of sprites graphics entry
    \param tileSize	size of sprites graphics
    \param tilePalette	address of sprites palette entry
    \param paletteSize	size of palette
    \param tilePaletteNumber	palette number (0..8)
    \param address	address of sprite graphics (8K-word steps)
    \param oamsize	default OAM size (OBJ_SIZE8_L16, OBJ_SIZE8_L32, OBJ_SIZE8_L64, OBJ_SIZE16_L32, OBJ_SIZE16_L64 and OBJ_SIZE32_L64)
*/
void oamInitGfxSet(u8 *tileSource, u16 tileSize, u8 *tilePalette, u16 paletteSize, u8 tilePaletteNumber, u16 address, u8 oamsize);

/*!\brief Initializes the default sprite size and address in VRAM
    \param address	address of sprite graphics (8K-word steps)
    \param oamsize	default OAM size (OBJ_SIZE8_L16, OBJ_SIZE8_L32, OBJ_SIZE8_L64, OBJ_SIZE16_L32, OBJ_SIZE16_L64 and OBJ_SIZE32_L64)
*/
void oamInitGfxAttr(u16 address, u8 oamsize);

/*!\brief initialize the dynamic sprite engine with each sprite size entries
    \param gfxsp0adr	address of large sprite graphics entry
    \param gfxsp1adr	address of small sprite graphics entry
    \param oamsp0init	address of large sprite number (useful when we have some hud sprites which are not update each frame)
    \param oamsp1init	address of small sprite number (useful when we have some hud sprites which are not update each frame)
    \param oamsize	default OAM size : OBJ_SIZE8_L16, OBJ_SIZE8_L32, OBJ_SIZE16_L32 (64pix size is not supported)
*/
void oamInitDynamicSprite(u16 gfxsp0adr, u16 gfxsp1adr, u16 oamsp0init, u16 oamsp1init, u8 oamsize);

/*!\brief Init Dynamic sprite engine on a screen (can be useful if you do not want to refresh sprite each frame).
 */
void oamInitDynamicSpriteScreen(void);

/*!\brief Must be call at the end of the frame, initialize the dynamic sprite engine for the next frame.
 */
void oamInitDynamicSpriteEndFrame(void);

/*!\brief Update VRAM graphics for sprites 32x32, 16x16 and 8x8 (can but call in  Vblank if needed).
 */
void oamVramQueueUpdate(void);

/*!\brief Add a 32x32 sprite on screen.<br>oambuffer[id] needs to be populate before.
    \param id the oam number to be used [0 - 127]
*/
void oamDynamic32Draw(u16 id);

/*!\brief Add a 32x32 sprite on screen.<br>oambuffer[id] needs to be populate before.
    !\brief    Sprite is never refresh (VRAM must be update before)
    !\brief    oamframeid is the VRAM entry number for sprite (eg.4 for VRAM offset 0x0040, 15 for 0x0CC...)
    !\brief    WARNING: as the sprite engine begin at VRAM address 0x0000, you need to take care of High bit 
    !\brief      if you want to use a VRAM entry number more than 0x00FF
    \param id the oam number to be used [0 - 127]
*/
void oamFix32Draw(u16 id);

/*!\brief Add a 16x16 sprite on screen.<br>oambuffer[id] needs to be populate before.
    \param id the oam number to be used  [0 - 127]
*/
void oamDynamic16Draw(u16 id);

/*!\brief Add a 16x16 sprite on screen.<br>oambuffer[id] needs to be populate before.
    !\brief    Sprite is never refresh (VRAM must be update before)
    !\brief    oamframeid is the VRAM entry number for sprite (eg.4 for VRAM offset 0x0040, 15 for 0x0CC...)
    !\brief    WARNING: as the sprite engine begin at VRAM address 0x0000, you need to take care of High bit 
    !\brief      if you want to use a VRAM entry number more than 0x00FF
    \param id the oam number to be used  [0 - 127]
*/
void oamFix16Draw(u16 id);

/*!\brief Add a 8x8 sprite on screen.<br>oambuffer[id] needs to be populate before.
    \param id the oam number to be used [0 - 127]
*/
void oamDynamic8Draw(u16 id);

/*!\brief Add a 8x8 sprite on screen.<br>oambuffer[id] needs to be populate before.
    !\brief    Sprite is never refresh (VRAM must be update before)
    !\brief    oamframeid is the VRAM entry number for sprite (eg.4 for VRAM offset 0x0040, 15 for 0x0CC...)
    !\brief    WARNING: as the sprite engine begin at VRAM address 0x0000, you need to take care of High bit 
    !\brief      if you want to use a VRAM entry number more than 0x00FF
    \param id the oam number to be used [0 - 127]
*/
void oamFix8Draw(u16 id);

/*!\brief Add a Meta sprite on screen (can be composed of 8x8,16x16 or 32x32 sprites).<br>oambuffer[id] needs to be populate before.
    \param id the oam number to be used [0 - 127].
    \param x x coordinate of the metasprite
    \param y y coordinate of the metasprite
    \param sprmeta	pointer to metasprite structure (must finish with 0xFFFF, see t_metasprites)
    \brief the meta structure is composed of:<br>
        x offset,y offset,gfx offset, attribute,  size (8,16,32),gfxfile<br>
        the function will reserve the number of sprites in oambuffer, beginning with id.
*/
void oamDynamicMetaDraw(u16 id, s16 x, s16 y, u8 *sprmeta);

#endif // SNES_SPRITES_INCLUDE
