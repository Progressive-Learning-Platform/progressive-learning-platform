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
int if_mode = 0;
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

start_ifdef(char *s) {
	if_mode = 1;
	if (find_define(s) == NULL) { /* it is defined */
		if_mode = 2;
	}
}

start_ifndef(char *s) {
	if_mode = 1;
	if (find_define(s) != NULL) {
		if_mode = 2;
	}
}

void yyerror(s)
char *s;
{
	err("[plppp] syntax error: %s\n", s); 
}

%}

%token COMMENT INCLUDE INC_STRING INC_BRACKET DEFINE IDENTIFIER NEWLINE TEXT
%token WS IFDEF IFNDEF ELSE ENDIF

%start program
%%

element
	: COMMENT { vlog("[pp parser] found comment\n"); program = emit(program, " "); } /* replace all comments with a space */
	| INCLUDE WS INC_STRING { vlog("[pp parser] include: %s\n", $3); handle_include($3); free($3); }
	| INCLUDE WS INC_BRACKET { vlog("[pp parser] include: %s\n", $3); handle_include($3); free($3); }
	| INC_STRING { vlog("[pp parser] string without inlude: %s\n", $1); program = emit(program, $1); free($1); }
	| INC_BRACKET { vlog("[pp parser] bracket without inlude: %s\n", $1); program = emit(program, $1); free($1); }
	| DEFINE WS IDENTIFIER '(' ')' WS { vlog("[pp parser] function like define: %s\n", $3); $3 = realloc($3, strlen($3)+3); $3 = strcat($3,"()"); start_define($3); free($3); }
	| DEFINE WS IDENTIFIER WS { vlog("[pp parser] define : %s\n", $3); start_define($3); free($3); }
	| DEFINE WS IDENTIFIER NEWLINE { vlog("[pp parser] empty define: %s\n", $3); install_define(defines, $3, NULL); free($3); }
	| IFDEF WS IDENTIFIER { vlog("[pp parser] IFDEF: %s\n", $3); start_ifdef($3); free($3); }
	| IFNDEF WS IDENTIFIER { vlog("[pp parser] IFNDEF: %s\n", $3); start_ifndef($3); free($3); }
	| ELSE { vlog("[pp parser] ELSE\n"); handle_else(); }
	| ENDIF { vlog("[pp parser] ENDIF\n"); handle_endif(); }
	| IDENTIFIER '(' ')' { $3 = realloc($3, strlen($3)+3); $3 = strcat($3, "()"); vlog("[pp parser] identifier: %s\n", $1); handle_text($1); free($1); }
	| IDENTIFIER { vlog("[pp parser] identifier %s\n", $1); handle_text($1); free($1); }
	| TEXT { vlog("[pp parser] text %s\n", $1); handle_text($1); free($1); }
	| NEWLINE { end_define(); }
	| WS { program = emit(program, " "); }
	| '(' { handle_text("("); }
	| ')' { handle_text(")"); }
	;

program
	: element
	| program element
	;

%%

