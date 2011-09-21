#include "stack.h"
#include "log.h"
#include "stdlib.h"

extern stack *parse_stack;

void push(char *s) {
	vlog("[stack] push: %s\n", s);
	stack *v = malloc(sizeof(stack));
	v->n = parse_stack;
	v->val = malloc(strlen(s)*sizeof(char));
	strcpy(v->val, s);
	parse_stack = v;
}

char* pop(void) {
	if (parse_stack != NULL) {
		char *ret = parse_stack->val;
		stack *del = parse_stack;
		parse_stack = parse_stack->n;
		free(del);
		return ret;
	} else {
		err("attempt to pop on empty stack");
		return NULL;
	}
}
