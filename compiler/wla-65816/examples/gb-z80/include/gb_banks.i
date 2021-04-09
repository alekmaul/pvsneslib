
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; game boy ROM bank map. two ways to define the ROM bank mapping.
; if the banks are all equal in size, use .BANKSIZE and .ROMBANKS,
; otherwise use .ROMBANKMAP definition.
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

/*
.ROMBANKMAP 
BANKSTOTAL 4  
BANKSIZE $4000
BANKS 4
.ENDRO
*/

.ROMBANKSIZE $4000
.ROMBANKS 4
