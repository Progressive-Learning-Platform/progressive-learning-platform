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
[ \t\v\n\f]		{ count_no_log(); program = emit(program, (char*)yytext); }
.			{ count(); program = emit(program, (char*)yytext); }

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
	vlog("[pp lexer] : %s\n", yytext);
}


