#include <stdio.h>
#include <string.h>
#include "symbol.h"
#include <stdlib.h>
#include "log.h"
#include "parse_tree.h"

extern symbol_table *sym;

/* lookup a symbol in the current scope */
int lookup(char *s) {
	symbol *curr = sym->s;
	while (curr != NULL) {
		if (curr->value == s)
			return 1;
		curr = curr->up;
	}
	return 0;	
}
	
node* new_symbol(symbol_table *t, node *n) {
	symbol *s = malloc(sizeof(symbol));

	s->up = t->s;
	s->type = 0;
	s->value = NULL;

	/* get the id and all attributes for this symbol */
	print_tree(n, stdout, 0); 
	
		


	/* create the symbol */
	return n;
}

symbol* find_symbol(symbol_table *t, char *v) {
	symbol *curr;
	if (t == NULL)
		return NULL;
	curr = t->s;
	/* search up from here, looking for the requested symbol */
	while (curr != NULL) {
		if (strcmp(curr->value,v) == 0)
			return curr;
		curr = curr->up;
	}
	return find_symbol(t->parent, v);
}
	

symbol_table* new_symbol_table(symbol_table *t) {
	symbol_table *table = malloc(sizeof(symbol_table));
	table->s = NULL;
	table->num_children = 0;
	if (t == NULL) {
		/* brand new global table */
		table->parent = NULL;
	} else {
		/* create a new child table */
		int i;
		symbol_table *n  = malloc(sizeof(symbol_table) + (sizeof(symbol_table*) * (t->num_children+1)));
		for (i=0; i < t->num_children; i++)
			n->children[i] = t->children[i];
		n->children[i] = table;
		n->parent = t->parent;
		n->num_children = t->num_children + 1;
		n->s = t->s;
		free(t->children);
		free(t);
		table->parent = n;
	} 
	return table;
}

