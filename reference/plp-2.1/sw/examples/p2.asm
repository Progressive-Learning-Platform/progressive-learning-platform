.org 0x10000000
j start
nop

start:
	li $t0, 0xf0400000 #vga
	li $t1, 1 #enable
	li $t2, vga_test_field #frame buffer

	sw $t2, 4($t0)
	sw $t1, 0($t0)

halt:
	j halt
	nop

vga_test_field:
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff
.word 0xffffffff