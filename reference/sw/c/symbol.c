#include "symbol.h"
#include <stdlib.h>
#include "log.h"

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
	
void insert(char *s) {
	int found = lookup(s);
	if (!found) {
		new_symbol(sym, s);
	} else {
		err("[parser] token %s already defined in this scope\n", s);
	}
}

void new_symbol(symbol_table *t, char *v) {
	/* insert our new symbol to the top of the list */
	symbol *symbol = malloc(sizeof(symbol));
	symbol->up = t->s;
	symbol->value = v;
	t->s = symbol;
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

