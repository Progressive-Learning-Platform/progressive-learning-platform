#
#timer routines
#

libplp_timer_read:
	lui $t0, 0xf060
	jr $31
	lw $v0, 0($t0)

libplp_timer_write:
	lui $t0, 0xf060
	jr $31
	sw $a0, 0($t0)

libplp_timer_wait:
	lui $t0, 0xf060		#address of the timer
	sw  $zero, 0($t0)	#clear the timer
libplp_timer_wait_loop:
	lw  $t1, 0($t0)		#get the timer
	slt $t2, $t1, $a0	#set if the timer is still less than the argument, and loop
	bne $zero, $t2, libplp_timer_wait_loop	
	nop
	jr $31
	nop
