
.org 0

#an ex/ex hazard

lui $1, 0xdead
ori $2, $1, 0xbeef

#mem/ex hazard (should forward)

addi $3, $0, 0xff
ori $4, $0, 0	#something that doesn't depend on anyone
addi $5, $3, 1

#mem/mem hazard

lui $6, 0x1000
sw $2, 0($6)
nop
lw $7, 0($6)
sw $7, 4($6)
nop

#mem/ex hazard (should stall, load use hazard)

lw $8, 0($6)
addiu $9, $8, 1
addiu $9, $9, 1
