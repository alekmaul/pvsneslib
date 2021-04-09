
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "analyze.h"
#include "memory.h"
#include "listfile.h"


/* read an integer from t */
#define READ_T t[3] + (t[2] << 8) + (t[1] << 16) + (t[0] << 24); t += 4;

/* read a double from t */
#define READ_DOU {															\
		dtmp = (unsigned char *)&dou;								\
		dtmp[0] = *(t++);														\
		dtmp[1] = *(t++);														\
		dtmp[2] = *(t++);														\
		dtmp[3] = *(t++);														\
		dtmp[4] = *(t++);														\
		dtmp[5] = *(t++);														\
		dtmp[6] = *(t++);														\
		dtmp[7] = *(t++);														\
	}


extern struct object_file *obj_first, *obj_last, *obj_tmp;
extern struct reference *reference_first, *reference_last;
extern struct section *sec_first, *sec_last, *sec_hd_first, *sec_hd_last;
extern struct stack *stacks_first, *stacks_last;
extern struct label *labels_first, *labels_last;
extern struct slot slots[256];
extern int rombanks, verbose_mode, section_overwrite, symbol_mode, discard_unreferenced_sections;
extern int emptyfill;
extern int *banks, *bankaddress;



int add_reference(struct reference *r) {

  r->file_id = obj_tmp->id;
  r->next = NULL;

  if (reference_first == NULL) {
    reference_first = r;
    reference_last = r;
    r->prev = NULL;
  }
  else {
    r->prev = reference_last;
    reference_last->next = r;
    reference_last = r;
  }

  return SUCCEEDED;
}


int add_stack(struct stack *sta) {

	/* parse the type */
	if ((sta->type & (1 << 7)) != 0)
		sta->relative_references = YES;
	else
		sta->relative_references = NO;
	sta->type &= ~(1 << 7);

  sta->file_id = obj_tmp->id;
  sta->next = NULL;
  sta->computed = NO;
  sta->under_work = NO;

  if (stacks_first == NULL) {
    stacks_first = sta;
    stacks_last = sta;
    sta->prev = NULL;
  }
  else {
    sta->prev = stacks_last;
    stacks_last->next = sta;
    stacks_last = sta;
  }

  return SUCCEEDED;
}


int add_section(struct section *s) {

  struct section *q;

  s->file_id = obj_tmp->id;
  s->next = NULL;
  s->alive = YES;

  if (strcmp(s->name, "BANKHEADER") == 0) {
    q = sec_hd_first;
    while (q != NULL) {
      if (q->bank == s->bank) {
				fprintf(stderr, "%s: ADD_SECTION: BANKHEADER section for bank %d was defined for the second time.\n", obj_tmp->name, s->bank);
				return FAILED;
      }
      q = q->next;
    }

    if (sec_hd_first == NULL) {
      sec_hd_first = s;
      sec_hd_last = s;
    }
    else {
      sec_hd_last->next = s;
      sec_hd_last = s;
    }
  }
  else {
    if (sec_first == NULL) {
      sec_first = s;
      sec_last = s;
      s->prev = NULL;
    }
    else {
      s->prev = sec_last;
      sec_last->next = s;
      sec_last = s;
    }
  }

  return SUCCEEDED;
}


int add_label(struct label *l) {

  l->next = NULL;

  if (labels_first == NULL) {
    labels_first = l;
    labels_last = l;
    l->prev = NULL;
  }
  else {
    l->prev = labels_last;
    labels_last->next = l;
    labels_last = l;
  }

  return SUCCEEDED;
}


int obtain_rombankmap(void) {

  int map_found = OFF, i, x, banksize, a;
  struct object_file *o;
  unsigned char *t;

  /* initialize values */
  for (i = 0; i < rombanks; i++)
    banks[i] = 0;

  o = obj_first;
  while (o != NULL) {
    if (o->format == WLA_VERSION_OBJ) {
      t = o->data + OBJ_ROMBANKMAP;

      /* obtain status */
      i = *t;
      t++;

      /* general rombanksize? */
      if (i == 0) {
				/* obtain banksize */
				banksize = READ_T;

				o->memorymap = t;
				map_found = ON;
				for (i = 0; i < o->rom_banks; i++) {
					if (banks[i] == 0) {
						banks[i] = banksize;
						bankaddress[i] = i * banksize;
					}
					else if (banks[i] != banksize) {
						fprintf(stderr, "OBTAIN_ROMBANKMAP: ROMBANKMAPs don't match.\n");
						return FAILED;
					}
				}
      }
      else {
				for (a = 0, x = 0; x < o->rom_banks; x++) {
					banksize = READ_T;
					if (banks[x] == 0) {
						banks[x] = banksize;
						bankaddress[x] = a;
					}
					else if (banks[x] != banksize) {
						fprintf(stderr, "OBTAIN_ROMBANKMAP: ROMBANKMAPs don't match.\n");
						return FAILED;
					}
					a += banksize;
				}
	
				o->memorymap = t;
				map_found = ON;
      }
    }

    o = o->next;
  }

  if (map_found == OFF) {
    fprintf(stderr, "OBTAIN_ROMBANKMAP: No object files.\n");
    return FAILED;
  }

  return SUCCEEDED;
}


