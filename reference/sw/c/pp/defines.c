#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"

define* install_define(define *d, char* m, char *e) {
	if (e == NULL) {
		e = malloc(1);
		*e = '\0';
	}

	/* base case, if d is null, it's a new list */
	if (d == NULL) {
		d = malloc(sizeof(define));
		d->next = NULL;
		d->macro = strdup(m);
		d->expansion = strdup(e);
	} else {
		/* first make sure that it's not already in the list */
		define *curr = d;
		while (curr != NULL) {
			if (strcmp(curr->macro, m) == 0) { /* it's a redefine */
				free(curr->expansion);
				curr->expansion = strdup(e);
				return d;
			}
			curr = curr->next;
		}

		curr = d;
		while (curr->next != NULL) curr = curr->next;
		curr->next = malloc(sizeof(define));
		curr = curr->next;
		curr->next = NULL;
		curr->macro = strdup(m);
		curr->expansion = strdup(e);
	}
	return d;
}

char* find_define(define *d, char* m) {
	define *curr = d;
	while (curr != NULL) {
		if (strcmp(curr->macro, m) == 0) {
			/* we have to recursively check for nested defines */
			if (find_define(d, curr->expansion) != NULL)
				return find_define(d, curr->expansion);
			return curr->expansion;
		}
		curr = curr->next;
	}
	return NULL;
}

void print_defines(FILE *f, define *d) {
	define *curr = d;
	while (curr != NULL) {
		fprintf(f, "%s : %s\n", curr->macro, curr->expansion);
		curr = curr->next;
	}
}
