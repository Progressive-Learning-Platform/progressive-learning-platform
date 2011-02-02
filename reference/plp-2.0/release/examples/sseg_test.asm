#writes to the seven segment module
#
# scrolls the following on the seven segments, with one second delay:
# 0123
# 4567
# 89ab
# cdef
#

.org 0x10000000

j start
nop

.include ../libplp/libplp_sseg.asm
.include ../libplp/libplp_timer.asm
.include ../libplp/libplp_plpid.asm

start:
	jal libplp_plpid_read_frequency
	nop
	move $s0, $v0	#board frequency
	
	li $s1, 0x00010203
	li $s2, 0x04050607
	li $s3, 0x08090a0b
	li $s4, 0x0c0d0e0f

program_loop:
	jal libplp_sseg_write
	move $a0, $s1
	jal libplp_timer_wait
	move $a0, $s0

	jal libplp_sseg_write
	move $a0, $s2
	jal libplp_timer_wait
	move $a0, $s0

	jal libplp_sseg_write
	move $a0, $s3
	jal libplp_timer_wait
	move $a0, $s0

	jal libplp_sseg_write
	move $a0, $s4
	jal libplp_timer_wait
	move $a0, $s0

	j program_loop
	nop
