/* target independent code generator functions */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "code_gen.h"
#include "handlers.h"

extern FILE* FILE_OUTPUT;
extern char *program;

void handle(node *n) {

	if (strcmp(n->id,"postfix_expr") == 0) {
		vlog("[code_gen] handle: postfix_expr \n");
		handle_postfix_expr(n);
	} else if (strcmp(n->id,"argument_expr_list") == 0) {
		vlog("[code_gen] handle: argument_expr_list \n");
		handle_argument_expr_list(n);
	} else if (strcmp(n->id,"unary_expr") == 0) {
		vlog("[code_gen] handle: unary_expr \n");
		handle_unary_expr(n);
	} else if (strcmp(n->id,"sizeof") == 0) {
		vlog("[code_gen] handle: sizeof \n");
		handle_sizeof(n);
	} else if (strcmp(n->id,"cast_expr") == 0) {
		vlog("[code_gen] handle: cast_expr \n");
		handle_cast_expr(n);
	} else if (strcmp(n->id,"multiply") == 0) {
		vlog("[code_gen] handle: multiply \n");
		handle_multiply(n);
	} else if (strcmp(n->id,"divide") == 0) {
		vlog("[code_gen] handle: divide \n");
		handle_divide(n);
	} else if (strcmp(n->id,"mod") == 0) {
		vlog("[code_gen] handle: mod \n");
		handle_mod(n);
	} else if (strcmp(n->id,"add") == 0) {
		vlog("[code_gen] handle: add \n");
		handle_add(n);
	} else if (strcmp(n->id,"sub") == 0) {
		vlog("[code_gen] handle: sub \n");
		handle_sub(n);
	} else if (strcmp(n->id,"shift_left") == 0) {
		vlog("[code_gen] handle: shift_left \n");
		handle_shift_left(n);
	} else if (strcmp(n->id,"shift_right") == 0) {
		vlog("[code_gen] handle: shift_right \n");
		handle_shift_right(n);
	} else if (strcmp(n->id,"less_than") == 0) {
		vlog("[code_gen] handle: less_than \n");
		handle_less_than(n);
	} else if (strcmp(n->id,"greater_than") == 0) {
		vlog("[code_gen] handle: greater_than \n");
		handle_greater_than(n);
	} else if (strcmp(n->id,"less_equal_than") == 0) {
		vlog("[code_gen] handle: less_equal_than \n");
		handle_less_equal_than(n);
	} else if (strcmp(n->id,"greater_equal_than") == 0) {
		vlog("[code_gen] handle: greater_equal_than \n");
		handle_greater_equal_than(n);
	} else if (strcmp(n->id,"equality") == 0) {
		vlog("[code_gen] handle: equality \n");
		handle_equality(n);
	} else if (strcmp(n->id,"equality_not") == 0) {
		vlog("[code_gen] handle: equality_not \n");
		handle_equality_not(n);
	} else if (strcmp(n->id,"bitwise_and") == 0) {
		vlog("[code_gen] handle: bitwise_and \n");
		handle_bitwise_and(n);
	} else if (strcmp(n->id,"bitwise_xor") == 0) {
		vlog("[code_gen] handle: bitwise_xor \n");
		handle_bitwise_xor(n);
	} else if (strcmp(n->id,"bitwise_or") == 0) {
		vlog("[code_gen] handle: bitwise_or \n");
		handle_bitwise_or(n);
	} else if (strcmp(n->id,"logical_and") == 0) {
		vlog("[code_gen] handle: logical_and \n");
		handle_logical_and(n);
	} else if (strcmp(n->id,"logical_or") == 0) {
		vlog("[code_gen] handle: logical_or \n");
		handle_logical_or(n);
	} else if (strcmp(n->id,"conditional") == 0) {
		vlog("[code_gen] handle: conditional \n");
		handle_conditional(n);
	} else if (strcmp(n->id,"assignment") == 0) {
		vlog("[code_gen] handle: assignment \n");
		handle_assignment(n);
	} else if (strcmp(n->id,"assign") == 0) {
		vlog("[code_gen] handle: assign \n");
		handle_assign(n);
	} else if (strcmp(n->id,"assign_mul") == 0) {
		vlog("[code_gen] handle: assign_mul \n");
		handle_assign_mul(n);
	} else if (strcmp(n->id,"assign_div") == 0) {
		vlog("[code_gen] handle: assign_div \n");
		handle_assign_div(n);
	} else if (strcmp(n->id,"assign_mod") == 0) {
		vlog("[code_gen] handle: assign_mod \n");
		handle_assign_mod(n);
	} else if (strcmp(n->id,"assign_add") == 0) {
		vlog("[code_gen] handle: assign_add \n");
		handle_assign_add(n);
	} else if (strcmp(n->id,"assign_sub") == 0) {
		vlog("[code_gen] handle: assign_sub \n");
		handle_assign_sub(n);
	} else if (strcmp(n->id,"assign_sll") == 0) {
		vlog("[code_gen] handle: assign_sll \n");
		handle_assign_sll(n);
	} else if (strcmp(n->id,"assign_srl") == 0) {
		vlog("[code_gen] handle: assign_srl \n");
		handle_assign_srl(n);
	} else if (strcmp(n->id,"assign_and") == 0) {
		vlog("[code_gen] handle: assign_and \n");
		handle_assign_and(n);
	} else if (strcmp(n->id,"assign_xor") == 0) {
		vlog("[code_gen] handle: assign_xor \n");
		handle_assign_xor(n);
	} else if (strcmp(n->id,"assign_or") == 0) {
		vlog("[code_gen] handle: assign_or \n");
		handle_assign_or(n);
	} else if (strcmp(n->id,"expression") == 0) {
		vlog("[code_gen] handle: expression \n");
		handle_expression(n);
	} else if (strcmp(n->id,"declaration") == 0) {
		vlog("[code_gen] handle: declaration \n");
		handle_declaration(n);
	} else if (strcmp(n->id,"declaration_specifier") == 0) {
		vlog("[code_gen] handle: declaration_specifier \n");
		handle_declaration_specifier(n);
	} else if (strcmp(n->id,"init_declarator_list") == 0) {
		vlog("[code_gen] handle: init_declarator_list \n");
		handle_init_declarator_list(n);
	} else if (strcmp(n->id,"init_declarator") == 0) {
		vlog("[code_gen] handle: init_declarator \n");
		handle_init_declarator(n);
	} else if (strcmp(n->id,"struct_union") == 0) {
		vlog("[code_gen] handle: struct_union \n");
		handle_struct_union(n);
	} else if (strcmp(n->id,"struct_declaration_list") == 0) {
		vlog("[code_gen] handle: struct_declaration_list \n");
		handle_struct_declaration_list(n);
	} else if (strcmp(n->id,"struct_declaration") == 0) {
		vlog("[code_gen] handle: struct_declaration \n");
		handle_struct_declaration(n);
	} else if (strcmp(n->id,"specifier_qualifier_list") == 0) {
		vlog("[code_gen] handle: specifier_qualifier_list \n");
		handle_specifier_qualifier_list(n);
	} else if (strcmp(n->id,"struct_declarator_list") == 0) {
		vlog("[code_gen] handle: struct_declarator_list \n");
		handle_struct_declarator_list(n);
	} else if (strcmp(n->id,"struct_declarator") == 0) {
		vlog("[code_gen] handle: struct_declarator \n");
		handle_struct_declarator(n);
	} else if (strcmp(n->id,"enum_specifier") == 0) {
		vlog("[code_gen] handle: enum_specifier \n");
		handle_enum_specifier(n);
	} else if (strcmp(n->id,"enum_list") == 0) {
		vlog("[code_gen] handle: enum_list \n");
		handle_enum_list(n);
	} else if (strcmp(n->id,"enumerator") == 0) {
		vlog("[code_gen] handle: enumerator \n");
		handle_enumerator(n);
	} else if (strcmp(n->id,"declarator") == 0) {
		vlog("[code_gen] handle: declarator \n");
		handle_declarator(n);
	} else if (strcmp(n->id,"direct_declarator") == 0) {
		vlog("[code_gen] handle: direct_declarator \n");
		handle_direct_declarator(n);
	} else if (strcmp(n->id,"pointer") == 0) {
		vlog("[code_gen] handle: pointer \n");
		handle_pointer(n);
	} else if (strcmp(n->id,"type_qualifier_list") == 0) {
		vlog("[code_gen] handle: type_qualifier_list \n");
		handle_type_qualifier_list(n);
	} else if (strcmp(n->id,"parameter_type_list") == 0) {
		vlog("[code_gen] handle: parameter_type_list \n");
		handle_parameter_type_list(n);
	} else if (strcmp(n->id,"parameter_list") == 0) {
		vlog("[code_gen] handle: parameter_list \n");
		handle_parameter_list(n);
	} else if (strcmp(n->id,"parameter_declaration") == 0) {
		vlog("[code_gen] handle: parameter_declaration \n");
		handle_parameter_declaration(n);
	} else if (strcmp(n->id,"identifier_list") == 0) {
		vlog("[code_gen] handle: identifier_list \n");
		handle_identifier_list(n);
	} else if (strcmp(n->id,"type_name") == 0) {
		vlog("[code_gen] handle: type_name \n");
		handle_type_name(n);
	} else if (strcmp(n->id,"abstract_declarator") == 0) {
		vlog("[code_gen] handle: abstract_declarator \n");
		handle_abstract_declarator(n);
	} else if (strcmp(n->id,"direct_abstract_declarator") == 0) {
		vlog("[code_gen] handle: direct_abstract_declarator \n");
		handle_direct_abstract_declarator(n);
	} else if (strcmp(n->id,"initializer") == 0) {
		vlog("[code_gen] handle: initializer \n");
		handle_initializer(n);
	} else if (strcmp(n->id,"initializer_list") == 0) {
		vlog("[code_gen] handle: initializer_list \n");
		handle_initializer_list(n);
	} else if (strcmp(n->id,"labeled_statement") == 0) {
		vlog("[code_gen] handle: labeled_statement \n");
		handle_labeled_statement(n);
	} else if (strcmp(n->id,"compound_statement") == 0) {
		vlog("[code_gen] handle: compound_statement \n");
		handle_compound_statement(n);
	} else if (strcmp(n->id,"declaration_list") == 0) {
		vlog("[code_gen] handle: declaration_list \n");
		handle_declaration_list(n);
	} else if (strcmp(n->id,"statement_list") == 0) {
		vlog("[code_gen] handle: statement_list \n");
		handle_statement_list(n);
	} else if (strcmp(n->id,"expression_statement") == 0) {
		vlog("[code_gen] handle: expression_statement \n");
		handle_expression_statement(n);
	} else if (strcmp(n->id,"selection_statement") == 0) {
		vlog("[code_gen] handle: selection_statement \n");
		handle_selection_statement(n);
	} else if (strcmp(n->id,"iteration_statement") == 0) {
		vlog("[code_gen] handle: iteration_statement \n");
		handle_iteration_statement(n);
	} else if (strcmp(n->id,"jump_statement") == 0) {
		vlog("[code_gen] handle: jump_statement \n");
		handle_jump_statement(n);
	} else if (strcmp(n->id,"translation_unit") == 0) {
		vlog("[code_gen] handle: translation_unit \n");
		handle_translation_unit(n);
	} else if (strcmp(n->id,"function_definition") == 0) {
		vlog("[code_gen] handle: function_definition \n");
		handle_function_definition(n);
	} 
}

void emit(char *s) {
	if (program == NULL) {
		program = strdup(s);
	} else {
		program = realloc(program, (strlen(program)+strlen(s))*sizeof(char));
		program = strcat(program, s);
	}
}
