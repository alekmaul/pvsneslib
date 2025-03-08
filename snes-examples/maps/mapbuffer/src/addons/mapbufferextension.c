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

#include <snes.h>
#include "mapbufferextension.h"

#define DTBSTATE_NONE 0
#define DTBSTATE_MANUAL 1
#define DTBSTATE_AUTO 2

#define DTBCHECKSPERFRAME 8
u8 dtb_updateIndex = 0;

void ManipulateDynamicTile(u8 index)
{
    if ((dtb_state[index] & DTBSTATE_MANUAL) == 0)
        return;

    switch (dtb_prop[index])
    {
    case 0xFF01:
    case 0xFF02:
    {
        if (dtb_hp[index] > 0)
        {
            --dtb_hp[index];
        }
        if (dtb_hp[index] == 0)
        {
            mapChangeTileByID(index, dtb_tilenumber[index] + 1, dtb_tileattr[index]);
            dtb_timer[index] = 64;
            dtb_state[index] = DTBSTATE_AUTO;
            break;
        }
    }
    break;
    }
}

u8 GetDynamicTileID()
{
    u8 i = 0;
    u8 found = 0;
    u8 freeslot = 0xFF;
    while (i < DTBUFFER_MAX)
    {
        if (dtb_address[i] == TileInfoAddress)
        {
            return i;
        }
        if (dtb_address[i] == 0xFFFF && freeslot == 0xFF)
        {
            // preserve first free slot
            freeslot = i;
        }
        ++i;
    }
    if (freeslot != 0xFF)
    {
        dtb_address[freeslot] = TileInfoAddress;
        dtb_tilenumber[freeslot] = TileInfoNumber;
        dtb_tileattr[freeslot] = TileInfoAttr;
        dtb_prop[freeslot] = TileInfoProp;
        dtb_timer[freeslot] = 100; // manual for timeout
        switch (dtb_prop[freeslot])
        {
        case 0xFF01:
            dtb_state[freeslot] = DTBSTATE_MANUAL;
            dtb_hp[freeslot] = 10;
            break;
        case 0xFF02:
            dtb_state[freeslot] = DTBSTATE_MANUAL;
            dtb_hp[freeslot] = 1;
            break;
        default:
            break;
        }
        return freeslot;
    }
    return 0xFF;
}

void DynamicTileAutoUpdate()
{
    if (IsMapBufferEnabled == 0)
        return;
    u8 i = dtb_updateIndex;
    u8 nm = DTBCHECKSPERFRAME + dtb_updateIndex;
    while (i < nm)
    {
        // Todo: Integrate Timout for 0xFF11;
        if (dtb_state[i] == DTBSTATE_NONE)
        {
            dtb_address[i] = 0xFFFF;
            ++i;
            continue;
        }
        else if ((dtb_state[i] == DTBSTATE_MANUAL))
        {
            // For Timeout
            switch (dtb_prop[i])
            {
            case 0xFF02:
            case 0xFF01:
                if (dtb_timer[i] > 0)
                {
                    --dtb_timer[i];
                }
                else
                {

                    dtb_address[i] = 0xFFFF;
                    dtb_state[i] = DTBSTATE_NONE;
                }
                break;
            }
        }
        else
        {
            switch (dtb_prop[i])
            {
            case 0xFF01:
            case 0xFF02:
                if (dtb_timer[i] > 0)
                {
                    --dtb_timer[i];
                }
                switch (dtb_timer[i])
                {
                case 61:
                case 3:
                    mapChangeTileByID(i, dtb_tilenumber[i] + 2, dtb_tileattr[i]);
                    break;
                case 58:
                case 6:
                    mapChangeTileByID(i, dtb_tilenumber[i] + 3, dtb_tileattr[i]);
                    break;
                case 55:
                case 9:
                    mapChangeTileByID(i, 0, dtb_tileattr[i]);
                    break;
                case 0:
                    mapChangeTileByID(i, dtb_tilenumber[i], dtb_tileattr[i]);
                    dtb_address[i] = 0xFFFF;
                    dtb_state[i] = DTBSTATE_NONE;
                    break;
                }
                break;
            }
        }
        ++i;
    }
    dtb_updateIndex += 8;
    if (dtb_updateIndex >= DTBUFFER_MAX)
        dtb_updateIndex = 0;
}