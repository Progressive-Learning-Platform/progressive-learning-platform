#ifndef SYMBOL
#define SYMBOL

/* symbol type */
typedef enum {
        type_label,
        type_struct,
        type_enum_constant,
        type_enum,
	type_constant,
	type_function,
	type_global,
	type_temporary
} sym_type;

typedef enum {
	xtype_none,
	xtype_auto,
	xtype_register,
	xtype_static,
	xtype_extern,
	xtype_typedef,
	xtype_enum
} sym_xtype;

typedef struct symbol_t {
	sym_type 	type;
	sym_xtype 	xtype;
	char *		value;
	struct symbol_t *	up;
} symbol;
	
typedef struct symbol_table_t {
	struct symbol_table_t *parent;
	int num_children;
	symbol *s;
	struct symbol_table_t *children[];
} symbol_table;

void new_symbol(symbol_table*, char*);
symbol *find_symbol(symbol_table*, char*);
symbol_table* new_symbol_table(symbol_table*);

#endif 
