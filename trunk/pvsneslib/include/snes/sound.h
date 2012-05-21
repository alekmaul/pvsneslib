/*---------------------------------------------------------------------------------

	Sound functions.

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

/*! \file sound.h
    \brief snes sound support.

	Really great thanks to shiru for sound engine and tools used in his Christmas Craze
	homebrew
	http://shiru.untergrund.net/
*/

#ifndef SNES_SOUND_INCLUDE
#define SNES_SOUND_INCLUDE

#include <snes/snestypes.h>
#include <snes/interrupts.h>

/*! \def REG_APU00
    \brief Main CPU to Sound CPU Communication Port 0 (R/W)
	7-0   APU I/O Data   (Write: Data to APU, Read: Data from APU)
	Caution: These registers should be written only in 8bit mode (there is a hardware 
	glitch that can cause a 16bit write to [2140h..2141h] to destroy [2143h], this 
	might happen only in some situations, like when the cartridge contains too 
	many ROM chips which apply too much load on the bus).

	Same thing for REG_APU01, REG_APU02 and REG_APU03 for addr 2141h..2143h
	REG_APU0001 and REG_APU0203 for 16 bits read/write access
*/
#define REG_APU00	(*(vuint8*)0x2140)
#define REG_APU01	(*(vuint8*)0x2141)
#define REG_APU02	(*(vuint8*)0x2142)
#define REG_APU03	(*(vuint8*)0x2143)
#define REG_APU0001	(*(vuint16*)0x2140)
#define REG_APU0203	(*(vuint16*)0x2142)

//command codes
#define SCMD_STEREO			0x0000
#define SCMD_VOLUME			0x1000
#define SCMD_MUSIC_STOP 	0x2000
#define SCMD_MUSIC_PLAY 	0x3000
#define SCMD_SFX_PLAY		0x4000
#define SCMD_RELOAD			0x5000

void soundCommand(unsigned int command,unsigned int param);

/*! \brief Play a sound effect
    \param channel number of channel where effect will be play (0..15) 
    \param idEffect identifier of the sound effect (0..n)
    \param pan panning effect (0..15)
*/
#define soundEffect(channel,idEffect,pan)	(soundCommand(SCMD_SFX_PLAY|((channel)<<8),(idEffect)|((pan)<<8)))

/*! \brief Stop current music
*/
#define soundMusicStop()		(spc_command(SCMD_MUSIC_STOP,0))

/*! \brief Play current music
*/
#define soundMusicPlay()		(spc_command(SCMD_MUSIC_PLAY,0))

/*! \brief Reload current music
*/
#define soundMusicReload()		(spc_command(SCMD_RELOAD,0))

/*! \brief Initializes SPC sound engine in SNES SPC700 memory
*/
void soundInit(void);

#endif //SNES_SOUND_INCLUDE
