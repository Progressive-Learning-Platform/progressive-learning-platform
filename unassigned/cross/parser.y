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
%token SB
%token LB

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
	: ADDU  REG REG REG NEWLINE		{craft(4,"addu",$2,$3,$4);}
	| SUBU  REG REG REG NEWLINE		{craft(4,"subu",$2,$3,$4);}
	| AND   REG REG REG NEWLINE		{craft(4,"and",$2,$3,$4);}
	| OR    REG REG REG NEWLINE		{craft(4,"or",$2,$3,$4);}
	| NOR   REG REG REG NEWLINE		{craft(4,"nor",$2,$3,$4);}
	| SLT   REG REG REG NEWLINE		{craft(4,"slt",$2,$3,$4);}
	| SLTU  REG REG REG NEWLINE		{craft(4,"sltu",$2,$3,$4);}
	| SLL   REG REG IMM NEWLINE		{craft(4,"sll",$2,$3,$4);}
	| SRL   REG REG IMM NEWLINE		{craft(4,"srl",$2,$3,$4);}
	| JR    REG NEWLINE			{craft(2,"jr",$2); craft(1,"nop");}
	| JALR  REG WORD NEWLINE		{craft(3,"jalr",$2,$3); craft(1,"nop");}
	| BEQ   REG REG WORD NEWLINE		{craft(4,"beq",$2,$3,$4); craft(1,"nop");}
	| BNE   REG REG WORD NEWLINE		{craft(4,"bne",$2,$3,$4); craft(1,"nop");}
	| ADDIU REG REG IMM NEWLINE		{craft(4,"addiu",$2,$3,$4);}
	| ANDI  REG REG IMM NEWLINE	 	{craft(4,"andi",$2,$3,$4);}
	| ORI   REG REG IMM NEWLINE		{craft(4,"ori",$2,$3,$4);}
	| SLTI  REG REG IMM NEWLINE		{craft(4,"slti",$2,$3,$4);}
	| SLTIU REG REG IMM NEWLINE		{craft(4,"sltiu",$2,$3,$4);}
	| LUI   REG IMM NEWLINE			{craft(3,"lui",$2,$3);}
	| LW    REG BASEOFFSET NEWLINE		{craft(3,"lw",$2,$3);}
	| SW    REG BASEOFFSET NEWLINE		{craft(3,"sw",$2,$3);}
	| J     WORD NEWLINE			{craft(2,"j",$2); craft(1,"nop");}
	| JAL   WORD NEWLINE			{craft(2,"jal",$2); craft(1,"nop");}
	| NOP   NEWLINE				{craft(1,"nop");}
	| B     WORD NEWLINE			{craft(2,"b",$2); craft(1,"nop");}
	| B	REG NEWLINE			{craft(2,"b",$2); craft(1,"nop");}
	| MOVE  REG REG NEWLINE			{craft(3,"move",$2,$3);}
	| LI    REG WORD NEWLINE		{craft(3,"li",$2,$3);}
	| LI	REG IMM NEWLINE			{craft(3,"li",$2,$3);}
	| LABEL NEWLINE				{craft(1,$1);}
	;

unsupported_instruction
	: J	REG NEWLINE		{craft(2,"jr",$2); craft(1,"nop");}
	| LA	REG WORD NEWLINE	{craft(3,"li",$2,$3);}
	| LA	REG REG NEWLINE		{craft(3,"li",$2,$3);}
	| SB	REG BASEOFFSET NEWLINE  {craft(3,"sw",$2,$3);}
	| LB	REG BASEOFFSET NEWLINE  {craft(3,"lw",$2,$3);}
	;

%%



