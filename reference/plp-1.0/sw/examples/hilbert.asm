#
#david fritz
#4.10.2010
#a hilbert curve drawn as a lindenmayer system
#


#
#preamble
#
.org 0x10000000

#set up the stack
li	$sp, 0x10001fff

#jump to the main function
j	entrypoint
nop

#
#data section
#
pos:
	.word 0	#col (x)
	.word 0	#row (y)
dir:
	.word 0	#0=down, 1=right, 2=up, 3=left

#
#text section
#
walk:
	addi	$sp, $sp, -4
	sw	$ra, 0($sp)

	#figure out which direction to walk in
	li	$t1, 1
	li	$t2, 2
	li	$t3, 3
	li	$t4, pos
	li	$t5, dir
	lw	$t6, 0($t5)
	li	$t7, 10		#walk distance
	li	$t9, -10	#negative walk distance
	
	beq	$t6, $zero, walk_down
	beq	$t6, $t1, walk_right
	beq	$t6, $t2, walk_up
	beq	$t6, $t3, walk_left
	nop
walk_down:
	lw	$t8, 4($t4)
	jal	draw_v
	move	$a0, $t7
	addu	$t8, $t8, $t7
	j	walk_done
	sw	$t8, 4($t4)
walk_right:
	lw	$t8, 0($t4)
	jal	draw_h
	move	$a0, $t7
	addu	$t8, $t8, $t7
	j	walk_done
	sw	$t8, 0($t4)
walk_up:
	lw	$t8, 4($t4)
	jal	draw_v
	move	$a0, $t9
	subu	$t8, $t8, $t7
	j	walk_done
	sw	$t8, 4($t4)
walk_left:
	lw	$t8, 0($t4)
	jal	draw_h
	move	$a0, $t9
	subu	$t8, $t8, $t7
	sw	$t8, 0($t4)
walk_done:
	lw	$ra, 0($sp)
	jr	$ra
	addi	$sp, $sp, 4

draw_v:
	

turn_90_cw:
	li	$t0, dir
	lw	$t1, 0($t0)
	addiu	$t1, $t1, -1
	slt	$t2, $t1, $zero
	beq	$t2, $zero, turn_done
	nop
	j	turn_done
	li	$t1, 3
turn_90_ccw:
	li	$t0, dir
	lw	$t1, 0($t0)
	addiu	$t1, $t1, 1
	li	$t3, 4
	bne	$t3, $t1, turn_done
	nop
	j	turn_done
	li	$t1, 0
turn_done:
	jr	$ra
	sw	$t1, 0($t0)

left:
	addi	$sp, $sp, -8
	sw	$ra, -4($sp)
	addi	$a0, $a0, -1
	sw	$a0, 0($sp)

	beq	$zero, $a0, left_done
	nop

	jal	turn_90_ccw
	nop
	jal	right
	lw	$a0, 0($sp)
	jal	walk
	nop
	jal	turn_90_cw
	nop
	jal	left
	lw	$a0, 0($sp)
	jal	walk
	nop
	jal	left
	lw	$a0, 0($sp)
	jal	turn_90_cw
	nop
	jal	walk
	nop
	jal	right
	lw	$a0, 0($sp)
	jal	turn_90_ccw
	nop
left_done:
	lw	$ra, -4($sp)
	jr	$ra
	addi	$sp, $sp, 8

right:
	addi	$sp, $sp, -8
	sw	$ra, -4($sp)
	addi	$a0, $a0, -1
	sw	$a0, 0($sp)

	beq	$zero, $a0, right_done
	nop

	jal	turn_90_cw
	nop
	jal	left
	lw	$a0, 0($sp)
	jal	walk
	nop
	jal	turn_90_ccw
	nop
	jal	right
	lw	$a0, 0($sp)
	jal	walk
	nop
	jal	right
	lw	$a0, 0($sp)
	jal	turn_90_ccw
	nop
	jal	walk
	nop
	jal	left
	lw	$a0, 0($sp)
	jal	turn_90_cw
	nop
right_done:
	lw	$ra, -4($sp)
	jr	$ra
	addi	$sp, $sp, 8
	
entrypoint:
	li	$a0, 6	#sixth order hilbert curve
	jal	left
	nop
done:
	j done
	nop
