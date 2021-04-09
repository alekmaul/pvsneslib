
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../defines.h"
#include "defines.h"


#if defined(GB)
char outname[] = "../opcodes_gb_tables.c";
#include "../opcodes_gb.c"
#elif defined(Z80)
char outname[] = "../opcodes_z80_tables.c";
#include "../opcodes_z80.c"
#elif defined(MCS6502)
char outname[] = "../opcodes_6502_tables.c";
#include "../opcodes_6502.c"
#elif defined(WDC65C02)
char outname[] = "../opcodes_65c02_tables.c";
#include "../opcodes_65c02.c"
#elif defined(MCS6510)
char outname[] = "../opcodes_6510_tables.c";
#include "../opcodes_6510.c"
#elif defined(W65816)
char outname[] = "../opcodes_65816_tables.c";
#include "../opcodes_65816.c"
#elif defined(SPC700)
char outname[] = "../opcodes_spc700_tables.c";
#include "../opcodes_spc700.c"
#elif defined(HUC6280)
char outname[] = "../opcodes_huc6280_tables.c";
#include "../opcodes_huc6280.c"
#endif


/* this program is used to generate the opcode decoding speedup tables */


int print_table(FILE *f, int *d) {

  int x;


  for (x = 0; x < 256; x++) {
    if ((x % 8) == 0)
      fprintf(f, "  ");
    fprintf(f, "%d", d[x]);
    if ((x % 8) == 7) {
      if (x == 255)
	fprintf(f, " };\n");
      else
	fprintf(f, ",\n");
    }
    else
      fprintf(f, ", ");
  }

  return SUCCEEDED;
}


int main(int argc, char *argv[]) {

  struct optcode *opt_tmp;
  FILE *out;
  char max_name[256];
  int x, opcode_n[256], opcode_p[256], a, b, n, max = 0, ob;



  /* generate opcode decoding jump tables */
  for (x = 0; x < 256; x++) {
    opcode_n[x] = 0;
    opcode_p[x] = 0;
  }

  opt_tmp = opt_table;
  a = 'A';
  b = 'a';
  ob = 'a';
  n = 0;
  x = 0;
  opcode_p[(int)a] = 0;
  opcode_p[(int)b] = 0;
  while (opt_table[n].type != -1) {
    if (strlen(opt_table[n].op) > max) {
      max = strlen(opt_table[n].op);
      strcpy(max_name, opt_table[n].op);
    }
    if (opt_tmp[n].op[0] != a) {
      opcode_n[(int)a] = x;
      opcode_n[(int)b] = x;
      a = opt_tmp[n].op[0];
      b = tolower((int)a);
      if (ob > b) {
	fprintf(stderr, "MAIN: Instruction are NOT in aplhabetical order (first letter): '%c' -> '%c'.\n", ob, b);
	return 1;
      }
      ob = b;
      opcode_p[(int)a] = n;
      opcode_p[(int)b] = n;
      x = 1;
      n++;
    }
    else {
      x++;
      n++;
    }
  }
  opcode_n[(int)a] = x;
  opcode_n[(int)b] = x;

  out = fopen(outname, "wb");
  if (out == NULL) {
    fprintf(stderr, "MAIN: Unable to create file \"%s\".\n", outname);
    return 1;
  }

  /* opcode_n[256] */
  fprintf(out, "int opcode_n[256] = {\n");
  print_table(out, opcode_n);

  /* opcode_p[256] */
  fprintf(out, "int opcode_p[256] = {\n");
  print_table(out, opcode_p);

  fclose(out);

#if QUIET == 0
  printf("MAIN: max opt[] (\"%s\") lenght was %d bytes.\n", max_name, max);
#endif

  return 0;
}
