
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

#include "include_file.h"
#include "pass_1.h"
#include "parse.h"


extern int ind, inz, i, unfolded_size, extra_definitions, d;
extern char *unfolded_buffer, emsg[256], tmp[4096];
extern FILE *file_out_ptr;

struct incbin_file_data *incbin_file_data_first = NULL, *ifd_tmp;
struct active_file_info *active_file_info_first = NULL, *active_file_info_last = NULL, *active_file_info_tmp;
struct file_name_info *file_name_info_first = NULL, *file_name_info_last = NULL, *file_name_info_tmp;
char *include_in_tmp = NULL, *tmp_a = NULL;
int include_in_tmp_size = 0, tmp_a_size = 0, file_name_id = 1, open_files = 0;

char *full_name = NULL;
int full_name_size = 0;

char *include_dir = NULL;
int include_dir_size = 0;

char *buffer = NULL;
int size = 0;


int create_full_name(char *dir, char *name) {

  char *t;
  int i;


  if (dir == NULL && name == NULL)
    return SUCCEEDED;

  /* compute the length of the new string */
  i = 0;
  if (dir != NULL)
    i += strlen(dir);
  if (name != NULL)
    i += strlen(name);
  i++;

  if (i > full_name_size) {
    t = realloc(full_name, i);
    if (t == NULL) {
      fprintf(stderr, "CREATE_FULL_NAME: Out of memory error.\n");
      return FAILED;
    }
    full_name = t;
    full_name_size = i;
  }

  if (dir != NULL) {
    strcpy(full_name, dir);
    if (name != NULL)
      strcat(full_name, name);
  }
  else
    strcpy(full_name, name);

  return SUCCEEDED;
}


