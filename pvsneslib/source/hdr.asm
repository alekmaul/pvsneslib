;---------------------------------------------------------------------------------
;
;	Copyright (C) 2013-2020
;		Alekmaul
;
;	This software is provided 'as-is', without any express or implied
;	warranty.  In no event will the authors be held liable for any
;	damages arising from the use of this software.
;
;	Permission is granted to anyone to use this software for any
;	purpose, including commercial applications, and to alter it and
;	redistribute it freely, subject to the following restrictions:
;
;	1.	The origin of this software must not be misrepresented; you
;		must not claim that you wrote the original software. If you use
;		this software in a product, an acknowledgment in the product
;		documentation would be appreciated but is not required.
;	2.	Altered source versions must be plainly marked as such, and
;		must not be misrepresented as being the original software.
;	3.	This notice may not be removed or altered from any source
;		distribution.
;
;---------------------------------------------------------------------------------

.include "comp_defs.asm"        ; Contains definitions for HiROM and FastROM

.ifdef HIROM                    ;   ==HiRom==

.MEMORYMAP                      ; Begin describing the system architecture.
  SLOTSIZE $10000               ; The slot is $10000 bytes in size. More details on slots later.
  DEFAULTSLOT 0                 ; There's only 1 slot in SNES, there are more in other consoles.
  SLOT 0 $0000                  ; Defines Slot 0's starting address.
  SLOT 1 $0 $2000               ; Used for low RAM allocation
  SLOT 2 $2000 $E000            ; Used for RAM allocation
  SLOT 3 $0 $10000              ; Used for global RAM allocation
  SLOT 4 $6000                  ; Used for SRAM storage.
.ENDME                          ; End MemoryMap definition

.ROMBANKSIZE $10000             ; Every ROM bank is 64 KBytes in size

.else                           ;   ==LoRom==

.MEMORYMAP                      ; Begin describing the system architecture.
  SLOTSIZE $8000                ; The slot is $8000 bytes in size. More details on slots later.
  DEFAULTSLOT 0                 ; There's only 1 slot in SNES, there are more in other consoles.
  SLOT 0 $8000                  ; Defines Slot 0's starting address.
  SLOT 1 $0 $2000               ; Used for low RAM allocation
  SLOT 2 $2000 $E000            ; Used for RAM allocation
  SLOT 3 $0 $10000              ; Used for global RAM allocation and data storage
.ENDME                          ; End MemoryMap definition

.ROMBANKSIZE $8000              ; Every ROM bank is 32 KBytes in size

.endif

.ROMBANKS 2                     ; Tell WLA we want to use 2 ROM Banks