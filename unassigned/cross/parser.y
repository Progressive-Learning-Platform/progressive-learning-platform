%{
#include <stdio.h>
#include <string.h>
#include "plp.h"

void yyerror(const char *str) {
	fprintf(stderr, "error: %s\n", str);
}

int yywrap() {
	return 1;
}

%}

%error-verbose

%token LABEL
%token IMM
%token REG
%token WORD
%token BASEOFFSET
%token DIRECTIVE
%token NEWLINE

%%

commands:
	| commands command
	;

command:
	r_type | i_type | directive | label | lwsw | branch | r2_li | jump | newline
	;

r_type:
	WORD REG REG REG NEWLINE
	{
		plp_handle_rtype($1,$2,$3,$4);
	}
	;

r2_li	: WORD REG REG NEWLINE  {plp_handle_r2li($1,$2,$3);}
	| WORD REG WORD NEWLINE {plp_handle_r2li($1,$2,$3);}
	| WORD REG IMM NEWLINE  {plp_handle_r2li($1,$2,$3);}
	;

i_type:
	WORD REG REG IMM NEWLINE
	{	
		plp_handle_itype($1,$2,$3,$4);
	}
	;

lwsw:
	WORD REG BASEOFFSET NEWLINE
	{
		plp_handle_lwsw($1,$2,$3);
	}
	;

branch:
	WORD REG REG WORD NEWLINE
	{
		plp_handle_branch($1,$2,$3,$4);
	}
	;

directive:
	DIRECTIVE NEWLINE
	{
		plp_handle_directive($1);
	}
	;

label:
	LABEL NEWLINE
	{
		plp_handle_label($1);
	}
	;

jump	: WORD REG NEWLINE  {plp_handle_jump($1,$2);}
	| WORD WORD NEWLINE {plp_handle_jump($1,$2);}
	;

newline:
	NEWLINE
	{};


%%



