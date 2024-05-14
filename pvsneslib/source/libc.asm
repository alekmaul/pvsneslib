.INCLUDE "hdr.asm"

.ifdef FASTROM
.BASE $80
.endif

.BANK 1

.DEFINE ORG_0 0
.ifdef HIROM
.REDEFINE ORG_0 $8000
.endif

.SECTION ".libc_mem" SEMIFREE ORG ORG_0

.accu 16
.index 16
.16bit

__builtin_memcpy:
memcpy:
      lda.b 4,s	; destination
      sta.b tcc__r0
      tay
      lda.b 8,s ; source
      tax
      lda.b 10,s ; source bank
      xba
      ora.b 6,s ; dest bank
      sta.b move_insn + 1
      lda.b 12,s ; length
      beq +
      dec a
      phb
      jsr move_insn
      plb
      lda.b 6,s
      sta.b tcc__r0h
+     rtl

__builtin_mempcpy:
mempcpy:
      lda.b 4,s	; destination
      sta.b tcc__r0
      tay
      lda.b 8,s ; source
      tax
      lda.b 10,s ; source bank
      xba
      ora.b 6,s ; dest bank
      sta.b move_insn + 1
      lda.b 12,s ; length
      beq +
      dec a
      phb
      jsr move_insn
      plb
+     lda.b 6,s
      sta.b tcc__r0h
      lda.b 4,s
      clc
      adc.b 12,s
      sta.b tcc__r0
      rtl

__builtin_memmove:
memmove:
      lda.b 6,s ; dest bank
      cmp.b 10,s
      beq memcpy	; different banks -> no overlap
      sta.b tcc__r0h
      lda.b 4,s ; dest
      cmp.b 8,s ; src
      beq __local_finished ; nop
      bcc memcpy        ; dest before src -> forward

      sta.b tcc__r0	; dest
      clc
      adc.b 12,s	; +size -> end of dest
      tay
      lda.b 8,s ; source
      clc
      adc.b 12,s	; + size -> end of source
      tax
      lda.b 10,s ; source bank
      xba
      ora.b 6,s ; dest bank
      sta.b move_backwards_insn + 1
      lda.b 12,s ; length
      beq __local_finished
      dec a
      phb
      jsr move_backwards_insn
      plb
__local_finished: rtl


__builtin_memset:
memset:
      lda.b 4,s		; ptr
      sta.b tcc__r0
      lda.b 6,s
      sta.b tcc__r0h
      lda.b 10,s	; count
      beq +
      tay
      lda.b 8,s		; character
      sep #$20
-     dey
      sta.b [tcc__r0],y
      bne -
      rep #$20
+     rtl

__builtin_bzero:
bzero:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s
      beq +
      tay
      lda.w #0
      sep #$20
-     dey
      sta.b [tcc__r9],y
      bne -
      rep #$20
+     rtl

.accu 16
.index 16

__builtin_strcmp:
strcmp:
      lda.b 4,s		; dest
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s		; src
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      ldy.w #0
      sep #$20
-     lda.b [tcc__r9],y
      sec
      sbc.b [tcc__r10],y
      beq + ; schaumermol
      bcs __local_gresser
      bcc __local_kloaner
+     lda.b [tcc__r9],y
      beq __local_gleich
      iny
      bra -
__local_gleich: rep #$20
      stz.b tcc__r0
      rtl
__local_gresser: rep #$20
      lda.w #1
      sta.b tcc__r0
      rtl
__local_kloaner: rep #$20
      lda.w #-1
      sta.b tcc__r0
      rtl

.accu 16
.index 16

__builtin_strncmp:
strncmp:
      lda.b 4,s		; dest
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s		; src
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      lda.b 12,s	; count
      beq __local_gleich
      tax
      ldy.w #0
      sep #$20
-     lda.b [tcc__r9],y
      sec
      sbc.b [tcc__r10],y
      beq + ; schaumermol
      bcs __local_gresser
      bcc __local_kloaner
+     lda.b [tcc__r9],y
      beq __local_gleich	; end of string => equality
      dex
      beq __local_gleich	; max. count reached => equality
      iny
      bra -

.accu 16
.index 16

__builtin_memcmp:
memcmp:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      lda.b 12,s
      beq __local_gleich
      tax
      ldy.w #0
      sep #$20
-     lda.b [tcc__r9],y
      sec
      sbc.b [tcc__r10],y
      beq + ; schaumermol
      bcs __local_gresser ; from strcmp
      bcc __local_kloaner
+     dex
      beq __local_gleich
      iny
      bra -

.accu 16
.index 16

__builtin_strcpy:
strcpy:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b 8,s
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      sep #$20
      ldy.w #$ffff
-     iny
      lda.b [tcc__r10],y
      sta.b [tcc__r9],y
      bne -
      rep #$20
      rtl

