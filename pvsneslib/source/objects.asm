;---------------------------------------------------------------------------------
;
;	Copyright (C) 2012-2024
;		Alekmaul, Nub1604
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
;   Special thanks to Nub1604 for the slope management
;
;---------------------------------------------------------------------------------

.DEFINE OB_NULL			    $FFFF		    ; null value for linked table
.DEFINE OB_ID_NULL	        $0000		    ; null value for obj id

.DEFINE OB_MAX				80		        ; total number of objects in the game
.DEFINE OB_TYPE_MAX		    64		        ; total number of type of objects in the game

.DEFINE OB_SIZE				64		        ; 64 bytes for each object

.DEFINE OB_SCR_XLR_CHK      -64             ; -64 is the minimum on "virtual" screen x position to update object
.DEFINE OB_SCR_XRR_CHK      320             ; 256+64 is the maximum on "virtual" screen x position to update object
.DEFINE OB_SCR_YLR_CHK      -64             ; -64 is the minimum on "virtual" screen y position to update object
.DEFINE OB_SCR_YRR_CHK      288             ; 224+64 is the maximum on "virtual" screen y position to update object

.DEFINE OB_SCR_XLE_CHK      -32             ; -32 is the minimum on screen x position to display sprite object
.DEFINE OB_SCR_XRI_CHK      256             ; 256 is the maximum on screen x position to display sprite object
.DEFINE OB_SCR_YLE_CHK      -32             ; -32 is the minimum on screen y position to display sprite object
.DEFINE OB_SCR_YRI_CHK      224             ; 224 is the maximum on screen y position to display sprite object

.DEFINE T_SOLID				$FF00
.DEFINE T_LADDER			$0001
.DEFINE T_FIRES				$0002
.DEFINE T_SPIKE				$0004
.DEFINE T_PLATE				$0008
.DEFINE T_SLOPES            $0020
;                           $0020             Type Slope 1x1 Up ◢ (action will be climb on it) for tile
;                           $0021             Type Slope 1x1 Down ◣ (action will be descend on it) for tile
;                           $0022  Type Slope 2x1 lower half Up ◢ (action will be climb on it) for tile
;                           $0023  Type Slope 2x1 lower half Down ◣ (action will be descend on it) for tile
;                           $0024  Type Slope 2x1 upper half Up ◢ (action will be climb on it) for tile
;                           $0025  Type Slope 2x1 upper half Down ◣ (action will be descend on it) for tile

.DEFINE ACT_CLIMB			$2000
.DEFINE ACT_DIE				$4000
.DEFINE ACT_BURN			$8000

.DEFINE GRAVITY       	    41              ; default values
.DEFINE MAX_Y_VELOCITY      (10*256)
.DEFINE FRICTION            $10

.DEFINE FRICTION1D          $0100

.STRUCT t_objs
prev	        DW				            ;  0 previous object linked list
next	        DW 				            ;  2 next object in linked list
type            DB				            ;  4 type of object, depend of the game
nID	            DB				            ;  5 obj ID in linked list

sprnum	        DW			                ;  6 sprite oam number of the object (multiple of 4)
sprid3216       DW		                    ;  8 sprite oam id in sprite oam memory
sprblk3216	    DW	                        ; 10 sprite oam address in sprites tiles memory
sprflip	        DB		                    ; 12 sprite flip attribute
sprpal	        DB			                ; 13 sprite palette
sprframe        DW				            ; 14 current frame

xpos		    DSB	3		                ; 16 x position on the screen (3 fixed point)
ypos		    DSB	3		                ; 19 y position on the screen (3 fixed point)

xofs            DW			    	        ; 22 x offset of the object (from the square 16x16 or 32x32)
yofs            DW				            ; 24 y offset of the object (from the square 16x16 or 32x32)
width           DW			    	        ; 26 width of the object (from the square 16x16 or 32x32)
height          DW			                ; 28 height of the object (from the square 16x16 or 32x32)
xmin            DW                          ; 30 min x coordinate for action of object, depends of game (ex: revert direction)
xmax            DW                          ; 32 max x coordinate for action of object, depends of game (ex: revert direction)

xvel	        DW			    	        ; 34 x velocity
yvel	        DW				            ; 36 y velocity

tilestand       DW                          ; 38 tile number object is standing on
tileabove       DW                          ; 40 tile number above object
tilesprop       DW                          ; 42 tile property stand on
tilebprop       DW                          ; 44 tile property on left/right side

action          DW			                ; 46 current action of the object, depends of game
status          DB			                ; 48 status of object regarding collision
tempo	        DB			                ; 49 if object needs tempo
count           DB				            ; 50 if object needs a counter
dir             DB				            ; 51 if object needs to manage direction

parentID		DW                          ; 52 obj ID of parent (useful for projectiles)
hitpoints		DB                          ; 54 number of hit points of object
sprrefresh		DB               			; 55 if object needs sprite to be refresh

onscreen        DB                          ; 56 to know if object is on screen or not

objnotused      DSB 7                       ; OB_SIZE-56-1 for future use

.ENDST

.BASE $00
.RAMSECTION ".reg_objects7e" BANK $7E SLOT RAMSLOT_0

objbuffers      INSTANCEOF t_objs OB_MAX    ; object struct in memory
objactives      DSW OB_TYPE_MAX				; active object list

objfctinit      DSB 4*OB_TYPE_MAX			; pointer to init function of objects
objfctupd	    DSB 4*OB_TYPE_MAX			; pointer to update function of object
objfctref	    DSB 4*OB_TYPE_MAX			; pointer to refresh function of sprite object

objtmpbuf	    DSW (OB_MAX*5)+1			; temporary buffer for object

objunused       DW							; buffer of unused objects
objnewid	    DW							; new id created for the last object
objgetid	    DW							; return value of objNew
objnextid	    DB							; each new object is given a unique nID

objfctcall	    DSB 2						; low address for C function call
objfctcallh	    DSB 2						; high address for C function call

objptr		    DW							; pointer to current object
objcidx		    DW							; index of loop object

objgravity	    DW							; default gravity for objects
objfriction     DW                          ; default friction for objects

objtokill       DB							; =1 if need to kill object

objneedrefresh  DB                          ; =1 if we need to refresh all spritesbecause object is out of screen

objtmp1	DW									; temporary vars for stuffs
objtmp2	DW
objtmp3	DW
objtmp4	DW

.ENDS

.BASE BASE_0
.SECTION ".objects0_text" superfree

.accu 16
.index 16
.16bit

;---------------------------------------------------------------------------------
; void objInitEngine(void)
objInitEngine:
	php
	phb
	phx
	phy

	sep #$20                                            ; change bank for object bank
	lda #$7e
	pha
	plb

	rep #$20
	ldx #0

_oieR0:
	stz objbuffers,x                                    ; clear all object memory
	inx
	inx
	txa
	cmp #(OB_SIZE*(OB_MAX))
	bne _oieR0

	lda #$0000
	ldy #$0001

_oieR1:
    tax
	stz	objbuffers.1.nID,x                                ;init and link up all the unused objects
	tya
	sta objbuffers.1.next,x
    txa
	cmp #(OB_SIZE*(OB_MAX-2))                           ; if max reached, go away
	beq _oieR2
	iny
	clc
	adc #OB_SIZE
	bra _oieR1

_oieR2:
	tya													; because obj size is 64
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	lda #OB_NULL
	sta objbuffers.1.next,x					            ; terminate linked list
	stz objunused										; top of unused list is object 0

	ldx #(2*(OB_TYPE_MAX-1))
_oieR3:
	sta objactives,x                                    ; each active object to OB_NULL
	dex
	dex
	bne _oieR3
	sta objactives,x									; for last one

	stz.w objnewid
	stz.w objgetid

	sep #$20
	lda #$1
	sta objnextid

    rep #$20
    lda #GRAVITY                                        ; set default values for gravity and other variables
    sta objgravity
    lda #FRICTION
    sta objfriction

	ply
	plx
	plb
	plp

	rtl

;---------------------------------------------------------------------------------
; void objInitGravity(u16 objgravity, u16 objfriction)
objInitGravity:
	php
	phb

	sep #$20                                            ; change bank for object bank
	lda #$7e
	pha
	plb

    lda 7,s												; get gravity (5+2)
    sta objgravity
    lda 8,s												; get friction (6+2)
    sta objfriction
    rep #$20

	plb
	plp

	rtl

