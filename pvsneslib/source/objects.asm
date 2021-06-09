;---------------------------------------------------------------------------------
;
;	Copyright (C) 2012-2021
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

.DEFINE OB_NULL			    $FFFF		    ; null value for linked table
.DEFINE OB_ID_NULL	        $0000		    ; null value for obj id

.DEFINE OB_MAX				64		        ; total number of objects in the game
.DEFINE OB_TYPE_MAX		    32		        ; total number of type of objects in the game

.DEFINE OB_SIZE				64		        ; 64 bytes for each object

.DEFINE OB_SCR_XLR_CHK      -64             ; -64 is the minimum on "virtual" screen x position to update object
.DEFINE OB_SCR_XRR_CHK      320             ; 256+64 is the maximum on "virtual" screen x position to update object
.DEFINE OB_SCR_YLR_CHK      -64             ; -64 is the minimum on "virtual" screen y position to update object
.DEFINE OB_SCR_YRR_CHK      288             ; 224+64 is the maximum on "virtual" screen y position to update object

.DEFINE T_SOLID				$FF00
.DEFINE T_LADDE				$0001
.DEFINE T_FIRES				$0002
.DEFINE T_SPIKE				$0004

.DEFINE ACT_WALK			$0001
.DEFINE ACT_JUMP			$0002
.DEFINE ACT_FALL			$0004
.DEFINE ACT_CLIMB			$0008
.DEFINE ACT_DIE				$0010
.DEFINE ACT_BURN			$0020

.DEFINE GRAVITY       	    41
.DEFINE MAX_Y_VELOCITY      (10*256) 
.DEFINE FRICTION            $10

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
onscreen	    DB			                ; 52 if object is on screen on not

objnotused      DSB 11                      ; OB_SIZE-52-1 for future use

.ENDST

.RAMSECTION ".reg_objects7e" bank $7E

objbuffers      INSTANCEOF t_objs OB_MAX    ; object struct in memory
objactives      DSW OB_TYPE_MAX				; active object list

objfctinit      DSB 4*OB_TYPE_MAX			; pointer to init function of objects
objfctupd	    DSB 4*OB_TYPE_MAX			; pointer to update function of object

objtmpbuf	    DSW (OB_MAX*5)+1			; temporary buffer for object 

objunused       DW							; buffer of unused objects
objnewid	    DW							; new id created for the last object
objgetid	    DW							; return value of objNew
objnextid	    DB							; each new object is given a unique nID

objfctcall	    DSB 2						; low address for C function call
objfctcallh	    DSB 2						; high address for C function call

objptr		    DW							; pointer to current object
objcidx		    DW							; index of loop object

objtokill       DB							; =1 if need to kill object

objtmp1	DW									; temporary vars for stuffs
objtmp2	DW
objtmp3	DW
objtmp4	DW

.ENDS

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
	
	ply
	plx
	plb 
	plp
	
	rtl

;---------------------------------------------------------------------------------
; void objInitFunctions(u8 objtype, void *initfct,void *updfct)
; 5 6-9 10-13 
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

    lda 9,s												; get bank + data init fonction address (6+1+2 & 8+1+2) 
    sta objfctinit,x
    lda 11,s
    sta objfctinit+2,x

    lda 13,s										    ; get bank + data update fonction address (10+1+2 & 12+1+2) 
    sta objfctupd,x
    lda 15,s
    sta objfctupd+2,x

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

;    lda objbuffers.1.xpos+1,x
;    sec
;    sbc.l x_pos

;    cmp.w #SCR_RR_CHK
;    bcc _objUpdAllRep312      ; IF_LE (C set if A>=SCR_RR_CHK)
;    cmp.w #SCR_LR_CHK
;    bcs _objUpdAllRep312      ;    IF_GE (bmi)

;    sep #$20
;    lda objbuffers.1.onscreen,x
;    beq _objUpdAllRep311
;    lda #$1
;    sta objrefresh
;    stz objbuffers.1.onscreen,x

