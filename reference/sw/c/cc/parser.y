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

%{
#include <stdio.h>
#include "log.h"
#include "parse_tree.h"
#include "symbol.h"
#include "line.h"

#define YYSTYPE node *

extern char yytext[];
extern int column, line;
extern symbol_table *sym;
extern node *parse_tree_head;

extern int yylex (void);

void yyerror(s)
char *s;
{
	lerr(line, "[plpcc] syntax error\n"); 
}

%}

%token IDENTIFIER CONSTANT STRING_LITERAL 
%token SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token ASM

%nonassoc IFX
%nonassoc ELSE

%start translation_unit
%%

primary_expression
	: IDENTIFIER { vlog("[parser] IDENTIFIER: %s\n", $1->id); /* the lexer already made this object a constant */ }
	| CONSTANT { vlog("[parser] CONSTANT: %s\n", $1->id); /* the lexer already made this object a constant */ } 
	| STRING_LITERAL { vlog("[parser] STRING_LITERAL: %s\n", $1->id); /* the lexer already made this object a string */ }
	| '(' expression ')' { vlog("[parser] EXPRESSION\n"); $$ = $2; }
	| ASM '(' STRING_LITERAL ')' { vlog("[parser] INLINE ASSEMBLY\n"); $$ = op("asm", 1, $3); }
	;

postfix_expression
	: primary_expression 
	| postfix_expression '[' expression ']' { vlog("[parser] POSTFIX_BRACKET_EXPRESSION\n"); $$ = op("postfix_expr", 2, $1, $3); }
	| postfix_expression '(' ')' { vlog("[parser] POSTFIX_PAREN_EXPRESSION\n"); $$ = op("postfix_expr", 2, $1, id("paren")); }
	| postfix_expression '(' argument_expression_list ')' { vlog("[parser] POSTFIX_ARG_EXPRESSION_LIST\n"); $$ = op("postfix_expr", 2, $1, $3); }
	| postfix_expression '.' IDENTIFIER { vlog("[parser] POSTFIX_._IDENTIFIER\n"); $$ = op("postfix_expr", 3, $1, id("dot"), $3); }
	| postfix_expression PTR_OP IDENTIFIER { vlog("[parser] POSTFIX_PTR_OP_IDENTIFIER\n"); $$ = op("postfix_expr", 3, $1, id("ptr"), $3); }
	| postfix_expression INC_OP { vlog("[parser] POSTFIX_INC_OP_EXPRESSION\n"); $$ = op("postfix_expr", 2, $1, id("inc")); }
	| postfix_expression DEC_OP { vlog("[parser] POSTFIX_DEC_OP_EXPRESSION\n"); $$ = op("postfix_expr", 2, $1, id("dec")); }
	;

argument_expression_list
	: assignment_expression { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); $$ = op("argument_expr_list", 1, $1); }
	| argument_expression_list ',' assignment_expression { vlog("[parser] ARG_EXPRESSION_LIST_,_ASSIGNMENT_EXPRESSION\n"); $$ = add_child($1, $3); }
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression { vlog("[parser] INC_OP_UNARY_EXPRESSION\n"); $$ = op("unary_expr", 2, id("inc"), $2); }
	| DEC_OP unary_expression { vlog("[parser] DEC_OP_UNARY_EXPRESSION\n"); $$ = op("unary_expr", 2, id("dec"), $2); }
	| unary_operator cast_expression { vlog("[parser] UNARY_OP_CAST_EXPRESSION\n"); $$ = op("unary_expr", 2, $1, $2); }
	| SIZEOF unary_expression { vlog("[parser] SIZEOF_UNARY_EXP\n"); $$ = op("sizeof", 1, $1); }
	| SIZEOF '(' type_name ')' { vlog("[parser] SIZEOF_TYPE_NAME\n"); $$ = op("sizeof", 1, $3); }
	;