int obtain_source_file_names(void) {

  struct source_file_name *s = NULL, **p;
  struct object_file *o;
  unsigned char *t, *m;
  int x, z;

  o = obj_first;
  while (o != NULL) {
    if (o->format == WLA_VERSION_OBJ)
      t = o->source_file_names;
    else
      t = o->data + LIB_SOURCE_FILE_NAMES;

    x = READ_T;

    p = &(o->source_file_names_list);
    for (; x > 0; x--) {
      s = malloc(sizeof(struct source_file_name));
      if (s == NULL) {
				fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
				return FAILED;
      }

      /* compute the length of the name */
      for (m = t, z = 0; *m != 0; m++, z++);

      s->name = malloc(z+1);
      if (s->name == NULL) {
				free(s);
				fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
				return FAILED;
      }

      /* get the name */
      for (z = 0; *t != 0; t++, z++)
        s->name[z] = *t;
      s->name[z] = 0;

      t++;
      s->id = *(t++);
      s->next = NULL;
      *p = s;
      p = &(s->next);
    }

    o->exported_defines = t;
    o = o->next;
  }

  return SUCCEEDED;
}


int obtain_memorymap(void) {

  struct object_file *o;
  int map_found = OFF, i, x;
  unsigned char *t;
  struct slot s[256];

  for (i = 0; i < 256; i++)
    slots[i].usage = OFF;

  o = obj_first;
  while (o != NULL) {
    if (o->format == WLA_VERSION_OBJ) {
      t = o->memorymap;

      /* obtain slots */
      i = *t;
      t++;

      for (x = 0; i > 0; i--, x++) {
				slots[x].usage = ON;
				slots[x].address =  READ_T;
				slots[x].size =  READ_T;
      }

      o->source_file_names = t;
      map_found = ON;
      break;
    }

    o = o->next;
  }

  if (map_found == OFF) {
    fprintf(stderr, "OBTAIN_MEMORYMAP: No object files.\n");
    return FAILED;
  }

  /* check if the following memorymaps equal to the previous one */
  o = o->next;
  while (o != NULL) {
    if (o->format == WLA_VERSION_OBJ) {
      for (i = 0; i < 256; i++)
				s[i].usage = OFF;
      t = o->memorymap;

      /* obtain slots */
      i = *t;
      t++;

      for (x = 0; i > 0; i--, x++) {
				s[x].usage = ON;
				s[x].address =  READ_T;
				s[x].size =  READ_T;
      }

      o->source_file_names = t;

      for (x = 0, i = 0; i < 256; i++) {
				if (s[i].usage == ON) {
					if (slots[i].usage == OFF) {
						x = 1;
						break;
					}
					if (slots[i].address == s[i].address && slots[i].size == s[i].size)
						continue;
					x = 1;
					break;
				}
				else {
					if (slots[i].usage == ON) {
						x = 1;
						break;
					}
				}
      }

      if (x == 1) {
				fprintf(stderr, "OBTAIN_MEMORYMAP: The object files are compiled for different memory architectures.\n");
				return FAILED;
      }
    }

    o = o->next;
  }

  return SUCCEEDED;
}


