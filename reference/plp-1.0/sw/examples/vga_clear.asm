#
#David Fritz
#
#3.9.2010
#
#Simple program to clear the vga framebuffer using two methods
#First, it clears (to black) the vga output by using the flat memory model
#(from 0x70000000-0x70012bff)
#Second, it clears (to white) the vga output by using the row/column interface
#(using 0x78000000-0x78000008)
#

.org 0x10000000
j entry
nop

.include ../asm/bsl_wait.asm

#s0 = address for the vga memory
#s1 = row/col data register
#s2 = row address
#s3 = col address
#s4 = one second in cycles
entry:
	lui $t0, 0x8000
	lw $s4, 4($t0)		#board frequency
main_loop:
	jal clear_memory_model
	nop

	li $t0, 0x40000000
	li $t1, 0x55
	sw $t1, 0($t0)
	
	move $a0, $s4
	jal bsl_wait
	nop

	jal clear_row_col_model
	nop

	li $t0, 0x40000000
	li $t1, 0xaa
	sw $t1, 0($t0)
	
	move $a0, $s4
	jal bsl_wait
	nop

	j main_loop
	nop

clear_memory_model:
	lui $s0, 0x7000
	li $t0, 0x70012c00
clear_memory_model_loop:
	sw $zero, 0($s0)
	addi $s0, $s0, 1
	bne $s0, $t0, clear_memory_model_loop
	nop
	jr $31
	nop

#t0 = data (white)
#t1 = row offset
#t2 = col offset
#t3 = row max
#t4 = col max
clear_row_col_model:
	li $s1, 0x78000000
	li $s2, 0x78000004
	li $s3, 0x78000008
	li $t0, 0x0000ffff
	move $t1, $zero
	move $t2, $zero
	li $t3, 240
	li $t4, 320
row_loop:
	beq $t1, $t3, row_done
	nop
col_loop:
	beq $t2, $t4, col_done
	nop
	
	#write the data to the current row and column
	sw $t1, 0($s2)		#the row
	sw $t2, 0($s3)		#the column
	sw $t0, 0($s1)		#the data

	#increment the column and jump back to the col loop
	addi $t2, $t2, 1
	j col_loop
	nop

col_done:
	#increment the row, clear the column, and jump back to the row
	move $t2, $zero
	addi $t1, $t1, 1
	j row_loop
	nop

row_done:
	jr $31
	nop
	