int include_file(char *name) {

  static int first_load = 0;
  int file_size, id;
  char *tmp_b, *n;
  FILE *f;


  /* create the full output file name */
  if (create_full_name(include_dir, name) == FAILED)
    return FAILED;

  f = fopen(full_name, "rb");
  id = 0;

  if (f == NULL && (include_dir == NULL || include_dir[0] == 0)) {
    sprintf(emsg, "Error opening file \"%s\".\n", name);
    if (first_load == 0)
      fprintf(stderr, "INCLUDE_FILE: %s", emsg);
    else
      print_error(emsg, ERROR_INC);
    return FAILED;
  }

  /* if failed try to find the file in the current directory */
  if (f == NULL) {
    fprintf(stderr, "%s:%d: ", get_file_name(active_file_info_last->filename_id), active_file_info_last->line_current);
    fprintf(stderr, "INCLUDE_FILE: Could not open \"%s\", trying \"%s\"... ", full_name, name);
    f = fopen(name, "rb");
    id = 1;
  }

  if (f == NULL) {
    fprintf(stderr, "not found.\n");
    sprintf(emsg, "Error opening file \"%s\".\n", full_name);
    if (first_load == 0)
      fprintf(stderr, "INCLUDE_FILE: %s", emsg);
    else
      print_error(emsg, ERROR_INC);
    return FAILED;
  }

  if (id == 1) {
    fprintf(stderr, "found.\n");
    strcpy(full_name, name);
  }

  first_load = 1;

  if (extra_definitions == ON) {
    redefine("WLA_FILENAME", 0.0, name, DEFINITION_TYPE_STRING, strlen(name));
    redefine("wla_filename", 0.0, name, DEFINITION_TYPE_STRING, strlen(name));
  }

  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  active_file_info_tmp = malloc(sizeof(struct active_file_info));
  if (active_file_info_tmp == NULL) {
    sprintf(emsg, "Out of memory while trying allocate error tracking data structure for file \"%s\".\n", full_name);
    print_error(emsg, ERROR_INC);
    return FAILED;
  }
  active_file_info_tmp->next = NULL;

  if (active_file_info_first == NULL) {
    active_file_info_first = active_file_info_tmp;
    active_file_info_last = active_file_info_tmp;
    active_file_info_tmp->prev = NULL;
  }
  else {
    active_file_info_tmp->prev = active_file_info_last;
    active_file_info_last->next = active_file_info_tmp;
    active_file_info_last = active_file_info_tmp;
  }

  active_file_info_tmp->line_current = 1;

  /* name */
  file_name_info_tmp = file_name_info_first;
  id = 0;
  while (file_name_info_tmp != NULL) {
    if (strcmp(file_name_info_tmp->name, full_name) == 0) {
      id = file_name_info_tmp->id;
      active_file_info_tmp->filename_id = id;
      break;
    }
    file_name_info_tmp = file_name_info_tmp->next;
  }

  if (id == 0) {
    file_name_info_tmp = malloc(sizeof(struct file_name_info));
    n = malloc(strlen(full_name)+1);
    if (file_name_info_tmp == NULL || n == NULL) {
      if (file_name_info_tmp != NULL)
	free(file_name_info_tmp);
      if (n != NULL)
	free(n);
      sprintf(emsg, "Out of memory while trying allocate info structure for file \"%s\".\n", full_name);
      print_error(emsg, ERROR_INC);
      return FAILED;
    }
    file_name_info_tmp->next = NULL;

    if (file_name_info_first == NULL) {
      file_name_info_first = file_name_info_tmp;
      file_name_info_last = file_name_info_tmp;
    }
    else {
      file_name_info_last->next = file_name_info_tmp;
      file_name_info_last = file_name_info_tmp;
    }

    strcpy(n, full_name);
    file_name_info_tmp->name = n;
    active_file_info_tmp->filename_id = file_name_id;
    file_name_info_tmp->id = file_name_id;
    file_name_id++;
  }

  /* reallocate buffer */
  if (include_in_tmp_size < file_size) {

    if (include_in_tmp != NULL)
      free(include_in_tmp);

    include_in_tmp = malloc(sizeof(char) * file_size);
    if (include_in_tmp == NULL) {
      sprintf(emsg, "Out of memory while trying to allocate room for \"%s\".\n", full_name);
      print_error(emsg, ERROR_INC);
      return FAILED;
    }

    include_in_tmp_size = file_size;
  }

  /* read the whole file into a buffer */
  fread(include_in_tmp, 1, file_size, f);
  fclose(f);

  if (size == 0) {
    buffer = malloc(sizeof(char) * (file_size + 4));
    if (buffer == NULL) {
      sprintf(emsg, "Out of memory while trying to allocate room for \"%s\".\n", full_name);
      print_error(emsg, ERROR_INC);
      return FAILED;
    }

    /* preprocess */
    preprocess_file(include_in_tmp, include_in_tmp + file_size, buffer, &size, full_name);

    buffer[size++] = 0xA;
    buffer[size++] = '.';
    buffer[size++] = 'E';
    buffer[size++] = ' ';

    open_files++;

    return SUCCEEDED;
  }

  tmp_b = malloc(sizeof(char) * (size + file_size + 4));
  if (tmp_b == NULL) {
    sprintf(emsg, "Out of memory while trying to expand the project to incorporate file \"%s\".\n", full_name);
    print_error(emsg, ERROR_INC);
    return FAILED;
  }

  /* reallocate tmp_a */
  if (tmp_a_size < file_size) {

    if (tmp_a != NULL)
      free(tmp_a);

    tmp_a = malloc(sizeof(char) * (file_size + 4));
    if (tmp_a == NULL) {
      sprintf(emsg, "Out of memory while allocating new room for \"%s\".\n", full_name);
      print_error(emsg, ERROR_INC);
      return FAILED;
    }

    tmp_a_size = file_size + 4;
  }

  /* preprocess */
  inz = 0;
  preprocess_file(include_in_tmp, include_in_tmp + file_size, tmp_a, &inz, full_name);

  tmp_a[inz++] = 0xA;
  tmp_a[inz++] = '.';
  tmp_a[inz++] = 'E';
  tmp_a[inz++] = ' ';

  open_files++;

  memcpy(tmp_b, buffer, i);
  memcpy(tmp_b + i, tmp_a, inz);
  memcpy(tmp_b + i + inz, buffer + i, size - i);

  free(buffer);

  size += inz;
  buffer = tmp_b;

  return SUCCEEDED;
}


