%{
#include "parser.tab.h"
%}

/* regexes */
delim	[ \t,]
newline \n
letter	[A-Za-z]
digit	[0-9]
number	-?{digit}+
alpha	[a-fA-f]
hextail	({digit}|{alpha})+
hex	0[xX]{hextail}

ws		{delim}+
label		\$?{letter}({letter}|{digit})*\:
imm		({number}|{hex})
reg		\$({letter}|{digit})+
word		{letter}+
baseoffset	{number}\({reg}\)
directive	\..+
comment		\#.+

%%

{ws}		{/*nothing, it's whitespace...*/}
{label}		yylval=strdup(yytext); return LABEL;
{imm}		yylval=strdup(yytext); return IMM;
{reg}		yylval=strdup(yytext); return REG;
{word}		yylval=strdup(yytext); return WORD;
{baseoffset}	yylval=strdup(yytext); return BASEOFFSET;
{directive}	yylval=strdup(yytext); return DIRECTIVE;
{comment}	/* nothing, strip comments */
{newline}	return NEWLINE;
%%


