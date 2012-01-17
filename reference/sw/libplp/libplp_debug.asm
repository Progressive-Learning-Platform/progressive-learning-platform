##########################
#
# libplp_debug
#
# plp in circuit debugger
#
# fritz 11.11.11
#
##########################

#
# usage:
#
# include libplp_debug.asm in your project
# and call libplp_debug_init after doing
# the following:
# 
# setup the stack
# setup and enable any wanted interrupts
#
# libplp_debug uses the uart, and the uart 
# interrupt, and will not currently forward
# non-debugger related uart traffic to the 
# user program
#

libplp_debug_user_int:
.word 0

libplp_debug_init:
	# copy the user interrupt vector and inject our own
	swm $i0, libplp_debug_user_int
	li $i0, libplp_debug_int

	#enable the uart interrupt and enable interrupts
	lwm $t0, 0xf0700000
	ori $t0, $t0, 0b101
	swm $t0, 0xf0700000
	jr $ra
	nop

libplp_debug_int:
	save
	push $ra

	# first check if it's a uart interrupt
	lwm $t0, 0xf0700004
	andi $t0, $t0, 0b100
	bne $t0, $zero, libplp_debug_int_valid
	nop
	# it's a different, interrupt, let it through
	pop $ra
	restore
	lwm $gp, libplp_debug_user_int
	jr $gp
	nop
	libplp_debug_int_valid:
		# debugger is enabled!
		jal libplp_debug_debugger
		nop

	pop $ra
	restore

	# debugger interrupt epilogue
	li $gp, 0xf0700000
	lw $fp, 4($gp)
	andi $fp, $fp, 0b011
	sw $fp, 4($gp)
	lw $fp, 0($gp)
	ori $fp, $fp, 0x1
	jr $i1
	sw $fp, 0($gp)

libplp_debug_debugger:
	# the main loop for the debugger
	# this process listens on the uart for 
	# commands which are:
	#
	# r - read, which is followed by:
	#   r <num>      - reg number
	#   b <address>  - bus address
	# w - write, which is followed by:
	#   r <num> <value>     - reg number
	#   b <address> <value> - bus address
	# s - step
	# b <address> - set breakpoint
	# c <address> - clear breakpoint
	# x - exit/run (leaves the debugger)
	#
	# NOTE: there is no error handling here, so make
	# sure you send correct data!
	move $s0, $ra

	libplp_debug_debugger_loop:

	jal libplp_uart_read
	nop

	li $t0, 'r'
	li $t1, 'w'
	li $t2, 's'
	li $t3, 'b'
	li $t4, 'c'
	li $t5, 'x'	

	beq $v0, $t0, libplp_debug_read
	nop
	beq $v0, $t1, libplp_debug_write
	nop
	#beq $v0, $t1, libplp_debug_step
	#nop
	#beq $v0, $t1, libplp_debug_break
	#nop
	#beq $v0, $t1, libplp_debug_clear
	#nop
	# otherwise we just bail
	jr $s0
	nop

libplp_debug_read:
	jal libplp_uart_read
	nop

	li $t0, 'r'

	beq $v0, $t0, libplp_debug_read_reg
	nop
	# read bus address
	jal libplp_uart_read_word
	nop
	lw $a0, 0($v0)
	j libplp_debug_read_done
	nop

	libplp_debug_read_reg:
		jal libplp_uart_read
		nop
		# reg number is in $v0
		# register offset in the stack is (32-n * 4)
		li $t0, 32
		subu $t0, $t0, $v0
		sll $t0, $t0, 2
		addu $t0, $t0, $sp
		lw $a0, 0($t0)	# register value
	
	libplp_debug_read_done:	
		jal libplp_uart_write_word
		nop
	j libplp_debug_debugger_loop
	nop

libplp_debug_write:
	jal libplp_uart_read
	nop

	li $t0, 'r'
	
	beq $v0, $t0, libplp_debug_write_reg
	nop
	# write bus address
	jal libplp_uart_read_word
	nop
	move $s1, $v0 #address
	jal libplp_uart_read_word
	nop
	sw $v0, 0($s1)
	j libplp_debug_write_done
	nop

	libplp_debug_write_reg:
		jal libplp_uart_read
		nop
		move $s1, $v0
		jal libplp_uart_read_word
		nop
		# reg number is in $v0
		# register offset in the stack is (32-n * 4)
		li $t0, 32
		subu $t0, $t0, $s1
		sll $t0, $t0, 2
		addu $t0, $t0, $sp
		sw $v0, 0($t0)	# register value
	
	libplp_debug_write_done:
		j libplp_debug_debugger_loop
		nop