int collect_dlr(void) {

  struct reference *r;
  struct stack *s;
  struct label *l;
  int section, x, i, n, q;
  unsigned char *t, *dtmp;
  double dou;

  section = 0;
  obj_tmp = obj_first;
  while (obj_tmp != NULL) {
    /* OBJECT FILE */
    if (obj_tmp->format == WLA_VERSION_OBJ) {
      t = obj_tmp->exported_defines;
      i = READ_T;

      /* load defines */
      for (; i > 0; i--) {
				l = malloc(sizeof(struct label));
				if (l == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}

				/* copy the name */
				for (x = 0; !(*t == 0 || *t == 1); t++, x++)
					l->name[x] = *t;
				l->name[x] = 0;
				if (*t == 0)
					l->status = LABEL_STATUS_DEFINE;
        else if (*t == 1)
					l->status = LABEL_STATUS_STACK;
				else {
					fprintf(stderr, "COLLECT_DLR: Unknown definition type \"%d\".\n", *t);
					return FAILED;
				}
				t++;

				READ_DOU;
				l->address = dou;
				l->base = 0;
				l->file_id = obj_tmp->id;
				l->section_status = OFF;

				add_label(l);
      }

      /* load labels */
      i = READ_T;

      for (; i > 0; i--) {
				l = malloc(sizeof(struct label));
				if (l == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}
				for (x = 0; !(*t == 0 || *t == 1 || *t == 2); t++, x++)
					l->name[x] = *t;
				l->name[x] = 0;

				if (*t == 0)
					l->status = LABEL_STATUS_LABEL;
        else if (*t == 1)
					l->status = LABEL_STATUS_SYMBOL;
        else if (*t == 2)
					l->status = LABEL_STATUS_BREAKPOINT;
				else {
					fprintf(stderr, "COLLECT_DLR: Unknown label type \"%d\".\n", *t);
					return FAILED;
				}

				t++;
				l->slot = *(t++);
				l->file_id_source = *(t++);
				x = READ_T;
				if (x == 0)
					l->section_status = OFF;
				else {
					l->section_status = ON;
					l->section = x + section;
				}
				l->address = READ_T;
				l->linenumber = READ_T;
				l->base = (((int)l->address) >> 16) & 0xFF;
				l->address = ((int)l->address) & 0xFFFF;
				l->bank = READ_T;
				l->file_id = obj_tmp->id;

				add_label(l);
      }

      i = READ_T;

      /* load references */
      for (; i > 0; i--) {
				r = malloc(sizeof(struct reference));
				if (r == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}
				for (x = 0; *t != 0; t++, x++)
					r->name[x] = *t;
				r->name[x] = 0;
				t++;
				r->type = *(t++);
				r->file_id_source = *(t++);
				r->slot = *(t++);
				x = READ_T;
				if (x == 0)
					r->section_status = OFF;
				else {
					r->section_status = ON;
					r->section = x + section;
				}
				r->linenumber = READ_T;
				r->address = READ_T;
				r->bank = READ_T;

				add_reference(r);
      }

      i = READ_T;

      /* load pending calculations */
      for (; i > 0; i--) {
				s = malloc(sizeof(struct stack));
				if (s == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}

				s->id = READ_T;
				s->type = *(t++);
				s->section = section + READ_T;
				s->file_id_source = *(t++);
				x = *(t++);
				s->position = *(t++);
				s->slot = *(t++);

				if (s->section == section)
					s->section_status = OFF;
				else
					s->section_status = ON;

				s->address = READ_T;
				s->linenumber = READ_T;
				s->bank = READ_T;
				s->stacksize = x;

				s->stack = malloc(sizeof(struct stackitem) * x);
				if (s->stack == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					free(s);
					return FAILED;
				}

				add_stack(s);

				for (n = 0; n != x; n++) {
					s->stack[n].type = *(t++);
					s->stack[n].sign = *(t++);
					if (s->stack[n].type == STACK_ITEM_TYPE_STRING) {
						for (q = 0; *t != 0; t++, q++)
							s->stack[n].string[q] = *t;
						s->stack[n].string[q] = 0;
						t++;
					}
					else {
						READ_DOU;
						s->stack[n].value = dou;
					}
				}
      }

      /* save pointer to data block area */
      obj_tmp->data_blocks = t;
    }
    /* LIBRARY FILE */
    else if (obj_tmp->format == WLA_VERSION_LIB) {
      t = obj_tmp->exported_defines;
      i = READ_T;

      /* load definitions */
      for (; i > 0; i--) {
				l = malloc(sizeof(struct label));
				if (l == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}

				/* copy the name */
				for (x = 0; !(*t == 0 || *t == 1); t++, x++)
					l->name[x] = *t;
				l->name[x] = 0;
				if (*t == 0)
					l->status = LABEL_STATUS_DEFINE;
        else if (*t == 1)
					l->status = LABEL_STATUS_STACK;
				else {
					fprintf(stderr, "COLLECT_DLR: Unknown definition type \"%d\".\n", *t);
					return FAILED;
				}
				t++;

				READ_DOU;
				l->address = dou;
				l->bank = obj_tmp->bank;
				l->slot = obj_tmp->slot;
				l->base = 0;
				l->file_id = obj_tmp->id;
				l->section_status = OFF;

				add_label(l);
      }

      i = READ_T;

      /* load labels and symbols */
      for (; i > 0; i--) {
				l = malloc(sizeof(struct label));
				if (l == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}
				for (x = 0; !(*t == 0 || *t == 1 || *t == 2); t++, x++)
					l->name[x] = *t;
				l->name[x] = 0;

				if (*t == 0)
					l->status = LABEL_STATUS_LABEL;
        else if (*t == 1)
					l->status = LABEL_STATUS_SYMBOL;
        else if (*t == 2)
					l->status = LABEL_STATUS_BREAKPOINT;
				else {
					fprintf(stderr, "COLLECT_DLR: Unknown label type \"%d\".\n", *t);
					return FAILED;
				}

				t++;
				x = READ_T;
				l->section = x + section;
				l->file_id_source = *(t++);
				l->linenumber = READ_T;
				l->section_status = ON;
				l->address = READ_T;
				l->base = (((int)l->address) >> 16) & 0xFF;
				l->address = ((int)l->address) & 0xFFFF;
				l->bank = obj_tmp->bank;
				l->slot = obj_tmp->slot;
				l->file_id = obj_tmp->id;

				add_label(l);
      }

      i = READ_T;

      /* load references */
      for (; i > 0; i--) {
				r = malloc(sizeof(struct reference));
				if (r == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}
				for (x = 0; *t != 0; t++, x++)
					r->name[x] = *t;
				r->name[x] = 0;
				t++;
				r->type = *(t++);
				x = READ_T;
				r->section = x + section;
				r->file_id_source = *(t++);
				r->linenumber = READ_T;
				r->section_status = ON;
				r->address = READ_T;

				r->bank = obj_tmp->bank;
				r->slot = obj_tmp->slot;

				add_reference(r);
      }

      i = READ_T;

      /* load pending calculations */
      for (; i > 0; i--) {
				s = malloc(sizeof(struct stack));
				if (s == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					return FAILED;
				}

				s->id = READ_T;
				s->type = *(t++);
				s->section_status = ON;
				s->section = section + READ_T;
				s->file_id_source = *(t++);
				x = *(t++);
				s->position = *(t++);
				s->address = READ_T;
				s->linenumber = READ_T;
				s->stacksize = x;
				s->bank = obj_tmp->bank;
				s->slot = obj_tmp->slot;

				s->stack = malloc(sizeof(struct stackitem) * x);
				if (s->stack == NULL) {
					fprintf(stderr, "COLLECT_DLR: Out of memory.\n");
					free(s);
					return FAILED;
				}

				add_stack(s);

				for (n = 0; n != x; n++) {
					s->stack[n].type = *(t++);
					s->stack[n].sign = *(t++);
					if (s->stack[n].type == STACK_ITEM_TYPE_STRING) {
						for (q = 0; *t != 0; t++, q++)
							s->stack[n].string[q] = *t;
						s->stack[n].string[q] = 0;
						t++;
					}
					else {
						READ_DOU;
            s->stack[n].value = dou;
					}
				}
      }

      /* save pointer to data block area */
      obj_tmp->data_blocks = t;
    }

    obj_tmp = obj_tmp->next;
    section += 0x1000;
  }

  return SUCCEEDED;
}


