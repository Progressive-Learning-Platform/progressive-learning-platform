#include "stack.h"
#include "log.h"
#include "stdlib.h"
#include "string.h"

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

/* i'm not winning any awards for this function... */
void print_stack(FILE *f) {
	/* create a reverse stack for printing */
	stack *v = NULL;
	stack *curr = parse_stack;

	while (curr != NULL) {
		stack *t = malloc(sizeof(stack));
		t->n = v;
		t->val = curr->val;
		v = t;
		curr = curr->n;
	}

	/* now just traverse and print, and simultaneously clean up our reverse stack */
	while (v != NULL) {
		stack *d = v;
		fprintf(f, "%s\n", v->val);
		v = v->n;
		free(d);
	}
}
