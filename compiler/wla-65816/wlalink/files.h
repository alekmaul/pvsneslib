
#ifndef _FILES_H
#define _FILES_H

int load_files(char *argv[], int argc, int have_flags);
int load_file(char *fn, int state, int bank, int slot, int base, int base_defined);
int load_file_data(char *fn, unsigned char **da, int *size);
char *get_file_name(int id);
char *get_source_file_name(int file_id, int source_id);

#endif
