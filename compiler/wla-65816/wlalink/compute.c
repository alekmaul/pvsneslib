
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "memory.h"
#include "compute.h"


extern unsigned char *rom;
extern int romsize, sms_checksum, gb_checksum, gb_complement_check, snes_checksum;
extern int snes_rom_mode;


int compute_checksums(void) {

  if (sms_checksum != 0)
    compute_sms_checksum();
  if (gb_complement_check != 0)
    compute_gb_complement_check();
  if (gb_checksum != 0)
    compute_gb_checksum();
  if (snes_checksum != 0)
    compute_snes_checksum();

  return SUCCEEDED;
}


int compute_gb_complement_check(void) {

  unsigned int i, x;

  if (romsize < 0x8000) {
    fprintf(stderr, "COMPUTE_GB_COMPLEMENT_CHECK: GB complement check computing requires a ROM of at least 32KB.\n");
    return FAILED;
  }

  i = 0;
  for (x = 0x134; x <= 0x14C; x++)
    i += rom[x];
  i += 25;
  mem_insert(0x14D, 0 - (i & 0xFF));

  return SUCCEEDED;
}


int compute_gb_checksum(void) {

  unsigned int i, x;

  if (romsize < 0x8000) {
    fprintf(stderr, "COMPUTE_GB_CHECKSUM: GB checksum computing requires a ROM of at least 32KB.\n");
    return FAILED;
  }

  i = 0;
  for (x = 0; x < 0x14E; x++)
    i += rom[x];
  for (x = 0x150; x < romsize; x++)
    i += rom[x];

  mem_insert(0x14E, (i >> 8) & 0xFF);
  mem_insert(0x14F, i & 0xFF);

  return SUCCEEDED;
}


int compute_snes_checksum(void) {

  unsigned int i, x, n, m, l, o;

  if (snes_rom_mode == SNES_ROM_MODE_LOROM) {
    if (romsize < 0x8000) {
      fprintf(stderr, "COMPUTE_SNES_CHECKSUM: SNES checksum computing for a LoROM image requires a ROM of at least 32KB.\n");
      return FAILED;
    }
  }
  else {
    if (romsize < 0x10000) {
      fprintf(stderr, "COMPUTE_SNES_CHECKSUM: SNES checksum computing for a HiROM image requires a ROM of at least 64KB.\n");
      return FAILED;
    }
  }

  /* n = data inside 4mbit blocks, m = data outside that */
  if (romsize < 512*1024) {
    n = romsize;
    m = 0;
  }
  else {
    n = (romsize/(512*1024))*512*1024;
    m = romsize - n;
  }

  /* sum all the bytes inside the 4mbit blocks */
  x = 0;
  for (i = 0; i < n; i++) {
    if (snes_rom_mode == SNES_ROM_MODE_LOROM) {
      /* skip the checksum bytes */
      if (!(i == 0x7FDC || i == 0x7FDD || i == 0x7FDE || i == 0x7FDF))
				x += rom[i];
    }
    else {
      /* skip the checksum bytes */
      if (!(i == 0xFFDC || i == 0xFFDD || i == 0xFFDE || i == 0xFFDF))
				x += rom[i];
    }
  }

  /* add to that the data outside the 4mbit blocks, ringbuffer style repeating 
     the remaining block until the the final part reaches 4mbits */
  for (o = 0, l = i; i < romsize; i++, o++)
    x += rom[(o % m) + l];

  /* 2*255 is for the checksum and its complement bytes that we skipped earlier */
  x += 2*255;

  /* compute the inverse checksum */
  l = (x & 0xFFFF) ^ 0xFFFF;

  /* insert the checksum bytes */
  if (snes_rom_mode == SNES_ROM_MODE_LOROM) {
    mem_insert(0x7FDC, l & 0xFF);
    mem_insert(0x7FDD, (l >> 8) & 0xFF);
    mem_insert(0x7FDE, x & 0xFF);
    mem_insert(0x7FDF, (x >> 8) & 0xFF);
  }
  else {
    mem_insert(0xFFDC, l & 0xFF);
    mem_insert(0xFFDD, (l >> 8) & 0xFF);
    mem_insert(0xFFDE, x & 0xFF);
    mem_insert(0xFFDF, (x >> 8) & 0xFF);
  }

  return SUCCEEDED;
}


int compute_sms_checksum(void) {

  unsigned int i, x;

  if (romsize < 0x8000) {
    fprintf(stderr, "COMPUTE_SMS_CHECKSUM: SMS/GG checksum computing requires a ROM of at least 32KBs.\n");
    return SUCCEEDED;
  }

  /* add together 32KB minus SMS/GG header */
  i = 0;
  for (x = 0; x < 0x7FF0; x++)
    i += rom[x];

  mem_insert(0x7FFA, i & 0xFF);
  mem_insert(0x7FFB, (i >> 8) & 0xFF);

  /* 32KB checksum and SMS export */
  mem_insert(0x7FFF, 0x4C);

  return SUCCEEDED;
}
