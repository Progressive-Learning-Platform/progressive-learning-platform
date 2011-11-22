#ifndef DEFINES
#define DEFINES

#include <stdio.h>
#include <stdlib.h>

typedef struct define_t {
	char *macro;
	char *expansion;
	struct define_t *next;
} define;

define* install_define(define *d, char* m, char *e);
char* find_define(define *d, char* m);
void print_defines(FILE *f, define *d);

#endif
