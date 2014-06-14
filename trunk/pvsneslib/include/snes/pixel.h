/*---------------------------------------------------------------------------------

	mixel mode for mode 0

	Copyright (C) 2012-2013
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

/*!	\file pixel.h
	\brief contains the basic defnitions for doing a pixel per pixel mode for SNES.
	\brief map entrgy is 0x5800 and tiles entry is 0x6000 for 14K bytes !
	\brief so don't use graphics after 0x5800
	\brief this pixel mode uses SNES mode 0 so only 3 colors can be used

*/
#ifndef SNES_PIXEL_INCLUDE
#define SNES_PIXEL_INCLUDE

#include <snes/snestypes.h>

/*!	\fn pixSetPixel(u8 x,u8 y, u8 color)
	\brief Draw a dot 
	\param x x coordinate of dot
	\param y y coordinate of dot
	\param color color of pixel (must be 1, 2 or 3)
*/
void pixSetPixel(u8 x,u8 y, u8 color);

/*!	\fn pixSetMode(u8 *pixPalette)
	\brief Init pixel mode with SNES mode 0
	\param pixPalette address of palette entry
*/
void pixSetMode(u8 *pixPalette);

#endif
