/*---------------------------------------------------------------------------------

    Interrupt registers and vector pointers

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

/*! \file interrupt.h
    \brief snes interrupt support.
*/

#ifndef SNES_INTERRUPT_INCLUDE
#define SNES_INTERRUPT_INCLUDE

#include <snes/snestypes.h>

/**
 * \brief VBlank ISR flag.
 *
 * Used to detect lag-frames in the VBlank ISR.
 *
 * This variable is set to a truthy (non-zero) value in WaitForVBlank()
 * and cleared in the NMI ISR after the call to nmi_handler.
 *
 * vblank_flag can be used in a custom nmi_handler to detect lag frames.  Within nmi_handler:
 *  - If vblank_flag is truthy (non-zero), the nmi_handler was called at the end of the frame.
 *  - If vblank_flag is 0, the nmi_handler was called in the middle of a lag frame.
 *
 * \b CAUTION: This variable SHOULD NOT be changed outside of WaitForVBlank()
 */
extern u8 vblank_flag;

/**
 * \brief Lag-frame counter.
 *
 * This variable is incremented on every VBlank Interrupt that occurs during a lag-frame.
 *
 * \b CAUTION: The lag frame counter cannot tell the difference between a lag-frame and
 * force-blank setup loading graphics to the PPU.
 *
 * \c lag_frame_counter can be modified.  This is useful in development builds to measure the
 * amount of lag in a level by resetting \c lag_frame_counter on level load and printing
 * \c lag_frame_counter on a pause screen or at the end of the level.
 */
extern u16 lag_frame_counter;


extern void *nmi_handler;

/** \brief VBlank NMI Enable  (0=Disable, 1=Enable) (Initially disabled on reset) */
#define INT_VBLENABLE		(1 << 7) 
/** \brief H/V IRQ (0=Disable, 1=At H=H + V=Any, 2=At V=V + H=0, 3=At H=H + V=V) */
#define INT_HVIRQ_H			(1 << 4)   
/** \brief H/V IRQ (0=Disable, 1=At H=H + V=Any, 2=At V=V + H=0, 3=At H=H + V=V) */
#define INT_HVIRQ_V			(1 << 5)  
/** \brief H/V IRQ (0=Disable, 1=At H=H + V=Any, 2=At V=V + H=0, 3=At H=H + V=V) */ 
#define INT_HVIRQ_HV		(2 << 4)  
/** \brief Joypad Enable    (0=Disable, 1=Enable Automatic Reading of Joypad) */
#define INT_JOYPAD_ENABLE 	(1)  
/** \brief V-Blank Period Flag (0=No, 1=VBlank)*/
#define VBL_READY 			(1 << 7)     
/** \brief H-Blank Period Flag (0=No, 1=HBlank)*/
#define HBL_READY 			(1 << 6)     
/** \brief Auto-Joypad-Read Busy Flag (1=Busy) (see 4200h, and 4218h..421Fh) */
#define PAD_BUSY 			(1)           

/**
 *  \brief 
 *      Interrupt Enable and Joypad Request (W).<br>
 *
 *      7     VBlank NMI Enable  (0=Disable, 1=Enable) (Initially disabled on reset)<br>
 *      6     Not used<br>
 *      5-4   H/V IRQ (0=Disable, 1=At H=H + V=Any, 2=At V=V + H=0, 3=At H=H + V=V)<br>
 *      3-1   Not used<br>
 *      0     Joypad Enable    (0=Disable, 1=Enable Automatic Reading of Joypad)<br>
 *<br>
 *      Disabling IRQs (via bit4-5) does additionally acknowledge IRQs.<br>
 *      There's no such effect when disabling NMIs (via bit7).<br>
*/
#define REG_NMITIMEN (*(vuint8 *)0x4200)

/*

4207h/4208h - HTIMEL/HTIMEH - H-Count Timer Setting (W)

  15-9  Not used
  8-0   H-Count Timer Value (0..339) (+/-1 in long/short lines) (0=leftmost)

The H/V-IRQ flag in Bit7 of TIMEUP, Port 4211h gets set when the H-Counter gets equal to the H-Count register value.

4209h/420Ah - VTIMEL/VTIMEH - V-Count Timer Setting (W)

  15-9  Not used
  8-0   V-Count Timer Value (0..261/311, NTSC/PAL) (+1 in interlace) (0=top)

The H/V-IRQ flag in Bit7 of TIMEUP, Port 4211h gets set when the V-Counter gets equal to the V-Count register value.
*/

