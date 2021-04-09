
/*
	wla - part of wla dx gb-z80/z80/6502/65c02/6510/65816/huc6280/spc-700
	macro assembler package by ville helin <vhelin@iki.fi>.
	this is gpl software.
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef UNIX
#include <unistd.h>
#endif

#include "main.h"
#include "defines.h"

#include "parse.h"
#include "include_file.h"
#include "pass_1.h"
#include "pass_2.h"
#include "pass_3.h"
#include "pass_4.h"
#include "listfile.h"


FILE *file_out_ptr = NULL;

/* amiga specific definitions */

#ifdef AMIGA
long __stack = 65536;
#endif

#ifdef AMIGA
#ifdef GB
char version_string[] = "$VER: WLA-GB 9.4 (09.09.2006)";
#endif
#ifdef Z80
char version_string[] = "$VER: WLA-Z80 9.4 (09.09.2006)";
#endif
#ifdef MCS6502
char version_string[] = "$VER: WLA-6502 9.4 (09.09.2006)";
#endif
#ifdef WDC65C02
char version_string[] = "$VER: WLA-65C02 9.4 (09.09.2006)";
#endif
#ifdef W65816
char version_string[] = "$VER: WLA-65816 9.4 (09.09.2006)";
#endif
#ifdef MCS6510
char version_string[] = "$VER: WLA-6510 9.4 (09.09.2006)";
#endif
#ifdef SPC700
char version_string[] = "$VER: WLA-SPC700 9.4 (09.09.2006)";
#endif
#ifdef HUC6280
char version_string[] = "$VER: WLA-HuC6280 9.4 (09.09.2006)";
#endif
#endif

char wla_version[] = "9.4";

char gba_tmp_name[32], gba_unfolded_name[32];

extern struct incbin_file_data *incbin_file_data_first, *ifd_tmp;
extern struct file_name_info *file_name_info_first;
extern struct label_def *label_tmp, *labels;
extern struct macro_static *macros_first;
extern struct definition *defines, *tmp_def;
extern struct export_def *export_first, *export_last;
extern struct stack *stacks_first, *stacks_tmp, *stacks_last, *stacks_header_first, *stacks_header_last;
extern struct repeat_runtime *repeat_stack;
extern struct section_def *sections_first;
extern struct macro_runtime *macro_stack;
extern struct label_def *unknown_labels;
extern struct filepointer *filepointers;
extern char *unfolded_buffer;
extern char *include_in_tmp, *tmp_a;
extern char *rom_banks, *rom_banks_usage_table;
extern char *include_dir, *buffer, *full_name;
extern int include_in_tmp_size, tmp_a_size, *banks, *bankaddress;

int output_format = OUTPUT_NONE, verbose_mode = OFF, test_mode = OFF;
int extra_definitions = OFF, commandline_parsing = ON, makefile_rules = NO;
int listfile_data = NO, quiet = NO;

char *final_name = NULL, *asm_name = NULL;



