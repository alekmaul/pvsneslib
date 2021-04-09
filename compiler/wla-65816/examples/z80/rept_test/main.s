
.MEMORYMAP
DEFAULTSLOT 0
SLOTSIZE $200
SLOT 0 $0000
SLOT 1 $0200
.ENDME

.ROMBANKMAP
BANKSTOTAL 10
BANKSIZE $200
BANKS 10
.ENDRO

.printv dec (-(1+2+3-(3+2)+1)-1)
.printt "\n"


.slot 0

.section "bossman" size 500 superfree
.repeat $200
  .db 1
.endr
.ends


.bank 0 slot 0
.org 0

.section "reptman" force
repoman:
.dw +
.rept 15
    jr nc,+
    nop
  +:add hl,hl          ; hl*=2
.endr
.db "moro!", repoman+6
.ends

.slot 1
.section "16-bit multiplication with .rept" superfree
Multiply16_rept:
; input: de,bc
; output: de*bc
    or a               ; clear carry
    ld hl,$0000        ; zero hl

.rept 15
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
.endr
    rl e
    rl d
    ret nc
    add hl,bc
    ret nc
    inc de
    ret
.ends

.section "16-bit multiplication" free
Multiply16:
; input: de,bc
; output: de*bc
    or a               ; clear carry
    ld hl,$0000        ; zero hl

    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2
    rl e
    rl d               ; de<<=1
    jr nc,+
    add hl,bc          ; if a bit was rotated out then add bc to hl
    jr nc,+
    inc de             ; if that hasn't overflowed then inc de
  +:add hl,hl          ; hl*=2

    rl e
    rl d
    ret nc
    add hl,bc
    ret nc
    inc de
    ret
.ends
