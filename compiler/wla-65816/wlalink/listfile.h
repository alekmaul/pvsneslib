
#ifndef __LISTFILE_H
#define __LISTFILE_H

struct listfileitem {
  char *sourcefilename;
  int linenumber;
  int address;
  int length;
};

int listfile_write_listfiles(struct section *e);
int listfile_block_read(unsigned char **d, struct section *s);

#endif
