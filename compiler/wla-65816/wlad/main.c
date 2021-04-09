
/*
 wlad - part of wla dx gb-z80/z80/6502/6510/65816 macro assembler package by
 ville helin <vhelin@cc.hut.fi>. this is gpl software.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "defines.h"
#include "opcodes_gb.c"



#ifdef AMIGA
char version_string[] = "$VER: WLAD 1.3 (21.10.2000)";
#endif

int databanks = OFF, strings = OFF, address = ON;
int bank_first, bank_rest, slot;



int main(int argc, char *argv[]) {

  FILE *fp = NULL;
  unsigned char *in;
  int i, fs, a, b;
  char *name;



  i = SUCCEEDED;

  if (!(argc == 2 || argc == 3))
    i = FAILED;

  if (argc == 3)
    i = parse_flags(argv[1]);

  if (databanks == ON && strings == ON)
    i = FAILED;

  if (i == FAILED) {
    fprintf(stderr, "\nWLAD GB-Z80 Disassembler v1.3\n");
    fprintf(stderr, "Written by Ville Helin in 2000\n");
    fprintf(stderr, "USAGE: %s [-ads] <BIN FILE>\n", argv[0]);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "a  Disable address output\n");
    fprintf(stderr, "d  Disassemble upper banks (one and up) as data\n");
    fprintf(stderr, "s  Disassemble upper banks as data with string detection\n\n");
    return 1;
  }

  name = argv[argc - 1];

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

  if (in[0x147] == 0xBE) {
    /* pocket voice */
    bank_first = 24 * 1024;
    bank_rest = 8 * 1024;
  }
  else {
    /* some other */
    bank_first = 16 * 1024;
    bank_rest = 16 * 1024;
  }

  a = fs / (1024 * 16);
  b = fs - (a * 1024 * 16);
  if (b != 0 || (a != 2 && a != 4 && a != 8 && a != 16 && a != 32 && a != 64 && a != 128 && a != 72 && a != 80 && a != 96)) {
    fprintf(stderr, "Unsupported ROM size.\n");
    return 1;
  }

  if (a == 2)
    b = 0;
  else if (a == 4)
    b = 1;
  else if (a == 8)
    b = 2;
  else if (a == 16)
    b = 3;
  else if (a == 32)
    b = 4;
  else if (a == 64)
    b = 5;
  else if (a == 128)
    b = 6;
  else if (a == 72)
    b = 0x52;
  else if (a == 80)
    b = 0x53;
  else if (a == 96)
    b = 0x54;

  /* output header */

  fprintf(stdout, "; this file was created with wlad - yet another gb-z80\n");
  fprintf(stdout, "; disassembler by ville helin <vhelin@cc.hut.fi>.\n");
  fprintf(stdout, "; disassembly of \"%s\".\n\n", name);
  fprintf(stdout, ".MEMORYMAP\n");
  fprintf(stdout, "SLOTSIZE $4000\n");
  fprintf(stdout, "DEFAULTSLOT 1\n");
  fprintf(stdout, "SLOT 0 $0000\n");
  fprintf(stdout, "SLOT 1 $4000\n");
  fprintf(stdout, ".ENDME\n\n");
  fprintf(stdout, ".BANKSIZE $4000\n");
  fprintf(stdout, ".ROMBANKS %d\n", a);

  b = 0;
  i = 0;
  slot = 0;

  /* output bank data */

  if (databanks == OFF && strings == OFF) {
    output_bank_opcodes(fs, &b, in, bank_first, &i);
    while (a != b)
      output_bank_opcodes(fs, &b, in, bank_rest, &i);
  }
  else if (databanks == ON && strings == OFF) {
    output_bank_opcodes(fs, &b, in, bank_first, &i);
    while (a != b)
      output_bank_data(fs, &b, in, bank_rest, &i);
  }
  else if (databanks == OFF && strings == ON) {
    output_bank_opcodes(fs, &b, in, bank_first, &i);
    while (a != b)
      output_bank_data_detect_strings(fs, &b, in, bank_rest, &i);
  }

  free(in);
  fclose(fp);

  return 0;
}


