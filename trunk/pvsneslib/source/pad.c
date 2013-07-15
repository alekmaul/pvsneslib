/*---------------------------------------------------------------------------------

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

#include <snes/pad.h>

extern u16 pad_keys[2];// = {0,0};
extern u16 pad_keysold[2];//  = {0,0};
extern u16 pad_keysrepeat[2];//  = {0,0};

//---------------------------------------------------------------------------------
#if 0
void scanPads(void) {
	unsigned int i;
	u16 pressed,released;

	while(REG_HVBJOY & PAD_BUSY); // Wait Auto-Joypad-Read Busy Flag not set

	for(i=0;i<2;i++) {
		pad_keysold[i] = pad_keys[i];
		pad_keys[i] = (REG_JOYxLH(i) & 0xFFF0);// ^ 0xFFF0;  // Read Auto-Joypad
	}
}

//; Put your own address here
define ScopeVars $0000

/*
    Nintendo SHVC Scope BIOS version 1.00
    Quickly disassembled and commented by Revenant on 31 Jan 2013
   
    This assembly uses xkas v14 syntax. It probably also assembles with bass, if there's
    any such thing as good fortune in the universe.

    How to use the SHVC Super Scope BIOS:
    (all variables are two bytes)
   
    1: Set "HoldDelay" and "RepDelay" for the button hold delay and repeat rate
   
    2:  "jsr GetScope" or "jsl GetScopeLong" once per frame
   
    3:  Read one of the following to get the scope input bits (see definitions below):
        - ScopeDown (for any flags that are currently true)
        - ScopeNow (for any flags that have become true this frame)
        - ScopeHeld (for any flags that have been true for a certain length of time)
        - ScopeLast (for any flags that were true on the previous frame)
   
    3a: If the bits read from ScopeNow indicate a valid shot, or if the Cursor button
        is being pressed, then read "ShotH"/"ShotV" to adjust for aim, or read
        "ShotHRaw"/"ShotVRaw" for "pure" coordinates
   
    3c: at some point, set "CenterH"/"CenterV" equal to "ShotHRaw"/"ShotVRaw"
        so that the aim-adjusted coordinates are "correct"
*/

//; Scope input bit definitions
define SCOPE_FIRE      $8000
define SCOPE_CURSOR    $4000
define SCOPE_TURBO     $2000
define SCOPE_PAUSE     $1000
define SCOPE_OFFSCREEN $0200
define SCOPE_NOISE     $0100

//; Hardware register definitions
define OPHCT     $213c
define OPVCT     $213d
define STAT78    $213f

define HVBJOY    $4212
define JOY2      $421a

//; Port 2 input pressed, held, and pressed last frame (2 bytes each)
//; These are for the BIOS use only. Use the Super Scope input vars below instead.
define Port2Down ScopeVars+0
define Port2Now  ScopeVars+2
define Port2Last ScopeVars+4

//; Number of frames left until a button is considered "held".
//; This is periodically initialized to either HoldDelay or RepDelay
//; depending on how long the same buttons are held down.
define ToHold    ScopeVars+6

//; Super Scope port inputs (2 bytes each)
//; These correspond to the actual port input and have these useful bits:
//; 0x8000 - fire
//; 0x4000 - cursor
//; 0x2000 - turbo
//; 0x1000 - pause
//; 0x0200 - offscreen
//; 0x0100 - noise
define ScopeDown ScopeVars+8
define ScopeNow  ScopeVars+10
define ScopeHeld ScopeVars+12
define ScopeLast ScopeVars+14

//; Delay values to use (in frames) for buttons to be considered held
//; initially, and delay value to use for a held button to be considered
//; repeated.
define HoldDelay ScopeVars+16
define RepDelay  ScopeVars+18

//; Horizontal/vertical shot position, adjusted for aim (2 bytes each)
define ShotH     ScopeVars+20
define ShotV     ScopeVars+22

//; Horizontal/vertical shot position, not adjusted (2 bytes each)
define ShotHRaw  ScopeVars+24
define ShotVRaw  ScopeVars+26

//; Center position used to adjust shots (2 bytes each)
//; 0x0000 is the center of the screen, positive values go to bottom right.
define CenterH   ScopeVars+28
define CenterV   ScopeVars+30

//; Number of frames elapsed since last shot was fired (2 bytes)
define SinceShot ScopeVars+32

//; For getting the scope status when in a different program bank.
//; This is the only non-BIOS code in this disassembly and is included
//; for your convenience.
GetScopeLong:
    phb
    phk
    plb
    jsr   GetScope
    plb
    rtl

db "START OF SCOPE BIOS"
    //; This is in the code for some reason. I'm only including it
    //; because this is supposed to be a complete disassembly.
    lda.w #$0081

