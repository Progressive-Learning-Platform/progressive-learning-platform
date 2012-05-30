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

#ifndef SYMBOL
#define SYMBOL
#include "types.h"

node* install_symbol(symbol_table*, node*);
node* install_function(symbol_table*, node*);
symbol *find_symbol(symbol_table*, char*);
symbol_table* new_symbol_table(symbol_table*);
void print_symbols(symbol_table*, FILE*, int);
void print_frames(symbol_table*, FILE*);
void install_parameters(symbol_table*, node*);
symbol *get_last_symbol(symbol_table*);

#endif 
