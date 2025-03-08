/*---------------------------------------------------------------------------------

    Generic console functions.

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

/*! \file console.h
    \brief snes generic console support.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <snes/snestypes.h>

#include <snes/background.h>
#include <snes/dma.h>
#include <snes/input.h>
#include <snes/interrupt.h>
#include <snes/sprite.h>
#include <snes/sound.h>
#include <snes/video.h>

extern u8 scr_txt_font_map[0x800]; /*!< \brief tilemap used for text display */
extern u8 scr_txt_dirty;           /*!< \brief flag to redraw text during vblank */

extern u16 snes_vblank_count; /*!< \brief Number of VBL since consoleInit called (16 bits longs so reset each 18 minutes in NTSC)*/
extern u8 snes_50hz;          /*!< \brief 1 if on a PAL/50Hz SNES */
extern u8 snes_fps;           /*!< \brief 50 if PAL console (50 Hz) or 60 if NTSC console (60Hz) */

/*! \fn rand(void)
    \brief return a randomized number
    \return unsigned short of a randozied number
*/
u16 rand(void);

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

/*! \fn  consoleInit()
    \brief Initialize console

    Generic init for SNES console.

*/
void consoleInit(void);

/*! \brief Send a message to the no$sns debug window
    \param fmt 	The Format string (see printf() documentation in your local C tutorial)
*/
void consoleNocashMessage(char *fmt, ...);

/*! \brief Execute the special WDM opcode which Mesen can break on
 */
void consoleMesenBreakpoint();

/*! \brief Send data to SRAM
    \param source the source to copy from
    \param size the size in bytes of the data to copy.
*/
void consoleCopySram(u8 *source, u16 size);

/*! \brief Load data from SRAM
    \param dest the destination to load into
    \param size the size in bytes of the data to copy.
*/
void consoleLoadSram(u8 *dest, u16 size);

/*! \brief Send data to SRAM with offset
    \param source the source to copy from
    \param size the size in bytes of the data to copy.
    \param offset the offset in bytes where the data should be copied
*/
void consoleCopySramWithOffset(u8 *source, u16 size, u16 offset);

/*! \brief Load data from SRAM with offset
    \param dest the destination to load into
    \param size the size in bytes of the data to copy.
    \param offset the offset from which the data should be loaded
*/
void consoleLoadSramWithOffset(u8 *dest, u16 size, u16 offset);

/*! \fn  consoleRegionIsOK()
    \brief Check compatibility between console (NTSC/PAL) and cartridge (country)
    \return 1 if cartridge and console are from the same region, 2 if not

    Check if console (PAL / NSTC) is the same as cartrdige region code
*/
u16 consoleRegionIsOK(void);

int sprintf(char *buf, const char *fmt, ...);

#endif
