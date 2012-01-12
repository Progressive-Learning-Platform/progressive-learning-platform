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

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+

%{
#include <stdio.h>
#include "parser.tab.h"
#include "log.h"

extern char* program;

extern char* emit(char*, char*);
void count();
void count_no_log();
void comment();
void single_line_comment();

%}

%option yylineno

%%
"/*"			{ comment(); return(COMMENT); }
"//"			{ single_line_comment(); return(COMMENT); }
"#include"		{ count(); return(INCLUDE); }
"#define"		{ count(); return(DEFINE); }
"#ifdef"		{ count(); return(IFDEF); }
"#ifndef"		{ count(); return(IFNDEF); }
"#else"			{ count(); return(ELSE); }
"#endif"		{ count(); return(ENDIF); }
\"(\\.|[^\\"\\n])*\"	{ count(); yylval = strdup((char*)yytext); return(INC_STRING); }
\<(\\.|[^\\"\\n])*\>	{ count(); yylval = strdup((char*)yytext); return(INC_BRACKET); }
{L}({L}|{D})*		{ count(); yylval = strdup((char*)yytext); return(IDENTIFIER); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
[ \t\v\f]*		{ count_no_log(); return(WS); }
\n			{ count_no_log(); return(NEWLINE); }
<<EOF>>			{ yypop_buffer_state(); if (!YY_CURRENT_BUFFER) { yyterminate(); } }
.			{ count(); yylval = strdup((char*)yytext); return(TEXT); }

%%

int yywrap()
{
	return(1);
}

void comment() {
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0);

	if ((c1 = input()) != '/' && c != 0)
	{
		goto loop;
	}
}

void single_line_comment() {
	char c;
	while ((c = input()) != '\n' && c != 0);
}

int column = 0;
int line = 0;

void count_no_log()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n') {
			column = 0;
			line++;
		}
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;
}

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n') {
			column = 0;
			line++;
		}
		else if (yytext[i] == '\t')
			column += 8 - (column % 8);
		else
			column++;

	/* log all tokens */
	vlog("[pp lexer] : \"%s\"\n", yytext);
}

void handle_include(char* i) {
	char *s = strdup(i+1);
	s[strlen(s)-1] = '\0';
	yyin = fopen(s, "r");
	if (yyin == NULL) {
		err("[pp lexer] cannot open include: %s\n", s);
	}
	yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
	BEGIN(INITIAL);
}