unary_operator
	: '&' { vlog("[parser] &\n"); $$ = id("&"); }
	| '*' { vlog("[parser] *\n"); $$ = id("*"); }
	| '+' { vlog("[parser] +\n"); $$ = id("+"); }
	| '-' { vlog("[parser] -\n"); $$ = id("-"); }
	| '~' { vlog("[parser] ~\n"); $$ = id("~"); }
	| '!' { vlog("[parser] !\n"); $$ = id("!"); }
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression { vlog("[parser] TYPE_NAME_CAST_EXPRESSION\n"); $$ = op("cast_expr", 2, $2, $4); }
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression { vlog("[parser] MULTIPLICATIVE_EXPRESSION_*_CAST_EXPRESSION\n"); $$ = op("multiply", 2, $1, $3); }
	| multiplicative_expression '/' cast_expression { vlog("[parser] MULTIPLICATIVE_EXPRESSION_/_CAST_EXPRESSION\n"); $$ = op("divide", 2, $1, $3); }
	| multiplicative_expression '%' cast_expression { vlog("[parser] MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION\n"); $$ = op("mod", 2, $1, $3); }
	;

additive_expression
	: multiplicative_expression 
	| additive_expression '+' multiplicative_expression { vlog("[parser] ADDITIVE_EXPRESSION_+_MULTIPLICATIVE_EXPRESSION\n"); $$ = op("add", 2, $1, $3); }
	| additive_expression '-' multiplicative_expression { vlog("[parser] ADDITIVE_EXPRESSION_-_MULTIPLICATIVE_EXPRESSION\n"); $$ = op("sub", 2, $1, $3); }
	;

shift_expression
	: additive_expression 
	| shift_expression LEFT_OP additive_expression { vlog("[parser] SHIFT_LEFT_ADDITIVE\n"); $$ = op("shift_left", 2, $1, $3); }
	| shift_expression RIGHT_OP additive_expression { vlog("[parser] SHIFT_RIGHT_ADDITIVE\n"); $$ = op("shift_right", 2, $1, $3); }
	;

relational_expression
	: shift_expression 
	| relational_expression '<' shift_expression { vlog("[parser] RELATIONAL_<_SHIFT\n"); $$ = op("less_than", 2, $1, $3); }
	| relational_expression '>' shift_expression { vlog("[parser] RELATIONAL_>_SIHFT\n"); $$ = op("greater_than", 2, $1, $3); }
	| relational_expression LE_OP shift_expression { vlog("[parser] RELATIONAL_LE_SHIFT\n"); $$ = op("less_equal_than", 2, $1, $3); }
	| relational_expression GE_OP shift_expression { vlog("[parser] RELATIONAL_GE_SHIFT\n"); $$ = op("greater_equal_than", 2, $1, $3); }
	;

equality_expression
	: relational_expression 
	| equality_expression EQ_OP relational_expression { vlog("[parser] EQUALITY_EQ_RELATIONAL\n"); $$ = op("equality", 2, $1, $3); }
	| equality_expression NE_OP relational_expression { vlog("[parser] EQUALITY_NE_RELATIONAL\n"); $$ = op("equality_not", 2, $1, $3); }
	;

and_expression
	: equality_expression 
	| and_expression '&' equality_expression { vlog("[parser] AND_EXPRESSION_&_EQUALITY\n"); $$ = op("bitwise_and", 2, $1, $3); }
	;

exclusive_or_expression
	: and_expression 
	| exclusive_or_expression '^' and_expression { vlog("[parser] XOR_^_AND_EXPRESSION\n"); $$ = op("bitwise_xor", 2, $1, $3); }
	;

inclusive_or_expression
	: exclusive_or_expression 
	| inclusive_or_expression '|' exclusive_or_expression { vlog("[parser] OR_|_XOR\n"); $$ = op("bitwise_or", 2, $1, $3); }
	;

logical_and_expression
	: inclusive_or_expression 
	| logical_and_expression AND_OP inclusive_or_expression { vlog("[parser] LOGICAL_AND_&&_OR\n"); $$ = op("logical_and", 2, $1, $3); }
	;

logical_or_expression
	: logical_and_expression 
	| logical_or_expression OR_OP logical_and_expression { vlog("[parser] LOGICAL_OR_||_LOGICAL_AND\n"); $$ = op("logical_or", 2, $1, $3); }
	;

conditional_expression
	: logical_or_expression 
	| logical_or_expression '?' expression ':' conditional_expression { vlog("[parser] LOGICAL_OR_?\n"); $$ = op("conditional", 3, $1, $3, $5); }
	;

assignment_expression
	: conditional_expression 
	| unary_expression assignment_operator assignment_expression { vlog("[parser] UNARY_ASSIGNMENT\n"); $$ = op("assignment", 3, $1, $2, $3); }
	;

