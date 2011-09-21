#ifndef STACK
#define STACK
#include <stdlib.h>
#include <stdio.h>

typedef struct stack_t {
	struct stack_t* n;
	char *val;
} stack;

void push(char *s);
char* pop(void);
void print_stack(FILE *f);

#endif 
