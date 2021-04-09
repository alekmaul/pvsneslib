
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "listfile.h"
#include "files.h"


/* output */
extern unsigned char *rom;


/* read an integer from t */
#define READ_T t[3] + (t[2] << 8) + (t[1] << 16) + (t[0] << 24); t += 4;


static int _listfile_sort(const void *a, const void *b) {

  struct listfileitem *la, *lb;
  int i;


  la = *((struct listfileitem **)a);
  lb = *((struct listfileitem **)b);
  i = strcmp(la->sourcefilename, lb->sourcefilename);
  if (i == 0) {
    /* both lines are from the same source */
    if (la->linenumber < lb->linenumber)
      return -1;
    return 1;
  }

  /* sort by source file name */
  return i;
}


static int _listfile_write_hex(FILE *f, int i) {

  if (i >= 10)
    fprintf(f, "%c", i + 'A' - 10);
  else
    fprintf(f, "%c", i + '0');

  return SUCCEEDED;
}


int listfile_write_listfiles(struct section *e) {

  struct listfileitem **l = NULL, *d = NULL;
  struct section *s;
  int n, i, j, k, m, o, p, sid = -1, add, w;
  char c, tmp[512], *b, *na;
  FILE *f;


  if (e == NULL)
    return FAILED;

  /* collect the list file items from the data */
  n = 0;
  i = 0;
  s = e;
  while (s != NULL) {
    if (s->listfile_items <= 0 || s->status == SECTION_STATUS_RAM) {
      s = s->next;
      continue;
    }

    n += s->listfile_items;
    d = realloc(d, sizeof(struct listfileitem) * n);
    if (d == NULL) {
      fprintf(stderr, "LISTFILE_WRITE_LISTFILES: Out of memory error.\n");
      return FAILED;
    }

    /* parse the list file information */
    add = 0;
    for (j = 0; j < s->listfile_items; j++) {
      c = s->listfile_cmds[j];
      if (c == 'k') {
	/* new line */
	if (s->listfile_ints[j*2 + 1] > 0) {
	  d[i].sourcefilename = get_source_file_name(s->file_id, sid);
	  d[i].linenumber = s->listfile_ints[j*2 + 0];
	  d[i].length = s->listfile_ints[j*2 + 1];
	  d[i].address = s->output_address + add;
	  add += s->listfile_ints[j*2 + 1];
	  i++;
	}
      }
      else if (c == 'f') {
	/* another file */
	sid = s->listfile_ints[j*2 + 0];
      }
    }

    s = s->next;
  }

  /* create pointers for sorting */
  l = malloc(sizeof(struct listfileitem *) * i);
  if (l == NULL) {
    fprintf(stderr, "LISTFILE_WRITE_LISTFILES: Out of memory error.\n");
    return FAILED;
  }

  for (j = 0; j < i; j++)
    l[j] = &d[j];

  /* sort by source file name and line number */
  qsort(l, i, sizeof(struct listfileitem *), _listfile_sort);

  /* write the listfiles */
  j = 0;
  while (j < i) {
    na = l[j]->sourcefilename;
    f = fopen(na, "rb");
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);

    b = malloc(n);
    if (b == NULL) {
      fprintf(stderr, "LISTFILE_WRITE_LISTFILES: Out of memory error.\n");
      fclose(f);
      return FAILED;
    }

    fread(b, 1, n, f);
    fclose(f);

    strcpy(tmp, na);
    for (k = strlen(tmp)-1; k >= 0; k--) {
      if (tmp[k] == '.')
	break;
    }
    strcpy(&tmp[k], ".lst");

    f = fopen(tmp, "wb");
    if (f == NULL) {
      fprintf(stderr, "LISTFILE_WRITE_LISTFILES: Could not open file \"%s\" for writing.\n", tmp);
      return FAILED;
    }

    /* write the lines */
    k = 0;
    m = 0;
    while (j < i && l[j]->sourcefilename == na) {
      /* goto line x */
      while (k < l[j]->linenumber-1) {
	for (o = 0; o < 40; o++)
	  fprintf(f, " ");
	while (1) {
	  if (b[m] == 0xA) {
	    m++;
	    if (b[m] == 0xD)
	      m++;
	    k++;
	    fprintf(f, "\n");
	    break;
	  }
	  else
	    fprintf(f, "%c", b[m++]);
	}
      }

      /* write the bytes */
      w = 0;
      for (p = 0, o = 0; o < l[j]->length; o++) {
	fprintf(f, "$");
	_listfile_write_hex(f, rom[l[j]->address + o] >> 4);
	_listfile_write_hex(f, rom[l[j]->address + o] & 15);
	fprintf(f, " ");
	p += 4;
	if ((o % 10) == 9 && o != 0 && o < l[j]->length-1) {
	  if (w == 0) {
	    /* write padding */
	    w = 1;
	    while (p < 40) {
	      fprintf(f, " ");
	      p++;
	    }

	    /* write the rest of the line */
	    while (m < n) {
	      if (b[m] == 0xA) {
		m++;
		if (b[m] == 0xD)
		  m++;
		k++;
		fprintf(f, "\n");
		break;
	      }
	      else
		fprintf(f, "%c", b[m++]);
	    }
	  }
	  else
	    fprintf(f, "\n");

	  p = 0;
	}
      }

      /* has the line been written already? */
      if (w == 0) {
	/* write padding */
	while (p < 40) {
	  fprintf(f, " ");
	  p++;
	}

	/* write the rest of the line */
	while (m < n) {
	  if (b[m] == 0xA) {
	    m++;
	    if (b[m] == 0xD)
	      m++;
	    k++;
	    fprintf(f, "\n");
	    break;
	  }
	  else
	    fprintf(f, "%c", b[m++]);
	}
      }
      else
	fprintf(f, "\n");

      j++;
    }

    /* write the rest of the file */
    while (m < n) {
      for (o = 0; o < 40; o++)
	fprintf(f, " ");
      while (m < n) {
	if (b[m] == 0xA) {
	  m++;
	  if (b[m] == 0xD)
	    m++;
	  k++;
	  fprintf(f, "\n");
	  break;
	}
	else
	  fprintf(f, "%c", b[m++]);
      }
    }

    fclose(f);

    if (b != NULL)
      free(b);
  }

  if (l != NULL)
    free(l);
  if (d != NULL)
    free(d);

  return SUCCEEDED;
}