/**
 *  \brief 
 *      V-Blank NMI Flag and CPU Version Number (R) (Read/Ack)<br>
 *
 *      7     Vblank NMI Flag  (0=None, 1=Interrupt Request) (set on Begin of Vblank)<br>
 *      6-4   Not used<br>
 *      3-0   CPU 5A22 Version Number (version 2 exists)<br>
 *<br>
 *      The NMI flag gets set at begin of Vblank (this happens even if NMIs are disabled). The flag gets reset automatically<br>
 *      at end of Vblank, and gets also reset after reading from this register.<br>
 *      The SNES has only one NMI source (vblank), and the NMI flag is automatically reset (on vblank end), so there's<br>
 *      normally no need to read/acknowledge the flag, except one special case: If one does disable and re-enable NMIs,<br>
 *      then an old NMI may be executed again; acknowledging avoids that effect.<br>
 *      The CPU includes another internal NMI flag, which gets set when "[4200h].7 AND [4210h].7" changes from 0-to-1, and<br>
 *      gets cleared when the NMI gets executed (which should happen around after the next opcode) (if a DMA transfer is<br>
 *      in progress, then it is somewhere after the DMA, in that case the NMI can get executed outside of the Vblank<br>
 *      period, ie. at a time when [4210h].7 is no longer set).<br>
*/
#define REG_RDNMI (*(vuint8 *)0x4210)

/**
 *  \brief 
 *      TIMEUP - H/V-Timer IRQ Flag (R) (Read/Ack)<br><br>
 *
 *      7     H/V-Count Timer IRQ Flag (0=None, 1=Interrupt Request)<br>
 *      6-0   Not used<br>
 *<br>
 *      The IRQ flag is automatically reset after reading from this register<br>
 *      (except when reading at the very time when the IRQ condition is true <br>
 *      (which lasts for 4-8 master cycles), then the CPU receives bit7=1, <br>
 *      but register bit7 isn't cleared). The flag is also automatically cleared <br>
 *      when disabling IRQs (by setting 4200h.Bit5-4 to zero).<br>
 *      Unlike NMI handlers, IRQ handlers MUST acknowledge IRQs, otherwise the IRQ <br>
 *      gets executed again (ie. immediately after the RTI opcode).<br>
*/
#define REG_TIMEUP (*(vuint8 *)0x4211)

/**
 *  \brief 
 *      H/V-Blank flag and Joypad Busy flag (R).<br>
 *
 *      7     V-Blank Period Flag (0=No, 1=VBlank)<br>
 *      6     H-Blank Period Flag (0=No, 1=HBlank)<br>
 *      5-1   Not used<br>
 *      0     Auto-Joypad-Read Busy Flag (1=Busy) (see 4200h, and 4218h..421Fh)<br>
 *<br>
 *      The Hblank flag gets toggled in ALL scanlines (including during Vblank/Vsync).<br>
 *      Both Vblank and Hblank are always toggling (even during Forced Blank, <br>
 *      and no matter if IRQs or NMIs are enabled<br>
*/
#define REG_HVBJOY (*(vuint8 *)0x4212)

/**
 *  \brief 
 *      Add a handler for the given interrupt mask.<br>
 *
 *      Specify the handler to use for the nmi interrupt.<br>
 *  \param handler 
 *      Address of the function to use as an interrupt service routine<br>
*/
#define nmiSet(handler) nmi_handler = handler;

/**
 *  \brief 
 *      Wait for vblank interrupt<br>
 *
 *      Waits for a vertical blank interrupt<br>
*/
void WaitForVBlank(void);

/**
 *  \brief 
 *      Wait for VBL flag to be OK<br>
*/
#define WaitVBLFlag                   \
    while ((REG_HVBJOY & VBL_READY))  \
        ;                             \
    while (!(REG_HVBJOY & VBL_READY)) \
    {                                 \
    };

/**
 *  \brief 
 *      Wait for vblank interrupt ntime times<br>
 *
 *  \param 
 *      ntime number of time to wait VBlank Interrupt<br>
*/
void WaitNVBlank(u16 ntime);

#endif // SNES_INTERRUPTS_INCLUDE
