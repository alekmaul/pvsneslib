
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "defines.h"
#include "pass_1.h"
#include "pass_2.h"
#include "pass_4.h"


#ifdef GB
extern int licenseecodeold;
extern int name_defined, licenseecodeold_defined, licenseecodenew_defined;
extern char name[32], licenseecodenew_c1, licenseecodenew_c2;
#endif

#ifdef Z80
extern char sdsctag_name_str[MAX_NAME_LENGTH], sdsctag_notes_str[MAX_NAME_LENGTH], sdsctag_author_str[MAX_NAME_LENGTH];
extern int sdsctag_name_type, sdsctag_notes_type, sdsctag_author_type, sdsc_ma, sdsc_mi;
extern int sdsctag_name_value, sdsctag_notes_value, sdsctag_author_value;
extern int computesmschecksum_defined, sdsctag_defined, smstag_defined;
#endif

#ifdef W65816
extern char name[32];
extern char snesid[4];
extern int snesid_defined, snesromsize;
extern int sramsize_defined, sramsize, country_defined, country;
extern int cartridgetype, cartridgetype_defined, licenseecode_defined, licenseecode;
extern int version_defined, version;
extern int hirom_defined, lorom_defined, slowrom_defined, fastrom_defined, snes_mode, name_defined;
extern int computesneschecksum_defined;
#endif

extern FILE *file_out_ptr;
extern int ind, inz, rambanks, rambanks_defined, ifdef;
extern int rombanks_defined, rombanks, cartridgetype, cartridgetype_defined;
extern int output_format, romgbc, romsgb, romdmg, max_address;
extern int romtype, verbose_mode, section_status, background_defined, memorymap_defined;
extern int emptyfill_defined, emptyfill, rombankmap_defined, section_id;
extern unsigned char *rom_banks, *rom_banks_usage_table;

char include_directives_name[] = "INCLUDE_DIRECTIVES:";

extern struct section_def *sections_first, *sections_last, *sec_tmp, *sec_next;




