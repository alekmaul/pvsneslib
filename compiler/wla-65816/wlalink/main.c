
/*
	wlalink - part of wla dx gb-z80/z80/6502/65c02/6510/65816/huc6280/spc-700
	macro assembler package by ville helin <vhelin@iki.fi>. this is gpl software.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "main.h"
#include "memory.h"
#include "files.h"
#include "check.h"
#include "analyze.h"
#include "write.h"
#include "compute.h"
#include "discard.h"
#include "listfile.h"

/* define this if you want to display debug information */
/*
#define _MAIN_DEBUG
*/

#ifdef AMIGA
char version_string[] = "$VER: WLALINK 5.7 (01.01.2007)";
#endif

struct object_file *obj_first = NULL, *obj_last = NULL, *obj_tmp;
struct reference *reference_first = NULL, *reference_last = NULL;
struct section *sec_first = NULL, *sec_last = NULL, *sec_hd_first = NULL, *sec_hd_last = NULL;
struct stack *stacks_first = NULL, *stacks_last = NULL;
struct label *labels_first = NULL, *labels_last = NULL;
struct slot slots[256];
unsigned char *rom, *rom_usage, *file_header = NULL, *file_footer = NULL;
int romsize, rombanks, banksize, verbose_mode = OFF, section_overwrite = OFF, symbol_mode = SYMBOL_MODE_NONE;
int pc_bank, pc_full, pc_slot, pc_slot_max;
int file_header_size, file_footer_size, *banks = NULL, *bankaddress = NULL;
int output_mode = OUTPUT_ROM, discard_unreferenced_sections = OFF;
int program_start, program_end, sms_checksum, snes_rom_mode = SNES_ROM_MODE_LOROM, snes_rom_speed = SNES_ROM_SPEED_SLOWROM;
int gb_checksum, gb_complement_check, snes_checksum, cpu_65816 = 0, snes_mode = 0;
int listfile_data = NO, smc_status = 0, snes_sramsize = 0;
int no_std_libraries = OFF, object_file_parameters = OFF;

extern int emptyfill;