int incbin_file(char *name, int *id, int *swap, int *skip, int *read) {

  struct incbin_file_data *ifd;
  char *in_tmp, *n;
  int file_size, q;
  FILE *f;


  /* create the full output file name */
  if (create_full_name(include_dir, name) == FAILED)
    return FAILED;

  f = fopen(full_name, "rb");
  q = 0;

  if (f == NULL && (include_dir == NULL || include_dir[0] == 0)) {
    sprintf(emsg, "Error opening file \"%s\".\n", name);
    print_error(emsg, ERROR_INB);
    return FAILED;
  }

  /* if failed try to find the file in the current directory */
  if (f == NULL) {
    fprintf(stderr, "%s:%d: ", get_file_name(active_file_info_last->filename_id), active_file_info_last->line_current);
    fprintf(stderr, "INCBIN_FILE: Could not open \"%s\", trying \"%s\"... ", full_name, name);
    f = fopen(name, "rb");
    q = 1;
  }

  if (f == NULL) {
    fprintf(stderr, "not found.\n");
    sprintf(emsg, "Error opening file \"%s\".\n", full_name);
    print_error(emsg, ERROR_INB);
    return FAILED;
  }

  if (q == 1) {
    fprintf(stderr, "found.\n");
    strcpy(full_name, name);
  }

  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fseek(f, 0, SEEK_SET);

  ifd = (struct incbin_file_data *)malloc(sizeof(struct incbin_file_data));
  n = malloc(sizeof(char) * (strlen(full_name)+1));
  in_tmp = (char *)malloc(sizeof(char) * file_size);
  if (ifd == NULL || n == NULL || in_tmp == NULL) {
    if (ifd != NULL)
      free(ifd);
    if (n != NULL)
      free(n);
    if (in_tmp != NULL)
      free(in_tmp);
    sprintf(emsg, "Out of memory while allocating data structure for \"%s\".\n", full_name);
    print_error(emsg, ERROR_INB);
    return FAILED;
  }

  /* read the whole file into a buffer */
  fread(in_tmp, 1, file_size, f);
  fclose(f);

  /* complete the structure */
  ifd->next = NULL;
  ifd->size = file_size;
  strcpy(n, name);
  ifd->name = n;
  ifd->data = in_tmp;

  /* find the index */
  q = 0;
  if (incbin_file_data_first != NULL) {
    ifd_tmp = incbin_file_data_first;
    while (ifd_tmp->next != NULL && strcmp(ifd_tmp->name, name) != 0) {
      ifd_tmp = ifd_tmp->next;
      q++;
    }
    if (ifd_tmp->next == NULL) {
      ifd_tmp->next = ifd;
      q++;
    }
  }
  else
    incbin_file_data_first = ifd;

  *id = q;

  /* SKIP bytes? */
  if (compare_next_token("SKIP", 4) == FAILED)
    *skip = 0;
  else {
    skip_next_token();
    inz = input_number();
    if (inz != SUCCEEDED) {
      print_error(".INCBIN needs the amount of skipped bytes.\n", ERROR_DIR);
      return FAILED;
    }

    *skip = d;
  }

  /* READ bytes? */
  if (compare_next_token("READ", 4) == FAILED)
    *read = file_size - *skip;
  else {
    skip_next_token();
    inz = input_number();
    if (inz != SUCCEEDED) {
      print_error(".INCBIN needs the amount of bytes for reading.\n", ERROR_DIR);
      return FAILED;
    }

    *read = d;
  }

  /* SWAP bytes? */
  if (compare_next_token("SWAP", 4) == FAILED)
    *swap = 0;
  else {
    if ((*read & 1) == 1) {
      sprintf(emsg, "File \"%s\" size is odd (%d)! Cannot perform SWAP.\n", full_name, *read);
      print_error(emsg, ERROR_INB);
      return FAILED;
    }
    *swap = 1;
    skip_next_token();
  }

  /* FSIZE? */
  if (compare_next_token("FSIZE", 5) == SUCCEEDED) {
    skip_next_token();

    /* get the definition label */
    if (get_next_token() == FAILED)
      return FAILED;

    add_a_new_definition(tmp, (double)file_size, NULL, DEFINITION_TYPE_VALUE, 0);
  }

  if (*skip + *read > file_size) {
    sprintf(emsg, "Overreading file \"%s\".\n", full_name);
    print_error(emsg, ERROR_INB);
    return FAILED;
  }

  return SUCCEEDED;
}


char get_file_name_error[] = "GET_FILE_NAME: Internal error.";


char *get_file_name(int id) {

  struct file_name_info *f;


  f = file_name_info_first;
  while (f != NULL) {
    if (id == f->id)
      return f->name;
    f = f->next;
  }

  return get_file_name_error;
}


int print_file_names(void) {

  struct incbin_file_data *d;
  struct file_name_info *f;


  f = file_name_info_first;
  d = incbin_file_data_first;

  /* handle the main file name differently */
  if (f != NULL) {
    if (f->next != NULL || d != NULL)
      fprintf(stderr, "%s \\\n", f->name);
    else
      fprintf(stderr, "%s\n", f->name);
    f = f->next;
  }

  /* included files */
  while (f != NULL) {
    if (f->next != NULL || d != NULL)
      fprintf(stderr, "\t%s \\\n", f->name);
    else
      fprintf(stderr, "\t%s\n", f->name);
    f = f->next;
  }

  /* incbin files */
  while (d != NULL) {
    if (d->next != NULL)
      fprintf(stderr, "\t%s \\\n", d->name);
    else
      fprintf(stderr, "\t%s\n", d->name);
    d = d->next;
  }

  return SUCCEEDED;
}


/* the mystery preprocessor - touch it and prepare for trouble ;) the preprocessor
   removes as much white space as possible from the source file. this is to make
   the parsing of the file, that follows, simpler. */
