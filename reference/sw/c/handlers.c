/* target specific handlers, one for each node op type */
/* each handle must call handle(node*) for children nodes, or just return */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_tree.h"
#include "symbol.h"
#include "log.h"
#include "code_gen.h"

extern symbol *labels;
extern symbol *constants;
char buffer[1024];

void handle_postfix_expr(node *n) {
	err("[code_gen] handle_postfix_expr not implemented\n");
}

void handle_argument_expr_list(node *n) {
	err("[code_gen] handle_argument_expr_list not implemented\n");
}

void handle_unary_expr(node *n) {
	err("[code_gen] handle_unary_expr not implemented\n");
}

void handle_sizeof(node *n) {
	err("[code_gen] handle_sizeof not implemented\n");
}

void handle_cast_expr(node *n) {
	err("[code_gen] handle_cast_expr not implemented\n");
}

void handle_multiply(node *n) {
	err("[code_gen] handle_multiply not implemented\n");
}

void handle_divide(node *n) {
	err("[code_gen] handle_divide not implemented\n");
}

void handle_mod(node *n) {
	err("[code_gen] handle_mod not implemented\n");
}

void handle_add(node *n) {
	err("[code_gen] handle_add not implemented\n");
}

void handle_sub(node *n) {
	err("[code_gen] handle_sub not implemented\n");
}

void handle_shift_left(node *n) {
	err("[code_gen] handle_shift_left not implemented\n");
}

void handle_shift_right(node *n) {
	err("[code_gen] handle_shift_right not implemented\n");
}

void handle_less_than(node *n) {
	err("[code_gen] handle_less_than not implemented\n");
}

void handle_greater_than(node *n) {
	err("[code_gen] handle_greater_than not implemented\n");
}

void handle_less_equal_than(node *n) {
	err("[code_gen] handle_less_equal_than not implemented\n");
}

void handle_greater_equal_than(node *n) {
	err("[code_gen] handle_greater_equal_than not implemented\n");
}

void handle_equality(node *n) {
	err("[code_gen] handle_equality not implemented\n");
}

void handle_equality_not(node *n) {
	err("[code_gen] handle_equality_not not implemented\n");
}

void handle_bitwise_and(node *n) {
	err("[code_gen] handle_bitwise_and not implemented\n");
}

void handle_bitwise_xor(node *n) {
	err("[code_gen] handle_bitwise_xor not implemented\n");
}

void handle_bitwise_or(node *n) {
	err("[code_gen] handle_bitwise_or not implemented\n");
}

void handle_logical_and(node *n) {
	err("[code_gen] handle_logical_and not implemented\n");
}

void handle_logical_or(node *n) {
	err("[code_gen] handle_logical_or not implemented\n");
}

void handle_conditional(node *n) {
	err("[code_gen] handle_conditional not implemented\n");
}

void handle_assignment(node *n) {
	err("[code_gen] handle_assignment not implemented\n");
}

void handle_assign(node *n) {
	err("[code_gen] handle_assign not implemented\n");
}

void handle_assign_mul(node *n) {
	err("[code_gen] handle_assign_mul not implemented\n");
}

void handle_assign_div(node *n) {
	err("[code_gen] handle_assign_div not implemented\n");
}

void handle_assign_mod(node *n) {
	err("[code_gen] handle_assign_mod not implemented\n");
}

void handle_assign_add(node *n) {
	err("[code_gen] handle_assign_add not implemented\n");
}

void handle_assign_sub(node *n) {
	err("[code_gen] handle_assign_sub not implemented\n");
}

void handle_assign_sll(node *n) {
	err("[code_gen] handle_assign_sll not implemented\n");
}

void handle_assign_srl(node *n) {
	err("[code_gen] handle_assign_srl not implemented\n");
}

void handle_assign_and(node *n) {
	err("[code_gen] handle_assign_and not implemented\n");
}

void handle_assign_xor(node *n) {
	err("[code_gen] handle_assign_xor not implemented\n");
}

void handle_assign_or(node *n) {
	err("[code_gen] handle_assign_or not implemented\n");
}

void handle_expression(node *n) {
	err("[code_gen] handle_expression not implemented\n");
}

void handle_declaration(node *n) {
	/* we only care to call the init_declarator_list */
	if (n->num_children == 2)
		handle(n->children[1]);
}

void handle_declaration_specifier(node *n) {
	err("[code_gen] handle_declaration_specifier not implemented\n");
}

void handle_init_declarator_list(node *n) {
	err("[code_gen] handle_init_declarator_list not implemented\n");
}

void handle_init_declarator(node *n) {
	err("[code_gen] handle_init_declarator not implemented\n");
}

void handle_struct_union(node *n) {
	err("[code_gen] handle_struct_union not implemented\n");
}

