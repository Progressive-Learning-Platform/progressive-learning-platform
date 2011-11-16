%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "defines.h"

#define YYSTYPE char*

extern int column, line;
extern int yylex (void);
extern void handle_include(char* s);
extern char* program;
extern char* emit(char*, char*);

extern define *defines;
int define_mode = 0;
char *define_buffer = NULL;
char *define_ident = NULL;

void start_define(char *s) {
	if (define_mode == 1) {
		err("[pp parser] define within a define\n");
	}
	define_ident = strdup(s);
	define_mode = 1;
	define_buffer = NULL;
}

void handle_text(char *s) {
	if (define_mode) { /* are we setting up a define? */
		define_buffer = emit(define_buffer,s);
	} else { /* is the identifier a define? */
		char *expansion = find_define(defines, s);
		program = emit(program, expansion == NULL ? s : expansion);
	}
}

void end_define(void) {
	if (define_mode) {
		define_mode = 0;
		defines = install_define(defines, define_ident, define_buffer);
		free(define_ident);
		free(define_buffer);
	} else {
		program = emit(program, "\n");
	}
}

void yyerror(s)
char *s;
{
	err("[plppp] syntax error: %s\n", s); 
}

%}

%token COMMENT INCLUDE INC_STRING INC_BRACKET DEFINE IDENTIFIER NEWLINE TEXT
%token WS

%start program
%%

element
	: COMMENT { vlog("[pp parser] found comment\n"); program = emit(program, " "); } /* replace all comments with a space */
	| INCLUDE WS INC_STRING { vlog("[pp parser] include: %s\n", $3); handle_include($3); }
	| INCLUDE WS INC_BRACKET { vlog("[pp parser] include: %s\n", $3); handle_include($3); }
	| INC_STRING { vlog("[pp parser] string without inlude: %s\n", $1); program = emit(program, $1); }
	| INC_BRACKET { vlog("[pp parser] bracket without inlude: %s\n", $1); program = emit(program, $1); }
	| DEFINE WS IDENTIFIER '(' ')' { vlog("[pp parser] function like define: %s\n", $3); start_define($3); }
	| DEFINE WS IDENTIFIER WS { vlog("[pp parser] define : %s\n", $3); start_define($3); }
	| DEFINE WS IDENTIFIER NEWLINE { vlog("[pp parser] empty define: %s\n", $3); defines = install_define(defines, $3, NULL); }
	| IDENTIFIER { vlog("[pp parser] identifier %s\n", $1); handle_text($1); }
	| TEXT { vlog("[pp parser] text %s\n", $1); handle_text($1); }
	| NEWLINE { end_define(); }
	| WS { program = emit(program, $1); }
	| '(' { handle_text("("); }
	| ')' { handle_text(")"); }
	;

program
	: element
	| program element
	;

%%

