/*---------------------------------------------------------------------------------

	Generic console functions.

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

/*! \file console.h
    \brief snes generic console support.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <snes/snestypes.h>

#include <snes/background.h>
#include <snes/dma.h>
#include <snes/interrupts.h>
#include <snes/pads.h>
#include <snes/sprites.h>
#include <snes/sound.h>
#include <snes/video.h>

extern volatile unsigned int  snes_vblank_count; /*!< \brief Number of VBL since consoleInit called */
extern unsigned short snes_50hz; /*!< \brief 1 if on a PAL/50Hz SNES */

/*! \fn rand(void)
	\brief return a randomized number
*/
unsigned int rand(void);

/*! \fn consoleSetTextCol(u16 colorChar, u16 colorBG)
	\brief Change Character and Background color of text
	\param colorChar	The BG color palette with RGB5 values of text itself
	\param colorBG	The BG color palette with RGB5 values of text background
*/
void consoleSetTextCol(u16  colorChar, u16 colorBG);

/*! \brief Output formatted string on a screen (tiles mode)
	\param x 	The X character number the string should start on
	\param y 	The Y character number the string should start on
	\param fmt 	The Format string (see printf() documentation in your local C tutorial)

	Output a text string to the screen. Before using this, you need to call consoleInitText().
	You can also print the value of variables this way, by putting a format string and additional parameters.
*/
void consoleDrawText(u16 x, u16 y, char *fmt, ...);

/*! \brief Output formatted string on a screen (tiles mode)
	\param x 	The X character number the string should start on
	\param y 	The Y character number the string should start on
	\param map 	The 0x800 char map entry to put string
	\param attributes	attributes for tiles
	\param fmt 	The Format string (see printf() documentation in your local C tutorial)

	Output a text string to the screen. Before using this, you need to call consoleInitText().
	You can also print the value of variables this way, by putting a format string and additional parameters.
*/
void consoleDrawTextMap(u8 x, u8 y, unsigned char *map, char attributes, char *fmt, ...);

/*! \brief Initialize the Text System
	\param bgNumber	The BG number to show Text on.
	\param paletteNumber	Palette number (0..16).
	\param gfxText	Pointer to tiles gfx for printing

	Reserve a BG and load the font characters into VRAM. It will only work in tiled display modes. 
	Also, please note that this function will overwrite BG palette number 0 and 1 to black and white, 
	and set the Font to use these two colors. You can change colors by calling consoleSetTextCol().
*/
void consoleInitText(u8 bgNumber,u8 paletteNumber, unsigned char *gfxText);

/*! \fn  consoleInit()
	\brief Initialize console

	Generic init for SNES console.

*/
void consoleInit(void);

#endif
