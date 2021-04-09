
/*
 icopy - part of wla dx gb-z80/z80/6502/6510/65816 macro assembler package by
 ville helin <vhelin@cc.hut.fi>. this is gpl software.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "defines.h"



int main(int argc, char *argv[]) {

  FILE *fp = NULL;
  unsigned char *in;
  char *name;
  int i, fs, s, e;


  i = SUCCEEDED;

  if (argc != 4) {
    fprintf(stderr, "\nICOPY Binary to GB-Z80 Code Converter v1.0\n");
    fprintf(stderr, "Written by Ville Helin in 2001\n");
    fprintf(stderr, "USAGE: %s <BIN FILE> <START> <END>\n\n", argv[0]);
    return 1;
  }

  name = argv[1];
  s = atoi(argv[2]);
  e = atoi(argv[3]);

  fp = fopen(name, "rb");
  if (fp == NULL) {
    fprintf(stderr, "MAIN: Could not open file \"%s\".\n", name);
    return 1;
  }

  fseek(fp, 0, SEEK_END);
  fs = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  in = malloc(sizeof(char) * fs);
  if (in == NULL) {
    fprintf(stderr, "MAIN: Out of memory. Could not allocate %d bytes.\n", fs);
    fclose(fp);
    return 1;
  }

  fread(in, 1, fs, fp);
  fclose(fp);

  if (e >= fs) {
    fprintf(stderr, "MAIN: End (%d) is too much, ending conversion to the last byte (%d).\n", e, fs - 1);
    e = fs - 1;
  }

  /* output header */

  fprintf(stdout, "; this file was created with icopy by ville helin <vhelin@cc.hut.fi>.\n");
  fprintf(stdout, "; listing of file \"%s\" (bytes %d-%d, file range 0...%d).\n", name, s, e, fs - 1);

  for ( ; s <= e; s++) {
    fprintf(stdout, "\tld\ta, %d\n", in[s]);
    fprintf(stdout, "\tldi\t(hl), a\n");
  }

  free(in);

  return 0;
}
