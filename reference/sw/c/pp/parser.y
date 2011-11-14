%{
#include <stdio.h>
#include "log.h"

extern int column, line;
extern int yylex (void);

void yyerror(s)
char *s;
{
	err("[plppp] syntax error: %s\n", s); 
}

%}

%token COMMENT

%%

comment
	: COMMENT { vlog("[pp parser] found comment\n"); emit(" "); } /* replace all comments with a space */
	;

%%

