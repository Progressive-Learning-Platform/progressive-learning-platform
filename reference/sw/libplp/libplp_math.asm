#
# libplp math routines
#
# fritz
# 2011.3.9
#

# this library requires the stack to be used, with the current stack pointer in $sp

#
# libplp_math_imult
#
# integer multiply, implemented using peasant multiplication
# loop ends in fewer than 32 iterations
#
libplp_math_imult:
	move $v0, $zero		#clear the result field
libplp_math_imult_loop:
	beq $a0, $zero, libplp_math_imult_done
	andi $t0, $a0, 0x1
	beq $t0, $zero, libplp_math_imult_skip
	srl $a0, $a0, 1
	addu $v0, $v0, $a1
libplp_math_imult_skip:
	j libplp_math_imult_loop
	sll $a1, $a1, 1
libplp_math_imult_done:
	jr $ra
	nop

#
# libplp_math_abs
#
# absolute value
#
libplp_math_abs:
	lui $t0, 0x8000
	addu $t0, $a0, $t0
	srl $t0, $t0, 31
	addiu $t0, $t0, -1
	sll $t1, $a0, 1
	and $t0, $t0, $t1
	jr $ra
	subu $v0, $a0, $t0

#
# libplp_math_parity
#
# parity, calculated by finding the population of the word, and returning the
# bottom bit. The population function is just too much fun.
#
libplp_math_parity:
	li $t0, 0x55555555
	li $t1, 0x33333333
	li $t2, 0x0f0f0f0f
	li $t3, 0x00ff00ff
	li $t4, 0x0000ffff
	srl  $t5, $a0, 1
	and  $t5, $t5, $t0
	and  $t6, $a0, $t0
	addu $a0, $t5, $t6
	srl  $t5, $a0, 2
	and  $t5, $t5, $t1
	and  $t6, $a0, $t1
	addu $a0, $t5, $t6
	srl  $t5, $a0, 4
	and  $t5, $t5, $t2
	and  $t6, $a0, $t2
	addu $a0, $t5, $t6
	srl  $t5, $a0, 8
	and  $t5, $t5, $t3
	and  $t6, $a0, $t3
	addu $a0, $t5, $t6
	srl  $t5, $a0, 16
	and  $t5, $t5, $t4
	and  $t6, $a0, $t4
	addu $a0, $t5, $t6
	jr $ra
	andi $v0, $a0, 0x1

#
# libplp_math_min
#
# signed min(x,y) function using branches
#
libplp_math_min:
	slt $t0, $a0, $a1
	beq $t0, $zero, libplp_math_min_y
	move $v0, $a0
	j libplp_math_min_done
	nop
libplp_math_min_y:
	move $v0, $a1
libplp_math_min_done:
	jr $ra
	nop

#
# libplp_math_max
#
# signed max(x,y) function using branches
#
libplp_math_max:
	slt $t0, $a0, $a1
	beq $t0, $zero, libplp_math_max_x
	move $v0, $a1
	j libplp_math_max_done
	nop
libplp_math_max_x:
	move $v0, $a0
libplp_math_max_done:
	jr $ra
	nop