int parse_data_blocks(void) {

  struct section *s;
  int section, i, y, x;
  unsigned char *t, *p;

  obj_tmp = obj_first;
  section = 0;
  while (obj_tmp != NULL) {

    /* OBJECT FILE */
    if (obj_tmp->format == WLA_VERSION_OBJ) {
      t = obj_tmp->data_blocks;
      p = obj_tmp->data + obj_tmp->size;
      for (y = 1; t < p; ) {
				x = *(t++);

				if (x == DATA_TYPE_BLOCK) {
					/* address */
					i = READ_T;
					/* amount of bytes */
					x = READ_T;

					for (; x > 0; x--, i++)
						if (mem_insert(i, *(t++)) == FAILED)
							return FAILED;
				}
				else if (x == DATA_TYPE_SECTION) {
					s = malloc(sizeof(struct section));
					if (s == NULL) {
						fprintf(stderr, "PARSE_DATA_BLOCKS: Out of memory.\n");
						return FAILED;
					}
					y++;

					/* name */
					i = 0;
					while (*t != 0 && *t != 1 && *t != 2 && *t != 3 && *t != 4 && *t != 5 && *t != 6 && *t != 7)
						s->name[i++] = *(t++);
					s->name[i] = 0;
					s->status = *(t++);
					s->id = section + READ_T;
					s->slot = *(t++);
					s->file_id_source = *(t++);
					s->address = READ_T;
					s->bank = READ_T;
					s->size = READ_T;
          s->alignment = READ_T;
					s->data = t;
					s->library_status = OFF;
					t += s->size;

					/* listfile block */
					if (listfile_block_read(&t, s) == FAILED)
						return FAILED;

					if (add_section(s) == FAILED)
						return FAILED;
				}
      }
      obj_tmp = obj_tmp->next;
      section += 0x1000;
      continue;
    }
    /* LIBRARY FILE */
    else if (obj_tmp->format == WLA_VERSION_LIB) {
      t = obj_tmp->data_blocks;
      p = obj_tmp->data + obj_tmp->size;
      for (y = 1; t < p; ) {

				s = malloc(sizeof(struct section));
				if (s == NULL) {
					fprintf(stderr, "PARSE_DATA_BLOCKS: Out of memory.\n");
					return FAILED;
				}
				y++;

				/* name */
				i = 0;
				while (*t != 0 && *t != 7)
					s->name[i++] = *(t++);
				s->name[i] = 0;
				s->status = *(t++);
				s->id = section + READ_T;
				s->file_id_source = *(t++);
				s->size = READ_T;
        s->alignment = READ_T;
				s->data = t;
				s->address = 0;
				s->bank = obj_tmp->bank;
				s->slot = obj_tmp->slot;
				s->base = obj_tmp->base;
				s->library_status = ON;
				s->base_defined = obj_tmp->base_defined;
				t += s->size;

				/* listfile block */
				if (listfile_block_read(&t, s) == FAILED)
					return FAILED;

				add_section(s);
      }
      obj_tmp = obj_tmp->next;
      section += 0x1000;
      continue;
    }
  }

  return SUCCEEDED;
}