;---------------------------------------------------------------------------------
; void objInitFunctions(u8 objtype, void *initfct,void *updfct,void *reffct)
; 5 6-9 10-13 14-16
objInitFunctions:
    php
    phb

    phx

    sep #$20                                            ; go to object bank
    lda #$7e
    pha
    plb

    lda 8,s												; get type of object (x4 to have correct entry) (5+1+2)
    rep #$20
    and #$00ff
    asl a
    asl a
    tax

    lda 9,s												; get bank + data init function address (6+1+2 & 8+1+2)
    sta objfctinit,x
    lda 11,s
    sta objfctinit+2,x

    lda 13,s										    ; get bank + data update function address (10+1+2 & 12+1+2)
    sta objfctupd,x
    lda 15,s
    sta objfctupd+2,x

    lda 17,s										    ; get bank + data update function address (14+1+2 & 16+1+2)
    sta objfctref,x
    lda 19,s
    sta objfctref+2,x

    plx
    plb
    plp
    rtl

;---------------------------------------------------------------------------------
; u16 objNew (u8 objtype, u16 x, u16 y)
; 5 6-7 8-9
objNew:
    php
    phb

    phx
    phy

    sep #$20                                            ; go to bank object
    lda #$7e
    pha
    plb

    lda 10,s	                                        ; get object type (5+1+2+2)

    rep #$20
    and #$00ff
    asl a
    tay

    lda objunused										; if no object left, return null id value
    cmp #OB_NULL
    beq _oiN0
    bra _oiN1

_oiN0:
    lda #OB_ID_NULL                                     ; put OB_ID_NULL and leave function
    sta tcc__r0
    bra _oiNEnd

_oiN1:
    sta objnewid
    asl a 												; size 64
    asl a
    asl a
    asl a
    asl a
    asl a
    tax
    lda objbuffers.1.next,x	    						; get the next unused object
    sta objunused

    lda #OB_NULL
    sta objbuffers.1.prev,x
    lda objactives,y    								; add new object to the top of it's active list
    sta objbuffers.1.next,x

    phx
    cmp #OB_NULL
    beq _oiN2
    asl a
    asl a
    asl a
    asl a
    asl a
    asl a
    tax
    lda objnewid
    sta objbuffers.1.prev,x

_oiN2:
    lda objnewid
    sta objactives,y
    plx

    sep #$20											; init object
    lda 10,s
    sta objbuffers.1.type,x
    lda objnextid
    sta objbuffers.1.nID,x
    inc a
    bne _oiN3   									    ; zero is invalid nID (for unused objects)
    inc a
_oiN3:
    sta objnextid

    rep #$20
    lda 11,s 											; store coordinates x
    sta objbuffers.1.xpos+1,x
    lda 13,s 											; store coordinates y
    sta objbuffers.1.ypos+1,x

    sep #$20
    lda objbuffers.1.nID,x                              ; calculate (pObj->nID<<8) + newObjIdx

    rep #$20
    and #$00ff											; build the object handle (combined nID and object index in buffer)
    xba
    clc
    adc objnewid
    sta tcc__r0
    sta objgetid

_oiNEnd:
    ply
    plx
    plb
    plp

    rtl

;---------------------------------------------------------------------------------
; long call to the subroutine pointed to in objfctcall, needed for C functions
jslcallfct:
    sep #$20
    lda.l objfctcall + 2
    pha
    rep #$20
    lda.l objfctcall
    dec a
    pha
    rtl

;---------------------------------------------------------------------------------
; void objGetPointer(u16 objhandle)
objGetPointer:
    php
    phb

    phx
    phy

    sep #$20                                            ; bank address of objects
    lda #$7e
    pha
    plb

    rep #$20											; grab the nID value from the id (top word of the id)
    lda 10,s                                            ; 5+1+2+2
    xba
    and #$00ff

    sta objptr
    lda 10,s
    and #$00ff	  									    ; grab the ptr to the object (from index, lower word of id 5+1+2+2)
	tax
	inx													; grab value for return value
	asl a 												; adapt to object size 64
	asl a
	asl a
	asl a
	asl a
	asl a
	tay
	sep #$20
	lda objbuffers.1.nID,y
	rep #$20
	and #$00ff
	clc
	cmp objptr											; is object still valid?
	beq _oigp1

	ldx #0												; no... object has died and it's buffer slot has been reallocated
_oigp1:
	txa
	sta objptr

	ply
	plx
	plb
	plp
	rtl

;---------------------------------------------------------------------------------
; void objKill(u16 objhandle)
objKill:
	php
	phb

	phx
	phy

	sep #$20                                            ; got o object bank
	lda #$7e
	pha
	plb

	rep #$20											; grab the nID value from the id (top word of the id)
	lda 10,s                                            ; handle of object (5+1+2+2)
	pha
	jsl objGetPointer
	pla

	lda objptr
	bne _oik1                                           ; if NULL, object is already dead
	brl _oikend

_oik1:
	dec a                                               ; point to correct object (size 64)
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	sta objptr

	lda objbuffers.1.prev,x                             ; remove it from its active list

	cmp #OB_NULL
	bne _oik2

	sep #$20                                            ; this object was at the top of its list... move next one up to top
	lda objbuffers.1.type,x
	rep #$20
	and #$00ff
	asl a
	tay
	lda objbuffers.1.next,x
	sta objactives,y

	cmp #OB_NULL
	beq _oik3

	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	lda #OB_NULL
	sta objbuffers.1.prev,x
	bra _oik3

_oik2:
	lda objbuffers.1.next,x
	tay
	lda objbuffers.1.prev,x
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	tya
	sta objbuffers.1.next,x

	cmp #OB_NULL
	beq _oik3

	ldx objptr
	lda objbuffers.1.prev,x
	tay
	lda objbuffers.1.next,x
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	tya
	sta objbuffers.1.prev,x

_oik3:
	ldx objptr										    ; add the object to the unused list
	lda objunused
	sta objbuffers.1.next,x
	lda 10,s
	and #$00ff
	sta objunused

	ldy #4
_oik4:                                                  ; init memory of object
	stz objbuffers.1.type,x
	inx
	inx
	iny
	iny
	cpy #OB_SIZE
	bne _oik4

_oikend:
	ply
	plx
	plb
	plp

	rtl

;---------------------------------------------------------------------------------
; void objKillAll(void)
objKillAll:
    php
    phb

    phx
    phy

    sep #$20                                            ; go to object bank
    lda #$7e
    pha
    plb

    rep #$20
    ldy #$0000

_oikal1:
    lda objactives,y									; walk through entire active buffer
_oikal2:
    sta objcidx
    cmp #OB_NULL                                        ;  there are still object left in the list
    beq _oikal3
    asl a
    asl a
    asl a
    asl a
    asl a
    asl a
    tax
    lda objbuffers.1.next,x
    pha

    sep #$20                                        	;	kill this object
    lda objbuffers.1.nID,x
    rep #$20
    and #$00ff
    xba
    clc
    adc objcidx
    pha
    jsl objKill
    pla

    pla
    bra _oikal2

_oikal3:
    iny
    iny
    cpy #OB_TYPE_MAX*2                                  ; loop for all objects
	bne _oikal1

	stz.w objnewid
	stz.w objgetid
	stz.w objunused

	sep #$20
	lda #$1
	sta objnextid

	ply
	plx
	plb
	plp
	rtl

objOamRefreshAll:
    php
    phx

    ldx #$0000
_oora:
    sep #$20
    lda #$1
    sta oambuffer.1.oamrefresh,x                            ; get refresh attribute
    rep #$20
    txa
    clc
    adc #16                                                                                                                                                                                               ; to be on correct index (16 bytes per oam)
    tax
    cmp #128*16
    bne _oora

    plx
    plp
    rts

;---------------------------------------------------------------------------------
; void objUpdateAll(void)
objUpdateAll:
	php
	phb

	phx
	phy

	sep #$20                                            ; goto object bank
	lda #$7e
	pha
	plb

    stz objneedrefresh                                  ; no global refresh needed

	rep #$20
	ldx #$0000

_oiual1:
    lda objactives,x									; walk through entire active buffer
	phx
_oiual10:
    sta objcidx                                         ; to keep value of current index
    cmp #OB_NULL                                        ; there are still object left in the list
    bne _oiual3
    brl _oiual2
_oiual3:
    asl a
    asl a
    asl a
    asl a
    asl a
    asl a
    tax 				                                ; x is now the current object

    lda objbuffers.1.next,x                             ; grab the next object to update
    pha                                                 ; will be restore at the end

    lda objbuffers.1.xpos+1,x
    sec
    sbc.l x_pos

    cmp.w #OB_SCR_XRR_CHK
    bcc _oiual3y                                        ; x is lower than map max
    cmp.w #OB_SCR_XLR_CHK
    bcc _oiual3y1                                       ; but x is lower than map min

