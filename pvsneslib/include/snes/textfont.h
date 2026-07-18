/*---------------------------------------------------------------------------------

    Text and Variable width font text management

    Copyright (C) 2026
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

    VWF inspired by untoxa's GB VWF engine

---------------------------------------------------------------------------------*/

/*!	\file textfont.h
    \brief contains the basic definitions for controlling the variable width font display.

*/
#ifndef SNES_VWF_INCLUDE
#define SNES_VWF_INCLUDE

#include <snes/snestypes.h>

extern u8 scr_txt_font_map[0x800]; /*!< \brief tilemap used for text display */
extern u8 scr_txt_dirty;           /*!< \brief flag to redraw text during vblank */

/*! \fn consoleInitvwText(u16 bpp, u8* fnttileadr, u8 *fntwidthadr, u16 tileidx)
    \brief Initialize the Variable Width Text System
    \param fnttileadr   Pointer to tiles data font 
    \param fntwidthadr	Pointer tile width font

    Transfert the variable width table into an internal table and store pointer to tiles in rom. 
    Call once before any other vwText function.
    You can change address of graphics and map with vwfntSetTextMapPtr() and vwfntSetTextGfxPtr().
*/
void consoleInitvwText(u8* fnttileadr, u8 *fntwidthadr);

/*! \fn consoleSetvwTextLineAddr(u16 lineidx, u16 tilevram, u16 mapvram)
    \brief assign VRAM destinations for one render line
    \param lineidx      ID of the line (0..4)
    \param tilevram 	VRAM word-address for tile pixel data
*/
void consoleSetvwTextLineAddr(u8 lineidx, u16 tilevram);

/*! \fn consoleSetvwTextClearLine(u16 lineidx)
    \brief zero tile & map buffers for one line, reset metadata
    \param lineidx      ID of the line (0..4)
*/
void consoleSetvwTextClearLine(u8 lineidx);

/*! \fn consoleDrawvwText(u16 lineidx, u16 xcol, char *text)
    \brief render a null-terminated string into a line buffer
    \param lineidx      ID of the line (0..4)
    \param xcol         Starting pixel column (0–255)
    \param ycol         Starting line (0–27)
    \param text         Text to display    

    Characters outside ASCII 0x20–0x7F are silently skipped. 
    Stops at null terminator or when the line is full (32 tiles).
*/
void consoleDrawvwText(u8 lineidx, u16 xcol, u8 ycol, char *text);

/*! \fn consolevwTextUpdate(void)
    \brief DMA all dirty line buffers to VRAM

    Call during / just before or after VBlank.  Call dmaCopyVram().
*/
void consolevwTextUpdate(void);

/*! \fn void consoleSetvwTextMapAttr(u8 mapAttr)
    \brief Sets the high byte attr into tilemap entry
    \param mapAttr      Attribute (VHPC CC00)

    Sets the high byte (palette/priority/flip bits) into every tilemap 
    entry written by consoleDrawvwText(). Default is 0.
    VHPC CC00 is
        V Vectiale / H horizontal Flip
        P Priority bit of background
        C Palette selection (0-7)

*/
void consoleSetvwTextMapAttr(u8 mapAttr);

/*! \fn void consoleSetvwTextBlkTile(u16 tileEntry)
    \brief Sets the blank tile entry to reset a line
    \param tileEntry      ID of the tile

    Sets the full 16-bit tilemap word used to blank a line's unused
    columns when consoleSetvwTextClearLine() is called. Default is 0.
*/
void consoleSetvwTextBlkTile(u16 tileEntry);

/*! \fn consoleUpdate(void);
    \brief Update current text buffer on screen
*/
void consoleUpdate(void);

/*! \brief Initialize the Text System
    \param palnum	palette entry (0..7)
    \param palsize	size of palette of text (4,16,etc ...)
    \param tilfont	Pointer to tiles gfx for printing
    \param palfont	Pointer to palette of text

    Reserve a BG and load the font characters into VRAM. It will only work in tiled display modes.
    You can change address of graphics and map with consoleSetTextVramAdr() and consoleSetTextVramBGAdr().
    Also, consoleSetTextPal can change the text palette.
*/
void consoleInitText(u8 palnum, u8 palsize, u8 *tilfont, u8 *palfont);

/*! \fn consoleSetTextGfxPtr(u16 vramfont)
    \brief Change text graphics address (4K aligned)
    \param vramfont	VRAM Background address (4K aligned)  (default is $0800)
*/
void consoleSetTextGfxPtr(u16 vramfont);

/*! \fn consoleSetTextMapPtr(u16 vrambgfont)
    \brief Change text Background map address for display (must be BG address)
    \param vrambgfont	VRAM Background map address (default is $0800)
*/
void consoleSetTextMapPtr(u16 offsetfont);

/*! \fn consoleSetTextOffset(u16 offsetfont)
    \brief Change text Background map address for display (must be BG address)
    \param offsetfont	VRAM offset for text (default is 0, as it is aligned with BG tiles)
*/
void consoleSetTextOffset(u16 offsetfont);

/*! \fn consoleSetTextPal(u8 palnum, u8 *palfont, u8 palsize)
    \brief Change text font palette
    \param palnum	palette entry (0..7)
    \param palfont	pointer to the new palette
    \param palsize	size of palette of text (4,16,etc ...)
*/
void consoleSetTextPal(u8 palnum, u8 *palfont, u8 palsize);

/*! \brief Output formatted string on a screen (tiles mode)
    \param x 	The X character number the string should start on
    \param y 	The Y character number the string should start on
    \param fmt 	The Format string (see printf() documentation in your local C tutorial)

    Output a text string to the screen. Before using this, you need to call consoleInitText().
    You can also print the value of variables this way, by putting a format string and additional parameters.
*/
void consoleDrawText(u16 x, u16 y, char *fmt, ...);

/*! \brief Output formatted string on a screen (2 colors tiles mode)
    \param x 	The X character number the string should start on
    \param y 	The Y character number the string should start on
    \param map 	The 0x800 char map entry to put string
    \param attributes	attributes for tiles
    \param fmt 	The Format string (see printf() documentation in your local C tutorial)

    Output a text string to the screen. Before using this, you need to call consoleInitText().
    You can also print the value of variables this way, by putting a format string and additional parameters.
*/
void consoleDrawTextMap(u16 x, u16 y, u8 *map, u8 attributes, char *fmt, ...);

/*! \brief Output formatted string on a screen, centered it (2 colors tiles mode)
    \param y 	The Y character number the string should start on
    \param map 	The 0x800 char map entry to put string
    \param attributes	attributes for tiles
    \param fmt 	The Format string (see printf() documentation in your local C tutorial)

    Output a text string to the screen. Before using this, you need to call consoleInitText().
    You can also print the value of variables this way, by putting a format string and additional parameters.
    The text will be centered on x axis.
*/
void consoleDrawTextMapCenter(u16 y, u16 *map, u8 attributes, char *fmt, ...);

#endif // SNES_VWF_INCLUDE
