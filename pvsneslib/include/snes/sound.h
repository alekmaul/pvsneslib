/*---------------------------------------------------------------------------------

	Sound functions.

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

/*! \file sound.h
    \brief snes sound support.

	Really great thanks to shiru for sound engine and tools used in his Christmas Craze
	homebrew
	http://shiru.untergrund.net/
	
	Also great thanks for mukunda for snesmod sound engine
	http://snes.mukunda.com/
	
	And special thanks to Kung Fu Furby for help debugging snesmod port with C interface
*/

#ifndef SNES_SOUND_INCLUDE
#define SNES_SOUND_INCLUDE

#include <snes/snestypes.h>
#include <snes/interrupts.h>

/*!	\brief Sound header(8 bytes) */
typedef struct {
	u8 pitch; 	/*!< default PITCH (1..6) (hz = PITCH * 2000) */
	u8 panning; /*!< default PANNING (0..15) */
	u8 volume; /*!< default VOLUME (0..15) */
	u8 length1; /*!< number of BRR chunks (BYTELEN/9) (max 4kilobytes??) low */
	u8 length2; /*!< number of BRR chunks (BYTELEN/9) (max 4kilobytes??) high */
	u8 addr1; /*!< address of BRR data low */
	u8 addr2; /*!< address of BRR data high */
	u8 bank; /*!< bank of BRR data */
} brrsamples;

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

/*! \fn  spcBoot(void)
	\brief boots the spc700 with sm-spc. call once at startup
*/
void spcBoot(void);

/*! \fn  spcSetBank(u8 *bank)
	\brief set soundbank origin. soundbank must have dedicated bank(s)
	\param bank	bank address
*/
void spcSetBank(u8 *bank);

/*! \fn  spcLoad(u16 musIndex)
	\brief load module into sm-spc. this function may take some time to execute
	\param musIndex	module_id
*/
void spcLoad(u16 musIndex);

/*! \fn  spcLoadEffect(u16 sfxIndex)
	\brief load sound effect into sm-spc. this function may take some time to execute
	\param sfxIndex	sfx_id
*/
void spcLoadEffect(u16 sfxIndex);

/*! \fn  spcPlay(u8 startPos)
	\brief play module. 
	note: this simply queues a message, use spcFlush if you want
	to wait until the message is processed
	
	another note: there may be a significant startup time from
	after the message is processed to when the song starts playing...
	to sync the program with the song start use spcFlush and then
	wait until SPC_P of the status register is set.
	\param startPos	starting position
*/
void spcPlay(u8 startPos);

/*! \fn  spcStop(void)
	\brief stop playing the current module. 
*/
void spcStop(void);

/*
;*************************************************************************
;* spcReadStatus
;*
;* returns:
;*   a = status register
;*
;* read status register from sm-spc
;*************************************************************************
.import spcReadStatus

;*************************************************************************
;* spcGetCues
;*
;* returns:
;*   a = (0..15) number of cues that occured since last call
;*
;* get number of cues that have passed (pattern effect SF1)
;*************************************************************************
.import spcGetCues
*/

/*! \fn  spcSetModuleVolume(u8 vol)
	\brief set the module playback volume
	\param vol	volume (0..255)
*/
void spcSetModuleVolume(u8 vol);

/*! \fn  spcFadeModuleVolume(u16 vol, u16 fadespeed)
	\brief fade the module volume towards the target
	\param vol	volume (0..255)
	\param fadespeed	fade speed (volume(0..255) += y every 32ms)
*/
void spcFadeModuleVolume(u16 vol, u16 fadespeed);

/*! \fn  spcFlush(void)
	\brief Flush message queue (force sync)
*/
void spcFlush(void);

/*! \fn  spcProcess(void)
	\brief Process messages
	This function will try to give messages to the spc until a few
	scanlines pass
	
	this function MUST be called every frame if you are using
	streamed sounds
*/
void spcProcess(void);

/*! \fn  spcEffect(u16 pitch,u16 sfxIndex, u8 volpan)
	\brief Play sound effect (load with spcLoadEffect)
	\param pitch	pitch (0-15, 8=32khz)
	\param sfxIndex	effect index (0-15)
	\param volpan	volume(0..15) AND panning(0..15) (volume*16+pan)
*/
void spcEffect(u16 pitch,u16 sfxIndex, u8 volpan);

/*! \fn  spcSetSoundTable(u16 sndTableAddr,u8 sndTableBank)
	\brief set the address of the SOUND TABLE
	\param sndTableAddr	address of sound table
	\param sndTableBank	bank of sound table
*/
void spcSetSoundTable(u16 sndTableAddr,u8 sndTableBank);

/*! \fn spcSetSoundEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr)
	\brief set the values and address of the SOUND TABLE for a sound entry
	\param vol	volume (0..15)
	\param panning	panning (0..15)
	\param pitch	PITCH (1..6) (hz = PITCH * 2000)
	\param length	length of brr sample
	\param sampleaddr	address of brr sample
	\param ptr	address of variable where sounds values will be stored
*/
void spcSetSoundEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr);

/*! \fn spcSetSoundDataEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr)
	\brief set the values of a sound entry
	\param vol	volume (0..15)
	\param panning	panning (0..15)
	\param pitch	PITCH (1..6) (hz = PITCH * 2000)
	\param length	length of brr sample
	\param sampleaddr	address of brr sample
	\param ptr	address of variable where sounds values will be stored
*/
void spcSetSoundDataEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr);

//---------------------------------------------------------------------------------
/*! \fn spcSetSoundDataEntry(u8 vol, u8 panning, u8 pitch, u16 length, u8 *sampleaddr, brrsamples *ptr)
	\brief set the address of the SOUND TABLE for a sound entry
	\param ptr	address of variable where sounds values will be stored
*/
void spcSetSoundTableEntry(brrsamples *ptr);

/*! \fn  spcAllocateSoundRegion(u8 size);
	\brief set the size of the sound region
	(this must be big enough to hold your longest/largest sound)
	this function will STOP module playback too
	\param size	size of sound region (size*256 bytes)
*/
void spcAllocateSoundRegion(u8 size);

/*! \fn  spcPlaySound(u8 sndIndex)
	\brief Play sound from memory (using default arguments)
	\param sndIndex	index in sound table
*/
void spcPlaySound(u8 sndIndex);

/*! \fn  spcPlaySoundV(u8 sndIndex, u16 volume)
	\brief Play sound from memory (using default arguments)
	\param sndIndex	index in sound table
	\param volume	volume (0..15)
*/
void spcPlaySoundV(u8 sndIndex, u16 volume);

#endif //SNES_SOUND_INCLUDE
