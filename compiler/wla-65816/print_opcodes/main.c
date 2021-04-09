
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../defines.h"
#include "defines.h"


#ifdef GB
#include "../opcodes_gb.c"
#endif
#ifdef Z80
#include "../opcodes_z80.c"
#endif
#ifdef MCS6502
#include "../opcodes_6502.c"
#endif
#ifdef WDC65C02
#include "../opcodes_65c02.c"
#endif
#ifdef MCS6510
#include "../opcodes_6510.c"
#endif
#ifdef W65816
#include "../opcodes_65816.c"
#endif
#ifdef SPC700
#include "../opcodes_spc700.c"
#endif


/* this program is used to print the opcodes */


int main(int argc, char *argv[]) {

  int i;



  i = 0;
  while (TRUE) {
    printf("%s\n", opt_table[i].op);
    i++;
    if (opt_table[i].type == -1)
      break;
  }

  return 0;
}
