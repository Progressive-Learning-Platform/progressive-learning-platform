          
# Test assembly

	.org 0x0
label1:
	.word 0xBEEFCAFE
	addu $2, $1, $3  #  sdfsdfsdf




woohoo:
        .word 0xDEADBEEF

	.org 0x80
instrs:

	lui  $3,50    #dfdsfsdf
	nop
	nop
	sll $1,$1,0x1

	addiu $2,  $3,   0b100100   
        lw $3,   40($2)