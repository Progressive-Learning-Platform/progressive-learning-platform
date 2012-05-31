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

/* target specific handlers, one for each node op type */
/* each handle must call handle(node*) for children nodes, or just return */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse_tree.h"
#include "symbol.h"
#include "log.h"
#include "code_gen.h"
#include "handlers.h"
#include "line.h"

/* the call/return instrcutions put 92 bytes of state on the stack, which sits between parameters and the local frame.
 * this gets added when looking up a parameter variable in get_offset and the o() macro
 */
#define PARAM_OFFSET 92
#define WORD 4

#define e(...) { v(n); sprintf(buffer, __VA_ARGS__); program = emit(program, buffer); }
#define v(x) { if (!is_visited(n->line) && ANNOTATE_SOURCE) { visit(n->line); sprintf(buffer, "#\n# LINE %d: %s#\n", n->line, get_line(n->line)); program = emit(program, buffer); }}
#define o(x) (get_offset(x->t, x->id) + (adjust * WORD))
#define g(x) (is_global(x->t, x->id))
#define push(x) { e("push %s\n", x); adjust++; }
#define pop(x) { e("pop %s\n", x); adjust--; }
#define eq(x,y) (strcmp(x,y) == 0)

extern char *program;
char *strings = NULL;
extern symbol *labels;
extern symbol *constants;
extern int ANNOTATE_SOURCE;
char buffer[1024];
int adjust = 1;
int LVALUE = 0;
int params = 0;
int param_words = 0;
int locals = 0;
symbol *last_symbol = NULL;

void epilogue(node *n) {
	e("addiu $sp, $sp, %d\n", locals);
	e("return\n");
}

/* 0 = local, 1 = global */
int is_global(symbol_table *t, char *s) {
	symbol *curr;

	if (t == NULL) {
		err("[code_gen] %s undeclared\n", s);
		return 0;
	}

	curr = t->s;
	
	while (curr != NULL) {
		if (strcmp(curr->value,s) == 0) {
			if (t->parent == NULL) {
				param_words = curr->size;
				return 1;
			} else {
				return 0;
			}
		}
		curr = curr->up;
	}
	return is_global(t->parent, s);
}

/* TODO: replace with get_symbol->offset */
int get_offset(symbol_table *t, char *s) {
	int offset = 0;
	symbol *curr;

	if (t == NULL) {
		err("code_gen] %s undeclared\n", s);
		return offset;
	}

	curr = t->s;

	/* look up in the current symbol table, then up to the next, and so forth */
	while (curr != NULL) {
		if (strcmp(curr->value,s) == 0) {
			vlog("found symbol %s %s, size %d, attr %08x, ", curr->type, curr->value, curr->size, curr->attr);
			if (curr->attr & ATTR_PARAM)
				/* if this is a parameter, then add another 92 bytes to the offset, to get past the saved state of the caller */
				offset += PARAM_OFFSET;
			vlog("%d\n", offset);
			param_words = curr->size; /* this is needed in case a parameter is being passed */
			return offset;
		}
		if (!(curr->attr & ATTR_FUNCTION)) /* functions don't sit on the stack */
			offset += curr->size * WORD;
		curr = curr->up;
	}
	
	/* if we get here, we need to look up another table */
	return offset + get_offset(t->parent, s);
}

void handle_identifier(node *n) {
	last_symbol = find_symbol(n->t, n->id);
	if (LVALUE) {
		if (g(n)) {
			/* just grab the global named pointer */
			e("li $t0, %s # *%s\n", n->id, n->id);
		} else {
			/* grab the identifier and put a pointer to it in t0 */
			e("addiu $t0, $sp, %d # *%s\n", o(n), n->id);
		}
	} else {
		if (g(n)) {
			/* grab and dereference */
			e("li $t0, %s\n", n->id);
			e("lw $t0, 0($t0) # %s\n", n->id);
		} else {
			/* grab the identifier and dereference it */
			e("lw $t0, %d($sp) # %s\n", o(n), n->id);	
		}
	}
}

