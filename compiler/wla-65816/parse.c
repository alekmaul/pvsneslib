
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#include "parse.h"
#include "pass_1.h"
#include "stack.h"


int input_number_error_msg = YES, ss, string_size, input_float_mode = OFF, parse_floats = YES;
int newline_beginning = ON;
char label[MAX_NAME_LENGTH], xyz[256];
char unevaluated_expression[256];
char expanded_macro_string[256];
double flo;

extern int i, size, d, macro_active;
extern char *buffer, tmp[4096], cp[256];
extern struct active_file_info *active_file_info_first, *active_file_info_last, *active_file_info_tmp;
extern struct definition *defines, *tmp_def, *next_def;
extern struct macro_runtime *macro_runtime_current;
extern int latest_stack;

#if defined(MCS6502) || defined(W65816) || defined(MCS6510) || defined(WDC65C02) || defined(HUC6280)
int operand_hint;
#endif


int compare_next_token(char *in, int s) {

  int a, t, d, k;
  char e;


  a = i;
  for (e = buffer[a]; e == ' ' || e == ',' || e == 0x0A; e = buffer[++a]);

  /* MACRO mode? */
  if (macro_active != 0 && e == '\\') {
    for (d = 0, k = 0; k < 16; k++) {
      e = buffer[++a];
      if (e >= '0' && e <= '9')
	d = (d * 10) + e - '0';
      else
	break;
    }

    if (d > macro_runtime_current->supplied_arguments) {
      if (input_number_error_msg == YES) {
	sprintf(xyz, "COMPARE_NEXT_SYMBOL: Macro \"%s\" wasn't called with enough arguments.\n", macro_runtime_current->macro->name);
	print_error(xyz, ERROR_NONE);
      }
      return FAILED;
    }

    a = macro_runtime_current->argument_data[d - 1]->start;

    e = buffer[a];
    for (t = 0; t < s && e != ' ' && e != ',' && e != 0x0A; ) {
      if (toupper(in[t]) != toupper(e))
	return FAILED;
      t++;
      e = buffer[++a];
    }
  }
  /* not in MACRO mode */
  else {
    for (t = 0; t < s && e != ' ' && e != ',' && e != 0x0A; ) {
      if (toupper(in[t]) != toupper(e))
	return FAILED;
      t++;
      e = buffer[++a];
    }
  }

  if (t == s)
    return SUCCEEDED;
  else
    return FAILED;
}


int input_next_string(void) {

  char e;
  int k, d;


  for (e = buffer[i++]; e == ' ' || e == ','; e = buffer[i++]);

  if (e == 0x0A)
    return INPUT_NUMBER_EOL;

  /* last choice is a label */
  tmp[0] = e;
  for (k = 1; k < MAX_NAME_LENGTH - 1; k++) {
    e = buffer[i++];
    if (e == 0x0A || e == ',') {
      i--;
      break;
    }
    else if (e == ' ')
      break;
    tmp[k] = e;
  }

  if (k == MAX_NAME_LENGTH - 1) {
    if (input_number_error_msg == YES) {
      sprintf(xyz, "The string is too long (max %d bytes).\n", MAX_NAME_LENGTH - 1);
      print_error(xyz, ERROR_NUM);
    }
    return FAILED;
  }

  tmp[k] = 0;

  /* expand eg. \1 and \@ */
  if (macro_active != 0) {
    d = 0;
    if (expand_macro_arguments(tmp, &d) == FAILED)
      return FAILED;
    if (d != 0)
      strcpy(tmp, expanded_macro_string);
  }

  return SUCCEEDED;
}


