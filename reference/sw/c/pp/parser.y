%{
#include <stdio.h>
#include "log.h"

extern int column, line;
extern int yylex (void);
extern char* program;
extern char* emit(char*, char*);

void yyerror(s)
char *s;
{
	err("[plppp] syntax error: %s\n", s); 
}

%}

%token COMMENT

%start program
%%

element
	: COMMENT { vlog("[pp parser] found comment\n"); program = emit(program, " "); printf("-----%s-----\n", program); } /* replace all comments with a space */
	;

program
	: element
	| program element
	;

%%

