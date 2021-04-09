
#ifndef _PASS_1_H
#define _PASS_1_H

int pass_1(void);
int evaluate_token(void);
int add_a_new_definition(char *name, double value, char *string, int type, int size);
int export_a_definition(char *n);
int redefine(char *name, double value, char *string, int type, int size);
int undefine(char *name);
int parse_directive(void);
int find_next_point(char *n);
int get_new_definition_data(int *b, char *c, int *size, double *data);

void print_error(char *error, int i);
void next_line(void);
void delete_stack(struct stack *s);

#endif
