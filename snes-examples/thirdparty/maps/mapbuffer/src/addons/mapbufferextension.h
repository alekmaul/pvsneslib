/*---------------------------------------------------------------------------------

    Copyright (C) 2024
        Christoph Silge

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

#ifndef MAPTILEEXTENSION_H
#define MAPTILEEXTENSION_H

#include <snes.h>
#define DTBUFFER_MAX 32 // same value like in the asm file!
extern u8 mapdirty;
extern u16 dtb_address[DTBUFFER_MAX];
extern u16 dtb_prop[DTBUFFER_MAX];
extern u8 dtb_hp[DTBUFFER_MAX];
extern u8 dtb_timer[DTBUFFER_MAX];
extern u8 dtb_tilenumber[DTBUFFER_MAX];
extern u8 dtb_tileattr[DTBUFFER_MAX];
extern u8 dtb_state[DTBUFFER_MAX];
extern u8 mapbuffer[0x4003]; // same value like in the asm file!

extern u16 TileInfoAddress; // collected data by mapGetMetaTilesInfo
extern u8 TileInfoNumber;   // collected data by mapGetMetaTilesInfo
extern u8 TileInfoAttr;     // collected data by mapGetMetaTilesInfo
extern u16 TileInfoProp;    // collected data by mapGetMetaTilesInfo

//----------public methods in C file----------------------
/// @brief creates or gets existing index from dtb_address buffer sets the last TileInfos from mapGetMetaTilesInfo
/// @return free id, 0xff when dtb_address buffer is
u8 GetDynamicTileID();

/// @brief demonstration of manual changing tiles
/// @param index dynamicTileID
void ManipulateDynamicTile(u8 index);
/// @brief demonstration of manual chat_objsnging tiles
/// this example
void DynamicTileAutoUpdate();

/// @brief should be set to zero if a map not using the feature
/// will be set to true when calling mapbuffersLoad
extern u8 IsMapBufferEnabled;
/// @brief loads a map into the mapbuffer
/// @param map pointer to the map file
/// @param size size of map
void mapbuffersLoad(u8 *map, u16 size);
/// @brief before VBLANK!
/// it calculates if the pre
void maptileQueueUpdate();
/// @brief after VBLANK!
void maptileVRAMUpdate();
/// @brief Call after mapload!
void dynamicTileBuffer_Init();

/// @brief Defered changes tile from dtb_address
/// in mapbuffer and queue it in
/// @param index DynamicTileID
/// @param tile new tile number
/// @param attr not fully implemented yet
void mapChangeTileByID(u8 index, u8 tile, u8 attr);

/// @brief Direct changes tile
/// @param xpos
/// @param ypos
/// @param tile new tile number
/// @param attr not fully implemented yet
void mapChangeTileByPos(u16 xpos, u16 ypos, u8 tile, u8 attr);

/// @brief
/// @param xpos
/// @param ypos
/// @return
u16 mapGetMetaTilesInfo(u16 xpos, u16 ypos);

#endif // MAPTILEEXTENSION_H