int main(int argc, char *argv[]) {

  int i, x, y, q, inz;
  struct section *s;
  float f;

  if (sizeof(double) != 8) {
    fprintf(stderr, "MAIN: sizeof(double) == %d != 8. WLALINK will not work properly.\n", (int)(sizeof(double)));
    return -1;
  }

  atexit(procedures_at_exit);

  i = SUCCEEDED;
  x = SUCCEEDED;

  if (argc > 1)
    x = parse_flags(argv[1]);
  else
    i = FAILED;

  if (x == FAILED && argc < 3)
    i = FAILED;
  if (x == SUCCEEDED && argc < 4)
    i = FAILED;

  if (i == FAILED) {
    printf("\nWLALINK GB-Z80/Z80/6502/65C02/6510/65816/HUC6280/SPC-700 WLA Macro Assembler Linker v5.7-pvs2\n");
    printf("Written by Ville Helin in 2000-2007\n");
#ifdef W65816
    printf("Modified for PVSneslib by Alekmaul in 2021\n");
#endif
    printf("USAGE: %s [-bdrvsSno] (<LINK FILE>|<OBJECT FILES...>) <OUTPUT FILE>\n", argv[0]);
    printf("Options:\n");
    printf("b  Program file output\n");
    printf("d  Discard unreferenced sections\n");
    printf("i  Write list files\n");
    printf("r  ROM file output (default)\n");
    printf("v  Verbose messages\n");
    printf("s  Write also a NO$GMB symbol file\n");
    printf("S  Write also a WLA symbol file\n");
    printf("n  Do not link standard libraries\n");
    printf("o  Take object files instead of link file\n\n");
    return 0;
  }

  /* load files */
  if (load_files(argv, argc, x == SUCCEEDED) == FAILED)
    return 1;

  /* check file types */
  if (check_file_types() == FAILED)
    return 1;

  /* check object headers */
  if (check_headers() == FAILED)
    return 1;

  /* obtain the amount of rom banks */
  if (obtain_rombanks() == FAILED)
    return 1;

  banks = malloc(sizeof(int) * rombanks);
  if (banks == NULL) {
    fprintf(stderr, "MAIN: Out of memory error.\n");
    return 1;
  }
  bankaddress = malloc(sizeof(int) * rombanks);
  if (bankaddress == NULL) {
    fprintf(stderr, "MAIN: Out of memory error.\n");
    return 1;
  }

  /* obtain rom bank map and check the project integrity */
  if (obtain_rombankmap() == FAILED)
    return 1;

  /* obtain memory map and check the project integrity */
  if (obtain_memorymap() == FAILED)
    return 1;

  /* calculate romsize */
  for (romsize = 0, x = 0; x < rombanks; x++)
    romsize += banks[x];

  /* obtain source file names used in compiling */
  if (obtain_source_file_names() == FAILED)
    return 1;

  /* collect all defines, labels and outside references */
  if (collect_dlr() == FAILED)
    return 1;

  /* take rom size and allocate memory */
  if (allocate_rom() == FAILED)
    return 1;

  /* parse data blocks */
  if (parse_data_blocks() == FAILED)
    return 1;

  /* clean up the structures */
  if (clean_up_dlr() == FAILED)
    return 1;

  /* drop all unreferenced sections */
  if (discard_unreferenced_sections == ON) {
    if (discard_unused_sections() == FAILED)
      return 1;
    /* remove dropped labels */
    discard_drop_labels();
  }

  /* correct 65816 library section addresses */
  if (cpu_65816 != 0) {
    if (correct_65816_library_sections() == FAILED)
      return 1;
  }

#ifdef _MAIN_DEBUG
  {
    printf("\n*********************************************\n");
    printf("JUST LOADED IN\n");
    printf("*********************************************\n\n");
  }

  {
    struct label *l;

    printf("LABELS:\n");
    l = labels_first;
    while (l != NULL) {
      printf("--------------------------------------\n");
      printf("name: \"%s\"\n", l->name);
      printf("sect: \"%d\"\n", l->section);
      printf("slot: \"%d\"\n", l->slot);
      printf("base: \"%d\"\n", l->base);
			printf("bank: \"%d\"\n", l->bank);
      printf("address: \"%d\"\n", (int)l->address);
      printf("status: \"%d\"\n", l->status);
      printf("file_id: \"%d\"\n", l->file_id);
      l = l->next;
    }
    printf("--------------------------------------\n");
  }

  {
    struct stack *s;

    printf("STACKS:\n");
    s = stacks_first;
    while (s != NULL) {
      printf("--------------------------------------\n");
      printf("result: \"%d\"\n", s->result);
      printf("id: \"%d\"\n", s->id);
      printf("file_id: \"%d\"\n", s->file_id);
      printf("bank: \"%d\"\n", s->bank);
      printf("linenumber: \"%d\"\n", s->linenumber);
      printf("type: \"%d\"\n", s->type);
      printf("position: \"%d\"\n", s->position);
      s = s->next;
    }
    printf("--------------------------------------\n");
  }
#endif

  /* insert sections */
  if (insert_sections() == FAILED)
    return 1;

#ifdef _MAIN_DEBUG
  {
    struct section *s;

    printf("SECTIONS:\n");
    s = sec_first;
    while (s != NULL) {
      printf("--------------------------------------\n");
      printf("file: \"%s\"\n", get_file_name(s->file_id));
      printf("name: \"%s\"\n", s->name);
      printf("id:   %d\n", s->id);
      printf("addr: %d\n", s->address);
      printf("stat: %d\n", s->status);
      printf("bank: %d\n", s->bank);
      printf("slot: %d\n", s->slot);
      printf("size: %d\n", s->size);
      s = s->next;
    }
    printf("--------------------------------------\n");
  }
#endif

  /* compute the labels' addresses */
  if (fix_labels() == FAILED)
    return 1;

  /* compute pending calculations */
  if (compute_pending_calculations() == FAILED)
    return 1;

#ifdef _MAIN_DEBUG
  {
    struct stack *s;

    printf("RESOLVED STACKS:\n");
    s = stacks_first;
    while (s != NULL) {
      printf("--------------------------------------\n");
      printf("result: \"%d\"\n", s->result);
      printf("id: \"%d\"\n", s->id);
      printf("file_id: \"%d\"\n", s->file_id);
      s = s->next;
    }
    printf("--------------------------------------\n");
  }
#endif

#ifdef _MAIN_DEBUG
  {
    struct reference *r;

    printf("REFERENCES:\n");
    r = reference_first;
    while (r != NULL) {
      printf("--------------------------------------\n");
      printf("name: \"%s\"\n", r->name);
      printf("file_id: \"%d\"\n", r->file_id);
      r = r->next;
    }
    printf("--------------------------------------\n");
  }
#endif

  /* transform computation stack definitions to ordinary definitions */
  if (transform_stack_definitions() == FAILED)
    return 1;

  /* fix references */
  if (fix_references() == FAILED)
    return 1;

  /* write checksums and other last minute data */
  if (compute_checksums() == FAILED)
    return 1;

  /* write rom file */
  if (write_rom_file(argv[argc - 1]) == FAILED)
    return 1;

#ifdef _MAIN_DEBUG
  {
    printf("\n*********************************************\n");
    printf("AFTER EVERYTHING\n");
    printf("*********************************************\n\n");
  }

  {
    struct label *l;

    printf("LABELS:\n");
    l = labels_first;
    while (l != NULL) {
      printf("--------------------------------------\n");
      printf("name: \"%s\"\n", l->name);
      printf("sect: \"%d\"\n", l->section);
      printf("slot: \"%d\"\n", l->slot);
      printf("base: \"%d\"\n", l->base);
      printf("address: \"%d\"\n", (int)l->address);
      printf("rom_address: \"%d\"\n", l->rom_address);
      printf("bank: \"%d\"\n", l->bank);
      printf("status: \"%d\"\n", l->status);
      printf("file_id: \"%d\"\n", l->file_id);
      l = l->next;
    }
    printf("--------------------------------------\n");
  }
#endif

  /* export symbolic information file */
  if (symbol_mode != SYMBOL_MODE_NONE) {
    if (write_symbol_file(argv[argc - 1], symbol_mode) == FAILED)
      return FAILED;
  }

  /* write list files */
  if (listfile_data == YES) {
    if (listfile_write_listfiles(sec_first) == FAILED)
      return FAILED;
  }

  /* show rom information */
  y = 0;
  if (verbose_mode == ON) {
    x = 0;
    for (i = 0; i < romsize; i++) {
      if (rom_usage[i] == 0 && x == 0) {
				x = 1;
				y = i;
      }
      else if (rom_usage[i] != 0 && x == 1) {
				if (y == (i - 1))
					fprintf(stderr, "Free space at $%.4x.\n", y);
				else
					fprintf(stderr, "Free space at $%.4x-$%.4x.\n", y, i - 1);
				x = 0;
      }
    }

    if (x == 1) {
      if (y == (i - 1))
				fprintf(stderr, "Free space at $%.4x.\n", y);
      else
				fprintf(stderr, "Free space at $%.4x-$%.4x.\n", y, i - 1);
    }

    for (y = 0, q = 0; y < romsize; q++) {
      for (x = 0, inz = 0; inz < banks[q]; inz++) {
				if (rom_usage[y++] == 0)
					x++;
      }
      f = (((float)x)/banks[q]) * 100.0f;
      if (f == 100.0f)
				printf("Bank %.2d has %.5d bytes (%.1f%%) free.\n", q, x, f);
      else
				printf("Bank %.2d has %.5d bytes (%.2f%%) free.\n", q, x, f);
    }

    /* ROM data */
    if (output_mode == OUTPUT_ROM) {
      for (i = 0, y = 0; i < romsize; i++) {
				if (rom_usage[i] == 0)
					y++;
      }

      fprintf(stderr, "%d unused bytes of total %d.\n", y, romsize);
      q = romsize;
    }
    /* program file data */
    else {
      for (i = program_start, y = 0; i < program_end; i++) {
				if (rom_usage[i] == 0)
					y++;
      }

      q = program_end - program_start + 1;
      fprintf(stderr, "%d unused bytes (%.2f%%) of total %d.\n", y, (((double)y) / q) * 100, q);
    }

    if (file_header_size != 0)
      fprintf(stderr, "File header size %d.\n", file_header_size);
    if (file_footer_size != 0)
      fprintf(stderr, "File footer size %d.\n", file_footer_size);

    if (smc_status != 0) {
      fprintf(stderr, "512 additional bytes from the SMC ROM header.\n");
      i = file_header_size + file_footer_size + 512;
    }
    else
      i = file_header_size + file_footer_size;

    s = sec_hd_first;
    while (s != NULL) {
      fprintf(stderr, "Bank %d header section size %d.\n", s->bank, s->size);
      i += s->size;
      s = s->next;
    }

    if (i != 0) {
      fprintf(stderr, "Total %d additional bytes (from headers and footers).\n", i);
      fprintf(stderr, "Total size %d bytes.\n", i + q);
    }
  }

  return 0;
}


