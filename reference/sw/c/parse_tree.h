#ifndef PARSE_STACK
#define PARSE_STACK

/* node types */
typedef enum {
	type_con,
	type_id,
	type_op
} node_type;

typedef struct node_t {
	node_type type;		/* node type */
	
	char *id;		/* the node id field, contains a constant if type_con, variable name if type_id, a keyword if type_op */
} node;

node *id(char *);

#endif

