#working on the pipelined machine so i'll write something that has no hazards and won't depend on forwarding


.org 0
start:
ori $0, $0, 0xff
or $1, $0, $0
or $2, $0, $0
or $3, $0, $0
or $4, $0, $0
or $5, $0, $0
nop
nop
nop
nop

ori $1, $0, 0x01 #$1 = 1
nop
nop
nop
nop

addu $2, $2, $1 #$2 = 1
nop
nop
nop
nop

addu $2, $2, $1 #$2 = 2
nop
nop
nop
nop

subu $3, $2, $1 #$3 = 1
nop
nop
nop
nop

and $4, $2, $1 #$4 = 0
nop
nop
nop
nop

or $5, $2, $1 #$5 = 3
nop
nop
nop
nop

nor $6, $2, $1 #6 = 0xfffffffc
nop
nop
nop
nop

slt $7, $2, $6 #7 = 0
nop
nop
nop
nop

sltu $8, $2, $6 #8 = 1
nop
nop
nop
nop

sll $9, $8, 4
nop
nop
nop
nop

srl $10, $9, 4
nop
nop
nop
nop

jal function
nop
nop
nop
nop
nop
nop

ori $12, $0, 0xdead
nop
nop
nop
nop
nop
nop

beq $3, $1, function2
nop
nop
nop
nop

return_label1:
nop
nop
nop
beq $3, $2, start
nop
nop
nop
nop

ori $14, $0, 0xabcd
nop
nop
nop
nop

addiu $15, $2, 5
nop
nop
nop
nop

andi $16, $2, 7
nop
nop
nop
nop

slti $17, $2, 0xffff
nop
nop
nop
nop

slti $18, $2, 0x0fff
nop
nop
nop
nop

sltiu $19, $2, 0xffff
nop
nop
nop
nop

lui $20, 0x1000
nop
nop
nop
nop

sw $11, 0($20)
nop
nop
nop
nop
nop
nop

lw $21, 0($20)
nop
nop
nop
nop


jalr $30, $0
nop
nop
nop
nop

function:
nop
nop
nop
nop
ori $11, $0, 0xbeef
nop
nop
nop
nop
jr $31
nop
nop
nop
nop

function2:
nop
nop
nop
ori $13, $0, 0xfeed
nop
nop
nop
j return_label1
nop
nop
nop
