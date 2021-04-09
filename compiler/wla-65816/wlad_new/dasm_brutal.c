
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "dasm_brutal.h"
#include "data.h"


extern unsigned char *in;
extern int arg_only_code, arg_strings, arg_address, arg_labels, arg_mode, arg_value, fs;



int dasm_brutal(void) {

  int i, b, a, s;


  if (arg_labels == ON)
    collect_labels_brutal();

  b = 0;
  a = 0;
  s = 0;
  while (a < arg_value) {
    printf(".BANK %d SLOT %d\n\n", b, s);
    s = 1;
    b++;
    i = 0;
    while (a < arg_value && i < 0x4000) {


      
    }
  }

  return SUCCEEDED;
}


int collect_labels_brutal(void) {

  int i, b, a, s, t;


  b = 0;
  a = 0;
  s = 0;
  i = 0;
  while (a < arg_value) {
    if (i >= 0x4000) {
      i -= 0x4000;
      s = 1;
      b++;
    }

    if (get_mnemonic_type(*(in + a), &t) == FAILED) {
      fprintf(stderr, "COLLECT_LABELS_BRUTAL: Unknown opcode $%x at $%x.\n", *(in + a), a);
      return FAILED;
    }

    /* types of t
       0 - 1B, no flow change
       1 - 2B, no flow change
       2 - 3B, no flow change
       3 - 2B, branch
       4 - 3B, branch
       5 - 1B, jump
       6 - 3B, jump
       7 - 1B, return
    */

    switch (t) {

    case 0:
    case 7:
      a++;
      i++;
      break;

    case 1:
      a += 2;
      i += 2;
      break;

    case 2:
      a += 3;
      i += 3;
      break;
    }

  }

  return SUCCEEDED;
}
