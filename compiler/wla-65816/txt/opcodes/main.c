
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../defines.h"



#ifdef GB
#include "../../opcodes_gb.c"
#endif
#ifdef Z80
#include "../../opcodes_z80.c"
#endif
#ifdef MCS6502
#include "../../opcodes_6502.c"
#endif
#ifdef WDC65C02
#include "../../opcodes_65c02.c"
#endif
#ifdef MCS6510
#include "../../opcodes_6510.c"
#endif
#ifdef W65816
#include "../../opcodes_65816.c"
#endif
#ifdef SPC700
#include "../../opcodes_spc700.c"
#endif
#ifdef HUC6280
#include "../../opcodes_huc6280.c"
#endif


/* this program is used to print out the mnemonic tables */


int main(int argc, char *argv[]) {

  int i, l, n;


  i = 0;
  while (1) {

    if (opt_table[i].type == -1)
      break;

    /* filter *.B, *.W and *.L away */
    l = strlen(opt_table[i].op);
    for (n = 0; n < l; n++) {
      if (opt_table[i].op[n] == '.')
	break;
    }

    if (n < l && (opt_table[i].op[n+1] == 'B' || opt_table[i].op[n+1] == 'W' || opt_table[i].op[n+1] == 'L')) {
      i++;
      continue;
    }

#ifdef REARRANGE

#ifdef GB
    printf("  { \"%s\", 0x%x, %d, %d },\n", opt_table[i].op, opt_table[i].hex, opt_table[i].type, opt_table[i].value);
#endif
#ifdef MCS6502
    printf("  { \"%s\", 0x%x, %d, %d },\n", opt_table[i].op, opt_table[i].hex, opt_table[i].type, opt_table[i].skip_8bit);
#endif
#ifdef WDC65C02
    printf("  { \"%s\", 0x%x, %d, %d },\n", opt_table[i].op, opt_table[i].hex, opt_table[i].type, opt_table[i].skip_8bit);
#endif
#ifdef MCS6510
    printf("  { \"%s\", 0x%x, %d, %d },\n", opt_table[i].op, opt_table[i].hex, opt_table[i].type, opt_table[i].skip_8bit);
#endif
#ifdef Z80
    printf("  { \"%s\", 0x%x, %d, %d, %d },\n", opt_table[i].op, opt_table[i].hex, opt_table[i].type, opt_table[i].hex_x, opt_table[i].value);
#endif
#ifdef W65816
    printf("  { \"%s\", 0x%x, %d, %d },\n", opt_table[i].op, opt_table[i].hex, opt_table[i].type, opt_table[i].skip_xbit);
#endif
#ifdef HUC6280
    printf("  { \"%s\", 0x%x, %d, %d },\n", opt_table[i].op, opt_table[i].hex, opt_table[i].type, opt_table[i].skip_8bit);
#endif

#endif



#ifdef NICELIST1

#ifdef GB
    if (opt_table[i].type == 8 || opt_table[i].type == 9) {
      int k;

      k = 0;
      printf("\"");
      while (1) {
	if (opt_table[i].op[k] == '*')
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
      k++;
      printf("%d", opt_table[i].value);
      while (1) {
	if (opt_table[i].op[k] == 0)
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
      printf("\" ");
    }
    else
      printf("\"%s\" ", opt_table[i].op);

    if ((opt_table[i].hex & 0xFF) == 0xCB)
      printf("$%.2x%.2x\n", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x\n", opt_table[i].hex);
#endif

#ifdef Z80
    if (opt_table[i].type == 8 || opt_table[i].type == 9 || opt_table[i].type == 10) {
      int k;

      k = 0;
      printf("\"");
      while (1) {
	if (opt_table[i].op[k] == '*')
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
      k++;
      printf("%d", opt_table[i].value);
      while (1) {
	if (opt_table[i].op[k] == 0)
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
      printf("\" ");
    }
    else
      printf("\"%s\" ", opt_table[i].op);

    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x", opt_table[i].hex);

    if (opt_table[i].type == 10 || opt_table[i].type == 5)
      printf(" x $%.2x", opt_table[i].hex_x);

    printf("\n");
#endif

#ifdef MCS6502
    printf("\"%s\" ", opt_table[i].op);

    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x\n", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x\n", opt_table[i].hex);
#endif

#ifdef WDC65C02
    printf("\"%s\" ", opt_table[i].op);

    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x\n", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x\n", opt_table[i].hex);
#endif

#ifdef MCS6510
    printf("\"%s\" ", opt_table[i].op);

    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x\n", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x\n", opt_table[i].hex);
#endif

#ifdef W65816
    printf("\"%s\" ", opt_table[i].op);

    if (opt_table[i].hex & 0xFF00 && opt_table[i].type != 8)
      printf("$%.2x%.2x\n", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else if (opt_table[i].type == 8)
      printf("$%.4x\n", opt_table[i].hex);
    else
      printf("$%.2x\n", opt_table[i].hex);
#endif

#ifdef HUC6280
    printf("\"%s\" ", opt_table[i].op);

    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x\n", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x\n", opt_table[i].hex);
#endif

#ifdef SPC700
    printf("\"%s\" ", opt_table[i].op);
    printf("$%.2x\n", opt_table[i].hex);
#endif

#endif



#ifdef NICELIST2

#ifdef GB
    if ((opt_table[i].hex & 0xFF) == 0xCB)
      printf("$%.2x%.2x ", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x ", opt_table[i].hex);

    if (opt_table[i].type == 8 || opt_table[i].type == 9) {
      int k;

      k = 0;
      printf("\"");
      while (1) {
	if (opt_table[i].op[k] == '*')
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
      k++;
      printf("%d", opt_table[i].value);
      while (1) {
	if (opt_table[i].op[k] == 0)
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
      printf("\"\n");
    }
    else
      printf("\"%s\"\n", opt_table[i].op);
#endif

#ifdef Z80
    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x", opt_table[i].hex);

    if (opt_table[i].type == 10 || opt_table[i].type == 5)
      printf(" x $%.2x", opt_table[i].hex_x);

    printf(" ");

    if (opt_table[i].type == 8 || opt_table[i].type == 9 || opt_table[i].type == 10) {
      int k;

      k = 0;
      printf("\"");
      while (1) {
	if (opt_table[i].op[k] == '*')
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
      k++;
      printf("%d", opt_table[i].value);
      while (1) {
	if (opt_table[i].op[k] == 0)
	  break;
	else
	  printf("%c", opt_table[i].op[k]);
	k++;
      }
    }
    else
      printf("\"%s", opt_table[i].op);

    printf("\"\n");
#endif

#ifdef MCS6502
    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x ", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x ", opt_table[i].hex);

    printf("\"%s\"\n", opt_table[i].op);
#endif

#ifdef WDC65C02
    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x ", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x ", opt_table[i].hex);

    printf("\"%s\"\n", opt_table[i].op);
#endif

#ifdef MCS6510
    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x ", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x ", opt_table[i].hex);

    printf("\"%s\"\n", opt_table[i].op);
#endif

#ifdef W65816
    if (opt_table[i].hex & 0xFF00 && opt_table[i].type != 8)
      printf("$%.2x%.2x ", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else if (opt_table[i].type == 8)
      printf("$%.4x ", opt_table[i].hex);
    else
      printf("$%.2x ", opt_table[i].hex);

    printf("\"%s\"\n", opt_table[i].op);
#endif

#ifdef HUC6280
    if (opt_table[i].hex & 0xFF00)
      printf("$%.2x%.2x ", opt_table[i].hex & 0xFF, (opt_table[i].hex >> 8) & 0xFF);
    else
      printf("$%.2x ", opt_table[i].hex);

    printf("\"%s\"\n", opt_table[i].op);
#endif

#ifdef SPC700
    printf("$%.2x ", opt_table[i].hex);
    printf("\"%s\"\n", opt_table[i].op);
#endif

#endif

    i++;
  }

  return 0;
}
