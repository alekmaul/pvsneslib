
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "main.h"
#include "memory.h"
#include "files.h"



extern struct object_file *obj_first;
extern int emptyfill, sms_checksum, snes_rom_mode, snes_rom_speed, smc_status;
extern int gb_checksum, gb_complement_check, snes_checksum, snes_mode, cpu_65816;
extern int snes_sramsize;

int emptyfill;



int check_file_types(void) {

  struct object_file *o;

  o = obj_first;
  while (o != NULL) {
    if (strncmp((char *)o->data, "WLAK", 4) == 0)
      o->format = WLA_VERSION_OBJ;
    else if (strncmp((char *)o->data, "WLAV", 4) == 0)
      o->format = WLA_VERSION_LIB;
    else {
      fprintf(stderr, "CHECK_FILE_TYPES: File \"%s\" is of unknown format (\"%c%c%c%c\").\n", o->name, o->data[0], o->data[1], o->data[2], o->data[3]);
      return FAILED;
    }
    o = o->next;
  }

  return SUCCEEDED;
}


int check_headers(void) {

  struct object_file *o;
  int x = 0, misc_bits = 0, more_bits = 0;

  o = obj_first;
  while (o != NULL) {
    if (o->format == WLA_VERSION_OBJ) {
      if (x == 0) {
				/* misc bits */
				emptyfill = *(o->data + OBJ_EMPTY_FILL);
				misc_bits = *(o->data + OBJ_MISC_BITS);
				sms_checksum = misc_bits & 1;
				snes_rom_mode = misc_bits & 2;
				snes_mode = misc_bits & 4;
				snes_rom_speed = misc_bits & 8;
				gb_checksum = misc_bits & 16;
				gb_complement_check = misc_bits & 32;
				snes_checksum = misc_bits & 64;
				cpu_65816 = misc_bits & 128;
				/* more bits */
				more_bits = *(o->data + OBJ_MORE_BITS);
				smc_status = more_bits & 1;
				snes_sramsize = (more_bits>>1) & 3;
				x++;
      }
      else if (emptyfill != *(o->data + OBJ_EMPTY_FILL) || misc_bits != *(o->data + OBJ_MISC_BITS) || more_bits != *(o->data + OBJ_MORE_BITS)) {
				fprintf(stderr, "CHECK_HEADERS: The object files are from different projects.\n");
				return FAILED;
      }
    }

    o = o->next;
  }

  if (x == 0) {
    fprintf(stderr, "CHECK_HEADERS: There's nothing to link.\n");
    return FAILED;
  }

  return SUCCEEDED;
}