assignment_operator
	: '=' { vlog("[parser] =\n"); $$ = op("assign", 0);}
	| MUL_ASSIGN { vlog("[parser] *=\n"); $$ = op("assign_mul", 0); }
	| DIV_ASSIGN { vlog("[parser] /=\n"); $$ = op("assign_div", 0); }
	| MOD_ASSIGN { vlog("[parser] MOD=\n"); $$ = op("assign_mod", 0); }
	| ADD_ASSIGN { vlog("[parser] +=\n"); $$ = op("assign_add", 0); }
	| SUB_ASSIGN { vlog("[parser] -=\n"); $$ = op("assign_sub", 0); }
	| LEFT_ASSIGN { vlog("[parser] <<=\n"); $$ = op("assign_sll", 0); }
	| RIGHT_ASSIGN { vlog("[parser] >>=\n"); $$ = op("assign_srl", 0); }
	| AND_ASSIGN { vlog("[parser] &=\n"); $$ = op("assign_and", 0); }
	| XOR_ASSIGN { vlog("[parser] ^=\n"); $$ = op("assign_xor", 0); }
	| OR_ASSIGN { vlog("[parser] |=\n"); $$ = op("assign_or", 0); }
	;

expression
	: assignment_expression { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); $$ = op("expression", 1, $1); }
	| expression ',' assignment_expression { vlog("[parser] EXPRESSION_,_ASSIGNMENT_EXPRESSION\n"); $$ = add_child($1, $3); }
	;

constant_expression
	: conditional_expression /* why is this here!? */
	;

declaration
	: declaration_specifiers ';' { vlog("[parser] DECLARATION_SPECIFIERS_;\n"); $$ = op("declaration", 1, $1); $$ = install_symbol(sym, $$); }
	| declaration_specifiers init_declarator_list ';' { vlog("[parser] DECLARATION_SPECIFIERS_INIT_DECLARATOR_LIST_;\n"); $$ = op("declaration", 2, $1, $2); $$ = install_symbol(sym, $$); }
	;

declaration_specifiers
	: storage_class_specifier { vlog("[parser] STORAGE_CLASS_SPECIFIER\n"); $$ = op("declaration_specifier", 1, $1); } 
	| storage_class_specifier declaration_specifiers { vlog("[parser] STORAGE_CLASS_SPECIFIER_DECLARATION_SPECIFIERS\n"); $$ = add_child($2, $1); } 
	| type_specifier { vlog("[parser] DECLARATION_SPECIFIER_TYPE_SPECIFIER\n"); $$ = op("declaration_specifier", 1, $1); }
	| type_specifier declaration_specifiers { vlog("[parser] TYPE_SPECIFIER_DECLARATION_SPECIFIERS\n"); $$ = add_child($2, $1); } 
	| type_qualifier { vlog("[parser] TYPE_QUALIFIER\n"); $$ = op("declaration_specifier", 1, $1); } 
	| type_qualifier declaration_specifiers { vlog("[parser] TYPE_QUALIFIER_DECLARATION_SPECIFIERS\n"); $$ = add_child($2, $1); } 
	;

init_declarator_list
	: init_declarator { vlog("[parser] INIT_DECLARATOR\n"); $$ = op("init_declarator_list", 1, $1); } 
	| init_declarator_list ',' init_declarator { vlog("[parser] INIT_DECLARATOR_LIST_,_INIT_DECLARATOR\n"); $$ = add_child($1, $3); } 
	;

init_declarator
	: declarator { vlog("[parser] DECLARATOR\n"); $$ = op("init_declarator", 1, $1); } 
	| declarator '=' initializer { vlog("[parser] DECLARATOR_=_INITIALIZER\n"); $$ = op("init_declarator", 2, $1, $3); }
	;

storage_class_specifier
	: TYPEDEF { vlog("[parser] TYPEDEF\n"); $$ = type("typedef"); }
	| EXTERN { vlog("[parser] EXTERN\n"); $$ = type("extern"); }
	| STATIC { vlog("[parser] STATIC\n"); $$ = type("static"); }
	| AUTO { vlog("[parser] AUTO\n"); $$ = type("auto"); }
	| REGISTER { vlog("[parser] REGISTER\n"); $$ = type("register"); }
	;

