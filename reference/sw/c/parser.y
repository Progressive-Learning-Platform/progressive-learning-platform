%{
#include <stdio.h>
#include "log.h"
#include "parse_tree.h"
#include "symbol.h"

#define YYSTYPE node *

extern char yytext[];
extern int column;
extern symbol_table *sym;
extern node *parse_tree_head;

extern int yylex (void);

void yyerror(s)
char *s;
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}

%}

/*
%union {
	char *val;
	struct node *n;
};
*/
%defines 

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

%nonassoc IFX
%nonassoc ELSE

/*%type <n> translation_unit external_declaration*/

%start translation_unit
%%

primary_expression
	: IDENTIFIER { vlog("[parser] IDENTIFIER: %s\n", $1->id); /* the lexer already made this object a constant */ }
	| CONSTANT { vlog("[parser] CONSTANT: %s\n", $1->id); /* the lexer already made this object a constant */ } 
	| STRING_LITERAL { vlog("[parser] STRING_LITERAL: %s\n", $1->id); /* the lexer already made this object a string */ }
	| '(' expression ')' { vlog("[parser] EXPRESSION"); $$ = op("expression", 1, $2); }
	;

postfix_expression
	: primary_expression 
	| postfix_expression '[' expression ']' { vlog("[parser] POSTFIX_BRACKET_EXPRESSION\n"); $$ = op("postfix_expr", 2, $1, $3); }
	| postfix_expression '(' ')' { vlog("[parser] POSTFIX_PAREN_EXPRESSION\n"); $$ = op("postfix_expr_paren", 1, $1); }
	| postfix_expression '(' argument_expression_list ')' { vlog("[parser] POSTFIX_ARG_EXPRESSION_LIST\n"); $$ = op("postfix_expr_paren", 2, $1, $3); }
	| postfix_expression '.' IDENTIFIER { vlog("[parser] POSTFIX_._IDENTIFIER\n"); $$ = op("postfix_expr_dot", 2, $1, $3); }
	| postfix_expression PTR_OP IDENTIFIER { vlog("[parser] POSTFIX_PTR_OP_IDENTIFIER\n"); $$ = op("postfix_expr_arrow", 2, $1, $3); }
	| postfix_expression INC_OP { vlog("[parser] POSTFIX_INC_OP_EXPRESSION\n"); $$ = op("postfix_expr_inc", 1, $1); }
	| postfix_expression DEC_OP { vlog("[parser] POSTFIX_DEC_OP_EXPRESSION\n"); $$ = op("postfix_expr_dec", 1, $1); }
	;

argument_expression_list
	: assignment_expression { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); $$ = op("argument_expr_list", 1, $1); }
	| argument_expression_list ',' assignment_expression { vlog("[parser] ARG_EXPRESSION_LIST_,_ASSIGNMENT_EXPRESSION\n"); $$ = add_child($1, $3); }
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression { vlog("[parser] INC_OP_UNARY_EXPRESSION\n"); $$ = op("unary_expr_inc", 1, $2); }
	| DEC_OP unary_expression { vlog("[parser] DEC_OP_UNARY_EXPRESSION\n"); $$ = op("unary_expr_dec", 1, $2); }
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
	: '=' { vlog("[parser] =\n"); $$ = op("=", 0);}
	| MUL_ASSIGN { vlog("[parser] *=\n"); $$ = op("*=", 0); }
	| DIV_ASSIGN { vlog("[parser] /=\n"); $$ = op("/=", 0); }
	| MOD_ASSIGN { vlog("[parser] MOD=\n"); $$ = op("MOD=", 0); }
	| ADD_ASSIGN { vlog("[parser] +=\n"); $$ = op("+=", 0); }
	| SUB_ASSIGN { vlog("[parser] -=\n"); $$ = op("-=", 0); }
	| LEFT_ASSIGN { vlog("[parser] <<=\n"); $$ = op("<<=", 0); }
	| RIGHT_ASSIGN { vlog("[parser] >>=\n"); $$ = op(">>=", 0); }
	| AND_ASSIGN { vlog("[parser] &=\n"); $$ = op("&=", 0); }
	| XOR_ASSIGN { vlog("[parser] ^=\n"); $$ = op("^=", 0); }
	| OR_ASSIGN { vlog("[parser] |=\n"); $$ = op("|=", 0); }
	;

expression
	: assignment_expression { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); $$ = op("expression", 1, $1); }
	| expression ',' assignment_expression { vlog("[parser] EXPRESSION_,_ASSIGNMENT_EXPRESSION\n"); $$ = add_child($1, $3); }
	;

constant_expression
	: conditional_expression /* why is this here!? */
	;