int input_number(void) {

  unsigned char e, ee;
  int k, p, q;
  double dn;


#if defined(MCS6502) || defined(W65816) || defined(MCS6510) || defined(WDC65C02) || defined(HUC6280)
  operand_hint = HINT_NONE;
#endif

  for (e = buffer[i++]; e == ' ' || e == ','; e = buffer[i++]);

  if (e == 0x0A)
    return INPUT_NUMBER_EOL;

  /* check the type of the expression */
  p = i;
  ee = e;
  while (ee != 0x0A) {
    /* string / symbol -> no calculating */
    if (ee == '"' || ee == ',' || (ee == '=' && buffer[p] == '=') ||
	(ee == '!' && buffer[p] == '='))
      break;
    if (ee == '-' || ee == '+' || ee == '*' || ee == '/' || ee == '&' || ee == '|' || ee == '^' ||
	ee == '<' || ee == '>' || ee == '#' || ee == '~') {
      /* launch stack calculator */
      p = stack_calculate(&buffer[i - 1], &d);
      if (p == STACK_CALCULATE_DELAY)
	break;
      else if (p == STACK_RETURN_LABEL)
	return INPUT_NUMBER_ADDRESS_LABEL;
      else
	return p;
    }
    ee = buffer[p];
    p++;
  }

  /* MACRO */
  if (macro_active != 0 && e == '\\') {

    struct macro_argument *ma;


    if (buffer[i] == '@') {
      i++;
      d = macro_runtime_current->macro->calls - 1;
      return SUCCEEDED;
    }

    for (d = 0, k = 0; k < 4; k++) {
      e = buffer[i++];
      if (e >= '0' && e <= '9')
        d = (d * 10) + (e - '0');
      else {
        i--;
        break;
      }
    }

    if (d > macro_runtime_current->supplied_arguments) {
      sprintf(xyz, "Referencing argument number %d inside macro \"%s\". The macro has only %d arguments.\n", d, macro_runtime_current->macro->name, macro_runtime_current->supplied_arguments);
      print_error(xyz, ERROR_NUM);
      return FAILED;
    }

    /* return the macro argument */
    ma = macro_runtime_current->argument_data[d - 1];
    k = ma->type;

    if (k == INPUT_NUMBER_ADDRESS_LABEL)
      strcpy(label, ma->string);
    else if (k == INPUT_NUMBER_STRING) {
      strcpy(label, ma->string);
      string_size = strlen(ma->string);
    }
    else if (k == INPUT_NUMBER_STACK)
      latest_stack = ma->value;
    else if (k == SUCCEEDED)
      d = ma->value;
    else {
      print_error("Macro argument list has been corrupted! Please send a bug report!\n", ERROR_ERR);
      return FAILED;
    }

    return k;
  }

  /* is it a hexadecimal value? */
  d = 0;
  if (e >= '0' && e <= '9') {
    for (k = 0; 1; k++) {
      if (buffer[i+k] >= '0' && buffer[i+k] <= '9')
	continue;
      if (buffer[i+k] >= 'a' && buffer[i+k] <= 'f')
	continue;
      if (buffer[i+k] >= 'A' && buffer[i+k] <= 'F')
	continue;
      if (buffer[i+k] == 'h' || buffer[i+k] == 'H') {
	d = 1;
	break;
      }
      break;
    }
  }

  if (e == '$' || d == 1) {
    if (d == 1)
      i--;
    for (d = 0, k = 0; k < 8; k++, i++) {
      e = buffer[i];
      if (e >= '0' && e <= '9')
	d = (d << 4) + e - '0';
      else if (e >= 'A' && e <= 'F')
	d = (d << 4) + e - 'A' + 10;
      else if (e >= 'a' && e <= 'f')
	d = (d << 4) + e - 'a' + 10;
      else if (e == 'h' || e == 'H') {
	i++;
	e = buffer[i];
	break;
      }
      else
	break;
    }

#if defined(MCS6502) || defined(W65816) || defined(MCS6510) || defined(WDC65C02) || defined(HUC6280)
    if (e == '.') {
      e = buffer[i+1];
      if (e == 'b' || e == 'B') {
	operand_hint = HINT_8BIT;
	i += 2;
      }
      else if (e == 'w' || e == 'W') {
	operand_hint = HINT_16BIT;
	i += 2;
      }
#if defined(W65816)
      else if (e == 'l' || e == 'L') {
	operand_hint = HINT_24BIT;
	i += 2;
      }
#endif
    }
#endif

    return SUCCEEDED;
  }

  if (e >= '0' && e <= '9') {
    q = 0;
    flo = e-'0';
    dn = 0.1;
    for (k = 0; k < 9; k++, i++) {
      e = buffer[i];
      if (e >= '0' && e <= '9') {
	if (q == 0) {
	  /* still parsing an integer */
	  flo = flo*10 + e-'0';
	}
	else {
	  flo = flo + dn*(e-'0');
	  dn /= 10.0;
	}
      }
      else if (e == '.') {
	if (q == 1) {
	  print_error("Syntax error.\n", ERROR_NUM);
	  return FAILED;
	}
	e = buffer[i+1];
	if (e >= '0' && e <= '9') {
	  /* float mode, read decimals */
	  if (parse_floats == NO)
	    break;
	  q = 1;
	}
#if defined(MCS6502) || defined(W65816) || defined(MCS6510) || defined(WDC65C02) || defined(HUC6280)
	else if (e == 'b' || e == 'B') {
	  operand_hint = HINT_8BIT;
	  i += 2;
	  break;
	}
	else if (e == 'w' || e == 'W') {
	  operand_hint = HINT_16BIT;
	  i += 2;
	  break;
	}
#if defined(W65816)
	else if (e == 'l' || e == 'L') {
	  operand_hint = HINT_24BIT;
	  i += 2;
	  break;
	}
#endif
#else
	else {
	  print_error("Syntax error.\n", ERROR_NUM);
	  return FAILED;
	}
#endif
      }
      else
	break;
    }

    /* drop the decimals */
    d = flo;

    if (q == 1 && input_float_mode == ON)
      return INPUT_NUMBER_FLOAT;

    return SUCCEEDED;
  }

  if (e == '%') {
    for (d = 0, k = 0; k < 32; k++, i++) {
      e = buffer[i];
      if (e == '0' || e == '1')
	d = (d << 1) + e - '0';
      else
	break;
    }

#if defined(MCS6502) || defined(W65816) || defined(MCS6510) || defined(WDC65C02) || defined(HUC6280)
    if (e == '.') {
      e = buffer[i+1];
      if (e == 'b' || e == 'B') {
	operand_hint = HINT_8BIT;
	i += 2;
      }
      else if (e == 'w' || e == 'W') {
	operand_hint = HINT_16BIT;
	i += 2;
      }
#if defined(W65816)
      else if (e == 'l' || e == 'L') {
	operand_hint = HINT_24BIT;
	i += 2;
      }
#endif
    }
#endif

    return SUCCEEDED;
  }

  if (e == '\'') {
    d = buffer[i++];
    e = buffer[i];
    if (e != '\'') {
      if (input_number_error_msg == YES) {
	sprintf(xyz, "Got '%c' (%d) when expected \"'\".\n", e, e);
	print_error(xyz, ERROR_NUM);
      }
      return FAILED;
    }
    i++;

    return SUCCEEDED;
  }

  if (e == '"') {

    for (k = 0; k < MAX_NAME_LENGTH - 1; ) {
      e = buffer[i++];

      if (e == '\\' && buffer[i] == '"') {
	label[k++] = '"';
	i++;
	continue;
      }

      if (e == '"')
	break;

      if (e == 0 || e == 0x0A) {
	print_error("String wasn't terminated properly.\n", ERROR_NUM);
	return FAILED;
      }

      label[k++] = e;
    }

    label[k] = 0;

    /* expand eg. \1 and \@ */
    if (macro_active != 0) {
      d = 0;
      if (expand_macro_arguments(label, &d) == FAILED)
	return FAILED;
      if (d != 0) {
	strcpy(label, expanded_macro_string);
	k = strlen(label);
      }
    }

    if (k == MAX_NAME_LENGTH - 1) {
      if (input_number_error_msg == YES) {
	sprintf(xyz, "The string is too long (max %d bytes).\n", MAX_NAME_LENGTH - 1);
	print_error(xyz, ERROR_NUM);
      }
      return FAILED;
    }

    label[k] = 0;
    string_size = k;
    return INPUT_NUMBER_STRING;
  }

  /* last choice is a label */
  label[0] = e;
  for (k = 1; k < MAX_NAME_LENGTH - 1; k++) {
    e = buffer[i++];
    if (e == 0x0A || e == ')' || e == ',' || e == ']') {
      i--;
      break;
    }
    else if (e == ' ')
      break;
    label[k] = e;
  }

  if (k == MAX_NAME_LENGTH - 1) {
    if (input_number_error_msg == YES) {
      sprintf(xyz, "The label is too long (max %d bytes).\n", MAX_NAME_LENGTH - 1);
      print_error(xyz, ERROR_NUM);
    }
    return FAILED;
  }

#if defined(MCS6502) || defined(W65816) || defined(MCS6510) || defined(WDC65C02) || defined(HUC6280)
  /* size hint? */
  if (label[k-2] == '.') {
    if (label[k-1] == 'b' || label[k-1] == 'B') {
      operand_hint = HINT_8BIT;
      k -= 2;
    }
    else if (label[k-1] == 'w' || label[k-1] == 'W') {
      operand_hint = HINT_16BIT;
      k -= 2;
    }
#if defined(W65816)
    else if (label[k-1] == 'l' || label[k-1] == 'L') {
      operand_hint = HINT_24BIT;
      k -= 2;
    }
#endif

  }
#endif

  label[k] = 0;

  /* expand eg. \1 and \@ */
  if (macro_active != 0) {
    d = 0;
    if (expand_macro_arguments(label, &d) == FAILED)
      return FAILED;
    if (d != 0)
      strcpy(label, expanded_macro_string);
  }

  /* check if the label is actually a definition */
  tmp_def = defines;
  while (tmp_def != NULL) {
    if (strcmp(label, tmp_def->alias) == 0) {
      if (tmp_def->type == DEFINITION_TYPE_VALUE) {
	d = tmp_def->value;
	return SUCCEEDED;
      }
      else if (tmp_def->type == DEFINITION_TYPE_STACK) {
	/* skip stack definitions -> use its name instead */
      }
      else {
	string_size = tmp_def->size;
	memcpy(label, tmp_def->string, string_size);
	label[string_size] = 0;
	return INPUT_NUMBER_STRING;
      }
    }
    tmp_def = tmp_def->next;
  }

  return INPUT_NUMBER_ADDRESS_LABEL;
}


