/*
    Copyright 2011 the PLP authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "parse_tree.h"
#include "symbol.h"
#include "log.h"

extern symbol_table* sym;
extern int line;
extern int column;

node *new_node(char *s) {
	node *n = NULL;
	
	n = malloc(sizeof(node));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node\n");
	}

	n->id = strdup(s);
	if (n->id == NULL) {
		err("[parse_tree] strdup failed in new_node\n");
	}
	n->num_children = 0;
	n->t = sym;
	n->line = line;
	n->column = column;

	return n;
}

node *str(char *s) {
	node *n = new_node(s);
	vlog("[parse_tree] string: %s\n", s);
	n->type = type_string;
	return n;
}

node *type(char *s) {
	node *n = new_node(s);
	vlog("[parse_tree] type: %s\n", s);
	n->type = type_type;
	return n;
}

node *con(char *s) {
	node *n = new_node(s);
	vlog("[parse_tree] con: %s\n", s);
	n->type = type_con;
	return n;
}

node *id(char *s) {
	node *n = new_node(s);
	vlog("[parse_tree] id: %s\n", s);
	n->type = type_id;
	return n;
}

node *op(char *t, int num_ops, ...) {
	va_list ap;
	int i;
	node *n = NULL;

	vlog("[parse_tree] op: %s ", t);

	n = malloc(sizeof(node) + ((num_ops) * sizeof(node*)));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->type = type_op;
	n->id = strdup(t);
	if (n->id == NULL) {
		err("strdup failed to allocate\n");
	}
	n->t = sym;
	n->line = line;
	n->column = column;
	n->num_children = num_ops;
	va_start(ap, num_ops);
	for (i=0; i<num_ops; i++) {
		n->children[i] = va_arg(ap, node*);
		vlog(": %s ", n->children[i]->id);
	}
	vlog("\n");
	va_end(ap);

	return n;
}

node *add_child(node *parent, node *child) {
	/* create a new node with +1 children and return that, deleting the current parent */
	vlog("[parse_tree] add_node: %s to %s\n", child->id, parent->id);

	parent = realloc(parent, sizeof(node) + ((parent->num_children + 1) * sizeof(node*)));
	if (parent == NULL) {
		err("[parse_tree] cannot reallocate node");
	}

	parent->num_children++;
	parent->children[parent->num_children-1] = child;

	return parent;
}

void print_tree(node *n, FILE *o, int depth) {
	/* depth first traversal */
	int i;	

	/* print ourselves */
	for (i=0; i<depth; i++)
		fprintf(o, "\t");
	switch (n->type) {
		case type_con:
			fprintf(o, "constant:");
			break;
		case type_id:
			fprintf(o, "id:");
			break;
		case type_op:
			fprintf(o, "op:");
			break;
		case type_type:
			fprintf(o, "type:");
			break;
		case type_string:
			fprintf(o, "string:");
			break;
	}
	fprintf(o, "%s\n", n->id);
	
	/* print children */
	for (i=0; i<n->num_children; i++)
		print_tree(n->children[i], o, depth+1);
}