declaration
	: declaration_specifiers ';' { vlog("[parser] DECLARATION_SPECIFIERS_;\n"); $$ = op("declarator", 1, $1); }
	| declaration_specifiers init_declarator_list ';' { vlog("[parser] DECLARATION_SPECIFIERS_INIT_DECLARATOR_LIST_;\n"); $$ = op("declarator", 2, $1, $2); }
	;

declaration_specifiers
	: storage_class_specifier { vlog("[parser] STORAGE_CLASS_SPECIFIER\n"); } 
	| storage_class_specifier declaration_specifiers { vlog("[parser] STORAGE_CLASS_SPECIFIER_DECLARATION_SPECIFIERS\n"); } 
	| type_specifier { vlog("[parser] DECLARATION_SPECIFIER_TYPE_SPECIFIER\n"); } 
	| type_specifier declaration_specifiers { vlog("[parser] TYPE_SPECIFIER_DECLARATION_SPECIFIERS\n"); } 
	| type_qualifier { vlog("[parser] TYPE_QUALIFIER\n"); } 
	| type_qualifier declaration_specifiers { vlog("[parser] TYPE_QUALIFIER_DECLARATION_SPECIFIERS\n"); } 
	;

init_declarator_list
	: init_declarator { vlog("[parser] INIT_DECLARATOR\n"); } 
	| init_declarator_list ',' init_declarator { vlog("[parser] INIT_DECLARATOR_LIST_,_INIT_DECLARATOR\n"); } 
	;

init_declarator
	: declarator { vlog("[parser] DECLARATOR\n"); } 
	| declarator '=' initializer { vlog("[parser] DECLARATOR_=_INITIALIZER\n"); $$ = op("initializer", 2, $1, $3); }
	;

storage_class_specifier
	: TYPEDEF { vlog("[parser] TYPEDEF\n"); }
	| EXTERN { vlog("[parser] EXTERN\n"); }
	| STATIC { vlog("[parser] STATIC\n"); }
	| AUTO { vlog("[parser] AUTO\n"); }
	| REGISTER { vlog("[parser] REGISTER\n"); }
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
	| struct_or_union_specifier { vlog("[parser] STRUCT_UNION\n"); }
	| enum_specifier { vlog("[parser] ENUM\n"); }
	| TYPE_NAME { vlog("[parser] TYPE_NAME\n"); }
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}' { vlog("[parser] STRUCT_IDENTIFIER_{}\n"); }
	| struct_or_union '{' struct_declaration_list '}' { vlog("[parser] STRUCT_{}\n"); }
	| struct_or_union IDENTIFIER { vlog("[parser] STRUCT_IDENTIFIER\n"); }
	;

struct_or_union
	: STRUCT { vlog("[parser] STRUCT\n"); }
	| UNION { vlog("[parser] UNION\n"); }
	;

struct_declaration_list
	: struct_declaration { vlog("[parser] STRUCT_DECLARATION\n"); } 
	| struct_declaration_list struct_declaration  { vlog("[parser] STRUCT_DECLARATION_LIST\n"); } 
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'  { vlog("[parser] SPECIFIER_QUALIFIER_LIST_STRUCT_DECLARATOR_LIST\n"); } 
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list { vlog("[parser] TYPE_SPECIFIER_SPEC_QUALIFIER_LIST\n"); }
	| type_specifier { vlog("[parser] SPECIFIER_QUALIFIER_LIST_TYPE_SPECIFIER\n"); }
	| type_qualifier specifier_qualifier_list { vlog("[parser] TYPE_QUALIFIER_SPEC_QUALIFIER_LIST\n"); }
	| type_qualifier { vlog("[parser] TYPE_QUALIFIER\n"); }
	;

struct_declarator_list
	: struct_declarator { vlog("[parser] STRUCT_DECLARATOR\n"); }
	| struct_declarator_list ',' struct_declarator { vlog("[parser] STRUCT_DECLARATOR_LIST_,_STRUCT_DECLARATOR\n"); }
	;

struct_declarator
	: declarator { vlog("[parser] DECLARATOR\n"); }
	| ':' constant_expression { vlog("[parser] :_CONSTANT\n"); }
	| declarator ':' constant_expression { vlog("[parser] DECLARATOR_:_CONSTANT\n"); }
	;

enum_specifier
	: ENUM '{' enumerator_list '}' { vlog("[parser] ENUM_{}\n"); }
	| ENUM IDENTIFIER '{' enumerator_list '}' { vlog("[parser] ENUM_IDENTIFIER_{}\n"); }
	| ENUM IDENTIFIER { vlog("[parser] ENUM_IDENTIFIER\n"); }
	;

enumerator_list
	: enumerator { vlog("[parser] ENUM\n"); }
	| enumerator_list ',' enumerator { vlog("[parser] ENUM_LIST_,_ENUM\n"); }
	;

