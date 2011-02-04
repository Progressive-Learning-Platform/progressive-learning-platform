%{
#include "plp.h"
#include "parser.tab.h"
%}

%option yylineno

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
{label}		yylval=strdup(yytext); return LABEL;
{imm}		yylval=strdup(yytext); return IMM;
{reg}		yylval=strdup(yytext); return REG;
{word}		yylval=strdup(yytext); return WORD;
{baseoffset}	yylval=strdup(yytext); return BASEOFFSET;
{directive}	yylval=strdup(yytext); return DIRECTIVE;
{comment}	/* nothing, strip comments */
{newline}	return NEWLINE;

. printf("[e] %d: bad input '%s'\n",yylineno,yytext);

%%