;_objUpdAllRep311:            
;    rep #$20
;    bra _oial31
            
;_objUpdAllRep312:            
    sep #$20
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
			
    lda objtokill										; object returned that it is now dead
    beq _oial4

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
    lda objbuffers.1.nID,x
    rep #$20
    and #$00ff
    xba
    clc
    adc objcidx
    pha
    jsl objKill
    pla
			
_oial31:
    pla
    brl _oiual10

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
    lsr a
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
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
    sta objtmp3

_oicm21:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDE										; if ladder, well avoid doing stuffs
	beq _oicm3

    cmp #T_FIRES                                        ; if fire, player is burning
    bne _oicm22
    lda #ACT_BURN										; player is now dying, no more checking
    sta objbuffers.1.action
    brl  _oicmend

_oicm22:
    cmp #T_SPIKE                                        ; if spike, player is dying too
	bne _oicm23
	lda #ACT_DIE																
	sta objbuffers.1.action
	brl  _oicmend

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
    beq _oicm6
    lda objbuffers.1.action,x				            ; if not climbing, well doing like if falling
    and #ACT_CLIMB
    beq _oicm6
    brl  _oicmtstx

    lda objbuffers.1.yvel,x
    clc
    adc #GRAVITY
    cmp #MAX_Y_VELOCITY+1                               ; add velocity regarding if we do not reach the maximum
    bmi _oicm6
    lda #MAX_Y_VELOCITY

_oicm6:
    sta objbuffers.1.yvel,x
	brl  _oicmtstx

_oicmtstyn:
    stz objbuffers.1.tilestand,x                           ; yvel<0, object is moving upwards

    xba
    ora	#$FF00
    clc
    adc	objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    lsr a
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
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x
		
_oicmtstyn2:
    ldx objtmp2
    sta objbuffers.1.tilesprop,x
    lda objbuffers.1.tilesprop,x
    cmp #T_LADDE										; if ladder, well avoid doing stuffs
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
    brl _oicmtstyn2

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
    sbc #FRICTION
    bpl _oicmtstx12
    stz objbuffers.1.xvel,x
    jmp _oicmend

_oicmtstx12:
    sta objbuffers.1.xvel,x
	lda objbuffers.1.ypos+1,x
	clc
	adc objbuffers.1.yofs,x
	pha
	
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
    
    lda #$1											    ; flag for unique test of tile for rope
    sta objtmp3
			
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
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

    ldx objtmp2
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicmtstx14
    stz objtmp3
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
    adc #FRICTION
    bmi _oicmtstxna
    stz objbuffers.1.xvel,x                             ; currently it is not ok to go left
    brl _oicmend
	
_oicmtstxna:
    sta objbuffers.1.xvel,x
    lda objbuffers.1.ypos+1,x
    clc
    adc objbuffers.1.yofs,x
    pha

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

    lda #$1											    ; flag for unique test of tile for rope
    sta objtmp3

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
    asl a                                               ; to have a 16 bit value for 8 pix
    tax
    lda	metatilesprop, x

    ldx objtmp2
    sta objbuffers.1.tilebprop,x
    lda objbuffers.1.tilebprop,x
    beq _oicmtstxnd

    stz objtmp3
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

.SECTION ".objects1_text" superfree

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

.SECTION ".objects2_text" superfree

;---------------------------------------------------------------------------------
; void objCollidObj(u16 objidx,u16 obj1idx)
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
	
	lda objtmp3
	clc 
	adc objbuffers.1.height,y
	cmp objtmp4
	bmi _oicoend
	
	lda #$0001                                         ; here, we have collsion
	sta.w objcollide
	
_oicoend:

	ply
	plx
	plb
	plp
	rtl

.ENDS    

.SECTION ".objects3_text" superfree

;---------------------------------------------------------------------------------
; void objUpdateXY(u16 objhandle)
objUpdateXY:
    php
    phb

    phx

    sep #$20                                            ; go to bank object
    lda #$7E
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