int preprocess_file(char *c, char *d, char *o, int *s, char *f) {

  register int t = 0, z = 0;
#if defined(W65816) || defined(SPC700)
  register int u = 0;
#endif
  char *e;


  e = o + (*s);
  while (c < d) {
    switch (*c) {
    case ';':
      /* clear a commented line */
      c++;
      for ( ; c < d && *c != 0x0A && *c != 0x0D; c++);
      e--;
      for ( ; e > o && *e == ' '; e--);
      if (e < o)
	e = o;
      else if (*e != ' ')
	e++;
      break;
    case '*':
      if (t == 0) {
	/* clear a commented line */
	c++;
	for ( ; c < d && *c != 0x0A && *c != 0x0D; c++);
      }
      else {
	/* multiplication! */
	c++;
	*e = '*';
	e++;
	t = 1;
      }
      break;
    case '/':
      if (*(c + 1) == '*') {
	/* remove an ANSI C -style block comment */
	t = 0;
	c += 2;
	while (t == 0) {
	  for ( ; c < d && *c != '/' && *c != 0x0A; c++);
	  if (c >= d) {
	    sprintf(emsg, "Comment wasn't terminated properly in file \"%s\".\n", f);
	    print_error(emsg, ERROR_INC);
	    return FAILED;
	  }
	  if (*c == 0x0A) {
	    *e = 0x0A;
	    e++;
	  }
	  if (*c == '/' && *(c - 1) == '*') {
	    t = 1;
	  }
	  c++;
	}
	e--;
	for ( ; e > o && *e == ' '; e--);
	if (e < o)
	  e = o;
	else if (*e != ' ')
	  e++;
      }
      else {
	c++;
	*e = '/';
	e++;
	t = 1;
      }
      break;
    case ':':
      /* finding a label resets the counters */
      c++;
      *e = ':';
      e++;
      t = 0;
      break;
    case 0x09:
    case ' ':
      /* remove extra white space */
      c++;
      *e = ' ';
      e++;
      for ( ; c < d && (*c == ' ' || *c == 0x09); c++);
      t = 1;
      if (z == 1)
	z = 2;
      break;
    case 0x0A:
      /* take away white space from the end of the line */
      c++;
      e--;
      for ( ; *e == ' '; e--);
      e++;
      *e = 0x0A;
      e++;
      t = 0;
      z = 0;
#if defined(W65816) || defined(SPC700)
      u = 0;
#endif
      break;
    case 0x0D:
      c++;
      break;
    case '\'':
      if (*(c + 2) == '\'') {
	*e = '\'';
	c++;
	e++;
	*e = *c;
	c++;
	e++;
	*e = '\'';
	c++;
	e++;
      }
      else {
	*e = '\'';
	c++;
	e++;
      }
      t = 1;
      break;
    case '"':
      /* don't alter the strings */
      *e = '"';
      c++;
      e++;
      t = 0;
      while (t == 0) {
	for ( ; c < d && *c != '"'; ) {
	  *e = *c;
	  c++;
	  e++;
	}
	if (*c == '"' && *(c - 1) != '\\')
	  t = 1;
	else {
	  *e = '"';
	  c++;
	  e++;
	}
      }
      *e = '"';
      c++;
      e++;
      break;
#if !defined(W65816) && !defined(SPC700)
    case '[':
      /* change '[' to '(' */
#endif
    case '(':
      *e = '(';
      c++;
      e++;
      for ( ; c < d && (*c == ' ' || *c == 0x09); c++);
      t = 1;
      break;
#if !defined(W65816) && !defined(SPC700)
    case ']':
      /* change ']' to ')' */
#endif
    case ')':
      /* go back? */
      if (t == 1 && *(e - 1) == ' ')
	e--;
      *e = ')';
      c++;
      e++;
      t = 1;
      break;
#if defined(W65816) || defined(SPC700)
    case '[':
      *e = *c;
      c++;
      e++;
      t = 1;
      u = 1;
      break;
#endif
    case ',':
    case '+':
    case '-':
      if (t == 0) {
	for ( ; c < d && (*c == '+' || *c == '-'); c++, e++)
	  *e = *c;
	t = 1;
      }
      else {
#if defined(W65816) || defined(SPC700)
	/* go back? */
	if (*(e - 1) == ' ' && u == 1)
	  e--;
#else
	/* go back? */
	if ((z == 3 || *c == ',') && *(e - 1) == ' ')
	  e--;
#endif
	*e = *c;
	c++;
	e++;
	for ( ; c < d && (*c == ' ' || *c == 0x09); c++);
	t = 1;
      }
      break;
    default:
      *e = *c;
      c++;
      e++;
      if (z == 0)
	z = 1;
      else if (z == 2)
	z = 3;
      t = 1;
      break;
    }
  }

  *s = e - o;

  return SUCCEEDED;
}
