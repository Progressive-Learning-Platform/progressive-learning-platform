#ifndef SYMBOL
#define SYMBOL
#include "types.h"

node* install_symbol(symbol_table*, node*);
node* install_function(symbol_table*, node*);
symbol *find_symbol(symbol_table*, char*);
symbol_table* new_symbol_table(symbol_table*);
void print_symbols(symbol_table*, FILE*, int);
void install_parameters(symbol_table*, node*);
symbol *get_last_symbol(symbol_table*);

#endif 
