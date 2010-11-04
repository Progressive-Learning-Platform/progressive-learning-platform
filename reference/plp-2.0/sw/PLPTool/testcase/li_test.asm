.org 0x800
somestring:
	.asciiz "Hey check this out bro! #commenttest"
	.ascii "Hey check this out bro! #commenttest"
label:
	li $3, label			# load pointer
	li $5, 0xDEADBEEF
	li $9, 8374
	li $11, mem
	.org 0xFF005
mem:
	  .word 0x4000
	  nop
