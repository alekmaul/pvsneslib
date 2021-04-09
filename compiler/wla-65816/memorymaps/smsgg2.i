
;==============================================================
; SMS/GG memory map (by Maxim)
; Large first chunk version
; If you're only paging in slot 2, you can fit data into the
; first 32KB better this way. The 16 byte slot is where the
; header goes so don't use it; it's there to make the numbering
; for the :label syntax work correctly.
;==============================================================

.MEMORYMAP
DEFAULTSLOT 0
SLOTSIZE $7ff0
SLOT 0 $0000 ; ROM (unpaged)
SLOTSIZE $0010
SLOT 1 $7ff0 ; Header
SLOTSIZE $4000
SLOT 2 $8000 ; ROM (paged)
SLOTSIZE $2000
SLOT 3 $c000 ; RAM
.ENDME

; 512KB (32*16KB) example rombankmap:
.ROMBANKMAP
BANKSTOTAL 32
BANKSIZE $7ff0
BANKS 1
BANKSIZE $0010
BANKS 1
BANKSIZE $4000
BANKS 30
.ENDRO

