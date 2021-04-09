
#ifndef _STACK_H
#define _STACK_H

int stack_calculate(char *in, int *value);
int stack_create_label_stack(char *label);
int resolve_stack(struct stack_item s[], int x);
int compute_stack(struct stack *sta, int x, double *result);

#endif