int main(int argc, char *argv[]) {

  int i = SUCCEEDED;

  if (sizeof(double) != 8) {
    fprintf(stderr, "MAIN: sizeof(double) == %d != 8. WLA will not work properly.\n", (int)sizeof(double));
    return -1;
  }

  atexit(procedures_at_exit);

  /* init the randon number generator */
  srand(time(NULL));

  if (argc >= 3) {
    if (parse_flags(argv[1]) == SUCCEEDED) {
      if (parse_defines_and_get_final_name(argv + 2, argc - 2) == FAILED)
				return 1;
    }
    else
      i = FAILED;
  }
  else
    i = FAILED;

  if (i == FAILED || output_format == OUTPUT_NONE) {
#ifdef GB
    printf("\nWLA GB-Z80 Macro Assembler v9.4\n");
#endif
#ifdef Z80
    printf("\nWLA Z80 Macro Assembler v9.4\n");
#endif
#ifdef MCS6502
    printf("\nWLA 6502 Macro Assembler v9.4\n");
#endif
#ifdef WDC65C02
    printf("\nWLA 65C02 Macro Assembler v9.4\n");
#endif
#ifdef W65816
    printf("\nWLA 65816 Macro Assembler v9.4-pvs2\n");
#endif
#ifdef MCS6510
    printf("\nWLA 6510 Macro Assembler v9.4\n");
#endif
#ifdef SPC700
    printf("\nWLA SPC-700 Macro Assembler v9.4\n");
#endif
#ifdef HUC6280
    printf("\nWLA HuC6280 Macro Assembler v9.4\n");
#endif
    printf("Written by Ville Helin in 1998-2006\n");
#ifdef W65816
    printf("Modified for PVSneslib by Alekmaul in 2021\n");
#endif
    printf("USAGE: %s -[iMqtvx]{lo} [DEFINITIONS] <ASM FILE> [OUTPUT FILE]\n", argv[0]);
    printf("Commands:             Options:\n");
    printf("l  Library file       i  Add list file information\n");
    printf("o  Object file        M  Output makefile rules\n");
    printf("                      q  Quiet\n");
    printf("                      t  Test compile\n");
    printf("                      v  Verbose messages\n");
    printf("                      x  Extra compile time definitions\n\n");

    return 0;
  }

  if (strcmp(asm_name, final_name) == 0) {
    fprintf(stderr, "MAIN: Input and output files share the same name!\n");
    return 1;
  }

  generate_tmp_names();

  file_out_ptr = fopen(gba_tmp_name, "wb");
  if (file_out_ptr == NULL) {
    fprintf(stderr, "MAIN: Error opening file \"%s\".\n", gba_tmp_name);
    return 1;
  }

  /* small inits */
  if (extra_definitions == ON)
    generate_extra_definitions();

  commandline_parsing = OFF;

  /* start the process */
  if (include_file(asm_name) == FAILED)
    return 1;

  if (pass_1() == FAILED)
    return 1;
  if (pass_2() == FAILED)
    return 1;
  if (pass_3() == FAILED)
    return 1;
  if (listfile_data == YES) {
    if (listfile_collect() == FAILED)
      return 1;
  }
  if (pass_4() == FAILED)
    return 1;

  return 0;
}


int parse_flags(char *f) {

  int l;

  if (*f != '-')
    return FAILED;

  l = strlen(f);
  if (l == 1)
    return FAILED;

  for (f++, l--; l > 0; l--, f++) {
    switch (*f) {

    case 'o':
      if (output_format != OUTPUT_NONE)
				return FAILED;
      output_format = OUTPUT_OBJECT;
      continue;

    case 'l':
      if (output_format != OUTPUT_NONE)
				return FAILED;
      output_format = OUTPUT_LIBRARY;
      continue;

    case 'i':
      listfile_data = YES;
      continue;

    case 'v':
      verbose_mode = ON;
      continue;

    case 't':
      test_mode = ON;
      continue;

    case 'M':
      makefile_rules = YES;
      test_mode = ON;
      verbose_mode = OFF;
      quiet = YES;
      continue;

    case 'q':
      quiet = YES;
      continue;

    case 'x':
      extra_definitions = ON;
      continue;

    default:
      return FAILED;
    }
  }

  return SUCCEEDED;
}


