#include "handlers.h"
#include "stack.h"
#include "symbol.h"

extern symbol_table *sym;

/* basic identifier, just put it on the stack */
void handle_identifier(char *s) {
	push(s);
}

void handle_function_declarator(char *s) {
	/* create a symbol table entry */
	new_symbol(sym, TYPE_FUNCTION, s);
};
