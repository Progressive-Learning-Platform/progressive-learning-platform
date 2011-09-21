#ifndef STACK
#define STACK
#include <stdlib.h>

typedef struct stack_t {
	struct stack_t* n;
	char *val;
} stack;

void push(char *s);
char* pop(void);
#endif 
