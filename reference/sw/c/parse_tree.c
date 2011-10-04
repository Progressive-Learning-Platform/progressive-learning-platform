#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "parse_tree.h"
#include "log.h"

node *new_node(char *s) {
	node *n = NULL;
	
	n = malloc(sizeof(node));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->id = strdup(s);
	n->num_children = 0;

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

	n = malloc(sizeof(node) + ((num_ops - 1) * sizeof(node*)));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->type = type_op;
	n->id = strdup(t);
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
	node *n = NULL;
	int i;

	vlog("[parse_tree] add_node: %s to %s\n", child->id, parent->id);

	n = malloc(sizeof(node) + ((parent->num_children + 1) * sizeof(node*)));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->type = parent->type;
	n->id = strdup(parent->id);
	n->num_children = parent->num_children+1;
	
	for (i=0; i<parent->num_children; i++)
		n->children[i] = parent->children[i];
	n->children[i] = child;

	/* get rid of the old parent */
	//free(parent->id);
	//free(parent);

	return n;
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
