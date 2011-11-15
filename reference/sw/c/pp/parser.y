%{
#include <stdio.h>
#include "log.h"

#define YYSTYPE char*

extern int column, line;
extern int yylex (void);
extern void handle_include(char* s);
extern char* program;
extern char* emit(char*, char*);

void yyerror(s)
char *s;
{
	err("[plppp] syntax error: %s\n", s); 
}

%}

%token COMMENT INCLUDE INC_STRING INC_BRACKET

%start program
%%

element
	: COMMENT { vlog("[pp parser] found comment\n"); program = emit(program, " "); } /* replace all comments with a space */
	| INCLUDE INC_STRING { vlog("[pp parser] include: %s\n", $2); handle_include($2); }
	| INCLUDE INC_BRACKET { vlog("[pp parser] include: %s\n", $2); handle_include($2); }
	| INC_STRING { vlog("[pp parser] string without inlude: %s\n", $1); program = emit(program, $1); }
	| INC_BRACKET { vlog("[pp parser] bracket without inlude: %s\n", $1); program = emit(program, $1); }
	;

program
	: element
	| program element
	;

%%

