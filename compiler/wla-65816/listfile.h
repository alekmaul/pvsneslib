
#ifndef _LISTFILE_H
#define _LISTFILE_H

int listfile_collect(void);
int listfile_block_write(FILE *f, struct section_def *s);

#endif
