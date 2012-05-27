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

#ifndef HANDLERS
#define HANDLERS
#include "types.h"

int is_global(symbol_table*, char*);
int get_offset(symbol_table*, char*);
void handle_identifier(node *);
void handle_constant(node *);
void handle_string(node *);
void handle_postfix_expr(node *);
void handle_argument_expr_list(node *);
void handle_unary_expr(node *);
void handle_sizeof(node *);
void handle_cast_expr(node *);
void handle_multiply(node *);
void handle_divide(node *);
void handle_mod(node *);
void handle_add(node *);
void handle_sub(node *);
void handle_shift_left(node *);
void handle_shift_right(node *);
void handle_less_than(node *);
void handle_greater_than(node *);
void handle_less_equal_than(node *);
void handle_greater_equal_than(node *);
void handle_equality(node *);
void handle_equality_not(node *);
void handle_bitwise_and(node *);
void handle_bitwise_xor(node *);
void handle_bitwise_or(node *);
void handle_logical_and(node *);
void handle_logical_or(node *);
void handle_conditional(node *);
void handle_assignment(node *);
void handle_expression(node *);
void handle_declaration(node *);
void handle_declaration_specifier(node *);
void handle_init_declarator_list(node *);
void handle_init_declarator(node *);
void handle_struct_union(node *);
void handle_struct_declaration_list(node *);
void handle_struct_declaration(node *);
void handle_specifier_qualifier_list(node *);
void handle_struct_declarator_list(node *);
void handle_struct_declarator(node *);
void handle_enum_specifier(node *);
void handle_enum_list(node *);
void handle_enumerator(node *);
void handle_declarator(node *);
void handle_direct_declarator(node *);
void handle_pointer(node *);
void handle_type_qualifier_list(node *);
void handle_parameter_type_list(node *);
void handle_parameter_list(node *);
void handle_parameter_declaration(node *);
void handle_identifier_list(node *);
void handle_type_name(node *);
void handle_abstract_declarator(node *);
void handle_direct_abstract_declarator(node *);
void handle_initializer(node *);
void handle_initializer_list(node *);
void handle_labeled_statement(node *);
void handle_compound_statement(node *);
void handle_declaration_list(node *);
void handle_statement_list(node *);
void handle_expression_statement(node *);
void handle_selection_statement(node *);
void handle_iteration_statement(node *);
void handle_jump_statement(node *);
void handle_translation_unit(node *);
void handle_function_definition(node *);
void handle_asm(node *);

#endif