enumerator
	: IDENTIFIER { vlog("[parser] ENUMERATOR_IDENTIFIER: %s\n", $1->id); }
	| IDENTIFIER '=' constant_expression { vlog("[parser] ENUMERATOR_IDENTIFIER_=_CONSTANT\n"); }
	;

type_qualifier
	: CONST { vlog("[parser] CONST\n"); }
	| VOLATILE { vlog("[parser] VOLATILE\n"); }
	;

declarator
	: pointer direct_declarator { vlog("[parser] POINTER_DIRECT_DECLARATOR\n"); $$ = op("pointer_declarator", 2, $1, $2); }
	| direct_declarator { vlog("[parser] DIRECT_DECLARATOR\n"); }
	;

direct_declarator
	: IDENTIFIER { vlog("[parser] DIRECT_DECLARATOR_IDENTIFIER: %s\n", $1->id); }
	| '(' declarator ')' { vlog("[parser] (_DECLARATOR_)\n"); }
	| direct_declarator '[' constant_expression ']' { vlog("[parser] DIRECT_DECLARATOR_[_CONSTANT_]\n"); }
	| direct_declarator '[' ']' { vlog("[parser] DIRECT_DECLARATOR_[]\n"); }
	| direct_declarator '(' parameter_type_list ')' { vlog("[parser] DIRECT_DECLARATOR_(_PARAM_TYPE_LIST_)\n"); $$ = op("declarator", 2, $1, $3); }
	| direct_declarator '(' identifier_list ')' { vlog("[parser] DIRECT_DECLARATOR_(_IDENTIFIER_LIST_)\n"); }
	| direct_declarator '(' ')' { vlog("[parser] DIRECT_DECLARATOR_()\n"); }
	;

pointer
	: '*' { vlog("[parser] POINTER_*\n"); $$ = op("pointer", 0); }
	| '*' type_qualifier_list { vlog("[parser] POINTER_*_TYPE_QUALIFIER_LIST\n"); }
	| '*' pointer { vlog("[parser] POINTER_*_POINTER\n"); }
	| '*' type_qualifier_list pointer { vlog("[parser] POINTER_*_TYPE_QUALIFIER_LIST_POINTER\n"); }
	;

type_qualifier_list
	: type_qualifier { vlog("[parser] TYPE_QUALIFIER\n"); }
	| type_qualifier_list type_qualifier { vlog("[parser] TYPE_QUALIFIER_LIST_TYPE_QUALIFIER\n"); }
	;


parameter_type_list
	: parameter_list { vlog("[parser] PARAMETER_LIST\n"); }
	| parameter_list ',' ELLIPSIS { vlog("[parser] PARAMETER_LIST_,_ELLIPSIS\n"); }
	;

parameter_list
	: parameter_declaration { vlog("[parser] PARAMETER_DECLARATION\n"); $$ = op("parameter_list", 1, $1); }
	| parameter_list ',' parameter_declaration { vlog("[parser] PARAMETER_LIST_,_PARAMETER_DECLARATION\n"); }
	;

parameter_declaration
	: declaration_specifiers declarator { vlog("[parser] DECLARATION_SPEC_DECLARATOR\n"); $$ = op("declaration", 2, $1, $2); }
	| declaration_specifiers abstract_declarator { vlog("[parser] DECLARATION_SPEC_ABSTRACT_DECLARATOR\n"); }
	| declaration_specifiers { vlog("[parser] DECLARATION_SPEC\n"); }
	;

identifier_list
	: IDENTIFIER { vlog("[parser] IDENTIFIER_LIST_IDENTIFIER: %s\n", $1->id); }
	| identifier_list ',' IDENTIFIER { vlog("[parser] IDENTIFIER_LIST_,_IDENTIFIER\n"); }
	;

type_name
	: specifier_qualifier_list { vlog("[parser] SPECIFIER_QUALIFIER_LIST\n"); }
	| specifier_qualifier_list abstract_declarator { vlog("[parser] SPECIFIER_QUALIFIER_LIST_ABSTRACT_DECLARATOR\n"); }
	;

abstract_declarator
	: pointer { vlog("[parser] POINTER\n"); }
	| direct_abstract_declarator { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR\n"); }
	| pointer direct_abstract_declarator { vlog("[parser] POINTER_DIRECT_ABSTRACT_DECLARATOR\n"); }
	;

direct_abstract_declarator
	: '(' abstract_declarator ')' { vlog("[parser] (_ABSTRACT_DECLARATOR_)\n"); }
	| '[' ']' { vlog("[parser] []\n"); }
	| '[' constant_expression ']' { vlog("[parser] [_CONSTANT_]\n"); }
	| direct_abstract_declarator '[' ']' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_[]\n"); }
	| direct_abstract_declarator '[' constant_expression ']' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_[_CONSTANT_]\n"); }
	| '(' ')' { vlog("[parser] ()\n"); }
	| '(' parameter_type_list ')' { vlog("[parser] (_PARAM_TYPE_LIST_)\n"); }
	| direct_abstract_declarator '(' ')' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_()\n"); }
	| direct_abstract_declarator '(' parameter_type_list ')' { vlog("[parser] DIRECT_ABSTRACT_DECLARATOR_(_PARAM_TYPE_LIST_)\n"); }
	;

initializer
	: assignment_expression { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); }
	| '{' initializer_list '}' { vlog("[parser] {_INIT_LIST_}\n"); }
	| '{' initializer_list ',' '}' { vlog("[parser] {_INIT_LIST_,_}\n"); }
	;