_oiual3y:                                               ; check now y coordinates
    lda objbuffers.1.ypos+1,x
    sec
    sbc.l y_pos

    cmp.w #OB_SCR_YRR_CHK
    bcc _oiual32                                        ; y is lower than map max
    cmp.w #OB_SCR_YLR_CHK
    bcs _oiual32                                        ; but y is greater than map min

_oiual3y1:
    jmp _oial4

_oiual32:                                               ; *** now we test if we are really on screen ***
    lda objbuffers.1.xpos+1,x
    sec
    sbc.l x_pos
    cmp.w #OB_SCR_XRI_CHK
    bcc _oiual3sy                                        ; x is lower than map max
    cmp.w #OB_SCR_XLE_CHK
    bcc _oiual3sy1                                       ; but x is lower than map min

_oiual3sy:                                               ; check now y coordinates
    lda objbuffers.1.ypos+1,x
    sec
    sbc.l y_pos

    cmp.w #OB_SCR_YRI_CHK
    bcc _oiuals32                                        ; y is lower than map max
    cmp.w #OB_SCR_YLE_CHK
    bcs _oiuals32                                        ; but y is greater than map min

_oiual3sy1:
    sep #$20                                            ; check if it was previously on screen to refresh all the objects
    lda objbuffers.1.onscreen,x
    beq _oiual3sy11                                      ; no ? no need to refresh
    stz objbuffers.1.onscreen,x
    lda objneedrefresh                                  ; if we have noticed a global refresh previously, don't do it again
    bne _oiual3sy11
    lda #1
    sta objneedrefresh
    jsr objOamRefreshAll                                ; do a global refresh of sprites

_oiual3sy11:                                            ; here we are not on screen but we manage object update
    bra _oiual321

_oiuals32: 
    sep #$20
    lda objbuffers.1.onscreen,x
    bne _oiual321                                       ; no ? we need to refresh
    lda #$1
    sta objbuffers.1.onscreen,x                         ; store that object is on screen
    lda objneedrefresh                                  ; if we have noticed a global refresh previously, don't do it again
    bne _oiual321
    lda #1
    sta objneedrefresh
    jsr objOamRefreshAll                                ; do a global refresh of sprites
_oiual321:
    lda objbuffers.1.type,x
    rep #$20
    and #$00ff
    asl a
    asl a
    tay

    lda objfctupd,y		                                ; get address of update function and call it
    sta objfctcall
    lda objfctupd+2,y
    sta objfctcallh

    lda objcidx
    pha

    sep #$20
    stz objtokill										; not killing object for now
	jsl jslcallfct
    rep #$20
    pla

    sep #$20
    lda objtokill										; object returned that it is now dead
    beq _oial4

_oial41:
    rep #$20
    lda objcidx
    asl a
    asl a
    asl a
    asl a
    asl a
    asl a
    tax
    sep #$20
    stz objbuffers.1.onscreen,x                         ; remove it from the screen
    lda objbuffers.1.nID,x
    rep #$20
    and #$00ff
    xba
    clc
    adc objcidx
    pha
    jsl objKill
    pla

    sep #$20
    lda objneedrefresh                                  ; it is dead, so remove sprite from screen
    bne _oial4                                         ; if we have noticed a global refresh previously, don't do it again
    lda #1
    sta objneedrefresh
    jsr objOamRefreshAll                                ; do a global refresh of sprites

_oial4:
    rep #$20                                            ; goto next object  (saved at beginning)
    pla
    brl _oiual10

_oiual2:
    plx
    inx
    inx
    cpx #OB_TYPE_MAX*2                                  ; loop with all type of objects
    beq _oiuaend
	brl _oiual1

_oiuaend:
	ply
	plx
	plb
	plp
	rtl

;---------------------------------------------------------------------------------
; void objRefreshAll(void)
objRefreshAll:
	php
	phb

	phx
	phy

	sep #$20                                            ; goto object bank
	lda #$7e
	pha
	plb

    stz objneedrefresh                                  ; no global refresh needed

	rep #$20
	ldx #$0000

_oiral1:
    lda objactives,x									; walk through entire active buffer
	phx
_oiral10:
    sta objcidx                                         ; to keep value of current index
    cmp #OB_NULL                                        ; there are still object left in the list
    bne _oiral3
    brl _oiral2
_oiral3:
    asl a
    asl a
    asl a
    asl a
    asl a
    asl a
    tax 				                                ; x is now the current object

    lda objbuffers.1.next,x                             ; grab the next object to update
    pha                                                 ; will be restore at the end

    sep #$20                                            ; check if it was previously on screen to refresh all the objects
    lda objbuffers.1.onscreen,x
    bne _oiral32                                        ; no ? no need to refresh

    rep #$20
    bra _oiral31

_oiral32:
    lda objbuffers.1.type,x
    rep #$20
    and #$00ff
    asl a
    asl a
    tay

    lda objfctref,y		                                ; get address of update function and call it
    sta objfctcall
    lda objfctref+2,y
    sta objfctcallh

    lda objcidx
    pha

    sep #$20
	jsl jslcallfct
    rep #$20
    pla

_oiral31:
    pla
    brl _oiral10

_oiral2:
    plx
    inx
    inx
    cpx #OB_TYPE_MAX*2                                  ; loop with all type of objects
    beq _oiraend
	brl _oiral1

_oiraend:
	ply
	plx
	plb
	plp
	rtl

.ENDS

.SECTION ".objects1_text" superfree

;---------------------------------------------------------------------------------
; void objCollidMap(u16 objhandle)
objCollidMap:
    php
    phb

    phx
    phy

    sep #$20                                            ; go to bank objects
    lda #$7e
    pha
    plb

	rep #$20
	lda 10,s											; grad the index of object (5+1+2+2)
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	sta objtmp2                                         ; to keep global handle for object

	lda objbuffers.1.yvel,x                               ; if yvel>=0 -> if object is falling
	bpl	_oicm1
	jmp _oicmtstyn
_oicm1:
    xba                                                ; compute ypos
    and	#$00FF
    clc
    adc	objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    clc
    adc objbuffers.1.height,x
	bpl + 												      ; if y<0, put it to 0 *FIX 221130 *
	lda #0000

+   lsr a
    lsr a
    and	#$FFFE
    tay

    lda objbuffers.1.xpos+1,x
    clc
    adc	objbuffers.1.xofs,x
    pha

    and	#$0007                                          ; compute the number of tiles to test
    clc
    adc	objbuffers.1.width,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta	objtmp1

    pla                                                 ; compute the offset line for y
    lsr a
    lsr a
    and	#$FFFE
    clc
    adc.w mapadrrowlut, y
    tay

    clc
    adc maptile_L1d                                     ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
	asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
    sta objtmp3
    bra	_oicm21         		                        ; speedup, saves some cycles

_oicm2:
    tya
    clc
    adc maptile_L1d  				                    ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
    sta objtmp3

_oicm21:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDER										; if ladder, well avoid doing stuffs
	beq _oicm3
	cmp #T_PLATE										; if on a plate, do same thing
	beq _oicm3

    cmp #T_FIRES                                        ; if fire, player is burning
    bne _oicm22
    lda #ACT_BURN										; player is now dying, no more checking
    sta objbuffers.1.action
    brl  _oicmtstx

_oicm22:
    cmp #T_SPIKE                                        ; if spike, player is dying too
	bne _oicm23
	lda #ACT_DIE
	sta objbuffers.1.action
	brl  _oicmtstx

_oicm23:
    and #$ff00											; keep only the high values for collision
    beq _oicm4

_oicm3:
    lda objtmp3
    sta objbuffers.1.tilestand,x                           ; store the tile we are standing on

    lda objbuffers.1.yvel+1,x
    and #$00ff
    clc
    adc objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    clc
    adc objbuffers.1.height,x
    inc a

    and #$fff8
    sec
    sbc objbuffers.1.yofs,x
    sec
    sbc objbuffers.1.height,x
    sta objbuffers.1.ypos+1,x
    stz objbuffers.1.yvel,x
    jmp	_oicmtstx

_oicm4:
    rep #$20                                            ; continue to test the tiles on x
    iny
    iny
    dec	objtmp1
    beq _oicm5
    brl _oicm2

