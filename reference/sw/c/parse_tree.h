#ifndef PARSE_STACK
#define PARSE_STACK
#include <stdio.h>
#include "types.h"

node *con(char *);
node *id(char *);
node *str(char *);
node *type(char *);
node *op(char *t, int num_ops, ...);
node *add_child(node *, node *); /* add a child node to this node */

void print_tree(node *, FILE *, int);

#endif

