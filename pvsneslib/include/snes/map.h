/*---------------------------------------------------------------------------------

	map management for mode 1

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

/*!	\file map.h
	\brief contains the functions to manage / scroll large background MAP on SNES.
	\brief this can only be use for mode 1 
    \brief the engine scrolls layer1 in x and y coordinates
    \brief layer address is : 6800 for Layer 1
*/
#ifndef SNES_MAPS_INCLUDE
#define SNES_MAPS_INCLUDE

#include <snes/snestypes.h>

extern u16 x_pos,y_pos; /*!< \brief Current value of camera in x & y coordinates */

/*!	\fn mapLoad(u8 *layer1map, u8 *layertiles, u8 *tilesprop)
	\brief Load map definition into memory
	\param layer1map address of map with tiles
    \param layertiles address of tiles definition
    \param tilesprop address of tiles property definition (blocker, spikes, and so on)
*/
void mapLoad(u8 *layer1map, u8 *layertiles, u8 *tilesprop);

/*!	\fn mapUpdate(void)
	\brief Update map regarding current camera position (must be done once per frame)
*/
void mapUpdate(void);

/*!	\fn mapVblank(void)
	\brief Display map regarding current buffer (must be done once per frame, near Vblank)
*/
void mapVblank(void);

/*!	\fn mapUpdateCamera(u16 xpos, u16 ypos)
	\brief Update map camera (must be done once per frame)
	\param xpos	x coordinate of object where we want to focus on
	\param ypos	x coordinate of object where we want to focus on
*/
void mapUpdateCamera(u16 xpos, u16 ypos);

/*!	\fn mapGetMetaTile(u16 xpos, u16 ypos)
	\brief Get metatile property from current map
	\param xpos	x coordinate of metatile
	\param ypos	x coordinate of metatile
    \return tile properties
*/
u16 mapGetMetaTile(u16 xpos, u16 ypos);

#endif