void handle_constant(node *n) {
	e("li $t0, %s\n", n->id);
	param_words = 1; /* constants are ALWAYS one word */
}

void handle_string(node *n) {
	char *s = gen_label();
	/* we use emit() to add to the string table */
	sprintf(buffer, "%s:\n", s);
	strings = emit(strings, buffer);
	sprintf(buffer, ".asciiw %s\n", n->id);
	strings = emit(strings, buffer);
	
	/* get a pointer to our string in $t0 */
	e("li $t0, %s\n", s);

	/* the translation unit handler will emit the strings string at the end of the program */
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
			lerr(n->line, "[code_gen] invalid lvalue\n");
		} else {
			/* post increment */
			e("lw $t1, 0($t0)\n");
			e("addiu $t2, $t1, 1\n");
			e("sw $t2, 0($t0)\n");
			e("move $t0, $t1\n");
		}
	} else if (strcmp(n->children[1]->id, "dec") == 0) {
		if (LVALUE) {
			lerr(n->line, "[code_gen] invalid lvalue\n");
		} else {
			/* post deccrement */
			e("lw $t1, 0($t0)\n");
			e("addiu $t2, $t1, -1\n");
			e("sw $t2, 0($t0)\n");
			e("move $t0, $t1\n");
		}
	} else if (strcmp(n->children[1]->id, "argument_expr_list") == 0) {
		/* function call */
		handle(n->children[1]);
		e("call %s\n", n->children[0]->id);
		e("addiu $sp, $sp, %d\n", params * WORD);
		e("move $t0, $v0\n");
		params = 0;
	} else if (strcmp(n->children[1]->id, "paren") == 0) {
		/* function call without arguments */
		e("call %s\n", n->children[0]->id);
		e("move $t0, $v0\n");
	} else if (strcmp(n->children[1]->id, "expression") == 0) {
		/* array index */
		if (last_symbol != NULL) {
			if (last_symbol->attr & ATTR_POINTER) {
				e("lw $t0, 0($t0)\n");
			}
		}
		push("$t0");
		prev_lvalue = LVALUE;
		LVALUE = 0;
		handle(n->children[1]); /* the offset in the array */
		LVALUE = prev_lvalue;
		pop("$t1");
		e("sll $t0, $t0, 2\n"); /*TODO: not all array indexes are multiples of 4! */
		e("addu $t0, $t0, $t1\n");
		if (!LVALUE) { /* dereference it */
			e("lw $t0, 0($t0)\n");
		}
	} else {
		lerr(n->line, "[code_gen] postfix expressions not fully implemented\n");
	}	
}

void handle_argument_expr_list(node *n) {
	int i;
	for (i=n->num_children; i>0; i--) {
		handle(n->children[i-1]);
		push("$t0");
		vlog("[code_gen] pushed parameter with size %d words\n", param_words);
		params += param_words;
		param_words = 0;
	}
}

