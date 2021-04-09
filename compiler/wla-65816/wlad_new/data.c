
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "data.h"


extern unsigned char *in;
extern int arg_only_code, arg_strings, arg_address, arg_labels, arg_mode, arg_value, fs;

struct label *labels_first = NULL, *labels_last = NULL;


int add_a_new_label(int address, char *label) {

  struct label *l;


  l = malloc(sizeof(struct label));
  if (l == NULL) {
    fprintf(stderr, "ADD_A_NEW_LABEL: Out of memory error.\n");
    return FAILED;
  }

  strcpy(l->label, label);
  l->address = address;
  l->next = NULL;

  if (labels_first == NULL) {
    labels_first = l;
    labels_last = l;
  }
  else {
    labels_last->next = l;
    labels_last = l;
  }

  return SUCCEEDED;
}


int get_mnemonic_type(int m, int *o) {


  return SUCCEEDED;
}
