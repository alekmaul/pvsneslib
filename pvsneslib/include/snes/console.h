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

extern u16 snes_vblank_count; /*!< \brief Number of VBL since consoleInit called (16 bits longs so reset each 18 minutes in NTSC)*/
extern u8 snes_50hz;          /*!< \brief 1 if on a PAL/50Hz SNES */
extern u8 snes_fps;           /*!< \brief 50 if PAL console (50 Hz) or 60 if NTSC console (60Hz) */

/*! \fn rand(void)
    \brief return a randomized number
    \return unsigned short of a randomized number
*/
u16 rand(void);

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
void consoleMesenBreakpoint(void);

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