__builtin_strncpy:
strncpy:
      lda.b 4,s
      sta.b tcc__r0
      lda.b 6,s
      sta.b tcc__r0h
      lda.b 8,s
      sta.b tcc__r10
      lda.b 10,s
      sta.b tcc__r10h
      lda.b 12,s
      beq ++	; zero bytes to copy -> done
      tax
      inx
      ldy.w #$ffff
      sep #$20
-     iny
      dex
      beq +	; max bytes reached -> done
      lda.b [tcc__r10],y
      sta.b [tcc__r0],y
      bne -
      bra +++   ; fill up with zeros
+     rep #$20
++    rtl
.accu 8
+++
-     sta.b [tcc__r0],y
      iny
      dex
      bne -
      rep #$20
      rtl

.accu 16
.index 16

__builtin_strlen:
strlen:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      ldy.w #$ffff
      sep #$20
-     iny
      lda.b [tcc__r9],y
      bne -
      rep #$20
      sty.b tcc__r0
      rtl

strchr:
      lda.b 4,s
      sta.b tcc__r0
      lda.b 6,s
      sta.b tcc__r0h
      sep #$20
-     lda.b [tcc__r0]
      beq + ; end of string
      cmp.b 8,s
      beq ++ ; found
      inc.b tcc__r0
      bra -
+     lda.b 8,s	; looking for zero?
      beq ++	; -> found
      ; nothing found
      rep #$20
      stz.b tcc__r0
      stz.b tcc__r0h
++    rep #$20
      rtl

strrchr:
      lda.b 4,s
      sta.b tcc__r0
      lda.b 6,s
      sta.b tcc__r0h
      ldy.w #0
      ldx.w #$ffff
      sep #$20
-     lda.b [tcc__r0],y
      beq ++ ; end of string
      cmp.b 8,s
      bne +
      tyx	; found, save index in x
+     iny
      bra -
      ; end of string
++    lda.b 8,s	; looking for zero?
      bne +
      tyx	; found
+     rep #$20
      cpx.w #$ffff	; found something?
      bne +
      ; found nothing -> return NULL
      stz.b tcc__r0
      stz.b tcc__r0h
      rtl
+     txa
      clc
      adc.b tcc__r0
      sta.b tcc__r0
      rtl

.ENDS

.SECTION ".libc_misc" SEMIFREE ORG ORG_0
.accu 16
.index 16

__builtin_ffs:
;ffs:
      lda.b 4,s
      bne +
      stz.b tcc__r0
      rtl
+     ldx.w #0
-     inx
      bit.w #1
      bne +
      lsr a
      bra -
+     stx.b tcc__r0
      rtl

__builtin_setjmp:
setjmp:
      lda.b 4,s
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      tsa
      sta.b [tcc__r9]
      lda.b 1,s
      ldy.w #2
      sta.b [tcc__r9],y
      iny
      iny
      lda.b 3,s
      sta.b [tcc__r9],y
      stz.b tcc__r0
      rtl

__builtin_longjmp:
longjmp:
      lda.b 8,s ; retval
      bne +
      inc a
+     sta.b tcc__r0
      lda.b 4,s ; jmp_buf
      sta.b tcc__r9
      lda.b 6,s
      sta.b tcc__r9h
      lda.b [tcc__r9]
      tas
      ldy.w #2
      lda.b [tcc__r9],y
      sta.b 1,s
      iny
      iny
      lda.b [tcc__r9],y
      sta.b 3,s
      rtl

.ENDS

.SECTION ".libc_cstd" SEMIFREE ORG ORG_0

.accu 16
.index 16
.16bit

__builtin_abort:
abort:
      sep #$20
      lda.b #$ff
      sta $fffd
      rep #$20
      stp

exit:
      lda.b 4,s
      sep #$20
      sta $fffd
      rep #$20
      stp

labs:
abs:
      lda.b 4,s
      bpl +
      eor #$ffff
      ina
+     sta.b tcc__r0
      rtl

exitl4:
      stp

.ENDS

.include "libc_c.asm"

.DEFINE RAMSLOT_0 0
.ifdef HIROM
.REDEFINE RAMSLOT_0 2
.endif

.DEFINE BASE_0 $00
.ifdef FASTROM
.ifdef HIROM
.REDEFINE BASE_0 $C0
.else
.REDEFINE BASE_0 $80
.endif
.else
.ifdef HIROM
.REDEFINE BASE_0 $40
.endif
.endif

.include "backgrounds.asm"
.include "consoles.asm"
.include "dmas.asm"
.include "input.asm"
.include "interrupts.asm"
.include "lzsss.asm"
.include "maps.asm"
.include "objects.asm"
.include "scores.asm"
.include "snesmodwla.asm"
.include "sounds.asm"
.include "sprites.asm"
.include "videos.asm"
.include "vblank.asm"