int pass_2(void) {

  if (ifdef > 0) {
    fprintf(stderr, "%x x .ENDIFs are missing.\n", ifdef);
    return FAILED;
  }

  if (verbose_mode == ON)
    printf("Directive checks...\n");

  if (section_status == ON) {
    fprintf(stderr, "%s The section \"%s\" was not closed.\n", include_directives_name, sections_last->name);
    return FAILED;
  }

  if (output_format != OUTPUT_LIBRARY && rombanks_defined == 0 && rombankmap_defined == 0) {
    fprintf(stderr, "%s ROMBANKS/ROMBANKMAP wasn't defined.\n", include_directives_name);
    return FAILED;
  }

#ifdef Z80
  /* SMSTAG */
  if (smstag_defined != 0) {
    mem_insert_absolute(0x7FF0, 0x54);
    mem_insert_absolute(0x7FF1, 0x4D);
    mem_insert_absolute(0x7FF2, 0x52);
    mem_insert_absolute(0x7FF3, 0x20);
    mem_insert_absolute(0x7FF4, 0x53);
    mem_insert_absolute(0x7FF5, 0x45);
    mem_insert_absolute(0x7FF6, 0x47);
    mem_insert_absolute(0x7FF7, 0x41);
  }

  /* SDSCTAG */
  if (sdsctag_defined != 0) {

    struct tm *ti;
    time_t t;
    int da, mo, ye_l, ye_h, ye, x, q;

    /* insert string pointers */
    if (create_a_new_section_structure() == FAILED)
      return FAILED;
    strcpy(sec_tmp->name, "!__WLA_SDSCTAG_STRINGS");
    sec_tmp->status = SECTION_STATUS_ABSOLUTE;
    fprintf(file_out_ptr, "A%d %d ", sec_tmp->id, 0x7FEA);

    /* insert the system line (0) */
    fprintf(file_out_ptr, "k0 ");

    /* the data */
    if (sdsctag_author_type == TYPE_VALUE)
      fprintf(file_out_ptr, "y%d ", sdsctag_author_value);
    else if (sdsctag_author_type == TYPE_LABEL)
      fprintf(file_out_ptr, "r%s ", sdsctag_author_str);
    else if (sdsctag_author_type == TYPE_STACK_CALCULATION)
      fprintf(file_out_ptr, "C%d ", sdsctag_author_value);
    else
      fprintf(file_out_ptr, "r *WLA_SDSC_PRG_AUTHOR_PTR ");

    if (sdsctag_name_type == TYPE_VALUE)
      fprintf(file_out_ptr, "y%d ", sdsctag_name_value);
    else if (sdsctag_name_type == TYPE_LABEL)
      fprintf(file_out_ptr, "r%s ", sdsctag_name_str);
    else if (sdsctag_name_type == TYPE_STACK_CALCULATION)
      fprintf(file_out_ptr, "C%d ", sdsctag_name_value);
    else
      fprintf(file_out_ptr, "r *WLA_SDSC_PRG_NAME_PTR ");

    if (sdsctag_notes_type == TYPE_VALUE)
      fprintf(file_out_ptr, "y%d ", sdsctag_notes_value);
    else if (sdsctag_notes_type == TYPE_LABEL)
      fprintf(file_out_ptr, "r%s ", sdsctag_notes_str);
    else if (sdsctag_notes_type == TYPE_STACK_CALCULATION)
      fprintf(file_out_ptr, "C%d ", sdsctag_notes_value);
    else
      fprintf(file_out_ptr, "r *WLA_SDSC_PRG_NOTES_PTR ");

    fprintf(file_out_ptr, "s ");

    /* create string sections */
    if (sdsctag_author_type == TYPE_STRING) {
      if (create_a_new_section_structure() == FAILED)
				return FAILED;
      strcpy(sec_tmp->name, "!__WLA_SDSCTAG_STRING_AUTHOR");
      sec_tmp->status = SECTION_STATUS_SEMIFREE;

      fprintf(file_out_ptr, "B0 0 O1 S%d L *WLA_SDSC_PRG_AUTHOR_PTR ", sec_tmp->id);
      for (q = 0; q < strlen(sdsctag_author_str); q++)
				fprintf(file_out_ptr, "d%d ", sdsctag_author_str[q]);
      fprintf(file_out_ptr, "d0 s ");
    }
    if (sdsctag_name_type == TYPE_STRING) {
      if (create_a_new_section_structure() == FAILED)
				return FAILED;
      strcpy(sec_tmp->name, "!__WLA_SDSCTAG_STRING_NAME");
      sec_tmp->status = SECTION_STATUS_SEMIFREE;

      fprintf(file_out_ptr, "B0 0 O1 S%d L *WLA_SDSC_PRG_NAME_PTR ", sec_tmp->id);
      for (q = 0; q < strlen(sdsctag_name_str); q++)
				fprintf(file_out_ptr, "d%d ", sdsctag_name_str[q]);
      fprintf(file_out_ptr, "d0 s ");
    }
    if (sdsctag_notes_type == TYPE_STRING) {
      if (create_a_new_section_structure() == FAILED)
				return FAILED;
      strcpy(sec_tmp->name, "!__WLA_SDSCTAG_STRING_NOTES");
      sec_tmp->status = SECTION_STATUS_SEMIFREE;

      fprintf(file_out_ptr, "B0 0 O1 S%d L *WLA_SDSC_PRG_NOTES_PTR ", sec_tmp->id);
      for (q = 0; q < strlen(sdsctag_notes_str); q++)
				fprintf(file_out_ptr, "d%d ", sdsctag_notes_str[q]);
      fprintf(file_out_ptr, "d0 s ");
    }

    /* create the time and date data */
    time(&t);
    ti = localtime(&t);
    da = ti->tm_mday;
    mo = ti->tm_mon + 1;
    ye = ti->tm_year + 1900;

    /* BCD the values */
    q = (sdsc_ma / 10);
    sdsc_ma = (q << 4) | (sdsc_ma - (q * 10));
    q = (sdsc_mi / 10);
    sdsc_mi = (q << 4) | (sdsc_mi - (q * 10));
    q = (da / 10);
    da = (q << 4) | (da - (q * 10));
    q = (mo / 10);
    mo = (q << 4) | (mo - (q * 10));

    q = (ye / 1000);
    ye -= (q * 1000);
    x = (ye / 100);
    ye -= (x * 100);
    ye_h = (q << 4) | x;

    q = (ye / 10);
    ye -= (q * 10);
    ye_l = (q << 4) | ye;

    mem_insert_absolute(0x7FE0, 0x53);
    mem_insert_absolute(0x7FE1, 0x44);
    mem_insert_absolute(0x7FE2, 0x53);
    mem_insert_absolute(0x7FE3, 0x43);
    mem_insert_absolute(0x7FE4, sdsc_ma);
    mem_insert_absolute(0x7FE5, sdsc_mi);
    mem_insert_absolute(0x7FE6, da);
    mem_insert_absolute(0x7FE7, mo);
    mem_insert_absolute(0x7FE8, ye_l);
    mem_insert_absolute(0x7FE9, ye_h);
  }
#endif

#ifdef W65816
  if (output_format != OUTPUT_LIBRARY) {
    /* snes cartridge information */
    if (snes_mode != 0) {
      inz = 32;
      if (hirom_defined != 0) {
				ind = 0xFFD5;
				inz += 1;
      }
      else
				ind = 0x7FD5;
      if (fastrom_defined != 0)
				inz += 16;
      mem_insert_absolute(ind, inz);
  
      if (cartridgetype_defined != 0) 
				mem_insert_absolute(ind + 1, cartridgetype);

      if (snesromsize != 0) 
				mem_insert_absolute(ind + 2, snesromsize);

      if (sramsize_defined != 0) 
				mem_insert_absolute(ind + 3, sramsize);

      if (country_defined != 0)
				mem_insert_absolute(ind + 4, country);

      if (licenseecode_defined != 0)
				mem_insert_absolute(ind + 5, licenseecode);

      if (version_defined != 0)
				mem_insert_absolute(ind + 6, version);
    }
  
    /* snes cartridge ID */
    if (snesid_defined != 0) {
      if (hirom_defined != 0)
				inz = 0xFFB2;
      else 
				inz = 0x7FB2;
      for (ind = 0; ind < 4; ind++) 
				mem_insert_absolute(inz + ind, snesid[ind]);
    }
    
    /* snes cartridge name */
    if (name_defined != 0) {
      if (hirom_defined != 0)
				inz = 0xFFC0;
      else
				inz = 0x7FC0;
      for (ind = 0; ind < 21; ind++)
				mem_insert_absolute(inz + ind, name[ind]);
    }
  }

#endif

#ifdef GB
  /* insert the descriptive data (not in library files) */
  if (output_format == OUTPUT_OBJECT) {
    if (romgbc != 0)
      mem_insert_absolute(323, 128);
    if (romdmg != 0)
      mem_insert_absolute(326, 0);
    if (romsgb != 0)
      mem_insert_absolute(326, 3);
    if (rambanks_defined != 0)
      mem_insert_absolute(329, rambanks);
    if (rombanks_defined != 0)
      mem_insert_absolute(328, romtype);
    if (cartridgetype_defined != 0)
      mem_insert_absolute(327, cartridgetype);
    if (licenseecodeold_defined != 0) {
      mem_insert_absolute(331, licenseecodeold);
    }
    if (licenseecodenew_defined != 0) {
      mem_insert_absolute(331, 51);
      mem_insert_absolute(324, licenseecodenew_c1);
      mem_insert_absolute(325, licenseecodenew_c2);
    }
    if (name_defined != 0) {
      if (romgbc != 0)
				inz = 15;
      else
				inz = 16;
      for (ind = 0; ind < inz; ind++)
				mem_insert_absolute(308 + ind, name[ind]);
    }
  }
#endif

  fclose(file_out_ptr);
  file_out_ptr = NULL;

  return SUCCEEDED;
}


int create_a_new_section_structure(void) {

	sec_tmp = malloc(sizeof(struct section_def));
	if (sec_tmp == NULL) {
		print_error("Out of memory while allocating room for a new SECTION.\n", ERROR_DIR);
		return FAILED;
	}

	sec_tmp->listfile_items = 0;
	sec_tmp->listfile_ints = NULL;
	sec_tmp->listfile_cmds = NULL;
	sec_tmp->maxsize_status = OFF;
	sec_tmp->data = NULL;
	sec_tmp->next = NULL;
	sec_tmp->id = section_id;
	sec_tmp->alive = ON;
	sec_tmp->advance_org = NO;
	section_id++;
	sec_tmp->filename_id = 0;
	sec_tmp->alignment = 1;

	if (sections_first == NULL) {
		sections_first = sec_tmp;
		sections_last = sec_tmp;
	}
	else {
		sections_last->next = sec_tmp;
		sections_last = sec_tmp;
	}

	return SUCCEEDED;
}
