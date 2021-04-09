
#ifndef _PASS_4_H
#define _PASS_4_H

int pass_4(void);
int new_unknown_reference(int type);
int mem_insert(unsigned char x);
int mem_insert_pad(void);
int mem_insert_absolute(unsigned int add, unsigned char x);
int export_definitions(FILE *final_ptr);
int export_source_file_names(FILE *final_ptr);

#ifdef W65816
int get_snes_cpu_bank(struct label_def *l);
#endif

#endif
