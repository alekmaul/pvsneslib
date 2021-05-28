/*---------------------------------------------------------------------------------

	Copyright (C) 2012-2013

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
/*! \file snes.h
\brief the master include file for snes applications.
*/

/*!
 \mainpage PVSnesLib Documentation


 \section intro Introduction
 Welcome to the PVSnesLib reference documentation.

 \section video_2D_api 2D engine API
 - \ref video.h "General video"
 - \ref background.h "2D Background Layers"
 - \ref lzss.h "LZ decompression routine"
 - \ref pixel.h "Pixel mode management"
 - \ref sprite.h "2D Sprites"

 \section audio_api Audio API
 - \ref sound.h "Mod tracker and Sound Engine"

 \section memory_api Memory
 - \ref dma.h "Direct Memory Access"
 
 \section system_api System
 - \ref snestypes.h "Custom SNES types"
 - \ref interrupt.h "Interrupts"

 \section user_io_api User Input/ouput
 - \ref pad.h "Keypad"
 - \ref console.h "Console and Debug Printing"

 \section Miscellaneous functions
 - \ref scores.h "Scoring management"

 \section external_links Usefull links
 - <a href="http://www.portabledev.com/wiki/doku.php">PVSnesLib wiki</a>
 - <a href="http://nocash.emubase.de/sns.htm">SNES hardware specifications documentation.</a>
 - <a href="http://code.google.com/p/snes-sdk/">SNES Sdk from Ulrich Hecht.</a>

 \section special_thanks Special Thanks
 - <a href="http://shiru.untergrund.net/">shiru for some source code and sound tools, which are parts of PVSnesLib.</a>
 - <a href="http://jiggawatt.org/badc0de/index.html">mic_ for some source code examples, parts of PVSnesLib are from them.</a>
 - <a href="http://snes.mukunda.com/">mukunda for snesmod source code. </a>
 - Kung Fu Furby for help on Snesmod convertion to PVSnesLib.
 - <a href="http://code.google.com/p/neo-myth-menu/">Neoflash Menu google code. </a>
 - <a href="http://www.devkitpro.org/">Devkitpro team for pvsneslib structure (lib, makefile, examples, and so on ...). </a>
*/


//adding the example page.
/*!
 <!-- EXAMPLES -->
    <!-- hello world -->
    \example hello_world/src/hello_world.c

    <!-- backgrounds -->
    \example graphics/Backgrounds/Mode0/Mode0.c
    \example graphics/Backgrounds/Mode1/Mode1.c
    \example graphics/Backgrounds/Mode1Png/Mode1.c
    \example graphics/Backgrounds/Mode1Scroll/Mode1Scroll.c
    \example graphics/Backgrounds/Mode1MixedScroll/Mode1MixedScroll.c
    \example graphics/Backgrounds/Mode3/Mode3.c
    \example graphics/Backgrounds/Mode7/Mode7.c

    <!-- effects -->
    \example graphics/Effects/HDMAGradient/HDMAGradient.c
    \example graphics/Effects/MosaicShading/MosaicShading.c
    \example graphics/Effects/Transparency/Transparency.c

    <!-- sprites -->
    \example graphics/Sprites/SimpleSprite/SimpleSprite.c
    \example graphics/Sprites/AnimatedSprite/AnimatedSprite.c
    \example graphics/Sprites/DynamicSprite/DynamicSprite.c

    <!-- palettes -->
    \example graphics/Palette/GetColors/GetColors.c

    <!-- keypad -->
    \example input/input.c

    <!-- timing -->
    \example timer/timer.c

    <!-- games -->
    \example games/likemario/LikeMario.c
    \example games/breakout/breakout.c

    <!-- audio -->
    \example audio/music/Music.c
    \example audio/tada/Tada.c

    <!-- debugging -->
    \example debug/debug.c

    <!-- random display -->
    \example random/random.c

    <!-- sram -->
    \example sram/sram.c

    <!-- scoring -->
    \example scoring/scoring.c

*/

#ifndef SNES_INCLUDE
#define SNES_INCLUDE

#include "snes/background.h"
#include "snes/console.h"
#include "snes/dma.h"
#include "snes/interrupt.h"
#include "snes/pad.h"
#include "snes/scores.h"
#include "snes/sound.h"
#include "snes/sprite.h"
#include "snes/video.h"

#endif // SNES_INCLUDE
