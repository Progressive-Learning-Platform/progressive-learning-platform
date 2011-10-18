#ifndef HANDLERS
#define HANDLERS
#include "types.h"

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
void handle_assign(node *);
void handle_assign_mul(node *);
void handle_assign_div(node *);
void handle_assign_mod(node *);
void handle_assign_add(node *);
void handle_assign_sub(node *);
void handle_assign_sll(node *);
void handle_assign_srl(node *);
void handle_assign_and(node *);
void handle_assign_xor(node *);
void handle_assign_or(node *);
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

#endif