_oicm5:
    ldx objtmp2                                         ; not standing on anything or ladder, now falling if not climbing
	stz objbuffers.1.tilestand,x

    lda objbuffers.1.tilesprop,x			                ; need again to verify if not tile standing
    beq _oicm61
    lda objbuffers.1.action,x				            ; if not climbing, well doing like if falling
    and #ACT_CLIMB
    beq _oicm61
    brl  _oicmtstx

_oicm61:
    lda objbuffers.1.yvel,x
    clc
    adc #GRAVITY
    cmp #MAX_Y_VELOCITY+1                                  ; add velocity regarding if we do not reach the maximum
    bmi _oicm6
    lda #MAX_Y_VELOCITY

_oicm6:
    sta objbuffers.1.yvel,x
	brl  _oicmtstx

_oicmtstyn:												   ; ------------------------------------------------------
    stz objbuffers.1.tilestand,x                           ; yvel<0, object is moving upwards

    xba
    ora	#$FF00
    clc
    adc	objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
	bpl +     											   ; if y < 0, put it to 0  *FIX 221126*

	lda #0000
+   lsr a
    lsr a
	and	#$FFFE
	tay

    lda objbuffers.1.xpos+1,x
    clc
    adc	objbuffers.1.xofs,x
    pha

    and	#$0007
	clc
	adc	objbuffers.1.width,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta	objtmp1

    pla
    lsr a
    lsr a
    and	#$FFFE
    clc
    adc.w mapadrrowlut, y
    tay

    clc
    adc maptile_L1d 					                ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
    bra	_oicmtstyn2		                                ; speedup, saves some cycles

_oicmtstyn1:
    tya
    clc
    adc maptile_L1d 					                ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

_oicmtstyn2:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDER										; if ladder, well avoid doing stuffs
    beq _oicmtstyn4

    and #$ff00											; keep only the high values for collision
    beq _oicmtstyn3

    lda objbuffers.1.yvel+1,x
    ora #$ff00
    clc
    adc objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    clc
    adc #8
    and #$fff8
    sec
    sbc objbuffers.1.yofs,x
    sta objbuffers.1.ypos+1,x
    stz objbuffers.1.yvel,x
    brl _oicmtstyn4

_oicmtstyn3:
    iny                                                 ; continue to test the tiles on x
    iny
    dec	objtmp1
    bne _oicmtstyn1

_oicmtstyn4:
    ldx objtmp2                                         ; not standing on anything, now falling
    lda objbuffers.1.yvel,x
    clc
    adc #GRAVITY
    cmp #MAX_Y_VELOCITY+1
    bmi _oicmtstyn5                                     ; add velocity regarding if we do not reach the maximum
    lda #MAX_Y_VELOCITY

_oicmtstyn5:
    sta objbuffers.1.yvel,x

_oicmtstx:
	ldx objtmp2
	lda objbuffers.1.xvel,x                               ; if xvel>=0 -> moving right
	bne _oicmtstx1
	jmp _oicmend
_oicmtstx1:
    bpl	_oicmtstx11
    jmp _oicmtstxn
_oicmtstx11:
    sec                                                 ; moving right
    sbc objfriction
    bpl _oicmtstx12
    stz objbuffers.1.xvel,x
    jmp _oicmend

_oicmtstx12:
    sta objbuffers.1.xvel,x
	lda objbuffers.1.ypos+1,x
	clc
	adc objbuffers.1.yofs,x
	bpl + 												      ; if y<0, put it to 0 *FIX 20221201 */
	lda #0000

+	pha
    and	#$0007
    clc
    adc objbuffers.1.height,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta objtmp1

    pla
    lsr a
    lsr a
    and	#$fffe
    tay

    lda objbuffers.1.xvel+1,x
    and #$00ff
    clc
    adc objbuffers.1.xpos+1,x
    clc
    adc objbuffers.1.xofs,x
    clc
    adc objbuffers.1.width,x
    lsr a
    lsr a
    and	#$fffe
    clc
    adc.w mapadrrowlut, y

_oicmtstx13:
    tay
    clc
    adc maptile_L1d
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

    ldx objtmp2
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicmtstx14
    and #$ff00											; keep only the high values for collision
    beq _oicmtstx14

    lda objbuffers.1.xvel+1,x
    and #$00ff
    clc
    adc objbuffers.1.xpos+ 1,x
    clc
    adc objbuffers.1.xofs,x
    clc
    adc objbuffers.1.width,x
    inc a
    and #$fff8
    sec
    sbc objbuffers.1.xofs,x
    sec
    sbc objbuffers.1.width,x
    sta objbuffers.1.xpos+ 1,x
    stz objbuffers.1.xvel,x
    brl _oicmend

_oicmtstx14:
    tya                                                 ; go through all y available
    clc
    adc.l maprowsize
    dec	objtmp1
    bne _oicmtstx13
    brl _oicmend

_oicmtstxn:
    clc                                                 ; moving left (xvel<0)
    adc objfriction
    bmi _oicmtstxna
    stz objbuffers.1.xvel,x                             ; currently it is not ok to go left
    brl _oicmend

_oicmtstxna:
    sta objbuffers.1.xvel,x
    lda objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
	bpl + 												      ; if y<0, put it to 0 *FIX 20221201 */
	lda #0000

+   pha
    and	#$0007
    clc
    adc objbuffers.1.height,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta objtmp1

    pla
    lsr a
    lsr a
    and	#$fffe
    tay

    lda objbuffers.1.xvel+1,x
    ora #$ff00
    clc
    adc objbuffers.1.xpos+1,x
    clc
    adc objbuffers.1.xofs,x

bpl _oicmtstxnb                                         ; if negative, don't bother to test, will change screen
    jmp _oicmend

_oicmtstxnb:
    lsr a
    lsr a
    and	#$fffe
    clc
    adc.w mapadrrowlut, y

_oicmtstxnc:
    tay
    clc
    adc maptile_L1d 					                ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

    ldx objtmp2
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicmtstxnd

    and #$ff00											; keep only the high values for collision
    beq _oicmtstxnd

    lda objbuffers.1.xvel+1,x
    ora #$ff00
    clc
    adc objbuffers.1.xpos+ 1,x
    clc
    adc objbuffers.1.xofs,x
    clc
    adc #8
    and #$fff8
    sec
    sbc objbuffers.1.xofs,x
    sta objbuffers.1.xpos+ 1,x
    stz objbuffers.1.xvel,x

    brl _oicmend

_oicmtstxnd:
    tya
    clc
    adc.l maprowsize
    dec	objtmp1
    bne _oicmtstxnc				                        ; go through all y available

_oicmend:
	ply
	plx
	plb
	plp
	rtl

.ENDS

.SECTION ".objects2_text" superfree

;---------------------------------------------------------------------------------
; void objCollidMap1D(u16 objhandle)
objCollidMap1D:
    php
    phb

    phx
    phy

    sep #$20                                                  ; go to bank objects
    lda #$7e
    pha
    plb

	rep #$20
	lda 10,s											      ; grad the index of object (5+1+2+2)
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	sta objtmp2                                               ; to keep global handle for object

	lda objbuffers.1.yvel,x                                   ;-- TEST Y ---------
	bpl	_oicm1d1                                              ; if yvel>=0 -> if object is moving down
	jmp _oicm1dtstyn
_oicm1d1:
    xba                                                       ; compute ypos
    and	#$00FF
    clc
    adc	objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    clc
    adc objbuffers.1.height,x
	bpl + 												      ; if y<0, put it to 0
	lda #0000

+   lsr a
    lsr a
    and	#$FFFE
    tay

    lda objbuffers.1.xpos+1,x
    clc
    adc	objbuffers.1.xofs,x
    pha

    and	#$0007                                          ; compute the number of tiles to test
    clc
    adc	objbuffers.1.width,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta	objtmp1

    pla                                                 ; compute the offset line for y
    lsr a
    lsr a
    and	#$FFFE
    clc
    adc.w mapadrrowlut, y
    tay

    clc
    adc maptile_L1d                                     ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
	asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
    sta objtmp3
    bra	_oicm1d21         		                        ; speedup, saves some cycles

_oicm1d2:
    tya
    clc
    adc maptile_L1d  				                    ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
    sta objtmp3

_oicm1d21:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDER										; if ladder, well avoid doing stuffs
	beq _oicm1d3
	cmp #T_PLATE										; if on a plate, do same thing
	beq _oicm1d3

    cmp #T_FIRES                                        ; if fire, player is burning
    bne _oicm1d22
    lda #ACT_BURN										; player is now dying, no more checking
    sta objbuffers.1.action
    brl  _oicm1dtstx