void procedures_at_exit(void) {

  struct source_file_name *f, *fn;
  struct object_file *o;
  struct reference *r;
  struct section *s;
  struct stack *sta;
  struct label *l;

  /* free all the dynamically allocated data structures */
  while (obj_first != NULL) {
    f = obj_first->source_file_names_list;
    while (f != NULL) {
      if (f->name != NULL)
				free(f->name);
      fn = f->next;
      free(f);
      f = fn;
    }
    if (obj_first->data != NULL)
      free(obj_first->data);
    if (obj_first->name != NULL)
      free(obj_first->name);
    o = obj_first;
    obj_first = obj_first->next;
    free(o);
  }

  while (labels_first != NULL) {
    l = labels_first;
    labels_first = labels_first->next;
    free(l);
  }

  while (reference_first != NULL) {
    r = reference_first;
    reference_first = reference_first->next;
    free(r);
  }

  while (stacks_first != NULL) {
    sta = stacks_first->next;
    free(stacks_first->stack);
    free(stacks_first);
    stacks_first = sta;
  }

  while (sec_first != NULL) {
    s = sec_first->next;
    if (sec_first->listfile_cmds != NULL)
      free(sec_first->listfile_cmds);
    if (sec_first->listfile_ints != NULL)
      free(sec_first->listfile_ints);
    free(sec_first);
    sec_first = s;
  }

  while (sec_hd_first != NULL) {
    s = sec_hd_first->next;
    free(sec_hd_first);
    sec_hd_first = s;
  }

  if (banks != NULL)
    free(banks);
  if (bankaddress != NULL)
    free(bankaddress);
}


