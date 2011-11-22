/*
    Copyright 2011 the PLP authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "log.h"

define* install_define(define *d, char* m, char *e) {
	if (e == NULL) {
		e = malloc(2);
		sprintf(e, " ");
	}

	vlog("[pp defines] installing define %s : %s\n", m, e);

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