_oicm1d22:
    cmp #T_SPIKE                                        ; if spike, player is dying too
	bne _oicm1d23
	lda #ACT_DIE
	sta objbuffers.1.action
	brl  _oicm1dtstx

_oicm1d23:
    and #$ff00											; keep only the high values for collision
    beq _oicm1d4

_oicm1d3:
    lda objtmp3
    sta objbuffers.1.tilestand,x                           ; store the tile we are standing on

    lda objbuffers.1.yvel+1,x
    and #$00ff
    clc
    adc objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    clc
    adc objbuffers.1.height,x
    inc a

    and #$fff8
    sec
    sbc objbuffers.1.yofs,x
    sec
    sbc objbuffers.1.height,x
    sta objbuffers.1.ypos+1,x
    stz objbuffers.1.yvel,x
    jmp	_oicm1dtstx

_oicm1d4:
    rep #$20                                            ; continue to test the tiles on x
    iny
    iny
    dec	objtmp1
    beq _oicm1d5
    brl _oicm1d2

_oicm1d5:
    ldx objtmp2                                         ; not standing on anything or ladder, now falling if not climbing
	stz objbuffers.1.tilestand,x

    lda objbuffers.1.tilesprop,x			                ; need again to verify if not tile standing
    beq _oicm1d61
    lda objbuffers.1.action,x				            ; if not climbing, well doing like if falling
    and #ACT_CLIMB
    beq _oicm1d61
    brl  _oicm1dtstx

_oicm1d61:
	brl  _oicm1dtstx                                          ; test x now

_oicm1dtstyn:
    stz objbuffers.1.tilestand,x                              ; yvel<0, object is moving upwards
    xba
    ora	#$FF00
    clc
    adc	objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
	bpl +     												  ; if y < 0, put it to 0
	lda #0000

+	lsr a
    lsr a
	and	#$FFFE
	tay

    lda objbuffers.1.xpos+1,x
    clc
    adc	objbuffers.1.xofs,x
    pha

    and	#$0007
	clc
	adc	objbuffers.1.width,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta	objtmp1

    pla
    lsr a
    lsr a
    and	#$FFFE
    clc
    adc.w mapadrrowlut, y
    tay

    clc
    adc maptile_L1d 					                ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
    bra	_oicm1dtstyn2		                                ; speedup, saves some cycles

_oicm1dtstyn1:
    tya
    clc
    adc maptile_L1d 					                ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

_oicm1dtstyn2:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDER										; if ladder, well avoid doing stuffs
    beq _oicm1dtstx

    and #$ff00											; keep only the high values for collision
    beq _oicm1dtstyn3

    lda objbuffers.1.yvel+1,x
    ora #$ff00
    clc
    adc objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    clc
    adc #8
    and #$fff8
    sec
    sbc objbuffers.1.yofs,x
    sta objbuffers.1.ypos+1,x
    stz objbuffers.1.yvel,x
    brl _oicm1dtstx

_oicm1dtstyn3:
    iny                                                       ;  continue to test the tiles on x
    iny
    dec	objtmp1
    bne _oicm1dtstyn1


_oicm1dtstx:                                                  ;-- TEST X ---------------------------
	ldx objtmp2
	lda objbuffers.1.xvel,x                                   ; if xvel>=0 -> moving right
	bne _oicm1dtstx1
	jmp _oicm1dend
_oicm1dtstx1:
    bpl	_oicm1dtstx11
    jmp _oicm1dtstxn

_oicm1dtstx11:
	lda objbuffers.1.ypos+1,x
	clc
	adc objbuffers.1.yofs,x
	bpl +     											 ; if y < 0, put it to 0
	lda #0000

+	pha
    and	#$0007
    clc
    adc objbuffers.1.height,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta objtmp1

    pla
    lsr a
    lsr a
    and	#$fffe
    tay

    lda objbuffers.1.xvel+1,x
    and #$00ff
    clc
    adc objbuffers.1.xpos+1,x
    clc
    adc objbuffers.1.xofs,x
    clc
    adc objbuffers.1.width,x
    lsr a
    lsr a
    and	#$fffe
    clc
    adc.w mapadrrowlut, y

_oicm1dtstx13:
    tay
    clc
    adc maptile_L1d
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

    ldx objtmp2
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicm1dtstx14
    and #$ff00											; keep only the high values for collision
    beq _oicm1dtstx14

    lda objbuffers.1.xvel+1,x
    and #$00ff
    clc
    adc objbuffers.1.xpos+ 1,x
    clc
    adc objbuffers.1.xofs,x
    clc
    adc objbuffers.1.width,x
    inc a
    and #$fff8
    sec
    sbc objbuffers.1.xofs,x
    sec
    sbc objbuffers.1.width,x
    sta objbuffers.1.xpos+ 1,x
    stz objbuffers.1.xvel,x
    brl _oicm1dend

_oicm1dtstx14:
    tya                                                 ; go through all y available
    clc
    adc.l maprowsize
    dec	objtmp1
    bne _oicm1dtstx13
    brl _oicm1dend

_oicm1dtstxn:
    lda objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
	bpl +     											  ; if y < 0, put it to 0
	lda #0000

+   pha

    and	#$0007
    clc
    adc objbuffers.1.height,x
    dec a
    lsr a
    lsr a
    lsr a
    inc a
    sta objtmp1

    pla
    lsr a
    lsr a
    and	#$fffe
    tay

    lda objbuffers.1.xvel+1,x
    ora #$ff00
    clc
    adc objbuffers.1.xpos+1,x
    clc
    adc objbuffers.1.xofs,x

    bpl _oicm1dtstxnb                                         ; if negative, don't bother to test, will change screen
    jmp _oicm1dend

_oicm1dtstxnb:
    lsr a
    lsr a
    and	#$fffe
    clc
    adc.w mapadrrowlut, y

_oicm1dtstxnc:
    tay
    clc
    adc maptile_L1d 					                ; get direct rom value
    tax
    phb
    sep #$20
    lda maptile_L1b.b
    pha
    plb
    rep #$20
    lda 0,x
    plb
    and #$03FF                                          ; to have only tile number (no flipx/y)
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

    ldx objtmp2
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicm1dtstxnd

    and #$ff00											; keep only the high values for collision
    beq _oicm1dtstxnd

    lda objbuffers.1.xvel+1,x
    ora #$ff00
    clc
    adc objbuffers.1.xpos+ 1,x
    clc
    adc objbuffers.1.xofs,x
    clc
    adc #8
    and #$fff8
    sec
    sbc objbuffers.1.xofs,x
    sta objbuffers.1.xpos+ 1,x
    stz objbuffers.1.xvel,x

    brl _oicm1dend

_oicm1dtstxnd:
    tya
    clc
    adc.l maprowsize
    dec	objtmp1
    bne _oicm1dtstxnc				                        ; go through all y available

_oicm1dend:
	ply
	plx
	plb
	plp
	rtl

.ENDS

.SECTION ".objects3_text" superfree

;---------------------------------------------------------------------------------
; void objLoadObjects(u8 *sourceO)
objLoadObjects:
	php
    phb

    phx
    phy

	sep	#$20
	lda	#$0
	pha
	plb

	rep	#$20							                ; load objects in temporary buffer
    lda	10,s
	sta.l	$4302                                       ; data offset of source
    lda #(OB_MAX*5)+1
    asl a
    sta.l $4305                                         ; 64*5+1 *2 values
    ldy #objtmpbuf.w                                    ; data offset of destination
    sty $2181

    sep	#$20
    lda #$7e
    sta.l $2183                                         ; bank address of destination
    lda	12,s
    sta.l	$4304                                       ; bank address of source
    ldx	#$8000
	stx	$4300

    lda #$01
    sta $420B

	sep	#$20
	lda	#$7e
	pha
	plb

	rep #$20
	ldx #$0000

_oilo1:
    lda objtmpbuf,x			                            ; if end , get out
    cmp #$ffff
    beq _oiloend
    lda objtmpbuf+8,x		                        	; get maxx
    pha
    lda objtmpbuf+6,x		                        	; get minx
    pha
    lda objtmpbuf+4,x		                        	; get type
    pha
    lda objtmpbuf+2,x			                        ; get y
    pha
    lda objtmpbuf,x				                        ; get x
    pha
    lda objtmpbuf+4,x			                        ; get type again
    asl a
    asl a
    tay                                                 ; because init function is for each type
    inx
    inx
    inx
    inx
    inx
    inx
    inx
    inx
    inx
    inx
    stx objcidx					                        ; save x
    lda objfctinit,y		                            ; get address of init function and call it
    sta objfctcall
    lda objfctinit+2,y
    sta objfctcallh

    jsl jslcallfct

    pla
    pla
    pla
    pla
    pla
    ldx objcidx					; get saved x
    bra _oilo1