type_specifier
	: VOID { vlog("[parser] VOID\n"); $$ = type("void"); }
	| CHAR { vlog("[parser] CHAR\n"); $$ = type("char"); }
	| SHORT { vlog("[parser] SHORT\n"); $$ = type("short"); }
	| INT { vlog("[parser] INT\n"); $$ = type("int"); }
	| LONG { vlog("[parser] LONG\n"); $$ = type("long"); }
	| FLOAT { vlog("[parser] FLOAT\n"); $$ = type("float"); }
	| DOUBLE { vlog("[parser] DOUBLE\n"); $$ = type("double"); }
	| SIGNED { vlog("[parser] SIGNED\n"); $$ = type("signed"); }
	| UNSIGNED { vlog("[parser] UNSIGNED\n"); $$ = type("unsigned"); }
	| struct_or_union_specifier { vlog("[parser] STRUCT_UNION\n"); /* do nothing */ }
	| enum_specifier { vlog("[parser] ENUM\n"); $$ = type("enum"); }
	| TYPE_NAME { vlog("[parser] TYPE_NAME\n"); $$ = type($1->id); }
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}' { vlog("[parser] STRUCT_IDENTIFIER_{}\n"); $$ = op("struct_union", 3, $1, $2, $4); }
	| struct_or_union '{' struct_declaration_list '}' { vlog("[parser] STRUCT_{}\n"); $$ = op("struct_union", 2, $1, $3); }
	| struct_or_union IDENTIFIER { vlog("[parser] STRUCT_IDENTIFIER\n"); $$ = op("struct_union", 2, $1, $2); }
	;

struct_or_union
	: STRUCT { vlog("[parser] STRUCT\n"); $$ = type("struct"); }
	| UNION { vlog("[parser] UNION\n"); $$ = type("union"); }
	;

struct_declaration_list
	: struct_declaration { vlog("[parser] STRUCT_DECLARATION\n"); $$ = op("struct_declaration_list", 1, $1); } 
	| struct_declaration_list struct_declaration  { vlog("[parser] STRUCT_DECLARATION_LIST\n"); $$ = add_child($1, $2); } 
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'  { vlog("[parser] SPECIFIER_QUALIFIER_LIST_STRUCT_DECLARATOR_LIST\n"); $$ = op("struct_declaration", 2, $1, $2); } 
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list { vlog("[parser] TYPE_SPECIFIER_SPEC_QUALIFIER_LIST\n"); $$ = add_child($2, $1); }
	| type_specifier { vlog("[parser] SPECIFIER_QUALIFIER_LIST_TYPE_SPECIFIER\n"); $$ = op("specifier_qualifier_list", 1, $1); }
	| type_qualifier specifier_qualifier_list { vlog("[parser] TYPE_QUALIFIER_SPEC_QUALIFIER_LIST\n"); $$ = add_child($2, $1); }
	| type_qualifier { vlog("[parser] TYPE_QUALIFIER\n"); $$ = op("specifier_qualifier_list", 1, $1); }
	;

struct_declarator_list
	: struct_declarator { vlog("[parser] STRUCT_DECLARATOR\n"); $$ = op("struct_declarator_list", 1, $1); }
	| struct_declarator_list ',' struct_declarator { vlog("[parser] STRUCT_DECLARATOR_LIST_,_STRUCT_DECLARATOR\n"); $$ = add_child($1, $2); }
	;

struct_declarator
	: declarator { vlog("[parser] DECLARATOR\n"); $$ = op("struct_declarator", 1, $1); }
	| ':' constant_expression { vlog("[parser] :_CONSTANT\n"); $$ = op("struct_declarator", 1, $2); }
	| declarator ':' constant_expression { vlog("[parser] DECLARATOR_:_CONSTANT\n"); $$ = op("struct_declarator", 2, $1, $3); }
	;

enum_specifier
	: ENUM '{' enumerator_list '}' { vlog("[parser] ENUM_{}\n"); $$ = op("enum_specifier", 1, $3); }
	| ENUM IDENTIFIER '{' enumerator_list '}' { vlog("[parser] ENUM_IDENTIFIER_{}\n"); $$ = op("enum_specifier", 2, $2, $4); }
	| ENUM IDENTIFIER { vlog("[parser] ENUM_IDENTIFIER\n"); $$ = op("enum_specifier", 1, $2); }
	;

enumerator_list
	: enumerator { vlog("[parser] ENUM\n"); $$ = op("enum_list", 1, $1); }
	| enumerator_list ',' enumerator { vlog("[parser] ENUM_LIST_,_ENUM\n"); $$ = add_child($1, $2); }
	;

