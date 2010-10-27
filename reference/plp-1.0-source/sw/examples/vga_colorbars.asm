#
#david fritz
#
#3.30.2010
#
#a program that displays color bars on the vga port
#

#
#we will split the screen into 8 sections, and display
#each of the 8 colors we support, with rgb=000 on the 
#left and rgb=111 on the right
#

.org 0x10000000			#start in RAM
j entrypoint			#jump to our entry point
nop

#our variables, all initialized to 0
row:
	.word 0
col:
	.word 0
vga_row:
	.word 0x78000004
vga_col:
	.word 0x78000008
vga_dat:
	.word 0x78000000

entrypoint:
	#we'll set up a double loop across the column and row
	#max row is 240
	#max col is 320
	
loop_col:
	#column loop
	li	$t0, col
	lw	$t0, 0($t0)
	li	$t1, 320
	beq	$t0, $t1, done	#program end
	nop
	
loop_row:
	#row loop
	li	$t0, row
	lw	$t0, 0($t0)
	li	$t1, 240
	beq	$t0, $t1, row_done
	nop

	#calculate the color to draw
	jal	calc_color
	nop
	move 	$a0, $v0
	
	#draw the color
	jal	draw_color
	nop

	#increment the row
	li	$t0, row
	lw	$t1, 0($t0)
	addi	$t1, $t1, 1
	sw	$t1, 0($t0)
	j	loop_row
	nop

row_done:
	#zero the row, increment the col
	li	$t0, row
	sw	$zero, 0($t0)
	li	$t0, col
	lw	$t1, 0($t0)
	addi	$t1, $t1, 1
	sw	$t1, 0($t0)
	j	loop_col
	nop

done:
	j done
	nop

#we split the color bars into 8 sections
#we can just add 40 to a temp, and increment
#our color by one, until we exceed the col
#and return
calc_color:
	move	$t0, $zero
	ori	$t1, $zero, 40
	li	$t2, col
	lw	$t2, 0($t2)
calc_color_loop:
	slt	$t3, $t1, $t2
	beq	$zero, $t3, calc_color_done
	nop
	addi	$t0, $t0, 1
	addi 	$t1, $t1, 40
	j	calc_color_loop
	nop
calc_color_done:
	move	$v0, $t0
	jr	$31
	nop

#draw the color to the vga module
draw_color:
	li	$t0, row
	lw	$t0, 0($t0)
	li	$t1, vga_row
	lw	$t1, 0($t1)
	sw	$t0, 0($t1)

	li	$t0, col
	lw	$t0, 0($t0)
	li	$t1, vga_col
	lw	$t1, 0($t1)
	sw	$t0, 0($t1)

	li	$t1, vga_dat
	lw	$t1, 0($t1)
	sw	$a0, 0($t1)
	
	jr	$31
	nop