_oiloend:
	ply
	plx
	plb
	plp
	rtl

.ENDS

.SECTION ".objects4_text" superfree

;---------------------------------------------------------------------------------
; u16 objCollidObj(u16 objhandle1, u16 objhandle2);
; 5-6 7-8
objCollidObj:
    php
    phb

    phx
    phy

    sep #$20                                            ; go to object bank
    lda #$7e
    pha
    plb

	rep #$20
	stz.w tcc__r0                                       ; no collision yet

	lda 12,s											; grad the index of object 2 (7+1+2+2)
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tay
	lda objbuffers.1.xpos+1,y
	clc
	adc objbuffers.1.xofs,y
	bmi _oicoend							            ; no collision checking if second is not on screen
	sta objtmp1

	lda 10,s											; grad the index of object 1 (5+1+2+2)
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax
	lda objbuffers.1.xpos+1,x			                ; same computation as object 2
	clc
	adc objbuffers.1.xofs,x
	bpl _oicor1						                    ; if negative, will be 0
	lda #$0000
_oicor1:
	sta objtmp2

	cmp objtmp1											; objleft
	bcs _oicor2
	adc objbuffers.1.width,x

	cmp objtmp1
	bmi _oicoend
	bra _oicor3

_oicor2:
	lda objtmp1
	clc
	adc objbuffers.1.width,y
	cmp objtmp2
	bmi _oicoend

_oicor3:
	lda objbuffers.1.ypos+1,y
	clc
	adc objbuffers.1.yofs,y
	sta objtmp3

	lda objbuffers.1.ypos+1,x			                ; same computation
	clc
	adc objbuffers.1.yofs,x
	bpl _oicor4         						        ; if negative, will be 0
	lda #$0000
_oicor4:
	sta objtmp4

	cmp objtmp3
	bcs _oicor5
	adc objbuffers.1.height,x
	cmp objtmp3
	bmi _oicoend

_oicor5:
	lda objtmp3
	clc
	adc objbuffers.1.height,y
	cmp objtmp4
	bmi _oicoend

	lda #$0001                                         ; here, we have collision
	sta.w tcc__r0

_oicoend:

	ply
	plx
	plb
	plp
	rtl

.ENDS

.SECTION ".objects5_text" superfree

;---------------------------------------------------------------------------------
; void objUpdateXY(u16 objhandle)
objUpdateXY:
    php
    phb

    phx

    sep #$20                                            ; go to bank object
    lda #$7e
    pha
    plb

	rep #$20
	lda 8,s                                             ; grab the index of object (5+1+2)
	asl a
	asl a
	asl a
	asl a
	asl a
	asl a
	tax

    clc
    lda objbuffers.1.xvel,x                             ; xvel is negative
    bpl _oicuxy2

    adc objbuffers.1.xpos,x
    sta objbuffers.1.xpos,x
    bcs _oicuxy1

    sep #$20
    dec objbuffers.1.xpos+2,x
    rep #$20
    brl _oicuxy1

_oicuxy2:
    adc objbuffers.1.xpos,x                             ; xvel is positive
    sta objbuffers.1.xpos,x
    bcc _oicuxy1
    sep #$20
    inc objbuffers.1.xpos+2,x
    rep #$20

_oicuxy1:
    clc
    lda objbuffers.1.yvel,x                             ; same thing for yvel
    bpl _oicuxy3                                   ; yvel is negative

    adc objbuffers.1.ypos,x
    sta objbuffers.1.ypos,x
    bcs _oicuxyend

    sep #$20
    dec objbuffers.1.ypos+2,x
    rep #$20
    brl _oicuxyend

_oicuxy3:
    adc objbuffers.1.ypos,x
    sta objbuffers.1.ypos,x
    bcc _oicuxyend
    sep #$20
    inc objbuffers.1.ypos+2,x
    rep #$20

_oicuxyend:
    plx
    plb
    plp
    rtl

.ENDS

.SECTION ".objects6_text" superfree

.accu 16
.index 16
.16bit

_lutcol: ; x - height lut for normal slopes
    .db $00,$01,$02,$03,$04,$05,$06,$07     ; 0020
    .db $07,$06,$05,$04,$03,$02,$01,$00     ; 0021
    .db $04,$04,$05,$05,$06,$06,$07,$07     ; 0022
    .db $07,$07,$06,$06,$05,$05,$04,$04     ; 0023
    .db $00,$00,$01,$01,$02,$02,$03,$03     ; 0024
    .db $03,$03,$02,$02,$01,$01,$00,$00     ; 0025
_lutcolInv:; x - height lut for h-flipped slopes
    .db $07,$06,$05,$04,$03,$02,$01,$00     ; 0020
    .db $00,$01,$02,$03,$04,$05,$06,$07     ; 0021
    .db $07,$07,$06,$06,$05,$05,$04,$04     ; 0022
    .db $04,$04,$05,$05,$06,$06,$07,$07     ; 0024
    .db $03,$03,$02,$02,$01,$01,$00,$00     ; 0023
    .db $00,$00,$01,$01,$02,$02,$03,$03     ; 0025

;note: objtmp1                              ; tileCount
;note: objtmp2                              ; object handle
;note: objtmp3                              ; tileprop
;note: objtmp4                              ; enable slope /
;note: tcc__r0 $00                          ; centerX
;note: tcc__r1 $02                          ; tile difference
;note: tcc__r2 $04                          ; centerX relative to tile
;note: tcc__r3 $06                          ; bottom spot
;note: tcc__r4 $08                          ; tileFlipState
;note: tcc__r5 $0A                          ; slope correction value

;---------------------------------------------------------------------------------
; Macro definition for slope management

.MACRO OE_SETVELOCYTY
    lda objbuffers.1.yvel,x
    clc
    adc #GRAVITY
    cmp #MAX_Y_VELOCITY+1
    bmi \1                                  ; add velocity regarding if we do not reach the maximum
    lda #MAX_Y_VELOCITY
.ENDM

.MACRO OE_GETTILEPROP                       ; arg1 == 1 store flipstate, Accumulator must contain value from mapadrrowlut
    clc
    adc maptile_L1d                         ; get direct rom
    tax
    phb                                     ; push current bank to stack
    sep #$20                                ; 8bit mode
    lda maptile_L1b.b                       ; load rom bank address from maptile definitions
    pha                                     ; push rom bank address to stack
    plb                                     ; set net bank address
    rep #$20                                ; 16 bit mode
    lda 0,x                                 ; get maptile
    plb                                     ; restore bank
    .if \1 == 1
        pha
        and #$C000                          ; store flip state
        sta $08
        pla
    .endif
    and #$03FF                              ; to have only tile number (no flipx/y)
	asl a                                   ; to have a 16 bit value for 8 pix
    tax                                     ; maptile to x
    lda	metatilesprop, x                    ; get tile type solid, empty...
.ENDM

.MACRO OE_GETYOFFSETLUT                     ; A must contain some y coordinate
    bpl + 						            ; if y<0, put it to 0 *FIX 221130 *
	lda #0000
+   lsr a
    lsr a
    and	#$FFFE                              ; mask to wordsize
.ENDM

.MACRO OE_SETCENTERXNEW
    OE_SETCENTERXOLD

    lda objbuffers.1.xvel,x
    bpl +
    lda objbuffers.1.xvel+1,x
    ora #$FF00
    bra ++
+   lda objbuffers.1.xvel+1,x
    and #$00ff
    bne ++
    inc a                                   ; fix pushback when walking right
++  clc
    adc	$00                                 ; new hor center position
    sta $00                                 ; store centerX
.ENDM

.MACRO OE_SETCENTERXOLD
    lda objbuffers.1.width,x                ;load width
    lsr a                                   ;half width to calc bottom tile on middle bottom position
    clc
    adc objbuffers.1.xpos+1,x
    clc
    adc	objbuffers.1.xofs,x                 ; new Left position

    sta $00                                 ; store centerX
.ENDM


