#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "parse_tree.h"
#include "log.h"

node *con(char *s) {
	node *n = NULL;
	
	vlog("[parse_tree] con: %s\n", s);
	
	n = malloc(sizeof(node));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->type = type_con;
	n->id = strdup(s);
	n->num_children = 0;

	return n;
}

node *id(char *s) {
	node *n = NULL;
	
	vlog("[parse_tree] id: %s\n", s);

	n = malloc(sizeof(node));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->type = type_id;
	n->id = strdup(s);
	n->num_children = 0;
	
	return n;
}

node *op(char *t, int num_ops, ...) {
	va_list ap;
	int i;
	node *n = NULL;

	vlog("[parse_tree] op: %s\n", t);

	n = malloc(sizeof(node) + ((num_ops - 1) * sizeof(node*)));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->type = type_op;
	n->id = strdup(t);
	n->num_children = num_ops;
	va_start(ap, num_ops);
	for (i=0; i<num_ops; i++)
		n->children[i] = va_arg(ap, node*);
	va_end(ap);

	return n;
}

node *add_child(node *parent, node *child) {
	/* create a new node with +1 children and return that, deleting the current parent */
	node *n = NULL;
	int i;

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
	free(parent->id);
	free(parent);

	return n;
}
