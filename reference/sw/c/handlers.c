#include "handlers.h"
#include "stack.h"

/* basic identifier, just put it on the stack */
void handle_identifier(char *s) {
	push(s);
}
