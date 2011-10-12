#ifndef CODE_GEN
#define CODE_GEN
#include "types.h"

void handle(node *);
void emit(char *);
int get_offset(symbol_table*, char*);

#endif