int get_next_token(void) {

  int q;


  while (1) {
    if (i == size)
      break;
    if (buffer[i] == ' ') {
      i++;
      newline_beginning = OFF;
      continue;
    }
    if (buffer[i] == 0x0A) {
      i++;
      next_line();
      continue;
    }
    break;
  }

  if (buffer[i] == '"') {
    for (ss = 0, i++; buffer[i] != 0x0A && buffer[i] != '"'; ) {
      if (buffer[i] == '\\' && buffer[i + 1] == '"') {
	tmp[ss++] = '"';
	i += 2;
      }
      else
	tmp[ss++] = buffer[i++];
    }

    if (buffer[i] == 0x0A) {
      print_error("GET_NEXT_TOKEN: String wasn't terminated properly.\n", ERROR_NONE);
      return FAILED;
    }
    tmp[ss] = 0;
    i++;

    /* expand eg. \1 and \@ */
    if (macro_active != 0) {
      q = 0;
      if (expand_macro_arguments(tmp, &q) == FAILED)
	return FAILED;
      if (q != 0) {
	strcpy(tmp, expanded_macro_string);
	ss = strlen(tmp);
      }
    }

    return GET_NEXT_TOKEN_STRING;
  }

  if (buffer[i] == '.') {
    tmp[0] = '.';
    i++;
    for (ss = 1; buffer[i] != 0x0A && buffer[i] != ' ' && ss <= MAX_NAME_LENGTH; ) {
      tmp[ss] = buffer[i];
      cp[ss - 1] = toupper((int)buffer[i]);
      i++;
      ss++;
    }
    cp[ss - 1] = 0;
  }
  else if (buffer[i] == '=' || buffer[i] == '>' || buffer[i] == '<' || buffer[i] == '!') {
    for (ss = 0; buffer[i] != 0x0A && (buffer[i] == '=' || buffer[i] == '!' || buffer[i] == '<' || buffer[i] == '>')
	   && ss <= MAX_NAME_LENGTH; tmp[ss++] = buffer[i++]);
  }
  else {
    for (ss = 0; buffer[i] != 0x0A && buffer[i] != ',' && buffer[i] != ' ' && ss <= MAX_NAME_LENGTH; ) {
      tmp[ss] = buffer[i];
      ss++;
      i++;
    }
    if (buffer[i] == ',')
      i++;
  }

  if (ss > MAX_NAME_LENGTH) {
    print_error("GET_NEXT_TOKEN: Too long for a token.\n", ERROR_NONE);
    return FAILED;
  }

  tmp[ss] = 0;

  /* expand eg. \1 and \@ */
  if (macro_active != 0) {
    q = 0;
    if (expand_macro_arguments(tmp, &q) == FAILED)
      return FAILED;
    if (q != 0) {
      strcpy(tmp, expanded_macro_string);
      ss = strlen(tmp);
    }
  }

  return SUCCEEDED;
}


