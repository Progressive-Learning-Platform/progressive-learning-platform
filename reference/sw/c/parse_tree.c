#include <stdlib.h>
#include "parse_tree.h"
#include "string.h"
#include "log.h"

node *id(char *s) {
	node *n = NULL;
	
	vlog("[parse_tree] id: %s\n", s);

	n = malloc(sizeof(node));
	if (n == NULL) {
		err("[parse_tree] cannot allocate node");
	}

	n->type = type_id;
	n->id	= s;
	
	return n;
}