void handle_struct_declaration_list(node *n) {
	err("[code_gen] handle_struct_declaration_list not implemented\n");
}

void handle_struct_declaration(node *n) {
	err("[code_gen] handle_struct_declaration not implemented\n");
}

void handle_specifier_qualifier_list(node *n) {
	err("[code_gen] handle_specifier_qualifier_list not implemented\n");
}

void handle_struct_declarator_list(node *n) {
	err("[code_gen] handle_struct_declarator_list not implemented\n");
}

void handle_struct_declarator(node *n) {
	err("[code_gen] handle_struct_declarator not implemented\n");
}

void handle_enum_specifier(node *n) {
	err("[code_gen] handle_enum_specifier not implemented\n");
}

void handle_enum_list(node *n) {
	err("[code_gen] handle_enum_list not implemented\n");
}

void handle_enumerator(node *n) {
	err("[code_gen] handle_enumerator not implemented\n");
}

void handle_declarator(node *n) {
	err("[code_gen] handle_declarator not implemented\n");
}

void handle_direct_declarator(node *n) {
	err("[code_gen] handle_direct_declarator not implemented\n");
}

void handle_pointer(node *n) {
	err("[code_gen] handle_pointer not implemented\n");
}

void handle_type_qualifier_list(node *n) {
	err("[code_gen] handle_type_qualifier_list not implemented\n");
}

void handle_parameter_type_list(node *n) {
	err("[code_gen] handle_parameter_type_list not implemented\n");
}

void handle_parameter_list(node *n) {
	err("[code_gen] handle_parameter_list not implemented\n");
}

void handle_parameter_declaration(node *n) {
	err("[code_gen] handle_parameter_declaration not implemented\n");
}

void handle_identifier_list(node *n) {
	err("[code_gen] handle_identifier_list not implemented\n");
}

void handle_type_name(node *n) {
	err("[code_gen] handle_type_name not implemented\n");
}

void handle_abstract_declarator(node *n) {
	err("[code_gen] handle_abstract_declarator not implemented\n");
}

void handle_direct_abstract_declarator(node *n) {
	err("[code_gen] handle_direct_abstract_declarator not implemented\n");
}

void handle_initializer(node *n) {
	err("[code_gen] handle_initializer not implemented\n");
}

void handle_initializer_list(node *n) {
	err("[code_gen] handle_initializer_list not implemented\n");
}

void handle_labeled_statement(node *n) {
	err("[code_gen] handle_labeled_statement not implemented\n");
}

void handle_compound_statement(node *n) {
	/* compound statements have four types:
		no children
		statement_list
		declaration_list
		declaration_list statement_list
	*/
	
	/* but who cares, just call handle on the children */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);		
}

void handle_declaration_list(node *n) {
	/* just call on the children */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);
}

void handle_statement_list(node *n) {
	err("[code_gen] handle_statement_list not implemented\n");
}

void handle_expression_statement(node *n) {
	err("[code_gen] handle_expression_statement not implemented\n");
}

void handle_selection_statement(node *n) {
	err("[code_gen] handle_selection_statement not implemented\n");
}

void handle_iteration_statement(node *n) {
	err("[code_gen] handle_iteration_statement not implemented\n");
}

void handle_jump_statement(node *n) {
	err("[code_gen] handle_jump_statement not implemented\n");
}

void handle_translation_unit(node *n) {
	/* translation units have external declarations and function declarations, just call handle on my children */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);
}

void handle_function_definition(node *n) {
	/* function definitions are one of four types:
		specifiers declarator declarations compound
		specifiers declarator compound
		declarator declarations compound
		declarator compound
	*/
	int d_index;	
	char *function_name;
	int i;
	symbol_table *scope;
	symbol *curr;

	/* emit a label for the function name */
	if (strcmp(n->children[0]->id, "declarator") == 0)
		d_index = 0;
	else
		d_index = 1;
	function_name = n->children[d_index]->children[0]->children[0]->id;
	
	/* emit the function name */
	sprintf(buffer,"%s:\n",function_name);
	emit(buffer);

	/* reserve space on the stack for declarations related to this function (parameters and declarations) */
	curr = n->t->s;
	while (strcmp(curr->value, function_name) != 0)
		curr = curr->up;
	/* start by finding the child symbol table that has assoc == n */
	for (i=0; i<n->t->num_children; i++) {
		if (n->t->children[i]->assoc == curr) {
			scope = n->t->children[i];
			break;
		}
	}
	/* add up the symbols */
	i=0;
	curr = scope->s;
	while (curr != NULL) {
		i++;
		curr = curr->up;
	}

	sprintf(buffer, "addiu $sp, $sp, -%d\n", i*4);
	emit(buffer);
	
	/* call handle on the compound statement */
	handle(n->children[n->num_children-1]);
}
