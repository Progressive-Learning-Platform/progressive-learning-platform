%{
#include "plp.h"
#include "parser.tab.h"
%}

%option yylineno

/* regexes */
delim	[ \t,]
newline \n
letter	[A-Za-z]
special [-_]
digit	[0-9]
number	-?{digit}+
hextail	({digit}|{alpha})+
hex	0[xX]{hextail}
alpha	[a-fA-F]

ws		{delim}+
label		{word}+\:
imm		({number}|{hex})
reg		\$(([0-2]?[0-9]|3[0-1])|zero|at|v[0-1]|a[0-3]|t[0-9]|s[0-8]|k[0-1]|gp|sp|fp|ra)
baseoffset	{number}\({reg}\)
comment		\#.+
word		({special}|{letter}|{number}|\$L)+
directive	[.].+
string		\".+\"

%%

 /* supported instructions */
addu		return ADDU;
subu		return SUBU;
and		return AND;
or		return OR;
nor		return NOR;
slt		return SLT;
sltu		return SLTU;
sll		return SLL;
srl		return SRL;
jr		return JR;
jalr		return JALR;
beq		return BEQ;
bne		return BNE;
addiu		return ADDIU;
andi		return ANDI;
ori		return ORI;
slti		return SLTI;
sltiu		return SLTIU;
lui		return LUI;
lw		return LW;
sw		return SW;
j		return J;
jal		return JAL;
nop		return NOP;
b		return B;
move		return MOVE;
li		return LI;

 /* unsupported instructions */
la		return LA;
sb		return SB;
lb		return LB;

{ws}		{/*nothing, it's whitespace...*/}
[.]ascii/{ws}\".+\".*		return ASCII;
{directive}	yylval=strdup(yytext); return DIRECTIVE;
{label}		yylval=strdup(yytext); return LABEL;
{imm}		yylval=strdup(yytext); return IMM;
{reg}		yylval=strdup(yytext); return REG;
{baseoffset}	yylval=strdup(yytext); return BASEOFFSET;
{string}	yylval=strdup(yytext); return STRING;
{comment}	/* nothing, strip comments */
{newline}	return NEWLINE;
{word}		yylval=strdup(yytext); return WORD;

. printf("[e] %d: bad input '%s'\n",yylineno,yytext);

%%
 

