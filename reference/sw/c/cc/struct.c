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
#include "log.h"
#include "types.h"
#include "symbol.h"
#include "struct.h"

symbol_table *struct_temp = NULL;
extern struct_table *structs;

/* install a symbol in (yet unnamed) struct. This just hijacks the install_symbol function
   pointing to our dummy struct table.
 */ 
node* install_struct_symbol(node* n) {
	vlog("[struct] installing symbol(s)\n");
	if (struct_temp == NULL)
		struct_temp = new_symbol_table(NULL);
	
	return install_symbol(struct_temp, n);
}

void install_struct(node* n) {
	struct_table *t = malloc(sizeof(struct_table));
	if (t == NULL)
		err("[STRUCT] could not create struct table\n");
	t->next = structs;
	t->name = n->id;
	t->s	= struct_temp;
	structs = t;

	/* clean up struct_temp for the next struct we may create */
	struct_temp = NULL;
}

/* try to find a symbol in a given struct */
symbol* find_struct_symbol(struct_table* t, char* n) {
	return find_symbol(t->s, n);
}

struct_table* find_struct(char* n) {
	struct_table *curr = structs;
	while (curr != NULL) {
		if (strcmp(curr->name,n) == 0)
			return curr;
		curr = curr->next;
	}
	return NULL;
}

void print_structs(FILE* f, int d) {
	struct_table *curr = structs;
	while (curr != NULL) {
		fprintf(f, "struct: %s\n", curr->name);
		print_symbols(curr->s, f, 1);
	}
}
