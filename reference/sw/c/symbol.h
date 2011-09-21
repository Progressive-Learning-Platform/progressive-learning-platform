#ifndef SYMBOL
#define SYMBOL

#define TYPE_FUNCTION 0

typedef struct symbol_list_t {
	struct symbol_list_t *next;
	int type;
	void *value;
} symbol_list;

typedef struct symbol_table_t {
	struct symbol_table_t *parent;
	struct symbol_table_t *child;
	symbol_list *s;
} symbol_table;

void new_symbol(symbol_table*,int,void*);
symbol_table* new_symbol_table(symbol_table*);

#endif 
