/* target specific handlers, one for each node op type */
/* each handle must call handle(node*) for children nodes, or just return */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_tree.h"
#include "symbol.h"
#include "log.h"
#include "code_gen.h"

#define e(...) { sprintf(buffer, __VA_ARGS__); emit(buffer); }
#define o(x) (get_offset(x->t, x->id) + (4*adjust))
#define push(x) { e("push %s\n", x); adjust++; }
#define pop(x) { e("pop %s\n", x); adjust--; }

extern symbol *labels;
extern symbol *constants;
char buffer[1024];
int adjust = 0;
int LVALUE = 0;

void handle_identifier(node *n) {
	if (LVALUE) {
		/* grab the identifier and put a pointer to it in t0 */
		e("addiu $t0, $sp, %d\n", o(n));
	} else {
		/* grab the identifier and dereference it */
		e("lw $t0, %d($sp)\n", o(n));	
	}
}

void handle_constant(node *n) {
	e("li $t0, %s\n", n->id);
}

void handle_string(node *n) {
	err("[code_gen] handle_string not implemented\n");
}

void handle_postfix_expr(node *n) {
	int prev_lvalue;
	
	/* the first child is always grabbed as a reference */
	prev_lvalue = LVALUE;
	LVALUE = 1;
	handle(n->children[0]);
	LVALUE = prev_lvalue;

	if (strcmp(n->children[1]->id, "inc") == 0) {
		if (LVALUE) {
			err("[code_gen] invalid lvalue\n");
		} else {
			/* post increment */
			e("lw $t1, 0($t0)\n");
			e("addiu $t2, $t1, 1\n");
			e("sw $t2, 0($t0)\n");
			e("move $t0, $t1\n");
		}
	} else {
		err("[code_gen] postfix expressions not fully implemented\n");
	}	
}

void handle_argument_expr_list(node *n) {
	err("[code_gen] handle_argument_expr_list not implemented\n");
}

void handle_unary_expr(node *n) {
	if (strcmp(n->children[0]->id, "inc") == 0) {
		if (LVALUE) {
			err("[code_gen] invalid lvalue\n");
		} else {
			/* preincrement the value and return it in t0 */
			LVALUE = 1;
			handle(n->children[1]);
			LVALUE = 0;
			e("lw $t1, 0($t0)\n");
			e("addiu $t1, $t1, 1\n");
			e("sw $t1, 0($t0)\n");
			e("move $t0, $t1\n");
		}
	} else if (strcmp(n->children[0]->id, "dec") == 0) {
		if (LVALUE) {
			err("[code_gen] invalid lvalue\n");
		} else {
			/* predecrement the value and return it in t0 */
			LVALUE = 1;
			handle(n->children[1]);
			LVALUE = 0;
			e("lw $t1, 0($t0)\n");
			e("addiu $t1, $t1, -1\n");
			e("sw $t1, 0($t0)\n");
			e("move $t0, $t1\n");
		}
	} else if (strcmp(n->children[0]->id, "&") == 0) {
		int prev_lvalue = LVALUE;
		LVALUE = 1;
		handle(n->children[1]);
		LVALUE = prev_lvalue;
	} else if (strcmp(n->children[0]->id, "*") == 0) {
		int prev_lvalue = LVALUE;
		LVALUE = 0;
		handle(n->children[1]);
		LVALUE = prev_lvalue;
	} else {
		err("[code_gen] unary expressions not fully implemented\n");
	}			
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
	/* TODO: HANDLE UNSIGNED COMPARE */
	/* return 1 if child 0 < child 1 */
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("slt $t0, $t1, $t0\n");
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
	/* get the lvalue */
	LVALUE = 1;
	handle(n->children[0]);
	LVALUE = 0;	

	push("$t0");

	/* and get the rvalue */
	handle(n->children[2]);

	pop("$t1");

	/* now make the assignment $t1 = $t0 */
	if (strcmp(n->children[1]->id, "assign") == 0) {
		e("sw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_mul") == 0) {
		e("lw $t2, 0($t1)\nmullo $t0, $t0, $t2\nsw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_div") == 0) {
		err("[code_gen] division not supported\n");
	} else if (strcmp(n->children[1]->id, "assign_mod") == 0) {
		err("[code_gen] modulo not supported\n");
	} else if (strcmp(n->children[1]->id, "assign_add") == 0) {
		e("lw $t2, 0($t1)\naddu $t0, $t0, $t2\nsw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_sub") == 0) {
		e("lw $t2, 0($t1)\nsubu $t0, $t2, $t0\nsw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_sll") == 0) {
		err("[code_gen] shift assign not currently implemented\n");
	} else if (strcmp(n->children[1]->id, "assign_srl") == 0) {
		err("[code_gen] shift assign not currently implemented\n");
	} else if (strcmp(n->children[1]->id, "assign_and") == 0) {
		e("lw $t2, 0($t1)\nand $t0, $t0, $t2\nsw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_xor") == 0) {
		err("[code_gen] xor assign not currently implemented\n");
	} else if (strcmp(n->children[1]->id, "assign_or") == 0) {
		e("lw $t2, 0($t1)\nor $t0, $t0, $t2\nsw $t0, 0($t1)\n");
	}
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
	/* expressions can be one or more of:
		assignment
		conditional
		logical_or
		logical_and
		bitwise_or
		bitwise_xor
		bitwise_and
		equality
		equality_not
		less_than
		greater_than
		less_equal_than
		greater_equal_than
		shift_left
		shift_right
		add
		sub
		multiply
		divide
		mod
		cast_expr
		unary_expr
		sizeof
		postfix_expr
		(id,constant,string,(expression))
	*/
	/* there are handlers for all except id,constant,string */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);
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
	/* init_declarator_lists have one or more init_declarators */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);
}

void handle_init_declarator(node *n) {
	node *x;

	/* init declarators may or may not have an initializer, if not, set the value to 0 */
	if (n->num_children == 2)  /* we have an initializer */
		handle(n->children[1]);
	else
		e("move $t0, $zero\n");

	/* get the id */
	if (n->children[0]->num_children == 1)
		x = n->children[0]->children[0]->children[0];
	else
		x = n->children[0]->children[1]->children[0];

	/* now make the assignment */
	e("sw $t0, %d($sp)\n", o(x));
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
	handle(n->children[0]);
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
	/* just a bunch of statements, call handle on them */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);
}

void handle_expression_statement(node *n) {
	/* it's either a ; or an expression */
	if (n->num_children == 1)
		handle(n->children[0]);
}

void handle_selection_statement(node *n) {
	err("[code_gen] handle_selection_statement not implemented\n");
}

void handle_iteration_statement(node *n) {
	/* iteration statements can be of four types:
	1	while   : expression statement
	2	do while: statement expression
	3	for     : expression_statement expression_statement statement
	4	for	: expression_statement expression_statement expression statement
	*/
	char *loop_label = gen_label();
	char *loop_label_done = gen_label();
	if (strcmp(n->children[0]->id, "expression") == 0) {
		/* type 1, while loop */
		/* handle the expression first, then test, then execute the statement, then jump back */
		sprintf(buffer, "%s:\n", loop_label);
		emit(buffer);
		handle(n->children[0]);
		sprintf(buffer, "beq $zero, $t0, %s\nnop\n", loop_label_done);
		emit(buffer);
		handle(n->children[1]);
		sprintf(buffer, "j %s\nnop\n%s:\n", loop_label, loop_label_done);
		emit(buffer);
	} else {
		err("this iteration statement not implemented yet");
	}
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
	symbol_table *scope = NULL;
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