int listfile_block_read(unsigned char **d, struct section *s) {

  unsigned char *t;
  int i;


  if (d == NULL || s == NULL)
    return FAILED;

  if (**d == 0) {
    /* no listfile information! */
    s->listfile_items = 0;
    s->listfile_cmds = NULL;
    s->listfile_ints = NULL;
    (*d)++;
    return SUCCEEDED;
  }

  /* we have listfile information */
  t = *d;
  t++;
  s->listfile_items = READ_T;
  s->listfile_cmds = malloc(s->listfile_items);
  s->listfile_ints = malloc(sizeof(int) * s->listfile_items*2);

  if (s->listfile_cmds == NULL || s->listfile_ints == NULL) {
    s->listfile_items = 0;
    fprintf(stderr, "LISTFILE_BLOCK_READ: Out of memory error.\n");
    return FAILED;
  }

  /* read the items */
  for (i = 0; i < s->listfile_items; i++) {
    s->listfile_cmds[i] = *(t++);
    if (s->listfile_cmds[i] == 'k') {
      /* new line */
      s->listfile_ints[i*2 + 0] = READ_T;
      s->listfile_ints[i*2 + 1] = READ_T;
    }
    else if (s->listfile_cmds[i] == 'f') {
      /* file name */
      s->listfile_ints[i*2 + 0] = READ_T;
    }
    else {
      s->listfile_items = 0;
      fprintf(stderr, "LISTFILE_BLOCK_READ: Unknown command %d.\n", s->listfile_cmds[i]);
      return FAILED;
    }
  }

  *d = t;

  return SUCCEEDED;
}