void handle_unary_expr(node *n) {
	if (strcmp(n->children[0]->id, "inc") == 0) {
		if (LVALUE) {
			lerr(n->line, "[code_gen] invalid lvalue\n");
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
			lerr(n->line, "[code_gen] invalid lvalue\n");
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
		if (!LVALUE) { /* dereference it */
			e("lw $t0, 0($t0)\n");
		}
	} else if (strcmp(n->children[0]->id, "+") == 0) {
		lerr(n->line, "[code_gen] unary + is just silly\n");
	} else if (strcmp(n->children[0]->id, "-") == 0) {
		lerr(n->line, "[code_gen] unary - is just silly\n");
	} else if (strcmp(n->children[0]->id, "~") == 0) {
		/* bitwise invert */
		if (LVALUE) {
			lerr(n->line, "[code_gen] invalid lvalue\n");
		} else {
			handle(n->children[1]);
			e("nor $t0, $t0, $t0\n");
		}
	} else if (strcmp(n->children[0]->id, "!") == 0) {
		/* boolean true/false */
		if (LVALUE) {
			lerr(n->line, "[code_gen] invalid lvalue\n");
		} else {
			char *done = gen_label();
			handle(n->children[1]);
			e("move $t1, $t0\n");
			e("move $t0, $zero\n");
			e("bne $t1, $t0, %s\n", done);
			e("nop\n");
			e("ori $t0, $zero, 1\n");
			e("%s:\n", done);
		}
	} else {
		lerr(n->line, "[code_gen] unary expressions not fully implemented\n");
	}			
}

void handle_sizeof(node *n) {
	lerr(n->line, "[code_gen] handle_sizeof not implemented\n");
}

void handle_cast_expr(node *n) {
	lerr(n->line, "[code_gen] handle_cast_expr not implemented\n");
}

void handle_multiply(node *n) {
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("mullo $t0, $t0, $t1\n");	
}

void handle_divide(node *n) {
	lerr(n->line, "[code_gen] handle_divide not implemented\n");
}

void handle_mod(node *n) {
	lerr(n->line, "[code_gen] handle_mod not implemented\n");
}

void handle_add(node *n) {
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("addu $t0, $t0, $t1\n");
}

void handle_sub(node *n) {
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("subu $t0, $t1, $t0\n");
}

void handle_shift_left(node *n) {
	/* shifting is a mess with plp, as there is no r-type shift instruction.
	 * so we're *stuck* with a loop structure to shift... YECH! 
	 */
	char *loop = gen_label();
	char *done = gen_label();
	handle(n->children[1]); /* the shift amount */
	push("$t0");
	handle(n->children[0]); /* the thing to shift */
	pop("$t1");
	e("%s:\n", loop);
	e("beq $t1, $zero, %s\n", done);
	e("nop\n");
	e("sll $t0, $t0, 1\n");
	e("addiu $t1, $t1, -1\n");
	e("j %s\n", loop);
	e("nop\n");
	e("%s:\n", done);
}

void handle_shift_right(node *n) {
	/* shifting is a mess with plp, as there is no r-type shift instruction.
	 * so we're *stuck* with a loop structure to shift... YECH! 
	 */
	char *loop = gen_label();
	char *done = gen_label();
	handle(n->children[1]); /* the shift amount */
	push("$t0");
	handle(n->children[0]); /* the thing to shift */
	pop("$t1");
	e("%s:\n", loop);
	e("beq $t1, $zero, %s\n", done);
	e("nop\n");
	e("srl $t0, $t0, 1\n");
	e("addiu $t1, $t1, -1\n");
	e("j %s\n", loop);
	e("nop\n");
	e("%s:\n", done);
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
	/* TODO: unsigned */
	/* return 1 if child 0 > child 1 */
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("slt $t0, $t0, $t1\n");
}

void handle_less_equal_than(node *n) {
	lerr(n->line, "[code_gen] handle_less_equal_than not implemented\n");
}

void handle_greater_equal_than(node *n) {
	lerr(n->line, "[code_gen] handle_greater_equal_than not implemented\n");
}

void handle_equality(node *n) {
	/* TODO: implement a better equivalence routine */
	char *done = gen_label();
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("move $t2, $t0\n");
	e("move $t0, $zero\n");
	e("bne $t1, $t2, %s\n", done);
	e("nop\n");
	e("ori $t0, $zero, 1\n");
	e("%s:\n", done);
}

void handle_equality_not(node *n) {
	/* TODO: implement a better equivalence routine */
	char *done = gen_label();
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("move $t2, $t0\n");
	e("move $t0, $zero\n");
	e("beq $t1, $t2, %s\n", done);
	e("nop\n");
	e("ori $t0, $zero, 1\n");
	e("%s:\n", done);
}

void handle_bitwise_and(node *n) {
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("and $t0, $t0, $t1\n");
}

void handle_bitwise_xor(node *n) {
	lerr(n->line, "[code_gen] handle_bitwise_xor not implemented\n");
}

void handle_bitwise_or(node *n) {
	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");
	e("or $t0, $t0, $t1\n");
}

void handle_logical_and(node *n) {
	char *done = gen_label();

	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");

	e("move $t2, $t0\n");
	e("move $t0, $zero\n");
	e("beq $t1, $zero, %s\n", done);
	e("nop\n");
	e("beq $t2, $zero, %s\n", done);
	e("nop\n");
	e("ori $t0, $zero, 1\n");
	e("%s:\n", done);
}

void handle_logical_or(node *n) {
	char *done = gen_label();
	char *next = gen_label();

	handle(n->children[0]);
	push("$t0");
	handle(n->children[1]);
	pop("$t1");

	e("move $t2, $t0\n");
	e("move $t0, $zero\n");
	e("beq $t1, $zero, %s\n", next);
	e("nop\n");
	e("ori $t0, $zero, 1\n");
	e("%s:\n", next);
	e("beq $t2, $zero, %s\n", done);
	e("nop\n");
	e("ori $t0, $zero, 1\n");
	e("%s:\n", done);
}

void handle_conditional(node *n) {
	char *fail = gen_label();
	char *done = gen_label();
	handle(n->children[0]);
	e("beq $t0, $zero, %s\n", fail);
	e("nop\n");
	handle(n->children[1]);
	e("j %s\n", done);
	e("%s:\n", fail);
	handle(n->children[2]);
	e("%s:\n", done);
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
		e("lw $t2, 0($t1)\n");
		e("mullo $t0, $t0, $t2\n");
		e("sw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_div") == 0) {
		lerr(n->line, "[code_gen] division not supported\n");
	} else if (strcmp(n->children[1]->id, "assign_mod") == 0) {
		lerr(n->line, "[code_gen] modulo not supported\n");
	} else if (strcmp(n->children[1]->id, "assign_add") == 0) {
		e("lw $t2, 0($t1)\n");
		e("addu $t0, $t0, $t2\n");
		e("sw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_sub") == 0) {
		e("lw $t2, 0($t1)\n");
		e("subu $t0, $t2, $t0\n");
		e("sw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_sll") == 0) {
		lerr(n->line, "[code_gen] shift assign not currently implemented\n");
	} else if (strcmp(n->children[1]->id, "assign_srl") == 0) {
		lerr(n->line, "[code_gen] shift assign not currently implemented\n");
	} else if (strcmp(n->children[1]->id, "assign_and") == 0) {
		e("lw $t2, 0($t1)\n");
		e("and $t0, $t0, $t2\n");
		e("sw $t0, 0($t1)\n");
	} else if (strcmp(n->children[1]->id, "assign_xor") == 0) {
		lerr(n->line, "[code_gen] xor assign not currently implemented\n");
	} else if (strcmp(n->children[1]->id, "assign_or") == 0) {
		e("lw $t2, 0($t1)\n");
		e("or $t0, $t0, $t2\n");
		e("sw $t0, 0($t1)\n");
	}
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
	lerr(n->line, "[code_gen] handle_declaration_specifier not implemented\n");
}