enumerator
	: IDENTIFIER { vlog("[parser] ENUMERATOR_IDENTIFIER: %s\n", $1->id); $$ = op("enumerator", 1, $1); }
	| IDENTIFIER '=' constant_expression { vlog("[parser] ENUMERATOR_IDENTIFIER_=_CONSTANT\n"); $$ = op("enumerator", 2, $1, $3); }
	;

type_qualifier
	: CONST { vlog("[parser] CONST\n"); $$ = type("const"); }
	| VOLATILE { vlog("[parser] VOLATILE\n"); $$ = type("volatile"); }
	;

declarator
	: pointer direct_declarator { vlog("[parser] POINTER_DIRECT_DECLARATOR\n"); $$ = op("declarator", 2, $1, $2); }
	| direct_declarator { vlog("[parser] DIRECT_DECLARATOR\n"); $$ = op("declarator", 1, $1); }
	;

direct_declarator
	: IDENTIFIER { vlog("[parser] DIRECT_DECLARATOR_IDENTIFIER: %s\n", $1->id); $$ = op("direct_declarator", 1, $1); }
	| '(' declarator ')' { vlog("[parser] (_DECLARATOR_)\n"); $$ = op("direct_declarator", 1, $2); }
	| direct_declarator '[' constant_expression ']' { vlog("[parser] DIRECT_DECLARATOR_[_CONSTANT_]\n"); $$ = add_child($1, $3); }
	| direct_declarator '[' ']' { vlog("[parser] DIRECT_DECLARATOR_[]\n"); }
	| direct_declarator '(' parameter_type_list ')' { vlog("[parser] DIRECT_DECLARATOR_(_PARAM_TYPE_LIST_)\n"); $$ = add_child($1, $3); }
	| direct_declarator '(' identifier_list ')' { vlog("[parser] DIRECT_DECLARATOR_(_IDENTIFIER_LIST_)\n"); $$ = add_child($1, $3); }
	| direct_declarator '(' ')' { vlog("[parser] DIRECT_DECLARATOR_()\n"); }
	;

pointer
	: '*' { vlog("[parser] POINTER_*\n"); $$ = op("pointer", 0); }
	| '*' type_qualifier_list { vlog("[parser] POINTER_*_TYPE_QUALIFIER_LIST\n"); $$ = op("pointer", 1, $2); }
	| '*' pointer { vlog("[parser] POINTER_*_POINTER\n"); $$ = op("pointer", 1, $2); }
	| '*' type_qualifier_list pointer { vlog("[parser] POINTER_*_TYPE_QUALIFIER_LIST_POINTER\n"); $$ = op("pointer", 2, $2, $3); }
	;

type_qualifier_list
	: type_qualifier { vlog("[parser] TYPE_QUALIFIER\n"); $$ = op("type_qualifier_list", 1, $1); }
	| type_qualifier_list type_qualifier { vlog("[parser] TYPE_QUALIFIER_LIST_TYPE_QUALIFIER\n"); $$ = add_child($1, $2); }
	;


parameter_type_list
	: parameter_list { vlog("[parser] PARAMETER_LIST\n"); $$ = op("parameter_type_list", 1, $1); }
	| parameter_list ',' ELLIPSIS { vlog("[parser] PARAMETER_LIST_,_ELLIPSIS\n"); $$ = add_child($1, $3); }
	;

parameter_list
	: parameter_declaration { vlog("[parser] PARAMETER_DECLARATION\n"); $$ = op("parameter_list", 1, $1); }
	| parameter_list ',' parameter_declaration { vlog("[parser] PARAMETER_LIST_,_PARAMETER_DECLARATION\n"); $$ = add_child($1, $3); }
	;

parameter_declaration
	: declaration_specifiers declarator { vlog("[parser] DECLARATION_SPEC_DECLARATOR\n"); $$ = op("parameter_declaration", 2, $1, $2); }
	| declaration_specifiers abstract_declarator { vlog("[parser] DECLARATION_SPEC_ABSTRACT_DECLARATOR\n"); $$ = op("parameter_declaration", 2, $1, $2); }
	| declaration_specifiers { vlog("[parser] DECLARATION_SPEC\n"); $$ = op("parameter_declaration", 1, $1); }
	;

identifier_list
	: IDENTIFIER { vlog("[parser] IDENTIFIER_LIST_IDENTIFIER: %s\n", $1->id); $$ = op("identifier_list", 1, $1); }
	| identifier_list ',' IDENTIFIER { vlog("[parser] IDENTIFIER_LIST_,_IDENTIFIER\n"); $$ = add_child($1, $3); }
	;

