#ifndef SYMBOL
#define SYMBOL

typedef struct symbol_list_t {
	struct symbol_list_t *next;
	char *value;
} symbol_list;

typedef struct symbol_table_t {
	struct symbol_table_t *parent;
	struct symbol_table_t *child;
	symbol_list *s;
} symbol_table;

void new_symbol(symbol_table*, char*);
symbol_table* new_symbol_table(symbol_table*);

#endif 