void handle_init_declarator_list(node *n) {
	/* init_declarator_lists have one or more init_declarators */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);
}

void handle_init_declarator(node *n) {
	node *x;

	/* get the id node */
	if (n->children[0]->num_children == 1)
		x = n->children[0]->children[0]->children[0];
	else
		x = n->children[0]->children[1]->children[0];

	if (g(x)) {
		/* global declaration */
		/* this is a special case, where we need to actually grab the constant value ourselves */
		/* By definition, global declarations may only be initialized to constants */
		e("%s:\n", x->id);
		if (n->num_children == 2) {
			/* this gets ugly...
			 * nodes are either initializer->constant or
			 * initializer->initializer_list->initializer->constant (with multiple initializers
			 */
			if (strcmp(n->children[1]->children[0]->id, "initializer_list") == 0) { /* mutliple initializers {1,2,3,4...} */
				int i;
				for (i=0; i<n->children[1]->children[0]->num_children; i++) {
					if (strcmp(n->children[1]->children[0]->children[i]->children[0]->id, "unary_expr") == 0) { /* this is a hack to detect negative numbers */
						e(".word -%s\n", n->children[1]->children[0]->children[i]->children[0]->children[1]->id);
					} else {
						e(".word %s\n", n->children[1]->children[0]->children[i]->children[0]->id);
					}
				}
			} else { /* single initializer */
				e(".word %s\n", n->children[1]->children[0]->id);
			}
		} else {
			/* it may still be an array declaration, just without an initializer, check the size on x */
			symbol *s = find_symbol(x->t, x->id);
			if (s == NULL) {
				e(".word 0\n");
			} else {
				e(".space %d\n", s->size);
			}
		}
	} else { /* a local variable initializer */
		/* init declarators may or may not have an initializer, if not, set the value to 0 */
		if (n->num_children == 2) { /* we have an initializer */
			 /* this gets ugly...
                         * nodes are either initializer->constant or
                         * initializer->initializer_list->initializer->constant (with multiple initializers
                         */
                        if (strcmp(n->children[1]->children[0]->id, "initializer_list") == 0) { /* mutliple initializers {1,2,3,4...} */
				int i;
				for (i=0; i<n->children[1]->children[0]->num_children; i++) {
					handle(n->children[1]->children[0]->children[i]);
					e("sw $t0, %d($sp)\n", o(x)+(WORD*i));
				}
			} else {
				handle(n->children[1]);
				e("sw $t0, %d($sp)\n", o(x));
			}
		} else {
			/* we could initialize values to zero here, but we won't. */
			/*e("move $t0, $zero\n");
			e("sw $t0, %d($sp)\n", o(x));*/
		}
	}
}

