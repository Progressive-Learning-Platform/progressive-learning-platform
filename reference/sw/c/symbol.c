#include "symbol.h"
#include <stdlib.h>

void new_symbol(symbol_table *t, int type, void *v) {
	/* insert our new symbol to the top of the list */
	symbol_list *symbol = malloc(sizeof(symbol_list));
	symbol->next = t->s;
	symbol->type = type;
	symbol->value = v;
	t->s = symbol;
}

symbol_table* new_symbol_table(symbol_table *t) {
	symbol_table *table = malloc(sizeof(symbol_table));
	table->child = NULL;
	table->parent = t;
	table->s = NULL;
	t->child = table;
	return table;
}