.MACRO OE_CPTETILENUM                       ; arg 1 == 1 reduce 1 check, arg 2 width or height
    pha
    and	#$0007                              ; compute the number of tiles to test
    clc
    adc \2                                  ; add width or height for right or bottom side
    dec a                                   ;
    lsr a                                   ;
    lsr a                                   ;
    lsr a                                   ;
    inc a                                   ;

    .if \1 == 1
    sec                                     ; if slope enabled reduce one horizonmtal check
    sbc objtmp4
    bpl +
    lda #$0000
    +:
    .endif

    sta	objtmp1                             ; store tilecount for testing to objtmp1
    pla                                     ; compute the offset line for y
    lsr a
    lsr a
    and	#$FFFE
.ENDM

.MACRO OE_SETYPOS                           ; arg == 1 bottom else 0 top
    lda objbuffers.1.yvel+1,x               ; load vel y to a   ;; calc new position
    .if \1 == 1                             ; arg == 1 bottom else top
        and #$00ff
    .else
        ora #$ff00
    .endif
    clc
    adc objbuffers.1.ypos+1,x               ;add pos to a
    clc                                     ; clear carry        \
    adc objbuffers.1.yofs,x                 ; add offset y to a   |  for avoid rounding errors

    .if \1 == 1                             ; arg == 1 bottom else 0 top
    clc                                     ; clear carry         |
    adc objbuffers.1.height,x               ; add heigth to a    /
    inc a                                   ; a +=1
    .else
        clc
        adc #8
    .endif

    and #$fff8                              ; mask to blocksize
    sec                                     ; set carry                 \
    sbc objbuffers.1.yofs,x                 ; subtract  offset y from a  |  for avoid rounding errors

    .if \1 == 1                             ; arg == 1 bottom else 0 top
        sec                                 ; set carry                  |
        sbc objbuffers.1.height,x           ; subtract  height y from a /
    .endif
    sta objbuffers.1.ypos+1,x
    stz objbuffers.1.yvel,x
.ENDM

.MACRO OE_ADDX                              ; arg == 1 right else 0 left
    lda objbuffers.1.xvel+1,x               ; load vel y to a   ;; calc new position
    .if \1 == 1                             ; arg == 1 right else 0 left
        and #$00ff
    .else
        ora #$ff00
    .endif
    clc
    adc objbuffers.1.xpos+1,x               ;add pos to a
    clc                                     ; clear carry        \
    adc objbuffers.1.xofs,x                 ; add offset y to a   |  for avoid rounding errors

    .if \1 == 1                             ; arg == 1 right else 0 left
    clc                                     ; clear carry         |
    adc objbuffers.1.width,x                ; add width to a    /
    .endif
.ENDM


.MACRO OE_SETXPOS                           ; arg == 1 right else 0 left
    OE_ADDX \1
    .if \1 == 1
    inc a                                   ; a +=1
    .else
        clc
        adc #8
    .endif

    and #$fff8                              ; mask to blocksize
    sec                                     ; set carry                 \
    sbc objbuffers.1.xofs,x                 ; subtract  offset y from a  |  for avoid rounding errors

    .if \1 == 1                             ; arg == 1 bottom else 0 top
        sec                                 ; set carry                  |
        sbc objbuffers.1.width,x            ; subtract  width x from a /
    .endif
    sta objbuffers.1.xpos+1,x
    stz objbuffers.1.xvel,x
.ENDM

.MACRO OE_SETOBJHANDLE_STK                  ; arg 1 = stackaddress
	lda \1,s					            ; grad the index of object (5+1+2+2)
    xba
    lsr a
    lsr a
	tax                                     ; transfer obj index to X
    sta objtmp2
.ENDM

;---------------------------------------------------------------------------------
; void objCollidMapWithSlopes(u16 objhandle)
objCollidMapWithSlopes:
    php
	phb
	phx
    phy
	sep #$20
	lda #$7e
	pha
	plb
    rep #$20                         ; A 16 bits
    stz  $0A

    OE_SETOBJHANDLE_STK 10
    stz objtmp4                             ; disable slope logic -> 0

_oicmsPrecheck
    ldx objtmp2                             ; load objecthandle
    lda objbuffers.1.yvel,x                 ; yvel
	bpl	_oicmsCheckSlopes1                  ; branch positive Y check >=0 -> if object is falling
	jmp _oicmsCheckXvelNotZero              ; continue without slopes

_oicmsCheckSlopes1                          ; calculate Middlebottom
    stz objtmp1                             ; zero out objtmp1 for
    OE_SETCENTERXOLD                        ; set center x
    lda objbuffers.1.ypos+1,x               ; load y pos
    clc
    adc objbuffers.1.yofs,x                 ; add y offs
    clc
    adc objbuffers.1.height,x               ; add height
    sta $06                                 ; store bottom spot
    dec a                                   ; one pixel up

    OE_GETYOFFSETLUT
    sta $02                                 ; presave tile difference
    tay                                     ; transfer it to y

_oicmsCheckSlopes11
    lda $00                                 ; load centerX
    lsr a
    lsr a
    and	#$FFFE
    clc
    adc.w mapadrrowlut, y
    tay
    OE_GETTILEPROP 0
    sta objtmp3

_oicmsCheckSlopes12
    and #T_SLOPES                           ; check if its slope
    beq ++                                  ; if not next try

    ldx objtmp2                             ; load object handle
    lda objtmp1
    bne +                                   ; if 0 check if new Y pos is on same tile
    lda objbuffers.1.yvel+1,x               ; load  handle
    and #$00ff
    clc
    adc  $06                                ; add bottom spot
    dec a
    OE_GETYOFFSETLUT
    tay
    cmp  $02                                ; --\ if new Y Tile == old Y Tile
    beq _oicmsEnableResolveSlope            ;    |  leave check
    lda objbuffers.1.yvel+1,x               ;    |
    and #$FF07                              ;    |
    stz objbuffers.1.yvel+1,x               ;   /   otherwise zero yvel out

+   bra _oicmsEnableResolveSlope            ; enable resolve slopes

++  lda objtmp1                             ; load tilecount
    bne _oicmsCheckXvelNotZero              ; tilecount is not 0

    lda objbuffers.1.yvel+1,x
    and #$00ff
    clc
    adc $06                                 ; add bottomspot
    dec a
    OE_GETYOFFSETLUT
    tay

    inc objtmp1
    brl _oicmsCheckSlopes11

_oicmsEnableResolveSlope:                   ; enable slope resolve
    inc objtmp4                             ;

_oicmsCheckXvelNotZero:                     ; check xvel != 0
	ldx objtmp2                             ; load objecthandle
	lda objbuffers.1.xvel,x                 ; if xvel>=0 -> moving right
	bne +
    brl _oicmsCheckYvelDirection            ; branch 0
+   bpl	_oicmsXRightReduceFriction                         ; xvel > 0 moving right
    jmp _oicmsXLeftReduceFriction

_oicmsXRightReduceFriction:                 ; moving right
    sec
    sbc objfriction                         ; reduce xvel by friction
    bpl _oicmststx12                        ; xvel still > 0 ?
    stz objbuffers.1.xvel,x                 ;
    jmp _oicmsCheckYvelDirection

_oicmststx12:                               ; resolve
    sta objbuffers.1.xvel,x                 ; store new xvel with reduced objfriction
	lda objbuffers.1.ypos+1,x               ; load y position
	clc
	adc objbuffers.1.yofs,x                 ; add y offset
	bpl + 							        ; if y<0, put it to 0 *FIX 20221201 */
	lda #0000

+	OE_CPTETILENUM 1 objbuffers.1.height,x
    bne +
    jmp _oicmsCheckYvelDirection;

+   tay                                     ; tile number to Y register
    ;Brain: CheckRight
    OE_ADDX 1
    lsr a
    lsr a
    and	#$fffe
    clc
    adc.w mapadrrowlut, y

_oicmsXResolveColRight:                     ; Apply Collision Right
    tay
    OE_GETTILEPROP 0

    ldx objtmp2                             ; load objecthandle
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicmsXSelectNextTile
    and #$ff00								; keep only the high values for collision
    beq _oicmsXSelectNextTile

    OE_SETXPOS 1                            ; arg == 1 resolve right collision
    brl _oicmsCheckYvelDirection

_oicmsXSelectNextTile:
    tya                                     ; go through all y available
    clc
    adc.l maprowsize
    dec	objtmp1
    bne _oicmsXResolveColRight
    brl _oicmsCheckYvelDirection

_oicmsXLeftReduceFriction:                  ; moving left (xvel<0)
    clc
    adc objfriction
    bmi _oicmststxna
    stz objbuffers.1.xvel,x                 ; currently it is not ok to go left
    brl _oicmsCheckYvelDirection