void handle_struct_union(node *n) {
	lerr(n->line, "[code_gen] handle_struct_union not implemented\n");
}

void handle_struct_declaration_list(node *n) {
	lerr(n->line, "[code_gen] handle_struct_declaration_list not implemented\n");
}

void handle_struct_declaration(node *n) {
	lerr(n->line, "[code_gen] handle_struct_declaration not implemented\n");
}

void handle_specifier_qualifier_list(node *n) {
	lerr(n->line, "[code_gen] handle_specifier_qualifier_list not implemented\n");
}

void handle_struct_declarator_list(node *n) {
	lerr(n->line, "[code_gen] handle_struct_declarator_list not implemented\n");
}

void handle_struct_declarator(node *n) {
	lerr(n->line, "[code_gen] handle_struct_declarator not implemented\n");
}

void handle_enum_specifier(node *n) {
	lerr(n->line, "[code_gen] handle_enum_specifier not implemented\n");
}

void handle_enum_list(node *n) {
	lerr(n->line, "[code_gen] handle_enum_list not implemented\n");
}

void handle_enumerator(node *n) {
	lerr(n->line, "[code_gen] handle_enumerator not implemented\n");
}

void handle_declarator(node *n) {
	/* declarators can have a direct_declarator or a pointer / dd pair */
	if (n->num_children == 2)
		handle(n->children[1]);
	else
		handle(n->children[0]);
}

void handle_direct_declarator(node *n) {
	/* do nothing with direct declarators, the symbol already exists */
	/* this is the place to initialize direct declarators to 0 if we need to */
}

void handle_pointer(node *n) {
	lerr(n->line, "[code_gen] handle_pointer not implemented\n");
}

void handle_type_qualifier_list(node *n) {
	lerr(n->line, "[code_gen] handle_type_qualifier_list not implemented\n");
}

void handle_parameter_type_list(node *n) {
	lerr(n->line, "[code_gen] handle_parameter_type_list not implemented\n");
}

void handle_parameter_list(node *n) {
	lerr(n->line, "[code_gen] handle_parameter_list not implemented\n");
}

