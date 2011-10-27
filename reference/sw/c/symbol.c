#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "symbol.h"
#include "log.h"
#include "parse_tree.h"
#include "line.h"

extern symbol_table *sym;

typedef struct id_chain_t {
	struct id_chain_t *up;
	char *id;
	int size;
	int pointer;
} id_chain;

/* get the size for this direct declarator. This can be a really tall order, 
as the size on direct declarators can be many things. For example:

int a;
int a[5];
int a[5][10][200][1000][1];
int a[5+CONSTANT];
*/

int get_ids_size(node *n) {
	/* for now we only support non-array types and array types of 1 dimension with no 
	 * computed size (ie constant only) */
	if (n->num_children == 1)
		return 1;
	else if (n->num_children == 2) {
		if (strcmp(n->children[1]->id, "constant_expression") == 0) {
			if (n->children[1]->children[0]->type == type_con) {
				char *endp;
				return strtol(n->children[1]->children[0]->id,&endp,0);
			} else {
				err("[symbol] non-literal constant array declarations not currently supported\n");
			}
		} else {
			return 1; /* this is for function declarations, array declarations without size specifiers */
		}
	} else {
		err("[symbol] multi-dimensional arrays not currently supported\n");
	}
}

id_chain* get_ids(id_chain* i, node *n) {
	/* is the current node an id? */
	if (n != NULL) {
		if (strcmp(n->id, "declarator") == 0) {
			/* declarators either have children[0] as the direct declarator or a pointer note */
			id_chain *t = malloc(sizeof(id_chain));
			if (t == NULL) {
				err("[symbol] cannot allocate id_chain\n");
			}
			t->up = i;
			if (strcmp(n->children[0]->id, "pointer") == 0) {
				t->pointer = 1;
				t->id = n->children[1]->children[0]->id;
				t->size = get_ids_size(n->children[1]->children[0]);
			} else {
				t->pointer = 0;
				t->id = n->children[0]->children[0]->id;
				t->size = get_ids_size(n->children[0]->children[0]);
			}
			i = t;
			lvlog(n->line, "[symbol] found id %s\n", i->id);
		} else {
			int j;
			/* call this on all children */
			for (j=0; j<n->num_children; j++)
				i = get_ids(i, n->children[j]);
		}
	}
	return i;
}

/* lookup a symbol in the current scope */
int lookup(symbol_table* t, char *s) {
	symbol *curr = t->s;
	while (curr != NULL) {
		if (strcmp(curr->value, s) == 0)
			return 1;
		curr = curr->up;
	}
	return 0;	
}

int match_attr(char* t) {
	return  
	strcmp(t, "struct_union")	== 0	? ATTR_STRUCT	: 
	strcmp(t, "enum") 		== 0	? ATTR_ENUM	:
	strcmp(t, "auto") 		== 0	? ATTR_AUTO	:
	strcmp(t, "register") 		== 0	? ATTR_REGISTER	:
	strcmp(t, "static") 		== 0	? ATTR_STATIC	:
	strcmp(t, "extern") 		== 0	? ATTR_EXTERN	:
	strcmp(t, "typedef") 		== 0	? ATTR_TYPEDEF	:
	strcmp(t, "unsigned") 		== 0	? ATTR_UNSIGNED	:
	strcmp(t, "signed") 		== 0	? ATTR_SIGNED	:
	strcmp(t, "volatile") 		== 0	? ATTR_VOLATILE	: 0;
}

char* match_type(char *t) {
	/* just call match_attr, if it returns 0, it's a type */
	if (match_attr(t) == 0) 
		return t;
	return NULL;
}
	
