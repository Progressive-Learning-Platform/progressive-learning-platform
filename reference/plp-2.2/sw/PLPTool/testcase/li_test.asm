.org 0x0
	beq $0,$0,label
	nop
        nop
somestring:
	.asciiz "Hey check this out bro! #commenttest"
	.ascii "Hey check this out bro! #commenttest"
	lui $15, 0x0FFF
	li $19, mem
label:
	lui $15, 0xDEAD
        ori $15, $15, 0xBEEF
	li $3, label			# load pointer
	li $5, 0xDEADBEEF
	li $9, 8374
	li $11, mem
	nop
	nop
	li $25, 5
	li $26, 6
        addu $27, $25, $26
	addu $20,$5,$9
	sw $15,12($0)
	jal jumptest
	nop
        nop
        nop
        j label
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
        lw $12,12($0)
	nop
	ori $15,$15,0x00FF
        #beq $0,$0,jumptest
	jr $31
        nop
        nop