int obtain_rombanks(void) {

  unsigned char *t;
  int rb = 0, k, s;

  /* obtain the biggest rom size */
  s = 0;
  obj_tmp = obj_first;
  while (obj_tmp != NULL) {
    if (obj_tmp->format == WLA_VERSION_OBJ) {

      t = obj_tmp->data + OBJ_ROMBANKS;
      k = t[3] + (t[2] << 8) + (t[1] << 16) + (t[0] << 24);

      obj_tmp->rom_banks = k;

      if (k != rb)
				s++;
      if (k > rb)
				rb = k;
    }
    obj_tmp = obj_tmp->next;
  }

  /* emptyfill has been obtained in the header checks */
  rombanks = rb;

  if (s > 1)
    fprintf(stderr, "OBTAIN_ROMBANKS: Using the biggest selected amount of ROM banks (%d).\n", rombanks);

  return SUCCEEDED;
}


int clean_up_dlr(void) {

  struct reference *r, *re;
  struct stack *s, *st;
  struct label *l, *la;
  struct section *se, *sec, *sect;

  se = sec_first;
  while (se != NULL) {
    /* remove duplicates of unique sections and all the related data */
    if (strlen(se->name) >= 3 && se->name[0] == '!' && se->name[1] == '_' && se->name[2] == '_') {
      sec = se->next;
      while (sec != NULL) {
				if (strcmp(se->name, sec->name) == 0) {
					sect = sec;

					sect->prev->next = sect->next;
					if (sect->next != NULL)
						sect->next->prev = sect->prev;
					else
						sec_last = sect;

					/* free references */
					r = reference_first;
					while (r != NULL) {
						if (r->section_status == ON && r->section == sect->id) {
							re = r;
							if (re->prev == NULL)
								reference_first = re->next;
							else
								re->prev->next = re->next;
							if (re->next == NULL)
								reference_last = re->prev;
							else
								re->next->prev = re->prev;

							r = r->next;
							free(re);
						}
						else
							r = r->next;
					}

					/* free stacks */
					s = stacks_first;
					while (s != NULL) {
						if (s->section_status == ON && s->section == sect->id) {
							st = s;
							if (st->stack != NULL) {
								free(st->stack);
								st->stack = NULL;
							}
							if (st->prev == NULL)
								stacks_first = st->next;
							else
								st->prev->next = st->next;
							if (st->next == NULL)
								stacks_last = st->prev;
							else
								st->next->prev = st->prev;

							s = s->next;
							free(st);
						}
						else
							s = s->next;
					}

					/* free labels */
					l = labels_first;
					while (l != NULL) {
						if (l->section_status == ON && l->section == sect->id) {
							la = l;
							if (la->prev == NULL)
								labels_first = la->next;
							else
								la->prev->next = la->next;
							if (la->next == NULL)
								labels_last = la->prev;
							else
								la->next->prev = la->prev;

							l = l->next;
							free(la);
						}
						else
							l = l->next;
					}

					sec = sec->next;
					free(sect);
				}
				else
					sec = sec->next;
      }
    }
    se = se->next;
  }

  return SUCCEEDED;
}