initializer_list
	: initializer { vlog("[parser] INITIALIZER\n"); }
	| initializer_list ',' initializer { vlog("[parser] INITIALIZER_LIST_,_INITIALIZER\n"); }
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
	: IDENTIFIER ':' statement { vlog("[parser] IDENTIFIER_:_STATEMENT\n"); }
	| CASE constant_expression ':' statement { vlog("[parser] CASE_CONSTANT_:_STATEMENT\n"); }
	| DEFAULT ':' statement { vlog("[parser] DEFAULT_:_STATEMENT\n"); }
	;

compound_statement
	: '{' '}' { vlog("[parser] {}\n"); }
	| '{' statement_list '}' { vlog("[parser] {_STATEMENT_LIST_}\n"); }
	| '{' declaration_list '}' { vlog("[parser] {_DECLARATION_LIST_}\n"); }
	| '{' declaration_list statement_list '}' { vlog("[parser] {_DECLARATION_LIST_STATEMENT_LIST_}\n"); $$ = op("compound", 2, $2, $3); }
	;

declaration_list
	: declaration { vlog("[parser] DECLARATION\n"); $$ = op("declaration_list", 1, $1); }
	| declaration_list declaration { vlog("[parser] DECLARATION_LIST_DECLARATION\n"); $$ = add_child($1, $2); }
	;

statement_list
	: statement { vlog("[parser] STATEMENT\n"); $$ = op("statement", 1, $1); }
	| statement_list statement { vlog("[parser] STATEMENT_LIST_STATEMENT\n"); $$ = add_child($1, $2); }
	;

expression_statement
	: ';' { vlog("[parser] ;\n"); }
	| expression ';' { vlog("[parser] EXPRESSION_;\n"); }
	;

selection_statement
	: IF '(' expression ')' statement %prec IFX { vlog("[parser] IF_(_EXPRESSION_)_STATEMENT\n"); }
	| IF '(' expression ')' statement ELSE statement { vlog("[parser] IF_(_EXPRESSION_)_STATEMENT_ELSE_STATEMENT\n"); }
	| SWITCH '(' expression ')' statement { vlog("[parser] SWITCH_(_EXPRESSION_)_STATEMENT\n"); }
	;

iteration_statement
	: WHILE '(' expression ')' statement { vlog("[parser] WHILE_(_EXPRESSION_)_STATEMENT\n"); }
	| DO statement WHILE '(' expression ')' ';' { vlog("[parser] DO_STATEMENT_WHILE_(_EXPRESSION_)_;\n"); }
	| FOR '(' expression_statement expression_statement ')' statement { vlog("[parser] FOR_(_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_)_STATEMENT\n"); }
	| FOR '(' expression_statement expression_statement expression ')' statement { vlog("[parser] FOR_(_EXPRESSION_STATEMENT_EXPRESSION_STATEMENT_EXPRESSION_)_STATEMENT\n"); }
	;

jump_statement
	: GOTO IDENTIFIER ';' { vlog("[parser] GOTO_IDENTIFIER\n"); }
	| CONTINUE ';' { vlog("[parser] CONTINUE\n"); }
	| BREAK ';' { vlog("[parser] BREAK\n"); }
	| RETURN ';' { vlog("[parser] RETURN\n"); }
	| RETURN expression ';' { vlog("[parser] RETURN_EXPRESSION\n"); $$ = op("return", 1, $2); }
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
	: declaration_specifiers declarator declaration_list compound_statement { vlog("[parser] DECLARATION_SPECIFIERS_DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT\n"); $$ = op("function_definition", 4, $1, $2, $3, $4); }
	| declaration_specifiers declarator compound_statement { vlog("[parser] DECLARATION_SPECIFIERS_DECLARATOR_COMPOUND_STATEMENT\n"); $$ = op("function_definition", 3, $1, $2, $3); }
	| declarator declaration_list compound_statement { vlog("[parser] DECLARATOR_DECLARATION_LIST_COMPOUND_STATEMENT\n"); $$ = op("function_definition", 3, $1, $2, $3); }
	| declarator compound_statement { vlog("[parser] DECLARATOR_COMPOUND_STATEMENT\n"); }
	;

%%

