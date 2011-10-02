#ifndef PARSE_STACK
#define PARSE_STACK

/* node types */
typedef enum {
	type_con,
	type_id,
	type_op
} node_type;

typedef struct node_t {
	node_type type;			/* node type */
	char *id;			/* the node id field, contains a constant if type_con, variable name if type_id, a keyword if type_op */
	int num_children; 		/* number of values in the next field */
	struct node_t *children[];	/* an array of child nodes */
} node;

node *con(char *);
node *id(char *);
node *op(char *t, int num_ops, ...);
node *add_child(node *, node *); /* add a child node to this node */

#endif

