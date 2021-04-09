
/*
 wlad - part of wla dx gb-z80/z80/6502/6510/65816 macro assembler package by/copyright
 ville helin <vhelin@cc.hut.fi>. this is gpl software.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "defines.h"
#include "dasm_brutal.h"
#include "dasm_analyzer.h"

#ifdef GB
#include "mnemonics_gb.c"
#endif


#ifdef AMIGA
char version_string[] = "$VER: WLAD 2.0 (12.06.2001)";
#endif


unsigned char *in;
int arg_only_code = OFF, arg_strings = OFF, arg_address = ON, arg_labels = OFF, arg_mode = ARG_MODE_NONE, arg_value, fs;



int main(int argc, char *argv[]) {

  FILE *fp = NULL;
  int i, r;
  char *name;


  i = SUCCEEDED;

  if (argc != 4)
    i = FAILED;

  if (argc == 4)
    i = parse_flags(argv[1]);

  if (arg_mode == ARG_MODE_NONE)
    i = FAILED;

  if (get_value(argv[2], &arg_value) == FAILED)
    i = FAILED;

  if (i == FAILED) {
#ifdef GB
    fprintf(stderr, "\nWLAD GB-Z80 Disassembler v2.0\n");
#endif
#ifdef Z80
    fprintf(stderr, "\nWLAD Z80 Disassembler v2.0\n");
#endif
    fprintf(stderr, "Copyright 2000-2001 Ville Helin\n");
    fprintf(stderr, "USAGE: %s -[adls]{bc} <CODE SIZE KB / START ADDRESS> <BIN FILE>\n", argv[0]);
    fprintf(stderr, "Commands:               Options:\n");
    fprintf(stderr, "b  Brutal disassembly   a  Disable address printing\n");
    fprintf(stderr, "c  Code analyzer        d  Don't disassemble the remaining data\n");
    fprintf(stderr, "                        l  Create labels\n");
    fprintf(stderr, "                        s  Detect strings\n\n");
    return 1;
  }

  name = argv[3];

  fp = fopen(name, "rb");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", name);
    return 1;
  }

  fseek(fp, 0, SEEK_END);
  fs = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  in = malloc(sizeof(char) * fs);
  if (in == NULL) {
    fprintf(stderr, "Out of memory. Could not allocate %d bytes.\n", fs);
    fclose(fp);
    return 1;
  }

  fread(in, 1, fs, fp);
  fclose(fp);

  if (arg_mode == ARG_MODE_BRUTAL)
    arg_value = arg_value << 10;

  if (arg_mode == ARG_MODE_BRUTAL && fs < arg_value)
    arg_value = fs;

  /* compute ROM banks */
  r = fs / (16 * 1024);
  i = fs - (r * 16 * 1024);
  if (i != 0)
    r++;

  /* output header */
  output_header(name, r);

  if (arg_mode == ARG_MODE_BRUTAL)
    dasm_brutal();
  else
    dasm_analyzer();

  free(in);

  return 0;
}


int letter_check(char c) {

  if (c >= 'a' && c <= 'z')
    return SUCCEEDED;
  else if (c >= 'A' && c <= 'Z')
    return SUCCEEDED;
  else if (c >= '0' && c <= '9')
    return SUCCEEDED;
  else if (c == ' ' || c == '!' || c == '?' || c == ',' || c == '.' || c == '-' || c == '*' || c == '|' || c == '&' || c == '/' || c == '\\' || c == '\'' || c == '"' || c == '@' || c == '#' || c == '£' || c == '$' || c == '%' || c == '(' || c == ')' || c == '[' || c == ']')
    return SUCCEEDED;

  return FAILED;
}


int parse_flags(char *f) {

  int l;


  if (*f != '-') {
    return FAILED;
  }

  l = strlen(f);
  if (l == 1)
    return FAILED;

  for (f++, l--; l > 0; l--, f++) {
    switch (*f) {

    case 'd':
      arg_only_code = ON;
      continue;

    case 's':
      arg_strings = ON;
      continue;

    case 'a':
      arg_address = OFF;
      continue;

    case 'l':
      arg_labels = ON;
      continue;

    case 'b':
      if (arg_mode != ARG_MODE_NONE)
	return FAILED;
      arg_mode = ARG_MODE_BRUTAL;
      continue;

    case 'c':
      if (arg_mode != ARG_MODE_NONE)
	return FAILED;
      arg_mode = ARG_MODE_ANALYZER;
      continue;

    default:
      return FAILED;
    }
  }

  return SUCCEEDED;
}


int output_header(char *name, int r) {

  fprintf(stdout, "; this file was created with wlad ");
#ifdef GB
  fprintf(stdout, "gb-z80\n");
#endif
#ifdef Z80
  fprintf(stdout, "z80\n");
#endif
  fprintf(stdout, "; disassembler by ville helin <vhelin@cc.hut.fi>.\n");
  fprintf(stdout, "; disassembly of \"%s\".\n\n", name);
  fprintf(stdout, ".MEMORYMAP\n");
  fprintf(stdout, "SLOTSIZE $4000\n");
  fprintf(stdout, "DEFAULTSLOT 1\n");
  fprintf(stdout, "SLOT 0 $0000\n");
  fprintf(stdout, "SLOT 1 $4000\n");
  fprintf(stdout, ".ENDME\n\n");
  fprintf(stdout, ".BANKSIZE $4000\n");
  fprintf(stdout, ".ROMBANKS %d\n", r);

  return SUCCEEDED;
}


int get_value(char *s, int *v) {

  *v = 0;

  if (*s == '$') {
    s++;
    while (*s != 0) {
      if (*s >= '0' && *s <= '9')
	*v = (*v << 4) + (*s - '0');
      else if (*s >= 'a' && *s <= 'f')
	*v = (*v << 4) + (*s - 'a' + 10);
      else if (*s >= 'A' && *s <= 'F')
	*v = (*v << 4) + (*s - 'A' + 10);
      else
	return FAILED;
      s++;
    }

    return SUCCEEDED;
  }

  if (*s >= '0' && *s <= '9') {
    while (*s != 0) {
      if (*s < '0' || *s > '9')
	return FAILED;
      *v = (*v * 10) + (*s - '0');
      s++;
    }

    return SUCCEEDED;
  }

  return FAILED;
}
