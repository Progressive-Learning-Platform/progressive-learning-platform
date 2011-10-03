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
	: IDENTIFIER { vlog("[parser] IDENTIFIER: %s\n", $1->id); }
	| CONSTANT { vlog("[parser] CONSTANT: %s\n", $1->id); } 
	| STRING_LITERAL { vlog("[parser] STRING_LITERAL: %s\n", $1->id); }
	| '(' expression ')' { vlog("[parser] EXPRESSION"); }
	;

postfix_expression
	: primary_expression /*  { vlog("[parser] PRIMARY_EXPRESSION\n"); } */
	| postfix_expression '[' expression ']' { vlog("[parser] POSTFIX_BRACKET_EXPRESSION\n"); }
	| postfix_expression '(' ')' { vlog("[parser] POSTFIX_PAREN_EXPRESSION\n"); }
	| postfix_expression '(' argument_expression_list ')' { vlog("[parser] POSTFIX_ARG_EXPRESSION_LIST\n"); }
	| postfix_expression '.' IDENTIFIER { vlog("[parser] POSTFIX_._IDENTIFIER\n"); }
	| postfix_expression PTR_OP IDENTIFIER { vlog("[parser] POSTFIX_PTR_OP_IDENTIFIER\n"); }
	| postfix_expression INC_OP { vlog("[parser] POSTFIX_INC_OP_EXPRESSION\n"); }
	| postfix_expression DEC_OP { vlog("[parser] POSTFIX_DEC_OP_EXPRESSION\n"); }
	;

argument_expression_list
	: assignment_expression /* { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); } */
	| argument_expression_list ',' assignment_expression { vlog("[parser] ARG_EXPRESSION_LIST_,_ASSIGNMENT_EXPRESSION\n"); }
	;

unary_expression
	: postfix_expression /* { vlog("[parser] POSTFIX_EXPRESSION\n"); } */
	| INC_OP unary_expression { vlog("[parser] INC_OP_UNARY_EXPRESSION\n"); }
	| DEC_OP unary_expression { vlog("[parser] DEC_OP_UNARY_EXPRESSION\n"); }
	| unary_operator cast_expression { vlog("[parser] UNARY_OP_CAST_EXPRESSION\n"); }
	| SIZEOF unary_expression { vlog("[parser] SIZEOF_UNARY_EXP\n"); }
	| SIZEOF '(' type_name ')' { vlog("[parser] SIZEOF_TYPE_NAME\n"); }
	;

unary_operator
	: '&' { vlog("[parser] &\n"); }
	| '*' { vlog("[parser] *\n"); }
	| '+' { vlog("[parser] +\n"); }
	| '-' { vlog("[parser] -\n"); }
	| '~' { vlog("[parser] ~\n"); }
	| '!' { vlog("[parser] !\n"); }
	;

cast_expression
	: unary_expression /* { vlog("[parser] UNARY_EXPRESSION\n"); } */
	| '(' type_name ')' cast_expression { vlog("[parser] TYPE_NAME_CAST_EXPRESSION\n"); }
	;

multiplicative_expression
	: cast_expression /* { vlog("[parser] CAST_EXPRESSION\n"); } */
	| multiplicative_expression '*' cast_expression { vlog("[parser] MULTIPLICATIVE_EXPRESSION_*_CAST_EXPRESSION\n"); }
	| multiplicative_expression '/' cast_expression { vlog("[parser] MULTIPLICATIVE_EXPRESSION_/_CAST_EXPRESSION\n"); }
	| multiplicative_expression '%' cast_expression { vlog("[parser] MULTIPLICATIVE_EXPRESSION_MOD_CAST_EXPRESSION\n"); }
	;

additive_expression
	: multiplicative_expression /* { vlog("[parser] MULTIPLICATIVE_EXPRESSION\n"); } */
	| additive_expression '+' multiplicative_expression { vlog("[parser] ADDITIVE_EXPRESSION_+_MULTIPLICATIVE_EXPRESSION\n"); }
	| additive_expression '-' multiplicative_expression { vlog("[parser] ADDITIVE_EXPRESSION_-_MULTIPLICATIVE_EXPRESSION\n"); }
	;

shift_expression
	: additive_expression /*  { vlog("[parser] ADDITIVE_EXPRESSION\n"); } */
	| shift_expression LEFT_OP additive_expression { vlog("[parser] SHIFT_LEFT_ADDITIVE\n"); }
	| shift_expression RIGHT_OP additive_expression { vlog("[parser] SHIFT_RIGHT_ADDITIVE\n"); }
	;

relational_expression
	: shift_expression /* { vlog("[parser] SHIFT_EXPRESSION\n"); } */
	| relational_expression '<' shift_expression { vlog("[parser] RELATIONAL_<_SHIFT\n"); }
	| relational_expression '>' shift_expression { vlog("[parser] RELATIONAL_>_SIHFT\n"); }
	| relational_expression LE_OP shift_expression { vlog("[parser] RELATIONAL_LE_SHIFT\n"); }
	| relational_expression GE_OP shift_expression { vlog("[parser] RELATIONAL_GE_SHIFT\n"); }
	;

equality_expression
	: relational_expression /* { vlog("[parser] RELATIONAL_EXPRESSION\n"); } */
	| equality_expression EQ_OP relational_expression { vlog("[parser] EQUALITY_EQ_RELATIONAL\n"); }
	| equality_expression NE_OP relational_expression { vlog("[parser] EQUALITY_NE_RELATIONAL\n"); }
	;

and_expression
	: equality_expression /* { vlog("[parser] EQUALITY_EXPRESSION\n"); } */
	| and_expression '&' equality_expression { vlog("[parser] AND_EXPRESSION_&_EQUALITY\n"); }
	;

