
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; hardware registers for pocket voice memory controller
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.DEFINE PV_SRAM_CTRL $0000
.DEFINE PV_BANK_L    $6000
.DEFINE PV_BANK_H    $7000
.DEFINE PV_SC_CMD    $A000
.DEFINE PV_STAT      $A000
.DEFINE PV_SC_DATA   $A001
.DEFINE PV_ROM_OFFS  $A003

.DEFINE PV_CMD_ENABLE_SC $80
.DEFINE PV_CMD_SMPL_PLAY $81
.DEFINE PV_CMD_SMPL_REC  $82

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; memory map for pocket voice memory controller
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.MEMORYMAP
DEFAULTSLOT 2
SLOTSIZE $6000
SLOT 1 $0000
SLOTSIZE $2000
SLOT 2 $6000
.ENDME

;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»
; rom bank map for the pocket voice cartridge
;»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»»

.ROMBANKMAP
BANKSTOTAL 510
BANKSIZE $6000
BANKS 1
BANKSIZE $2000
BANKS 509
.ENDRO