node* install_symbol(symbol_table *t, node *n) {
	symbol *s = malloc(sizeof(symbol));
	symbol *end;
	if (s == NULL) {
		err("[symbol] could not allocate symbol\n");
	}
	end = t->s;

	/* we want the end of the list to keep things in order in the backend */
	if (end != NULL)
		while (end->up != NULL)
			end = end->up;

	s->up = NULL;
	s->attr = 0;
	s->type = NULL;
	s->value = NULL;

	/* get the id and all attributes for this symbol */
	
	/* the first child node should be the types and attributes */
	if (strcmp(n->children[0]->id,"declaration_specifier") != 0) {
		lerr(n->line, "[symbol] cannot extract type/attr information\n");
	} else {
		/* all children of declaration_specifiers should be type:id */
		node *types = n->children[0];
		node *decs  = n->num_children > 1 ? n->children[1] : NULL;
		int i;
		for (i=0; i<types->num_children; i++) {
			if (types->children[i]->type != type_type) {
				lerr(n->line, "[symbol] found non-type in type specifiers\n");
			} else {
				char *type = match_type(types->children[i]->id);
				if (type == NULL)
					s->attr |= match_attr(types->children[i]->id);
				else
					s->type = type;
			}
		}
		/* now we get the id of the symbol */
		/* the next child should be a init_declarator_list, with one or more direct_declarators buried inside, among other things */
		/* just do a search for "direct_declarator", and return each one's children[0]->id field, which is an identifier */
		id_chain *ids = get_ids(NULL, decs);
		if (ids == NULL && strcmp(n->id, "parameter_declaration") != 0) 
			lerr(n->line, "[symbol] no declarators found\n");
		while (ids != NULL) {
			id_chain *d = ids;
			if (lookup(t, ids->id)) {
				lerr(n->line, "[symbol] symbol %s already declared in this scope\n", ids->id);
			}
			if (strcmp(n->id, "parameter_declaration") == 0)
				s->attr |= ATTR_PARAM;
			if (ids->pointer)
				s->attr |= ATTR_POINTER; 
			s->value = ids->id;
			if (end == NULL) /* first node */
				t->s = s;
			else
				end->up = s;
			vlog("[symbol] created symbol: %s, type %s, attr 0x%08x\n", s->value, s->type, s->attr);
			if (ids->up != NULL) {
				end = s;
				s = malloc(sizeof(symbol));
				if (s == NULL) {
					err("[symbol] cannot allocate symbol\n");
				}
				s->up = NULL;
				s->attr = t->s->attr;
				s->type = t->s->type;
				s->value = NULL;
			}
			ids = ids->up;
			free(d);
		}
	}	
		
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
	if (table == NULL) {
		err("[symbol] could not allocate new table\n");
	}

	vlog("[symbol] creating new table\n");

	table->s = NULL;
	table->num_children = 0;
	table->assoc = NULL;
	table->children = NULL;

	if (t == NULL) {
		/* brand new global table */
		table->parent = NULL;
	} else {
		/* create a new child table */
		t->children = realloc(t->children, (sizeof(symbol_table*) * (t->num_children+1)));
		if (t->children == NULL) {
			err("[symbol] cannot realloc children\n");
		}
		t->num_children++;
		t->children[t->num_children-1] = table;
		table->parent = t;
	} 
	return table;
}

void print_symbols(symbol_table* t, FILE* o, int depth) {
	/* print all of my symbols and then call my children to do the same */
	symbol *curr = NULL;
	int i,j;
	if (t == NULL) 
		return;
	curr = t->s;
	while (curr != NULL) {
		for (i=0; i<depth; i++)
			fprintf(o, "\t");
		fprintf(o, "id: %s, type: %s, attributes: 0x%08x\n", curr->value, curr->type, curr->attr);
		/* print any symbol tables that are subordinate to this symbol */
		for(j=0; j<t->num_children; j++)
			if (t->children[j]->assoc == curr)
				print_symbols(t->children[j], o, depth+1);
		curr = curr->up;
	}
	for (i=0; i<t->num_children; i++)
		if (t->children[i]->assoc == NULL)
			print_symbols(t->children[i], o, depth+1);
}

node* install_function(symbol_table *t, node *n) {
	/* function definition nodes are one of four types:
		1: specifiers, declarator, declaration list, compound statement
		2: specifiers, declarator, compound statement
		3: declarator, declaration list, compound statement
		4: declarator, compound statement
	*/
	vlog("[symbol] installing function handler\n"); 

	if (strcmp(n->id, "function_definition") != 0) {
		err("[symbol] did not find function definition\n");
	}
	if (strcmp(n->children[0]->id, "declaration_specifier") == 0) {
		node *temp_node = op("declaration", 2, n->children[0], op("init_declarator_list", 1, n->children[1]));
		install_symbol(t, temp_node); /* install the function */
		/* the last created symbol table should be the one associated with this function. */
		t->children[t->num_children-1]->assoc = get_last_symbol(t);

		if (strcmp(n->children[2]->id, "declaration_list") == 0) {
			/* type 1 */
		}
		install_parameters(t->children[t->num_children-1], n->children[1]);
	} else {
		node *temp_node = op("declaration", 2, op("declaration_specifier", 1, type("void")), op("init_declarator_list", 1, n->children[0]));
		install_symbol(t, temp_node); /* install the function */
		/* the last created symbol table should be the one associated with this function. */
		t->children[t->num_children-1]->assoc = get_last_symbol(t);
		if (strcmp(n->children[1]->id, "declaration_list") == 0) {
			/* type 3 */
		}
		install_parameters(t->children[t->num_children-1], n->children[0]);
	}

	t->s->attr |= ATTR_FUNCTION;


	return n;
}

void install_parameters(symbol_table *t, node *n) {
	int i;
	if (n->children[0]->num_children == 1)
		return;
	n = n->children[0]->children[1]->children[0]; /* the parameter list */
	/* each of n's children is a parameter declaration */
	for (i=0; i<n->num_children; i++)
		install_symbol(t, n->children[i]);
}

symbol *get_last_symbol(symbol_table *t) {
	symbol *ret = NULL;
	symbol *curr = t->s;
	while (curr != NULL) {
		ret = curr;
		curr = curr->up;
	}
	return ret;
}
