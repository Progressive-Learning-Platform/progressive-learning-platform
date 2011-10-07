#include <stdio.h>
#include <string.h>
#include "symbol.h"
#include <stdlib.h>
#include "log.h"
#include "parse_tree.h"

extern symbol_table *sym;

typedef struct id_chain_t {
	struct id_chain_t *up;
	char *id;
	int pointer;
} id_chain;

id_chain* get_ids(id_chain* i, node *n) {
	/* is the current node an id? */
	if (n != NULL) {
		if (strcmp(n->id, "declarator") == 0) {
			/* declarators either have children[0] as the direct declarator or a pointer note */
			id_chain *t = malloc(sizeof(id_chain));
			t->up = i;
			if (strcmp(n->children[0]->id, "pointer") == 0) {
				t->pointer = 1;
				t->id = n->children[1]->children[0]->id;
			} else {
				t->pointer = 0;
				t->id = n->children[0]->children[0]->id;
			}
			i = t;
			vlog("[symbol] found id %s\n", i->id);
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
int lookup(char *s) {
	symbol *curr = sym->s;
	while (curr != NULL) {
		if (curr->value == s)
			return 1;
		curr = curr->up;
	}
	return 0;	
}

int match_attr(char* t) {
	return  
	strcmp(t, "struct_union")	== 0	? ATTR_STRUCT	: 
	strcmp(t, "enum") 		== 0	? ATTR_ENUM	:
	strcmp(t, "const") 		== 0	? ATTR_CONSTANT	:
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
	
node* new_symbol(symbol_table *t, node *n) {
	symbol *s = malloc(sizeof(symbol));

	s->up = t->s;
	s->attr = 0;
	s->type = NULL;
	s->value = NULL;

	/* get the id and all attributes for this symbol */
	//print_tree(n, stdout, 0); 
	
	/* the first child node should be the types and attributes */
	if (strcmp(n->children[0]->id,"declaration_specifier") != 0) {
		err("[symbol] cannot extract type/attr information\n");
		exit(-1);
	} else {
		/* all children of declaration_specifiers should be type:id */
		node *types = n->children[0];
		node *decs   = n->children[1];
		int i;
		for (i=0; i<types->num_children; i++) {
			if (types->children[i]->type != type_type) {
				err("[symbol] found non-type in type specifiers\n");
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
		if (strcmp(decs->id, "init_declarator_list") != 0) {
			err("[symbol] missing init_declarator_list\n");
		} else {
			/* just do a search for "direct_declarator", and return each one's children[0]->id field, which is an identifier */
			id_chain *ids = get_ids(NULL, decs);
			if (ids == NULL) 
				err("[symbol] no declarators found\n");
			while (ids != NULL) {
				id_chain *d = ids;
				if (lookup(ids->id)) {
					err("[symbol] symbol %s already declared in this scope\n", ids->id);
				}
				if (ids->pointer)
					s->attr |= ATTR_POINTER; 
				s->value = ids->id;
				t->s = s;
				vlog("[symbol] created symbol: %s, type %s, attr 0x%08x\n", s->value, s->type, s->attr);
				if (ids->up != NULL) {
					s = malloc(sizeof(symbol));
					s->up = t->s;
					s->attr = t->s->attr;
					s->type = t->s->type;
					s->value = NULL;
				}
				ids = ids->up;
				free(d);
			}
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

	vlog("[symbol] creating new table\n");

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
		free(t);
		table->parent = n;
	} 
	return table;
}

void print_symbols(symbol_table* t, FILE* o, int depth) {
	/* print all of my symbols and then call my child to do the same */
	symbol *curr = NULL;
	int i;
	if (t == NULL) 
		return;
	curr = t->s;
	while (curr != NULL) {
		for (i=0; i<depth; i++)
			fprintf(o, "\t");
		fprintf(o, "id: %s, type: %s, attributes: 0x%08x\n", curr->value, curr->type, curr->attr);
		curr = curr->up;
	}
	for (i=0; i<t->num_children; i++)
		print_symbols(t->children[i], o, depth+1);
}
