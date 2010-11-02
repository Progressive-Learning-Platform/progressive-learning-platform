#wait routine
#wait time is in cycles
#
#uses the timer hardware

bsl_wait:
	lui $t0, 0x9000		#address of the timer
	sw  $zero, 0($t0)	#clear the timer
bsl_wait_loop:
	lw  $t1, 0($t0)		#get the timer
	slt $t2, $t1, $a0	#set if the timer is still less than the argument, and loop
	bne $zero, $t2, bsl_wait_loop	
	nop
	jr $31