void procedures_at_exit(void) {

  struct file_name_info *f, *ft;
  struct export_def *export_tmp;
  struct section_def *s1, *s2;
  struct label_def *l1, *l2;
  struct macro_static *m;
  struct filepointer *f1, *f2;
  int i;

  /* free all the dynamically allocated data structures and close open files */
  if (file_out_ptr != NULL)
    fclose(file_out_ptr);

  if (macro_stack != NULL)
    free(macro_stack);

  if (repeat_stack != NULL)
    free(repeat_stack);

  if (final_name != NULL)
    free(final_name);

  if (asm_name != NULL)
    free(asm_name);

  if (include_dir != NULL)
    free(include_dir);

  if (full_name != NULL)
    free(full_name);

  tmp_def = defines;
  while (tmp_def != NULL) {
    defines = tmp_def->next;
    free(tmp_def);
    tmp_def = defines;
  }

  m = macros_first;
  while (m != NULL) {
    /* free the argument labels */
    if (m->nargument_names > 0) {
      for (i = 0; i < m->nargument_names; i++)
				free(m->argument_names[i]);
      free(m->argument_names);
    }
    macros_first = m->next;
    free(m);
    m = macros_first;
  }

  label_tmp = labels;
  while (label_tmp != NULL) {
    labels = label_tmp->next;
    free(label_tmp);
    label_tmp = labels;
  }

  l1 = unknown_labels;
  while (l1 != NULL) {
    l2 = l1->next;
    free(l1);
    l1 = l2;
  }

  export_tmp = export_first;
  while (export_tmp != NULL) {
    export_last = export_tmp->next;
    free(export_tmp);
    export_tmp = export_last;
  }

  ifd_tmp = incbin_file_data_first;
  while(ifd_tmp != NULL) {
    incbin_file_data_first = ifd_tmp->next;
    if (ifd_tmp->data != NULL)
      free(ifd_tmp->data);
    if (ifd_tmp->name != NULL)
      free(ifd_tmp->name);
    free(ifd_tmp);
    ifd_tmp = incbin_file_data_first;
  }

  stacks_tmp = stacks_first;
  while (stacks_tmp != NULL) {
    free(stacks_tmp->stack);
    stacks_first = stacks_tmp->next;
    free(stacks_tmp);
    stacks_tmp = stacks_first;
  }

  stacks_tmp = stacks_header_first;
  while (stacks_tmp != NULL) {
    free(stacks_tmp->stack);
    stacks_first = stacks_tmp->next;
    free(stacks_tmp);
    stacks_tmp = stacks_first;
  }

  if (unfolded_buffer != NULL)
    free(unfolded_buffer);

  if (buffer != NULL)
    free(buffer);

  if (include_in_tmp != NULL)
    free(include_in_tmp);
  if (tmp_a != NULL)
    free(tmp_a);

  if (rom_banks != NULL)
    free(rom_banks);
  if (rom_banks_usage_table != NULL)
    free(rom_banks_usage_table);
  if (banks != NULL)
    free(banks);
  if (bankaddress != NULL)
    free(bankaddress);

  f = file_name_info_first;
  while (f != NULL) {
    if (f->name != NULL)
      free(f->name);
    ft = f->next;
    free(f);
    f = ft;
  }

  s1 = sections_first;
  while (s1 != NULL) {
    if (s1->data != NULL)
      free(s1->data);
    if (s1->listfile_cmds != NULL)
      free(s1->listfile_cmds);
    if (s1->listfile_ints != NULL)
      free(s1->listfile_ints);
    s2 = s1->next;
    free(s1);
    s1 = s2;
  }

  f1 = filepointers;
  while (f1 != NULL) {
    f2 = f1->next;
    if (f1->f != NULL)
      fclose(f1->f);
    if (f1->filename != NULL)
      free(f1->filename);
    free(f1);
    f1 = f2;
  }

  /* remove the tmp files */
  remove(gba_tmp_name);
  remove(gba_unfolded_name);
}


int generate_tmp_names(void) {

#ifdef UNIX
  sprintf(gba_tmp_name, ".wla%da", (int)getpid());
  sprintf(gba_unfolded_name, ".wla%db", (int)getpid());
#endif

#ifdef AMIGA
  sprintf(gba_tmp_name, "wla_a.tmp");
  sprintf(gba_unfolded_name, "wla_b.tmp");
#endif

#ifdef MSDOS
#if 1 /*ndef WIN32*/
  sprintf(gba_tmp_name, "wla_a.tmp");
  sprintf(gba_unfolded_name, "wla_b.tmp");
#else
  sprintf(gba_tmp_name, ".wla%lda", GetCurrentProcessId());
  sprintf(gba_unfolded_name, ".wla%ldb", GetCurrentProcessId());
#endif  
#endif

  return SUCCEEDED;
}


int generate_extra_definitions(void) {

  char *q, c[256];
  time_t t;
  int a, s;

  /* generate WLA_TIME */
  time(&t);
  q = ctime(&t);
  strcpy(c, q);
  /* remove the linefeed */
  s = strlen(c);
  for (a = 0; a < s; a++) {
    if (c[a] == 0x0A) {
      c[a] = 0;
      break;
    }
  }

  if (add_a_new_definition("WLA_TIME", 0.0, c, DEFINITION_TYPE_STRING, strlen(c)) == FAILED)
    return FAILED;
  if (add_a_new_definition("wla_time", 0.0, c, DEFINITION_TYPE_STRING, strlen(c)) == FAILED)
    return FAILED;
  if (add_a_new_definition("WLA_VERSION", 0.0, wla_version, DEFINITION_TYPE_STRING, strlen(wla_version)) == FAILED)
    return FAILED;
  if (add_a_new_definition("wla_version", 0.0, wla_version, DEFINITION_TYPE_STRING, strlen(wla_version)) == FAILED)
    return FAILED;

  return SUCCEEDED;
}