int skip_next_token(void) {

  for (; buffer[i] == ' ' || buffer[i] == ','; i++);

  if (buffer[i] == 0x0A)
    return FAILED;

  if (buffer[i] == '"') {
    for (i++; buffer[i] != 0x0A && buffer[i] != '"'; i++);
    if (buffer[i] == 0x0A) {
      print_error("SKIP_NEXT_TOKEN: String wasn't terminated properly.\n", ERROR_NONE);
      return FAILED;
    }
    i++;

    return SUCCEEDED;
  }

  for (; buffer[i] != 0x0A && buffer[i] != ' ' && buffer[i] != ','; i++);

  return SUCCEEDED;
}


int expand_macro_arguments(char *in, int *s) {

  char t[256];
  int i, k, d;


  for (i = 0; i < MAX_NAME_LENGTH; i++) {
    if (in[i] == '\\') {
      if (in[i + 1] == '"' || in[i + 1] == 'n' || in[i + 1] == '\\') {
	expanded_macro_string[i] = in[i];
	i++;
	expanded_macro_string[i] = in[i];
	continue;
      }
      break;
    }
    expanded_macro_string[i] = in[i];
    if (in[i] == 0)
      return SUCCEEDED;
  }

  k = i;
  i++;

  (*s)++;

  if (in[i] == '@') {
    i++;
    sprintf(&expanded_macro_string[k], "%d%c", macro_runtime_current->macro->calls - 1, 0);
    k = strlen(expanded_macro_string);

    for (; i < MAX_NAME_LENGTH; i++, k++) {
      expanded_macro_string[k] = in[i];
      if (in[i] == 0) {
	break;
      }
    }

    strcpy(t, expanded_macro_string);
    expand_macro_arguments(t, &d);
    return SUCCEEDED;
  }

  if (in[i] <= '0' || in[i] >= '9') {
    if (input_number_error_msg == YES) {
      sprintf(xyz, "EXPAND_MACRO_ARGUMENTS: Unsupported special character '%c'.\n", in[i]);
      print_error(xyz, ERROR_NUM);
    }
    return FAILED;
  }

  for (d = 0; in[i] != 0; i++) {
    if (in[i] >= '0' && in[i] <= '9')
      d = (d * 10) + in[i] - '0';
    else
      break;
  }

  if (d > macro_runtime_current->supplied_arguments) {
    if (input_number_error_msg == YES) {
      sprintf(xyz, "Macro \"%s\" wasn't called with enough arguments.\n", macro_runtime_current->macro->name);
      print_error(xyz, ERROR_NUM);
    }
    return FAILED;
  }

  d = macro_runtime_current->argument_data[d - 1]->start;

  for (; k < MAX_NAME_LENGTH; d++, k++) {
    if (buffer[d] == 0 || buffer[d] == ' ' || buffer[d] == 0x0A || buffer[d] == ',')
      break;
    expanded_macro_string[k] = buffer[d];
  }

  for (; k < MAX_NAME_LENGTH; i++, k++) {
    expanded_macro_string[k] = in[i];
    if (in[i] == 0) {
      break;
    }
  }

  strcpy(t, expanded_macro_string);
  expand_macro_arguments(t, &d);

  return SUCCEEDED;
}
