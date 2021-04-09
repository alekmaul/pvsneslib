
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../defines.h"
#include "defines.h"
#include "main.h"


#ifdef GB
#include "../opcodes_gb.c"
#endif
#ifdef Z80
#include "../opcodes_z80.c"
#endif
#ifdef MCS6502
#include "../opcodes_6502.c"
#endif
#ifdef MCS6510
#include "../opcodes_6510.c"
#endif
#ifdef W65816
#include "../opcodes_65816.c"
#endif


/* this program is used to print the opcodes */


int main(int argc, char *argv[]) {

  char *t;
  int i;


  i = 0;
  printf("struct optcode {\n");
  printf("  char *op;\n");
  printf("  int  type;\n");
  printf("  int  hex;\n");
  printf("  int  branch;\n};\n\n");
  printf("struct optcode opt_table[] = {\n");
  while (TRUE) {
    printf("  { \"\t");

    t = opt_table[i].op;

    while (*t != 0 && *t != ' ') {
      printf("%c", *t);
      t++;
    }

#ifdef GB
    if (*t == ' ') {
      t++;
      printf("\t");

      while (*t != 0) {
	if (*t == '*') {
	  if (opt_table[i].type == 8)
	    printf("$%x", opt_table[i].value);
	  else
	    printf("%d", opt_table[i].value);
	}
	else if (*t == ',') {
	  printf(", ");
	}
	else
	  printf("%c", *t);
	t++;
      }
    }
#endif

    printf("\", %d, 0x%x, ", opt_table[i].type, opt_table[i].hex);
    printf("%d },\n", get_mnemonic_status(i));

    i++;
    if (opt_table[i].type == -1)
      break;
  }

  printf("  { \"E\", 0, -1 }\n};\n");

  return 0;
}

/* 0 = no flow changes
   1 = jump
   2 = branch
   3 = return
   4 = conditional return */

int get_mnemonic_status(int i) {

  int k;


  k = opt_table[i].hex;

  /* rst */
  if (k == 0xff || k == 0xf7 || k == 0xef || k == 0xe7 || k == 0xdf || k == 0xd7 || k == 0xcf || k == 0xc7)
    return 1;

  /* ret/reti */
  if (k == 0xc9 || k == 0xd9)
    return 3;

  /* ret cond */
  if (k == 0xc8 || k == 0xc0 || k == 0xd8 || k == 0xd0)
    return 4;

  /* jr */
  if (k == 0x18)
    return 2;

  /* jr cond */
  if (k == 0x28 || k == 0x20 || k == 0x30 || k == 0x38)
    return 2;

  /* jp */
  if (k == 0xc3 || k == 0xe9)
    return 1;

  /* jp cond */
  if (k == 0xca || k == 0xc2 || k == 0xd2 || k == 0xda)
    return 2;

  /* call */
  if (k == 0xcd)
    return 2;

  /* call cond */
  if (k == 0xcc || k == 0xc4 || k == 0xd4 || k == 0xdc)
    return 2;

  return 0;
}