int parse_flags(char *f) {

  int l, output_mode_defined = 0;

  if (*f != '-')
    return FAILED;

  l = strlen(f);
  if (l == 1)
    return FAILED;

  for (f++, l--; l > 0; l--, f++) {
    switch (*f) {
    case 'v':
      verbose_mode = ON;
      continue;
    case 'i':
      listfile_data = YES;
      continue;
    case 's':
      symbol_mode = SYMBOL_MODE_NOCA5H;
      continue;
    case 'S':
      symbol_mode = SYMBOL_MODE_WLA;
      continue;
    case 'b':
      if (output_mode_defined == 1)
				return FAILED;
      output_mode_defined++;
      output_mode = OUTPUT_PRG;
      continue;
    case 'r':
      if (output_mode_defined == 1)
				return FAILED;
      output_mode_defined++;
      output_mode = OUTPUT_ROM;
      continue;
    case 'd':
      discard_unreferenced_sections = ON;
      continue;
    case 'n':
      no_std_libraries = ON;
      continue;
    case 'o':
      object_file_parameters = ON;
      continue;
    default:
      return FAILED;
    }
  }

  return SUCCEEDED;
}


int allocate_rom(void) {

  rom = malloc(sizeof(unsigned char) * romsize);
  if (rom == NULL) {
    fprintf(stderr, "ALLOCATE_ROM: Out of memory.\n");
    return FAILED;
  }
  rom_usage = malloc(sizeof(unsigned char) * romsize);
  if (rom_usage == NULL) {
    fprintf(stderr, "ALLOCATE_ROM: Out of memory.\n");
    return FAILED;
  }
  memset(rom, emptyfill, romsize);
  memset(rom_usage, 0, romsize);

  return SUCCEEDED;
}