void handle_parameter_declaration(node *n) {
	lerr(n->line, "[code_gen] handle_parameter_declaration not implemented\n");
}

void handle_identifier_list(node *n) {
	lerr(n->line, "[code_gen] handle_identifier_list not implemented\n");
}

void handle_type_name(node *n) {
	lerr(n->line, "[code_gen] handle_type_name not implemented\n");
}

void handle_abstract_declarator(node *n) {
	lerr(n->line, "[code_gen] handle_abstract_declarator not implemented\n");
}

void handle_direct_abstract_declarator(node *n) {
	lerr(n->line, "[code_gen] handle_direct_abstract_declarator not implemented\n");
}

void handle_initializer(node *n) {
	handle(n->children[0]);
}

void handle_initializer_list(node *n) {
	lerr(n->line, "[code_gen] handle_initializer_list not implemented\n");
}

void handle_labeled_statement(node *n) {
	lerr(n->line, "[code_gen] handle_labeled_statement not implemented\n");
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
	if (n->num_children == 3) { /* if else statement */
		char *selection_label_else = gen_label();
		char *selection_label_done = gen_label();
		handle(n->children[0]);
		e("beq $t0, $zero, %s\n\tnop\n", selection_label_else);
		handle(n->children[1]);
		e("j %s\n\tnop\n", selection_label_done);
		e("%s:\n", selection_label_else);
		handle(n->children[2]);
		e("%s:\n", selection_label_done);
	} else {
	//	lerr(n->line, "selection statement not fully implemented\n");
	}
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
		e("%s:\n", loop_label);
		handle(n->children[0]);
		e("beq $zero, $t0, %s\n", loop_label_done);
		e("nop\n");
		handle(n->children[1]);
		e("j %s\n", loop_label);
		e("nop\n");
		e("%s:\n", loop_label_done);
	} else if (n->num_children > 2) {
		/* type 3 or 4, for loop */ 
		handle(n->children[0]); /* initial expression */
		e("%s:\n", loop_label); /* beginning of for loop */
		handle(n->children[1]); /* condition, with result in $t0 */
		e("beq $t0, $zero, %s\n", loop_label_done);
		e("nop\n");
		if (n->num_children == 3) { /* no last statement */
			handle(n->children[2]);
		} else { 
			handle(n->children[3]); /* statement */
			handle(n->children[2]); /* last expression */
		}
		e("j %s\n", loop_label);
		e("nop\n");
		e("%s:\n", loop_label_done);
	} else {
		lerr(n->line, "this iteration statement not implemented yet");
	} 
}

void handle_jump_statement(node *n) {
	node *t = n->children[0];
	if (strcmp(t->id, "return") == 0) {
		if (n->num_children == 2)
			handle(n->children[1]);
			last_symbol = NULL; /* we want to support something like my_function()[5], which assumes that the pointer returned is just that, and doesn't need dereference magic */
			e("move $v0, $t0\n");
		epilogue(n);
	} else {
		lerr(n->line, "[code_gen] jump statements not fully implemented\n");
	}
}

void handle_translation_unit(node *n) {
	/* translation units have external declarations and function declarations, just call handle on my children */
	int i;
	for (i=0; i<n->num_children; i++)
		handle(n->children[i]);
	/* emit the strings if there are any */
	if (strings != NULL) {
		e("%s\n", strings);
	}

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
	e("%s:\n",function_name);

	/* reserve space on the stack for declarations related to this function (declarations only, parameters are pushed on the stack by the caller) */
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
		if (!(curr->attr & ATTR_PARAM))
			i += curr->size;
		curr = curr->up;
	}

	e("addiu $sp, $sp, -%d\n", i*WORD);
	locals = i*WORD;
	
	/* call handle on the compound statement */
	handle(n->children[n->num_children-1]);

	epilogue(n);
}

void handle_asm(node *n) {
	/* the first child should just be a string, emit it */
	e("#inline assembly begin\n")
	e("%s\n",n->children[0]->id);
	e("#inline assembly end\n");
}
