#include "symbol.h"
#include <stdlib.h>
#include "log.h"

extern symbol_table *sym;

/* lookup a symbol in the current scope */
int lookup(char *s) {
	symbol_list *curr = sym->s;
	while (curr != NULL) {
		if (curr->value == s)
			return 1;
		curr = curr->next;
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
	symbol_list *symbol = malloc(sizeof(symbol_list));
	symbol->next = t->s;
	symbol->value = v;
	t->s = symbol;
}

symbol_table* new_symbol_table(symbol_table *t) {
	symbol_table *table = malloc(sizeof(symbol_table));
	table->child = NULL;
	table->parent = t;
	table->s = NULL;
	if (t != NULL)
		t->child = table;
	return table;
}

