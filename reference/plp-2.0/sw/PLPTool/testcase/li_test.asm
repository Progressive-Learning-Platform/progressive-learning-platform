.org 0x0
	beq $0,$0,label
	nop
somestring:
	.asciiz "Hey check this out bro! #commenttest"
	.ascii "Hey check this out bro! #commenttest"
label:
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
	ori $15,$15,0x00FF
