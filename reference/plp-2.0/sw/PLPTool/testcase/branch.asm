.org 0x0
	beq $0,$0,label
	nop
somestring:
	.asciiz "Hey check this out bro! #commenttest"
	.ascii "Hey check this out bro! #commenttest"
	lui $15, 0x0FFF
	li $19, mem
label:
	bne $0,$0,label
	lui $15, 0xDEAD
        nop
	nop

        ori $15, $15, 0xBEEF
	li $3, label			# load pointer
	li $5, 0xDEADBEEF
	li $9, 8374
	li $11, mem
	jal jumptest
	nop
	j label
	li $4, 0xFEEDBEEF #beef should not be written to $4
	.org 0x800
mem:
	  .word 0x4000
	  nop
jumptest:
	addu $15,$3,$5
	nop
	nop
	nop
	nop
	jr $31
	ori $15,$15,0x00FF
