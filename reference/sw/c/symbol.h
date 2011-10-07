#ifndef SYMBOL
#define SYMBOL

#include "parse_tree.h"

#define ATTR_STRUCT 	0x00000001
#define ATTR_ENUM	0x00000002
#define ATTR_CONSTANT	0x00000004
#define ATTR_AUTO	0x00000008
#define ATTR_REGISTER	0x00000010
#define ATTR_STATIC	0x00000020
#define ATTR_EXTERN	0x00000040
#define ATTR_TYPEDEF	0x00000080
#define ATTR_UNSIGNED	0x00000100
#define ATTR_SIGNED	0x00000200
#define ATTR_VOLATILE	0x00000400

#define ATTR_LABEL 	0x00000800
#define ATTR_FUNCTION	0x00001000
#define ATTR_GLOBAL	0x00002000
#define ATTR_TEMPORARY	0x00004000

#define ATTR_POINTER	0x00008000

typedef struct symbol_t {
	unsigned int		attr;
	char *			type;
	char *			value;
	struct symbol_t *	up;
} symbol;
	
typedef struct symbol_table_t {
	struct symbol_table_t *parent;
	int num_children;
	symbol *s;
	struct symbol_table_t *children[];
} symbol_table;

node* new_symbol(symbol_table*, node*);
symbol *find_symbol(symbol_table*, char*);
symbol_table* new_symbol_table(symbol_table*);


#endif 
