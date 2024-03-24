** to do **

There are two main types of SNES cartridges, the SNES community refers to them as LoROM and HiROM cartridges. Both have different memory mapping. 

## HiRom and LoRom

In LoROM mode, the ROM is always mapped in the upper half of each bank, thus 32 Kilobytes per chunk.  

The banks $00 – $7D (address: $8000 - $FFFF) hold continuous data, as well as banks $80 - $FF. SRAM on the cartridge is mapped continuously and repeatedly - 8 Kilobyte of SRAM are mapped at $0000 - $1FFF, $2000 – $3FFF, $4000 – $5FFF and so on.  

Because the WRAM of the SNES is mapped at bank $7E - $7F, these banks do not map to the last SRAM/ROM chunks. This memory has to be accessed via the banks $80 - $FF. There is no other way of accessing this memory both in LoROM and HiROM mode.

LoROM was established to make sure that the system banks ($00 – $3F) higher pages (>7) are actually used. This is done by loading the entire ROM only in higher pages and in 32 Kilobyte chunks. 32 KB * $80 banks == 4 Megabyte.

HiROM is a bit more complex to understand. Unlike LoROM, it does not use $80 (128) banks to map the ROM into the address space of the SNES, but only $40 (64) banks.  

Also unlike LoROM, these banks are used to their full extend, that is, 64 KB per chunk. 64 KB * $40 banks == 4 Megabytes. The banks $40 – $7D (address: $0000 - $FFFF) hold continuous data, as well as banks $C0 - $FF. 

Beware that HiROM also creates mappings for banks $00 – $3F and $80 - $BF. As those are system banks, their lower pages (<8) are already mapped - but the higher pages are free, so that many portions of the ROM are mirrored four times into the address space of the SNES. SRAM on the cartridge is mapped into the banks $20 – $3F, in 8 Kilobyte chunks.  

As there are only 32 banks reserved for this, the possible SRAM amount accessible in HiROM is theoretically lower than in LoROM (256 KB vs. 512 KB). Because the WRAM of the SNES is mapped at bank $7E - $7F, these banks do not map to the last ROM chunks. This memory has to be accessed via the $80 - $FF banks

Banks $80 - $FF can also be used for faster memory access. Many portions of memory <$80 are accessed at 2.68 MHz (200 ns). Accessing memory >$80 is done at 3.58 MHz (120 ns) if the value at address $420D (hardware register) is set to 1.

LoROM basically means that the address line A15 is ignored by the cartridge, so the cartridge doesn't distinguish between $0000-$7FFF and $8000-$FFFF in any bank. Smaller ROMs use this model to prevent wasted space in banks $00–$3F.

### FastRom
 
SNES games are designed to run under SlowROM (2.68 MHz) to FastROM (3.58 MHz). FastROM allows the SNES CPU read data and opcodes from the ROM 33.58% faster compared to SlowROM.  

Depending on the game, FastROM will make the game run about 10%-33% faster compared to the original SlowROM version. 

This depends on how frequent the game accesses the ROM chip, since the other componenets such as WRAM @ 2.68 MHz, PPU @ 3.58 MHz, DMA @ 2.68 MHz and SRAM @ 2.68 MHz will stay at the same speed.  

## HiRom and LoRom with PVSneslib

PVSneslib is shipped with a full set of lib binaries with 4 different configurations:
- LoROM - SlowROM (already present by default)  
- LoROM - FastROM (all sections with .BASE $80)  
- HiROM - SlowROM (core sections with .BASE $00 ORG $8000 and everything else with .BASE $40 ORG 0)  
- HiROM - FastROM (core sections with .BASE $80 ORG $8000 and everything else with .BASE $C0 ORG 0)  

Notice that BANKS $7E and $7F are for RAM use, so HiROM - SlowROM won't be able to use that banks for storing ROM.  
TCC compiler was changed to have some small functions that inyect .BASE directives before **.SECTIONS** and **.RAMSECTIONS**  

To use HiRom, you must specify it inside your makefile
```bash
export HIROM=1
```

To use FastRom, you must specify it inside your makefile
```bash
export FASTROM=1
```

# memory_mapping example to help you with coding

In this example, all the source code are in the **src** directory. It consists of only one file **memory_mapping.c**.

We declared that we are using Fast and Hi rom in **Makefile**, just before including **snes_rules**:
```bash
# Tell the compiler to export a Mode 21 (HiROM) memory mapped ROM
HIROM=1
# Tell the compiler to use FastROM. $4206 reg is enabled on start and reset, nmi and vectors jumps with an ofset of $80 banks
FASTROM=1

include ${PVSNESLIB_HOME}/devkitsnes/snes_rules
```
That's all, the source code is exactly the same as a LoRow/SlowRom source code.

Some parts of this article are from https://en.wikibooks.org/wiki/Super_NES_Programming/SNES_memory_map#LoROM.