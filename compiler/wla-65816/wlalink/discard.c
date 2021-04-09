
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "discard.h"
#include "files.h"
#include "write.h"


extern struct reference *reference_first, *reference_last;
extern struct section *sec_first, *sec_last;
extern struct label *labels_first, *labels_last;
extern struct stack *stacks_first, *stacks_last;


int discard_unused_sections(void) {

  struct section *s;
  int a = 0, b = -1;

  /* iterate section discarding until there's no change in the
     amount of dropped sections */
  while (a != b) {
    s = sec_first;
    while (s != NULL) {
      s->referenced = 0;
      s = s->next;
    }

    a = b;
    discard_iteration();

    b = 0;
    s = sec_first;
    while (s != NULL) {
      if (s->referenced == 0)
				s->alive = NO;
      else
				s->alive = YES;
      if (s->alive == NO)
				b++;
      s = s->next;
    }
  }

  /* announce all the unreferenced sections that will get dropped */
  s = sec_first;
  while (s != NULL) {
    if (s->alive == NO)
      fprintf(stderr, "DISCARD: %s:%s: Section \"%s\" was discarded.\n",
							get_file_name(s->file_id), get_source_file_name(s->file_id, s->file_id_source), s->name);
    s = s->next;
  }

  return SUCCEEDED;
}


int discard_iteration(void) {

  struct reference *r;
  struct stackitem *si;
  struct section *s, *se;
  struct label *l;
  struct stack *st;
  int g;

  /* check section names for special characters '!', and
     check if the section is of proper type */
  s = sec_first;
  while (s != NULL) {
    if (strstr(s->name, "data") || s->name[0] == '!' || !(s->status == SECTION_STATUS_FREE || s->status == SECTION_STATUS_SEMIFREE || s->status == SECTION_STATUS_SUPERFREE)) {
      s->referenced++;
      s->alive = YES;
    }
    s = s->next;
  }

  /* loop through references */
  r = reference_first;
  while (r != NULL) {
    /* references to local labels don't count */
    if (is_label_section_local(r->name) == SUCCEEDED || is_label_anonymous(r->name) == SUCCEEDED) {
      r = r->next;
      continue;
    }
    l = labels_first;
    while (l != NULL) {
      if (strcmp(l->name, r->name) == 0)
				break;
      l = l->next;
    }
    if (l != NULL && l->section_status == ON) {
      s = sec_first;
      while (s->id != l->section)
				s = s->next;
      if (s == NULL)
				fprintf(stderr, "DISCARD_ITERATION: Internal error!\n");
      if (r->section_status == OFF)
				s->referenced++;
      else if (r->section != s->id) {
				se = sec_first;
				while (se->id != r->section)
					se = se->next;
				if (se->alive == YES)
					s->referenced++;
      }
    }
    r = r->next;
  }

  /* loop through computations */
  st = stacks_first;
  while (st != NULL) {
    si = st->stack;
    g = 0;
    while (g != st->stacksize) {
      if (si->type == STACK_ITEM_TYPE_STRING && is_label_anonymous(si->string) == FAILED) {
				l = labels_first;
				while (l != NULL) {
					if (strcmp(l->name, si->string) == 0 && l->section_status == ON) {
						s = sec_first;
						while (s->id != l->section)
							s = s->next;
						if (st->section_status == OFF)
							s->referenced++;
						else if (st->section != s->id) {
							se = sec_first;
							while (se->id != st->section)
								se = se->next;
							if (se->alive == YES)
								s->referenced++;
						}
					}
					l = l->next;
				}
      }
      si++;
      g++;
    }
    st = st->next;
  }

  return SUCCEEDED;
}


/* drop labels that are inside discarded sections */
int discard_drop_labels(void) {

  struct label *l, *t;
  struct section *s;

  l = labels_first;
  while (l != NULL) {
    if (l->section_status == ON) {
      /* find the section */
      s = sec_first;
      while (s->id != l->section)
				s = s->next;
      /* is it dead? */
      if (s->alive == NO) {
				/* nope! remove the label! */
				if (l->prev == NULL)
					labels_first = l->next;
				else
					l->prev->next = l->next;
				if (l->next != NULL)
					l->next->prev = l->prev;
				t = l->next;
				free(l);
				l = t;
      }
      else
				l = l->next;
    }
    else
      l = l->next;
  }

  return SUCCEEDED;
}
