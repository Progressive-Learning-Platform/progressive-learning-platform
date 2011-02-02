%{
#include <stdio.h>
#include <string.h>
#include "plp.h"

void yyerror(const char *str) {
	fprintf(stderr, "[e] %d : %s\n", yyget_lineno(), str);
}

int yywrap() {
	return 1;
}

%}

%error-verbose
/* special tokens */
%token LABEL
%token IMM
%token REG
%token WORD
%token BASEOFFSET
%token DIRECTIVE
%token NEWLINE

/* supported instructions */
%token ADDU
%token SUBU
%token AND
%token OR
%token NOR
%token SLT
%token SLTU
%token SLL
%token SRL
%token JR
%token JALR
%token BEQ
%token BNE
%token ADDIU
%token ANDI
%token ORI
%token SLTI
%token SLTIU
%token LUI
%token LW
%token SW
%token J
%token JAL
%token NOP
%token B
%token MOVE
%token LI

/* unsupported instructions */
%token LA

%%

commands 
	:
	| commands command
	;

command 
	: supported_instruction | unsupported_instruction | junk
	;

junk    
	: DIRECTIVE	{/* do nothing */}
	| NEWLINE	{/* do nothing */}
	;

supported_instruction
	: ADDU  REG REG REG NEWLINE		{craft(4,"addu",$1,$2,$3);}
	| SUBU  REG REG REG NEWLINE
	| AND   REG REG REG NEWLINE
	| OR    REG REG REG NEWLINE
	| NOR   REG REG REG NEWLINE
	| SLT   REG REG REG NEWLINE
	| SLTU  REG REG REG NEWLINE
	| SLL   REG REG IMM NEWLINE
	| SRL   REG REG IMM NEWLINE
	| JR    REG NEWLINE
	| JALR  REG WORD NEWLINE
	| BEQ   REG REG WORD NEWLINE
	| BNE   REG REG WORD NEWLINE
	| ADDIU REG REG IMM NEWLINE
	| ANDI  REG REG IMM NEWLINE 
	| ORI   REG REG IMM NEWLINE
	| SLTI  REG REG IMM NEWLINE
	| SLTIU REG REG IMM NEWLINE
	| LUI   REG IMM NEWLINE
	| LW    REG BASEOFFSET NEWLINE
	| SW    REG BASEOFFSET NEWLINE
	| J     WORD NEWLINE
	| JAL   WORD NEWLINE
	| NOP   NEWLINE
	| B     WORD NEWLINE
	| MOVE  REG REG NEWLINE
	| LI    REG WORD NEWLINE
	| LI	REG IMM NEWLINE
	| LABEL NEWLINE
	;

unsupported_instruction
	: J	REG NEWLINE		{printf("unsupported jump\n");}
	| LA	REG WORD NEWLINE	{printf("unsupported la\n");}
	| LA	REG REG NEWLINE		{printf("unsupported la\n");}
	;

%%