exclusive_or_expression
	: and_expression /* { vlog("[parser] AND_EXPRESSION\n"); } */
	| exclusive_or_expression '^' and_expression { vlog("[parser] XOR_^_AND_EXPRESSION\n"); }
	;

inclusive_or_expression
	: exclusive_or_expression /* { vlog("[parser] XOR_EXPRESSION\n"); } */
	| inclusive_or_expression '|' exclusive_or_expression { vlog("[parser] OR_|_XOR\n"); }
	;

logical_and_expression
	: inclusive_or_expression /* { vlog("[parser] OR_EXPRESSION\n"); } */
	| logical_and_expression AND_OP inclusive_or_expression { vlog("[parser] LOGICAL_AND_&&_OR\n"); }
	;

logical_or_expression
	: logical_and_expression /* { vlog("[parser] LOGICAL_AND\n"); } */
	| logical_or_expression OR_OP logical_and_expression { vlog("[parser] LOGICAL_OR_||_LOGICAL_AND\n"); }
	;

conditional_expression
	: logical_or_expression /* { vlog("[parser] LOGICAL_OR\n"); } */
	| logical_or_expression '?' expression ':' conditional_expression { vlog("[parser] LOGICAL_OR_?\n"); }
	;

assignment_expression
	: conditional_expression /* { vlog("[parser] CONDITIONAL_EXPRESSION\n"); } */
	| unary_expression assignment_operator assignment_expression { vlog("[parser] UNARY_ASSIGNMENT\n"); }
	;

assignment_operator
	: '=' { vlog("[parser] =\n"); }
	| MUL_ASSIGN { vlog("[parser] *=\n"); }
	| DIV_ASSIGN { vlog("[parser] /=\n"); }
	| MOD_ASSIGN { vlog("[parser] MOD=\n"); }
	| ADD_ASSIGN { vlog("[parser] +=\n"); }
	| SUB_ASSIGN { vlog("[parser] -=\n"); }
	| LEFT_ASSIGN { vlog("[parser] <<=\n"); }
	| RIGHT_ASSIGN { vlog("[parser] >>=\n"); }
	| AND_ASSIGN { vlog("[parser] &=\n"); }
	| XOR_ASSIGN { vlog("[parser] ^=\n"); }
	| OR_ASSIGN { vlog("[parser] |=\n"); }
	;

expression
	: assignment_expression /* { vlog("[parser] ASSIGNMENT_EXPRESSION\n"); } */
	| expression ',' assignment_expression { vlog("[parser] EXPRESSION_,_ASSIGNMENT_EXPRESSION\n"); }
	;

constant_expression
	: conditional_expression /* { vlog("[parser] CONDITIONAL_EXPRESSION\n"); } */
	;

declaration
	: declaration_specifiers ';' { vlog("[parser] DECLARATION_SPECIFIERS_;\n"); }
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
	| declarator '=' initializer { vlog("[parser] DECLARATOR_=_INITIALIZER\n"); }
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
	: pointer direct_declarator { vlog("[parser] POINTER_DIRECT_DECLARATOR\n"); }
	| direct_declarator { vlog("[parser] DIRECT_DECLARATOR\n"); }
	;

direct_declarator
	: IDENTIFIER { vlog("[parser] DIRECT_DECLARATOR_IDENTIFIER: %s\n", $1->id); } //$$ = op("declarator", $1, $2); }
	| '(' declarator ')' { vlog("[parser] (_DECLARATOR_)\n"); }
	| direct_declarator '[' constant_expression ']' { vlog("[parser] DIRECT_DECLARATOR_[_CONSTANT_]\n"); }
	| direct_declarator '[' ']' { vlog("[parser] DIRECT_DECLARATOR_[]\n"); }
	| direct_declarator '(' parameter_type_list ')' { vlog("[parser] DIRECT_DECLARATOR_(_PARAM_TYPE_LIST_)\n"); }
	| direct_declarator '(' identifier_list ')' { vlog("[parser] DIRECT_DECLARATOR_(_IDENTIFIER_LIST_)\n"); }
	| direct_declarator '(' ')' { vlog("[parser] DIRECT_DECLARATOR_()\n"); }
	;

pointer
	: '*' { vlog("[parser] POINTER_*\n"); }
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
	: parameter_declaration { vlog("[parser] PARAMETER_DECLARATION\n"); }
	| parameter_list ',' parameter_declaration { vlog("[parser] PARAMETER_LIST_,_PARAMETER_DECLARATION\n"); }
	;

parameter_declaration
	: declaration_specifiers declarator { vlog("[parser] DECLARATION_SPEC_DECLARATOR\n"); }
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
	| '{' declaration_list statement_list '}' { vlog("[parser] {_DECLARATION_LIST_STATEMENT_LIST_}\n"); }
	;

declaration_list
	: declaration { vlog("[parser] DECLARATION\n"); }
	| declaration_list declaration { vlog("[parser] DECLARATION_LIST_DECLARATION\n"); }
	;

statement_list
	: statement { vlog("[parser] STATEMENT\n"); }
	| statement_list statement { vlog("[parser] STATEMENT_LIST_STATEMENT\n"); }
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
	| RETURN expression ';' { vlog("[parser] RETURN_EXPRESSION\n"); }
	;

translation_unit
	: external_declaration { vlog("[parser] EXTERNAL_DECLARATION\n"); $$ = $1; parse_tree_head = $$; }
	| translation_unit external_declaration { vlog("[parser] TRANSLATION_UNIT_EXTERNAL_DECLARATION\n"); $$ = op("translation_unit", 2, $1, $2); parse_tree_head = $$; }
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

