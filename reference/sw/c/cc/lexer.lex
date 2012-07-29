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
FS			(f|F|l|L)
IS			(u|U|l|L)*

%{
#include <stdio.h>
#include "parser.tab.h"
#include "parse_tree.h"
#include "log.h"
#include "symbol.h"

void count();
void count_no_log();
void comment();
int check_type(char*);

extern symbol_table *sym;

%}

%option yylineno

%%
"/*"			{ comment(); }

"auto"			{ count(); return(AUTO); }
"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"char"			{ count(); return(CHAR); }
"const"			{ count(); return(CONST); }
"continue"		{ count(); return(CONTINUE); }
"default"		{ count(); return(DEFAULT); }
"do"			{ count(); return(DO); }
"double"		{ count(); return(DOUBLE); }
"else"			{ count(); return(ELSE); }
"enum"			{ count(); return(ENUM); }
"extern"		{ count(); return(EXTERN); }
"float"			{ count(); return(FLOAT); }
"for"			{ count(); return(FOR); }
"goto"			{ count(); return(GOTO); }
"if"			{ count(); return(IF); }
"int"			{ count(); return(INT); }
"long"			{ count(); return(LONG); }
"register"		{ count(); return(REGISTER); }
"return"		{ count(); return(RETURN); }
"short"			{ count(); return(SHORT); }
"signed"		{ count(); return(SIGNED); }
"sizeof"		{ count(); return(SIZEOF); }
"static"		{ count(); return(STATIC); }
"struct"		{ count(); return(STRUCT); }
"switch"		{ count(); return(SWITCH); }
"typedef"		{ count(); return(TYPEDEF); }
"union"			{ count(); return(UNION); }
"unsigned"		{ count(); return(UNSIGNED); }
"void"			{ count(); return(VOID); }
"volatile"		{ count(); return(VOLATILE); }
"while"			{ count(); return(WHILE); }
"__asm"			{ count(); return(ASM); }

{L}({L}|{D})*		{ yylval = id((char*)yytext); count(); return(check_type((char*)yytext)); }

0[xX]{H}+{IS}?		{ yylval = con((char*)yytext); count(); return(CONSTANT); }
0{D}+{IS}?		{ yylval = con((char*)yytext); count(); return(CONSTANT); }
{D}+{IS}?		{ yylval = con((char*)yytext); count(); return(CONSTANT); }
L?'(\\.|[^\\'])+'	{ yylval = con((char*)yytext); count(); return(CONSTANT); }

{D}+{E}{FS}?		{ yylval = con((char*)yytext); count(); return(CONSTANT); }
{D}*"."{D}+({E})?{FS}?	{ yylval = con((char*)yytext); count(); return(CONSTANT); }
{D}+"."{D}*({E})?{FS}?	{ yylval = con((char*)yytext); count(); return(CONSTANT); }

L?\"(\\.|[^\\"])*\"	{ yylval = str((char*)yytext); count(); return(STRING_LITERAL); }

"..."			{ count(); return(ELLIPSIS); }
">>="			{ count(); return(RIGHT_ASSIGN); }
"<<="			{ count(); return(LEFT_ASSIGN); }
"+="			{ count(); return(ADD_ASSIGN); }
"-="			{ count(); return(SUB_ASSIGN); }
"*="			{ count(); return(MUL_ASSIGN); }
"/="			{ count(); return(DIV_ASSIGN); }
"%="			{ count(); return(MOD_ASSIGN); }
"&="			{ count(); return(AND_ASSIGN); }
"^="			{ count(); return(XOR_ASSIGN); }
"|="			{ count(); return(OR_ASSIGN); }
">>"			{ count(); return(RIGHT_OP); }
"<<"			{ count(); return(LEFT_OP); }
"++"			{ count(); return(INC_OP); }
"--"			{ count(); return(DEC_OP); }
"->"			{ count(); return(PTR_OP); }
"&&"			{ count(); return(AND_OP); }
"||"			{ count(); return(OR_OP); }
"<="			{ count(); return(LE_OP); }
">="			{ count(); return(GE_OP); }
"=="			{ count(); return(EQ_OP); }
"!="			{ count(); return(NE_OP); }
";"			{ count(); return(';'); }
("{"|"<%")		{ count(); sym = new_symbol_table(sym); return('{'); }
("}"|"%>")		{ count(); sym = sym->parent; return('}'); }
","			{ count(); return(','); }
":"			{ count(); return(':'); }
"="			{ count(); return('='); }
"("			{ count(); return('('); }
")"			{ count(); return(')'); }
("["|"<:")		{ count(); return('['); }
("]"|":>")		{ count(); return(']'); }
"."			{ count(); return('.'); }
"&"			{ count(); return('&'); }
"!"			{ count(); return('!'); }
"~"			{ count(); return('~'); }
"-"			{ count(); return('-'); }
"+"			{ count(); return('+'); }
"*"			{ count(); return('*'); }
"/"			{ count(); return('/'); }
"%"			{ count(); return('%'); }
"<"			{ count(); return('<'); }
">"			{ count(); return('>'); }
"^"			{ count(); return('^'); }
"|"			{ count(); return('|'); }
"?"			{ count(); return('?'); }

[ \t\v\n\f]		{ count_no_log(); }
.			{ printf("[e] %d: bad input '%s'\n", yylineno, yytext); }

%%

int yywrap()
{
	return(1);
}


void comment()
{
	char c, c1;

loop:
	while ((c = input()) != '*' && c != 0);
//		putchar(c);

	if ((c1 = input()) != '/' && c != 0)
	{
		//unput(c1);
		goto loop;
	}

//	if (c != 0)
//		putchar(c1);
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
	vlog("[lexer] : %s\n", yytext);
}


int check_type(char *t)
{
	vlog("[lexer] check_type: %s : ", t);
	symbol *s = find_symbol(sym, t);
	if (s != NULL) {
		if (s->attr & ATTR_TYPEDEF)
			return(TYPE_NAME);
		vlog("TYPE_NAME\n");
	}
	vlog("IDENTIFIER\n");
	return(IDENTIFIER);
}
