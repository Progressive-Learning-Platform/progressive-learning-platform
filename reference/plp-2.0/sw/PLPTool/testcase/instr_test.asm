li $18,0xffff0000
li $19,0x0000ffff
li $20,0xf0f00f0f
li $21,0x00000001
addu $22,$21,$20
li $23,0x00000001
li $24,4
subu $22,$20,$23
addi $22,$22,1
and $22,$22,$19
or $22,$22,$18
andi $22,$22,0x00ff
ori $22,$22,0xff00
nop
nop
sw $22,0x4000($24)
nop
nop
sll $22,$22,4
nop
nop
nop
nop
nop
nop