int parse_defines_and_get_final_name(char **c, int n) {

  int x;

  while (1) {
    if (n == 0)
      break;
    if (strlen(*c) > 2) {
      if (**c != '-' || *((*c) + 1) != 'D')
				break;
      else
				if (parse_and_add_definition(*c) == FAILED)
					return FAILED;
    }
    if (strlen(*c) <= 2)
      break;
    c++;
    n--;
  }

  /* allocate room for names */
  if (n == 1 || n == 2) {
    asm_name = malloc(strlen(*c)+1);
    if (n == 2)
      final_name = malloc(strlen(*(c+1))+1);
    else
      final_name = malloc(strlen(*c)+1+4);

    if (asm_name == NULL || final_name == NULL) {
      if (asm_name != NULL) {
				free(asm_name);
				asm_name = NULL;
      }
      if (final_name != NULL) {
				free(final_name);
				final_name = NULL;
      }
      fprintf(stderr, "PARSE_DEFINES_AND_GET_FINAL_NAME: Out of memory error.\n");
      return FAILED;
    }
  }

  /* both infile and outfile were given */
  if (n == 2) {
    strcpy(asm_name, *c);
    c++;
    strcpy(final_name, *c);
    return SUCCEEDED;
  }
  /* only the infile was given -> construct the outfile name */
  else if (n == 1) {
    strcpy(asm_name, *c);
    for (x = 0; x < strlen(*c) && *((*c) + x) != '.'; x++)
      final_name[x] = *((*c) + x);
    final_name[x++] = '.';
    if (output_format == OUTPUT_OBJECT) {
      final_name[x++] = 'o';
      final_name[x] = 0;
    }
    else if (output_format == OUTPUT_LIBRARY) {
      final_name[x++] = 'l';
      final_name[x++] = 'i';
      final_name[x++] = 'b';
      final_name[x] = 0;
    }
    return SUCCEEDED;
  }

  fprintf(stderr, "PARSE_DEFINES_AND_GET_FINAL_NAME: Error in commandline options.\n");
  return FAILED;
}


int parse_and_add_definition(char *c) {

  char n[MAX_NAME_LENGTH];
  int i;

  c += 2;
  for (i = 0; i < (MAX_NAME_LENGTH - 1) && *c != 0 && *c != '='; i++, c++)
    n[i] = *c;
  n[i] = 0;

  if (*c == 0)
    return add_a_new_definition(n, 0.0, NULL, DEFINITION_TYPE_VALUE, 0);
  else if (*c == '=') {
    c++;
    if (*c == 0)
      return FAILED;

    /* hexadecimal value? */
    if (*c == '$' || ((c[strlen(c)-1] == 'h' || c[strlen(c)-1] == 'H') && (*c >= '0' && *c <= '9'))) {
      if (*c == '$')
				c++;
      for (i = 0; *c != 0; c++) {
				if (*c >= '0' && *c <= '9')
					i = (i << 4) + *c - '0';
				else if (*c >= 'a' && *c <= 'f')
					i = (i << 4) + *c - 'a' + 10;
				else if (*c >= 'A' && *c <= 'F')
					i = (i << 4) + *c - 'A' + 10;
				else if ((*c == 'h' || *c == 'H') && *(c+1) == 0)
					break;
				else {
					fprintf(stderr, "PARSE_AND_ADD_DEFINITION: Error in value.\n");
					return FAILED;
				}
      }
      return add_a_new_definition(n, (double)i, NULL, DEFINITION_TYPE_VALUE, 0);
    }

    /* decimal value? */
    if (*c >= '0' && *c <= '9') {
      for (i = 0; *c != 0; c++) {
				if (*c >= '0' && *c <= '9')
					i = (i * 10) + *c - '0';
				else {
					fprintf(stderr, "PARSE_AND_ADD_DEFINITION: Error in value.\n");
					return FAILED;
				}
      }
      return add_a_new_definition(n, (double)i, NULL, DEFINITION_TYPE_VALUE, 0);
    }

    /* string definition */
    return add_a_new_definition(n, 0.0, c, DEFINITION_TYPE_STRING, strlen(c));
  }

  return FAILED;
}
