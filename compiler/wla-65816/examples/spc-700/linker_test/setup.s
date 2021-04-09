
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; includes
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.INCLUDE "defines.i"

.define jee (outro-intro)


.define TXT1 "hello"
.define TXT2 "hello"
.define TXTX 10



.ifeq TXT2 "mello"
	shfoifh
	w0gjwopjg
.endif

.BANK 0 SLOT 0
.ORG $300

.dw result+6
	
josse
--
-	beq -
+	beq +
	beq --
++	beq ++

---	beq ++
	beq +
--	nop
-	dec x
	beq ++++
	dec a
+	beq --
	nop
++	beq ---
---	beq ++
	beq ++
--	nop
-	dec x
	beq -
	dec a
+	beq --
	nop
++	beq ---
++++    beq +++

intro	bbs $aa.0, intro
+:
-
+++:
	bbs $aa.1, intro
	bbs $aa.2, intro
	bbs $aa.3, intro
	bbs $aa.4, intro
++
	bbs $aa.5, intro
+++
	bbs $aa.6, intro
-	bbs $aa.7, intro
--	bbc $aa.0, outro
	bbc $aa.1, outro
+
	bbc $aa.2, outro

+	bbc $aa.3, outro
	bbc $aa.4, outro
++	bbc $aa.5, outro
-	bbc $aa.6, outro
	bbc $aa.7, outro
outro	.db $ff, $ff, $ff
	tset1 !foul
	set1 $a0.0
	set1 $a0.1
	set1 $a0.2
	set1 $a0.3
	set1 $a0.4
	set1 $a0.5
	set1 $a0.6
	set1 $a0.7
	set1 $a0,4
	clr1 $a0.0
	clr1 $a0.1
	clr1 $a0.2
	clr1 $a0.3
	clr1 $a0.4
	clr1 $a0.5
	clr1 $a0.6
	clr1 $a0.7
	clr1 $a0,4
	tcall $0
	tcall $1
	tcall $2
	tcall $3
	tcall $4
	tcall $5
	tcall $6
	tcall $7
	tcall $8
	tcall $9
	tcall $A
	tcall $B
	tcall $C
	tcall $D
	tcall $E
	tcall $F
	beq _f
	nop
__      das
	beq _b
	adc $1a, $2b
	adc $1a, #$2b
	nop
	sleep
foul:	push a
	pop psw
jee	.dw jee, result-result
	and a, [jee]  +        Y
	and a,!foul   +  Y
	das
	div ya,x
	asl 10  +  X
	asl jee+x