int output_bank_opcodes(int fs, int *b, unsigned char *in, int bank_size, int *i) {

  char bu[256], tm[256];
  int q, oa, p, t, x, a, ad;
  struct optcode *ot;



  a = 0;
  ad = *i + bank_size;
  fprintf(stdout, "\n.BANK %d SLOT %d\n.ORG 0\n\n", *b, slot);

  for ( ; *i < ad; ) {
    oa = a;
    ot = &opt_table[0];
    x = 1;

    while (ot->type != -1) {

      /* type 0 */

      if (ot->type == 0 && in[*i] == ot->hex) {
	(*i)++;
	a++;
	if (address == ON)
	  fprintf(stdout, "\t%s\t\t; $%.4x\n", ot->op, oa);
	else
	  fprintf(stdout, "\t%s\n", ot->op);
	x = 0;
	break;
      }

      /* type 1 */

      if (ot->type == 1 && in[*i] == ot->hex) {
	(*i)++;
	a++;
	for (t = strlen(ot->op), x = 0, p = 0; x < t; ) {
	  if (ot->op[x] == 'x') {
	    bu[p] = 0;
	    sprintf(tm, "$%.2x", in[(*i)++]);
	    strcat(bu, tm);
	    p += 3;
	    x++;
	    a++;
	  }
	  else
	    bu[p++] = ot->op[x++];
	}

	bu[p] = 0;
	if (address == ON)
	  fprintf(stdout, "\t%s\t\t; $%.4x\n", bu, oa);
	else
	  fprintf(stdout, "\t%s\n", bu);
	x = 0;
	break;
      }

      /* type 2 */

      if (ot->type == 2 && in[*i] == ot->hex) {
	(*i)++;
	a++;
	for (t = strlen(ot->op), x = 0, p = 0; x < t; ) {
	  if (ot->op[x] == '?') {
	    bu[p] = 0;
	    q = in[(*i)++];
	    q += in[(*i)++] << 8;
	    sprintf(tm, "$%.4x", q);
	    strcat(bu, tm);
	    p += 5;
	    x++;
	    a += 2;
	  }
	  else
	    bu[p++] = ot->op[x++];
	}

	bu[p] = 0;
	if (address == ON)
	  fprintf(stdout, "\t%s\t\t; $%.4x\n", bu, oa);
	else
	  fprintf(stdout, "\t%s\n", bu);
	x = 0;
	break;
      }

      /* type 3 */

      else if (ot->type == 3 && in[*i + 1] == ((ot->hex & 0xFF00) >> 8) && in[*i] == (ot->hex & 0xFF)) {
	*i += 2;
	a += 2;
	if (address == ON)
	  fprintf(stdout, "\t%s\t\t; $%.4x\n", ot->op, oa);
	else
	  fprintf(stdout, "\t%s\n", ot->op);
	x = 0;
	break;
      }

      ot = &opt_table[x++];
    }
    if (x != 0) {
      if (address == ON)
	fprintf(stdout, ".DB $%.2x\t\t; %.4x\n", in[(*i)++], oa);
      else
	fprintf(stdout, ".DB $%.2x\n", in[(*i)++]);
      a++;
    }
  }

  (*b)++;
  slot = 1;
  return SUCCEEDED;
}


int output_bank_data(int fs, int *b, unsigned char *in, int bank_size, int *i) {

  int ad;



  fprintf(stdout, "\n.BANK %d SLOT %d\n.ORG 0\n\n", *b, slot);

  if (address == ON) {
    for (ad = *i + bank_size; *i < ad; *i += 8) {
      fprintf(stdout, ".DB $%.2x $%.2x $%.2x $%.2x $%.2x $%.2x $%.2x $%.2x\t; $%.4x\n", in[*i], in[*i + 1], in[*i + 2], in[*i + 3], in[*i + 4], in[*i + 5], in[*i + 6], in[*i + 7], *i);
    }
  }
  else {
    for (ad = *i + bank_size; *i < ad; *i += 8) {
      fprintf(stdout, ".DB $%.2x $%.2x $%.2x $%.2x $%.2x $%.2x $%.2x $%.2x\n", in[*i], in[*i + 1], in[*i + 2], in[*i + 3], in[*i + 4], in[*i + 5], in[*i + 6], in[*i + 7]);
    }
  }

  (*b)++;
  slot = 1;
  return SUCCEEDED;
}


int output_bank_data_detect_strings(int fs, int *b, unsigned char *in, int bank_size, int *i) {

  int ad, t, x;
  char c;



  fprintf(stdout, "\n.BANK %d SLOT %d\n.ORG 0\n\n", *b, slot);

  for (ad = *i + bank_size; *i < ad; ) {
    if (letter_check(in[*i]) == SUCCEEDED && letter_check(in[*i + 1]) == SUCCEEDED && letter_check(in[*i + 2]) == SUCCEEDED) {
      t = *i;
      fprintf(stdout, ".DB \"");
      c = in[(*i)++];
      while (letter_check(c) == SUCCEEDED) {
	fprintf(stdout, "%c", c);
	c = in[(*i)++];
      }
      (*i)--;

      if (address == ON)
	fprintf(stdout, "\"\t; $%.4x\n", t);
      else
	fprintf(stdout, "\"\n");
    }
    else {
      fprintf(stdout, ".DB");
      t = *i;
      for (x = 0; *i < ad && x < 8 && letter_check(in[*i]) == FAILED; x++, (*i)++)
	fprintf(stdout, " $%.2x", in[*i]);

      if (address == ON)
	fprintf(stdout, "\t; $%.4x\n", t);
      else
	fprintf(stdout, "\n");
    }
  }

  (*b)++;
  slot = 1;
  return SUCCEEDED;
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
      databanks = ON;
      continue;

    case 's':
      strings = ON;
      continue;

    case 'a':
      address = OFF;
      continue;

    default:
      return FAILED;
    }
  }

  return SUCCEEDED;
}
