#working on the pipelined machine so i'll write something that has no hazards and won't depend on forwarding


.org 0
start:
ori $0, $0, 0xff
or $1, $0, $0
or $2, $0, $0
or $3, $0, $0
or $4, $0, $0
or $5, $0, $0
ori $1, $0, 0x01 #$1 = 1
addu $2, $2, $1 #$2 = 1
addu $2, $2, $1 #$2 = 2
subu $3, $2, $1 #$3 = 1
and $4, $2, $1 #$4 = 0
or $5, $2, $1 #$5 = 3
nor $6, $2, $1 #6 = 0xfffffffc
slt $7, $2, $6 #7 = 0
sltu $8, $2, $6 #8 = 1
sll $9, $8, 4
srl $10, $9, 4
jal function
nop
ori $12, $0, 0xdead
beq $3, $1, function2
return_label1:
beq $3, $2, start
nop
ori $14, $0, 0xabcd
addiu $15, $2, 5
andi $16, $2, 7
slti $17, $2, 0xffff
slti $18, $2, 0x0fff
sltiu $19, $2, 0xffff
lui $20, 0x1000
sw $11, 0($20)
lw $21, 0($20)
jalr $30, $0
nop
function:
ori $11, $0, 0xbeef
jr $31
nop
function2:
ori $13, $0, 0xfeed
j return_label1
nop
nop