type_name
	: specifier_qualifier_list { vlog("[parser] SPECIFIER_QUALIFIER_LIST\n"); $$ = op("type_name", 1, $1); }
	| specifier_qualifier_list abstract_declarator { vlog("[parser] SPECIFIER_QUALIFIER_LIST_ABSTRACT_DECLARATOR\n"); $$ = op("type_name", 2, $1, $2); }
	;

abstract_declarator
	: pointer { vlog("[parser] POINTER\n"); $$ = op("abstract_declarator", 1, $1); }
	| direct_abstract_declarator { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR\n"); $$ = op("abstract_declarator", 1, $1); }
	| pointer direct_abstract_declarator { vlog("[parser] POINTER_DIRECT_ABSTRACT_DECLARATOR\n"); $$ = op("abstract_declarator", 2, $1, $2); }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')' { vlog("[parser] (_ABSTRACT_DECLARATOR_)\n"); $$ = op("direct_abstract_declarator", 1, $2); }
	| '[' ']' { vlog("[parser] []\n"); $$ = op("direct_abstract_declarator", 0); }
	| '[' constant_expression ']' { vlog("[parser] [_CONSTANT_]\n"); $$ = op("direct_abstract_declarator", 1, $2); }
	| direct_abstract_declarator '[' ']' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_[]\n"); /* do nothing */}
	| direct_abstract_declarator '[' constant_expression ']' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_[_CONSTANT_]\n"); $$ = add_child($1, $3); }
	| '(' ')' { vlog("[parser] ()\n"); $$ = op("direct_abstract_declarator", 0); }
	| '(' parameter_type_list ')' { vlog("[parser] (_PARAM_TYPE_LIST_)\n"); $$ = op("direct_abstract_declarator", 1, $2); }
	| direct_abstract_declarator '(' ')' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_()\n"); /* do nothing */}
	| direct_abstract_declarator '(' parameter_type_list ')' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_(_PARAM_TYPE_LIST_)\n"); $$ = add_child($1, $3); }
	;

initializer
	: assignment_expression { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); $$ = op("initializer", 1, $1); }
	| '{' initializer_list '}' { vlog("[parser] {_INIT_LIST_}\n"); $$ = op("initializer", 1, $2); }
	| '{' initializer_list ',' '}' { vlog("[parser] {_INIT_LIST_,_}\n"); $$ = op("initializer", 1, $2); }
	;

initializer_list
	: initializer { vlog("[parser] INITIALIZER\n"); $$ = op("initializer_list", 1, $1); }
	| initializer_list ',' initializer { vlog("[parser] INITIALIZER_LIST_,_INITIALIZER\n"); $$ = add_child($1, $3); }
	;

statement
	: labeled_statement { vlog("[parser] LABELED_STATEMENT\n"); }
	| compound_statement { vlog("[parser] COMPOUND_STATEMENT\n"); }
	| expression_statement { vlog("[parser] EXPRESSION_STATEMENT\n"); }
	| selection_statement { vlog("[parser] SELECTION_STATEMENT\n"); }
	| iteration_statement { vlog("[parser] ITERATION_STATEMENT\n"); }
	| jump_statement { vlog("[parser] JUMP_STATEMENT\n"); }
	;

labeled_statement
	: IDENTIFIER ':' statement { vlog("[parser] IDENTIFIER_:_STATEMENT\n"); $$ = op("labeled_statement", 2, $1, $3); }
	| CASE constant_expression ':' statement { vlog("[parser] CASE_CONSTANT_:_STATEMENT\n"); $$ = op("labeled_statement", 2, $2, $4); }
	| DEFAULT ':' statement { vlog("[parser] DEFAULT_:_STATEMENT\n"); $$ = op("labeled_statement", 1, $3); }
	;

compound_statement
	: '{' '}' { vlog("[parser] {}\n"); $$ = op("compound_statement", 0); }
	| '{' statement_list '}' { vlog("[parser] {_STATEMENT_LIST_}\n"); $$ = op("compound_statement", 1, $2); }
	| '{' declaration_list '}' { vlog("[parser] {_DECLARATION_LIST_}\n"); $$ = op("compound_statement", 1, $2); }
	| '{' declaration_list statement_list '}' { vlog("[parser] {_DECLARATION_LIST_STATEMENT_LIST_}\n"); $$ = op("compound_statement", 2, $2, $3); }
	;

