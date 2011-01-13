#a simple clock program
#
#uses the switches to set the time
# switch 0 will increase the minutes at a 1/4 second rate
# switch 1 will increase the hours at a 1/4 second rate

.org 0x10000000

j start
nop

.include ../libplp/libplp_sseg.asm
.include ../libplp/libplp_timer.asm
.include ../libplp/libplp_plpid.asm
.include ../libplp/libplp_switches.asm

start:
	jal libplp_plpid_read_frequency
	nop
	move $s0, $v0	#board frequency / 4
	srl $s0, $s0, 2
	
	li $s1, 0x01020000	#the time, starting at 12:00
	li $s2, 0x00000000	#a quarter second counter (rolls over at 240)

main_loop:
	#check the switches
	jal handle_switches
	nop
	jal update_time
	nop

	#check for counter overflow
	jal handle_overflow
	nop
	jal update_time	
	nop

	#update the counter
	addiu $s2, $s2, 1

	#write the time
	jal libplp_sseg_write
	move $a0, $s1

	#wait
	jal libplp_timer_wait
	move $a0, $s0

	j main_loop
	nop

#---------------------

handle_switches:
	#save the ra
	move $s3, $ra	

	jal libplp_switches_read
	nop
	
	#switch masks
	ori $t0, $zero, 0x01
	ori $t1, $zero, 0x02

	and $t2, $t0, $v0
	beq $t2, $t0, handle_switches_minute
	nop
handle_switches_1:
	and $t2, $t1, $v0
	beq $t2, $t1, handle_switches_hour
	nop

	j handle_switches_done
	nop

handle_switches_minute:
	addiu $s1, $s1, 1
	j handle_switches_1
	nop

handle_switches_hour:
	li $t0, 0x00010000
	addu $s1, $s1, $t0

handle_switches_done:
	jr $s3
	nop

handle_overflow:
	li $t0, 240
	bne $s2, $t0, handle_overflow_done
	nop

	#overflow
	# set the counter back to zero and increase the minutes
	move $s2, $zero
	addiu $s1, $s1, 1

handle_overflow_done:
	jr $ra
	nop

#update the time variable by checking if our hours/minutes have overflowed
update_time:
	ori $t0, $zero, 10	#minute 0 / hour 0 overflow
	ori $t1, $zero, 6	#minute 1 overflow
	ori $t2, $zero, 3	#12 hour overflow
	ori $t3, $zero, 0x00ff	#mask

	and $t4, $s1, $t3
	beq $t4, $t0, update_time_minute_0
	nop
update_time_1:
	sll $t3, $t3, 8
	and $t4, $s1, $t3
	srl $t4, $t4, 8
	beq $t4, $t1, update_time_minute_1
	nop
update_time_2:
	sll $t3, $t3, 8
	and $t4, $s1, $t3
	srl $t4, $t4, 16
	beq $t4, $t0, update_time_hour_0
	nop
update_time_3:	#special case for 12 hour overflow
	beq $t4, $t2, update_time_hour_1
	nop

update_time_done:
	jr $ra
	nop

update_time_minute_0:
	li $t6, 0xffffff00
	and $s1, $s1, $t6
	addiu $s1, $s1, 0x0100
	j update_time_1
	nop
update_time_minute_1:
	li $t6, 0xffff00ff
	li $t7, 0x00010000
	and $s1, $s1, $t6
	addu $s1, $s1, $t7
	j update_time_2
	nop
update_time_hour_0:
	li $t6, 0xff00ffff
	li $t7, 0x01000000
	and $s1, $s1, $t6
	addu $s1, $s1, $t7
	j update_time_3
	nop
update_time_hour_1:
 	li $t6, 0x01000000
	and $t7, $t6, $s1
	bne $t6, $t7, update_time_done
	nop
	li $t6, 0x0001ffff
	and $s1, $s1, $t6
	j update_time_done
	nop