GetScope:   
    php
    sep   #$20
   
    //; Has the PPU counter been latched?
    //; If not, don't get the scanline location
    lda   {STAT78}
    and.b #$40
    beq   NoShot
   
    //; Get the horizontal scanline location (bits 0-7)
    lda   {OPHCT}
    sta   {ShotH}
    sta   {ShotHRaw}
   
    //; Get the horizontal scanline location (bit 8)
    lda   {OPHCT}
    and.b #$01
    sta   {ShotH}+1
    sta   {ShotHRaw}+1
   
    //; Get the vertical scanline location (bits 0-7)
    lda   {OPVCT}
    sta   {ShotV}
    sta   {ShotVRaw}
   
    //; Get the vertical scanline location (bit 8)
    lda   {OPVCT}
    and.b #$01
    sta   {ShotV}+1
    sta   {ShotVRaw}+1
   
    //; Factor in the horizontal offset factor
    rep   #$20
    lda   {CenterH}
    clc
    adc   {ShotH}
    sta   {ShotH}
   
    //; Factor in the vertical offset factor
    lda   {CenterV}
    clc
    adc   {ShotV}
    sta   {ShotV}
   
    //; update number of frames since last shot
    //; (what happens if 65536 frames elapse between shots?)
    stz   {SinceShot}
    bra   GetInput
NoShot:
    inc   {SinceShot}

    //; Wait for valid joypad input
GetInput:
    sep   #$20
WaitInput:
    lda   {HVBJOY}
    and.b #$01
    bne   WaitInput
   
    //; Get joypad 2 input
    //; using a typical method to seperate frame input / total input
    rep   #$20
    lda   {JOY2}
    sta   {Port2Down}
    eor   {Port2Last}
    and   {Port2Down}
    sta   {Port2Now}
    lda   {Port2Down}
    sta   {Port2Last}
   
    //; Check if the controller in port 2 is a Super Scope.
    //; For a 16-bit auto joypad read, bits 0-7 should be always 1
    //; and bits 10-11 should be always 0.
    lda   {Port2Down}
    and.w #$0CFF
    cmp.w #$00FF
    bne   NoScope
   
    //; has a shot already happened this frame?
    //; If so, then only pay attention to the pause button bit
    lda   {SinceShot}
    beq   GetButtons
   
    //; Check which already-held buttons are still held
    lda   {Port2Down}
    and   {ScopeLast}
    sta   {ScopeLast}
   
    //; Check pause button held status
    lda   {Port2Down}
    and.w #{SCOPE_PAUSE}
    sta   {ScopeDown}
   
    //; Check pause button pressed status
    lda   {Port2Now}
    and.w #{SCOPE_PAUSE}
    sta   {ScopeNow}
   
    //; return from input check
    plp
    rts
   
    //; Get button status when NOT paused
GetButtons:
    lda   {Port2Down}
    sta   {ScopeDown}
    eor   {ScopeLast}
    and   {Port2Down}
    sta   {ScopeNow}
    sta   {ScopeHeld}
   
    //; if no bits are set on port 2, don't check for "held buttons".
    lda   {Port2Down}
    beq   NotHolding
    //; else if the bits aren't the same as last frame, don't check either.
    cmp   {ScopeLast}
    bne   NotHolding
   
    //; if a certain number of frames have elapsed with the same buttons
    //; held down, consider them "officially held".
    dec   {ToHold}
    bne   NotHeld
   
    lda   {Port2Down}
    sta   {ScopeHeld}
   
    //; set the remaining delay to the repeat value
    lda   {RepDelay}
    sta   {ToHold}
    bra   NotHeld
   
    //; set the remaining delay to the normal value
NotHolding:
    lda   {HoldDelay}
    sta   {ToHold}
NotHeld:
    lda   {Port2Down}
    sta   {ScopeLast}
   
    //; return from input check
    plp
    rts
   
//; If no scope is connected, zero out all inputs
NoScope:
    stz   {Port2Down}
    stz   {Port2Now}
    stz   {ScopeDown}
    stz   {ScopeNow}
    stz   {ScopeHeld}
   
    //; return from input check
    plp
    rts
	
#endif

//------------------------------------------------------------------------------
unsigned short padsDown(unsigned short value) {
	return (pad_keys[value] & ~pad_keysold[value]);
}

//------------------------------------------------------------------------------
unsigned short padsUp(unsigned short value) {
	return (pad_keys[value] ^ pad_keysold[value]) & (~pad_keys[value]);
}

//------------------------------------------------------------------------------
void padsClear(unsigned short value) {
	pad_keys[value] = 0;
	pad_keysold[value]  = 0;
	pad_keysrepeat[value]  = 0;
}