_oicmststxna:
    sta objbuffers.1.xvel,x
    lda objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
	bpl + 									; if y<0, put it to 0 *FIX 20221201 */
	lda #0000

+   OE_CPTETILENUM 1 objbuffers.1.height,x;
    ;Brain: CheckLeft
    tay

    OE_ADDX 0
    bpl _oicmststxnb                        ; if negative, don't bother to test, will change screen
    jmp _oicmsCheckYvelDirection

_oicmststxnb:
    lsr a
    lsr a
    and	#$fffe
    clc
    adc.w mapadrrowlut, y

_oicmststxnc:                               ; Apply Collision Left
    tay
    OE_GETTILEPROP 0
    ldx objtmp2                             ; load objecthandle
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicmststxnd

    and #$ff00								; keep only the high values for collision
    beq _oicmststxnd
    OE_SETXPOS 0                            ; arg == 0 resolve left collision

    brl _oicmsCheckYvelDirection

_oicmststxnd:
    tya
    clc
    adc.l maprowsize
    dec	objtmp1
    bne _oicmststxnc				        ; go through all y available
    brl _oicmsCheckYvelDirection

_oicmsResolveSlope1:
    lda objbuffers.1.yvel+1,x
    and #$00ff
    clc
    adc $06                                 ; add bottom spot
    dec a
    sta objtmp4

    OE_GETYOFFSETLUT
    tay                                     ; transfer it to y
    OE_SETCENTERXNEW                        ; calculate
    lsr a
    lsr a
    and	#$FFFE
    clc
    adc.w mapadrrowlut, y
    tay
    OE_GETTILEPROP 1                        ; arg1 == 1 store tile flipstate
    sta objtmp3
    cmp #$0030
    bcs _oicmsPrepYvelDown
    cmp #$0020
    bcc _oicmsPrepYvelDown

_oicmsResolveSlope2:
    lda $00                                 ; load centerX
    and #$0007
    sta $04                                 ; store centerX relative to tile

    lda objtmp3                             ; load tileprop index
    sec
    sbc #T_SLOPES                           ; set index of _lutcol 0-5
    asl a                                   ; \
    asl a                                   ;  | offseting for _lutcol address
    asl a                                   ; /
    adc $04                                 ; offsets to tileperfect
    tax                                     ; transfer index _lutcol + tileoffset to x register

    lda $08                                 ; load flip state
    sep #$20                                ; set to 8 bit
    xba                                     ; swap High to Low
    and #$40                                ; Check if tile is horizontaly flipped
    beq +
    lda.l _lutcol,x                         ; get Slope y value on x pixel not flipped
    bra ++
+   lda.l _lutcolInv,x                      ; get Slope y value on x pixel flipped
++  sta $0A                                 ; store slope correction value

_oicmsResolveSlope3:
    ldx objtmp2
    lda objtmp4                             ; posY

    and #$07
    cmp $0A                                 ; compare to correction value
    rep #$20
    bcs +
    brl _oicms61

 +  lda objtmp3
    sta objbuffers.1.tilestand,x                           ; store the tile we are standing on

    lda objtmp4
    and #$fff8
    sec                                     ; set carry                 \
    sbc objbuffers.1.yofs,x                 ; subtract  offset y from a  |  for avoid rounding errors
    sec                                     ; set carry                  |
    sbc objbuffers.1.height,x               ; subtract  height y from a /
    sep #$20
    clc
    adc $0A                                 ; slope correction value
    rep #$20
    inc a

    sta objbuffers.1.ypos+1,x
    stz objbuffers.1.yvel,x
    ;jmp	_oicmsend
    brl _oicmsend

_oicmsCheckYvelDirection:
    ldx objtmp2                             ; restore object index to
    lda objtmp4                             ; if slope resolve is set
    beq +
    brl _oicmsResolveSlope1
 +  lda objbuffers.1.yvel,x
    bpl	_oicms11                            ; yvel > 0 moving down
    jmp _oicmststyn                         ; yvel moving up

_oicmsPrepYvelDown:
    rep #$20
    ldx objtmp2
    stz objtmp4
    lda objbuffers.1.yvel,x

_oicms11:
    xba
    and	#$00FF
    clc
    adc	objbuffers.1.ypos+1,x               ; add y pos
    clc
    adc objbuffers.1.yofs,x                 ; add y offs
    clc
    adc objbuffers.1.height,x               ; add                         ; a contains y velocity so enw ypos
	bpl + 						            ; if y<0, put it to 0 *FIX 221130 *
	lda #0000

+   lsr a
    lsr a
    and	#$FFFE                              ; only even numbers
    tay                                     ; transfer it to y

    lda objbuffers.1.xpos+1,x               ; load x pos
    clc
    adc	objbuffers.1.xofs,x                 ; add offset (left edge of object)
    OE_CPTETILENUM 0 objbuffers.1.width,x
    clc
    adc.w mapadrrowlut, y                   ; add row

    tay
    OE_GETTILEPROP 0
    sta objtmp3
    bra _oicms21                            ; goto tile type check

_oicms2:
    tya
    OE_GETTILEPROP 0
    sta objtmp3

_oicms21:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDER							; if ladder, well avoid doing stuffs
	beq _oicms3
	cmp #T_PLATE							; if on a plate, do same thing
	beq _oicms3

    cmp #T_FIRES                            ; if fire, player is burning
    bne _oicms22
    lda #ACT_BURN							; player is now dying, no more checking
    sta objbuffers.1.action
    jmp  _oicmsend

_oicms22:
    cmp #T_SPIKE                            ; if spike, player is dying too
	bne _oicms23
	lda #ACT_DIE
	sta objbuffers.1.action
	jmp  _oicmsend

_oicms23:                                   ; check tile collision
    and #$ff00					            ; keep only the high values for collision
    beq _oicms4                             ; if no collision

_oicms3:                                    ; positioning on  bottom collision
    lda objtmp3
    sta objbuffers.1.tilestand,x            ; store the tile we are standing on
    OE_SETYPOS 1                            ;
    jmp	_oicmsend

_oicms4:                                    ; increment Y-Register for next tile check on x-axis
    rep #$20                                ; continue to test the tiles on x
    iny                                     ; y +=1
    iny                                     ; y +=1
    dec	objtmp1                             ; tilecount on x to check -= 1
    beq _oicms5
    brl _oicms2

_oicms5:
    ldx objtmp2                             ; not standing on anything or ladder, now falling if not climbing
	stz objbuffers.1.tilestand,x

    lda objbuffers.1.tilesprop,x            ; need again to verify if not tile standing
    beq _oicms61
    lda objbuffers.1.action,x               ; if not climbing, well doing like if falling
    and #ACT_CLIMB
    beq _oicms61
    brl  _oicmsend

_oicms61:
    OE_SETVELOCYTY _oicms6

_oicms6:
    sta objbuffers.1.yvel,x
	brl _oicmsend

_oicmststyn:					            ; yvel<0, object is moving upwards----------------------------
    stz objbuffers.1.tilestand,x            ;  zero out "tilestand"
    xba
    ora	#$FF00
    clc
    adc	objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
	bpl +     					            ; if y < 0, put it to 0  *FIX 221126*

	lda #0000
+   lsr a
    lsr a
	and	#$FFFE
	tay

    lda objbuffers.1.xpos+1,x
    clc
    adc	objbuffers.1.xofs,x

    OE_CPTETILENUM 0 objbuffers.1.width

    clc
    adc.w mapadrrowlut, y
    tay

    OE_GETTILEPROP 0
    bra	_oicmststyn2		                ; speedup, saves some cycles

_oicmststyn1:
    tya
    OE_GETTILEPROP 0

_oicmststyn2:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDER							; if ladder, well avoid doing stuffs
    beq _oicmststyn4

    and #$ff00								; keep only the high values for collision
    beq _oicmststyn3

    OE_SETYPOS 0 ;arg 0 == top correction
    bra _oicmststyn4

_oicmststyn3:                               ; continue to test next tile on x axis
    iny                                     ; increase Y Register for next
    iny
    dec	objtmp1                             ; decrease tilecount
    bne _oicmststyn1                        ; if tilecount not 0 jump to _oicmststyn1 for next collision test

_oicmststyn4:                               ; not standing on anything, now falling by adding gravity
    ldx objtmp2                             ; load objecthandle
    OE_SETVELOCYTY _oicmststyn5
_oicmststyn5:
    sta objbuffers.1.yvel,x

_oicmsend:
	ply
	plx
	plb
	plp
	rtl
.ENDS
