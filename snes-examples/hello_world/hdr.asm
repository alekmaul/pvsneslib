;==LoRom==                      ; We'll get to HiRom some other time.

.MEMORYMAP                      ; Begin describing the system architecture.
  SLOTSIZE $8000                ; The slot is $8000 bytes in size. More details on slots later.
  DEFAULTSLOT 0                 ; There's only 1 slot in SNES, there are more in other consoles.
  SLOT 0 $8000                  ; Defines Slot 0's starting address.
  SLOT 1 $0 $2000
  SLOT 2 $2000 $E000
  SLOT 3 $0 $10000
.ENDME          ; End MemoryMap definition

.ROMBANKSIZE $8000              ; Every ROM bank is 32 KBytes in size
.ROMBANKS 8                     ; 2 Mbits - Tell WLA we want to use 8 ROM Banks

.SNESHEADER
  ID "SNES"                     ; 1-4 letter string, just leave it as "SNES"

  NAME "LIBSNES HELLO WORLD  "  ; Program Title - can't be over 21 bytes,
  ;    "123456789012345678901"  ; use spaces for unused bytes of the name.

  SLOWROM
  LOROM

  CARTRIDGETYPE $00             ; $00=ROM, $01=ROM+RAM, $02=ROM+SRAM, $03=ROM+DSP1, $04=ROM+RAM+DSP1, $05=ROM+SRAM+DSP1, $13=ROM+Super FX
  ROMSIZE $08                   ; $08=2 Megabits, $09=4 Megabits,$0A=8 Megabits,$0B=16 Megabits,$0C=32 Megabits
  SRAMSIZE $00                  ; $00=0 kilobits, $01=16 kilobits, $02=32 kilobits, $03=64 kilobits
  COUNTRY $01                   ; $01= U.S., $00=Japan, $02=Europe, $03=Sweden/Scandinavia, $04=Finland, $05=Denmark, $06=France, $07=Netherlands, $08=Spain, $09=Germany, $0A=Italy, $0B=China, $0C=Indonesia, $0D=Korea
  LICENSEECODE $00              ; Just use $00
  VERSION $00                   ; $00 = 1.00, $01 = 1.01, etc.
.ENDSNES

.SNESNATIVEVECTOR               ; Define Native Mode interrupt vector table
  COP EmptyHandler
  BRK EmptyHandler
  ABORT EmptyHandler
  NMI VBlank
  IRQ EmptyHandler
.ENDNATIVEVECTOR

.SNESEMUVECTOR                  ; Define Emulation Mode interrupt vector table
  COP EmptyHandler
  ABORT EmptyHandler
  NMI EmptyHandler
  RESET tcc__start                   ; where execution starts
  IRQBRK EmptyHandler
.ENDEMUVECTOR
