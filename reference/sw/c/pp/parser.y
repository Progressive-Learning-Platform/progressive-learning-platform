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
char *define_buffer = NULL;
char *define_ident = NULL;

enum define_mode_t {
	DEF_FALSE,
	DEF_TRUE
};

enum if_mode_t {
	IF_NONE,
	IF_FALSE,
	IF_TRUE
};

enum define_mode_t define_mode;
enum if_mode_t if_mode;

void start_define(char *s) {
	if (if_mode != IF_FALSE) {
		if (define_mode == DEF_TRUE) {
			err("[pp parser] define within a define\n");
		}
		define_ident = strdup(s);
		define_mode = DEF_TRUE;
		define_buffer = NULL;
	}
}

void handle_text(char *s) {
	if (if_mode != IF_FALSE) {
		if (define_mode == DEF_TRUE) { /* are we setting up a define? */
			define_buffer = emit(define_buffer,s);
		} else { /* is the identifier a define? */
			char *expansion = find_define(defines, s);
			program = emit(program, expansion == NULL ? s : expansion);
		}
	}
}

void end_define(void) {
	if (if_mode != IF_FALSE) {
		if (define_mode == DEF_TRUE) {
			define_mode = DEF_FALSE;
			defines = install_define(defines, define_ident, define_buffer);
			free(define_ident);
			free(define_buffer);
		} else {
			program = emit(program, "\n");
		}
	}
}

void start_ifdef(char *s) {
	if (if_mode != IF_NONE) {
		err("[pp parser] nested ifdef not currently allowed\n");
	}
	if (find_define(defines, s) != NULL) { /* it is defined */
		if_mode = IF_TRUE;
	} else {
		if_mode = IF_FALSE;
	}
}

void start_ifndef(char *s) {
	if (if_mode != IF_NONE) {
		err("[pp parser] nested ifdef not currently allowed\n");
	}	
	if (find_define(defines, s) == NULL) {
		if_mode = IF_TRUE;
	} else {
		if_mode = IF_FALSE;
	}
}

void handle_else(void) {
	if (if_mode == IF_NONE) {
		err("[pp parser] #else encountered without ifdef/ifndef/if\n");
	} else if (if_mode == IF_TRUE) {
		if_mode = IF_FALSE;
	} else {
		if_mode = IF_TRUE;
	}
}

void handle_endif(void) {
	if (if_mode == IF_NONE) {
		err("[pp parser] #endif encountered without ifdef/ifndef/if\n");
	} else {
		if_mode = IF_NONE;
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
	| DEFINE WS IDENTIFIER NEWLINE { vlog("[pp parser] empty define: %s\n", $3); if (define_mode == DEF_FALSE) {install_define(defines, $3, NULL); free($3); } else { err("[pp parser] define within a define\n"); } }
	| IFDEF WS IDENTIFIER { vlog("[pp parser] IFDEF: %s\n", $3); start_ifdef($3); free($3); }
	| IFNDEF WS IDENTIFIER { vlog("[pp parser] IFNDEF: %s\n", $3); start_ifndef($3); free($3); }
	| ELSE { vlog("[pp parser] ELSE\n"); handle_else(); }
	| ENDIF { vlog("[pp parser] ENDIF\n"); handle_endif(); }
	| IDENTIFIER '(' ')' { $3 = realloc($3, strlen($3)+3); $3 = strcat($3, "()"); vlog("[pp parser] identifier: %s\n", $1); handle_text($1); free($1); }
	| IDENTIFIER '(' IDENTIFIER { vlog("[pp parser] %s ( %s\n", $1, $3); handle_text($1); handle_text("("); handle_text($3); free($1); free($3); }
	| IDENTIFIER '(' TEXT { vlog("[pp parser] %s ( %s\n", $1, $3); handle_text($1); handle_text("("); handle_text($3); free($1); free($3); }
	| IDENTIFIER '(' WS { vlog("[pp parser] %s (\n", $1); handle_text($1); handle_text("("); program = emit(program, " "); free($1); }
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