declaration_list
	: declaration { vlog("[parser] DECLARATION\n"); $$ = op("declaration_list", 1, $1); }
	| declaration_list declaration { vlog("[parser] DECLARATION_LIST_DECLARATION\n"); $$ = add_child($1, $2); }
	;

statement_list
	: statement { vlog("[parser] STATEMENT\n"); $$ = op("statement_list", 1, $1); }
	| statement_list statement { vlog("[parser] STATEMENT_LIST_STATEMENT\n"); $$ = add_child($1, $2); }
	;

expression_statement
	: ';' { vlog("[parser] ;\n"); $$ = op("expression_statement", 0); }
	| expression ';' { vlog("[parser] EXPRESSION_;\n"); $$ = op("expression_statement", 1, $1); }
	;

selection_statement
	: IF '(' expression ')' statement %prec IFX { vlog("[parser] IF_(_EXPRESSION_)_STATEMENT\n"); $$ = op("selection_statement", 2, $3, $5); }
	| IF '(' expression ')' statement ELSE statement { vlog("[parser] IF_(_EXPRESSION_)_STATEMENT_ELSE_STATEMENT\n"); $$ = op("selection_statement", 3, $3, $5, $7); }
	| SWITCH '(' expression ')' statement { vlog("[parser] SWITCH_(_EXPRESSION_)_STATEMENT\n"); $$ = op("selection_statement", 2, $3, $5); }
	;

iteration_statement
	: WHILE '(' expression ')' statement { vlog("[parser] WHILE_(_EXPRESSION_)_STATEMENT\n"); $$ = op("iteration_statement", 2, $3, $5); }
	| DO statement WHILE '(' expression ')' ';' { vlog("[parser] DO_STATEMENT_WHILE_(_EXPRESSION_)_;\n"); $$ = op("iteration_statement", 2, $2, $5); }
	| FOR '(' expression_statement expression_statement ')' statement { vlog("[parser] FOR_(_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_)_STATEMENT\n"); $$ = op("iteration_statement", 3, $3, $4, $6); }
	| FOR '(' expression_statement expression_statement expression ')' statement { vlog("[parser] FOR_(_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_EXPRESSION_)_STATEMENT\n"); $$ = op("iteration_statement", 4, $3, $4, $5, $7); }
	;

jump_statement
	: GOTO IDENTIFIER ';' { vlog("[parser] GOTO_IDENTIFIER\n"); $$ = op("jump_statement", 2, id("goto"), $2); }
	| CONTINUE ';' { vlog("[parser] CONTINUE\n"); $$ = op("jump_statement", 1, id("continue")); }
	| BREAK ';' { vlog("[parser] BREAK\n"); $$ = op("jump_statement", 1, id("break")); }
	| RETURN ';' { vlog("[parser] RETURN\n"); $$ = op("jump_statement", 1, id("return")); }
	| RETURN expression ';' { vlog("[parser] RETURN_EXPRESSION\n"); $$ = op("jump_statement", 2, id("return"), $2); }
	;

translation_unit
	: external_declaration { vlog("[parser] EXTERNAL_DECLARATION\n"); $$ = op("translation_unit", 1, $1); parse_tree_head = $$; }
	| translation_unit external_declaration { vlog("[parser] TRANSLATION_UNIT_EXTERNAL_DECLARATION\n"); $$ = add_child($1, $2); parse_tree_head = $$; }
	;

external_declaration
	: function_definition { vlog("[parser] FUNCTION_DEFINITION\n"); }
	| declaration { vlog("[parser] DECLARATION\n"); }
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement { vlog("[parser] DECLARATION_SPECIFIERS_DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT\n"); $$ = op("function_definition", 4, $1, $2, $3, $4); $$ = install_function(sym, $$); }
	| declaration_specifiers declarator compound_statement { vlog("[parser] DECLARATION_SPECIFIERS_DECLARATOR_COMPOUND_STATEMENT\n"); $$ = op("function_definition", 3, $1, $2, $3); $$ = install_function(sym, $$); }
	| declarator declaration_list compound_statement { vlog("[parser] DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT\n"); $$ = op("function_definition", 3, $1, $2, $3); $$ = install_function(sym, $$); }
	| declarator compound_statement { vlog("[parser] DECLARATOR_COMPOUND_STATEMENT\n"); $$ = op("function_definition", 2, $1, $2); $$ = install_function(sym, $$); }
	;

%%

