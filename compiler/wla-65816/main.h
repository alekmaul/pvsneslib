
#ifndef _MAIN_H
#define _MAIN_H

void procedures_at_exit(void);
int parse_flags(char *f);
int parse_defines_and_get_final_name(char **c, int n);
int parse_and_add_definition(char *c);
int generate_tmp_names(void);
int generate_extra_definitions(void);

#endif
