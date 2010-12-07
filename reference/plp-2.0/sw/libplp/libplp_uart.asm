#
#uart routines
#

libplp_uart_read:
	lui $t0, 0xf000		#base address for the uart
libplp_uart_read_loop:
	lw  $t1, 4($t0)		#get the uart status
	andi $t1, $t1, 0x02	#mask for the data ready bit
	beq $t1, $zero, libplp_uart_read_loop
	nop
	lw  $v0, 8($t0)		#data is ready, get it!
	sw  $t1, 0($t0)		#clear the ready flag
	jr $31
	nop

libplp_uart_write:
	lui $t0, 0xf000		#uart base address
libplp_uart_write_loop:
	lw  $t1, 4($t0)		#get the uart status
	andi $t1, $t1, 0x01	#mask for the cts bit
	beq $t1, $zero, libplp_uart_write_loop
	nop
	sw  $a0, 12($t0)	#write the data to the output buffer
	sw  $t1, 0($t0)		#send the data!
	jr $31
	nop

libplp_uart_write_string: 		#we have a pointer to the string in a0, just loop and increment until we see a \0
	move $t9, $31		#save the return address
	move $t8, $a0		#save the argument
libplp_uart_write_string_multi_word:
	lw $a0, 0($t8)		#first 1-4 characters
	beq $a0, $zero, libplp_uart_write_string_done
	nop
libplp_uart_write_string_loop:
	ori $t7, $zero, 4
	jal libplp_uart_write	#write this byte
	addiu $t7, $t7, -1
	srl $a0, $a0, 8
	bne $a0, $zero, libplp_uart_write_string_loop
	nop
	bne $t7, $zero, libplp_uart_write_string_multi_word
	addiu $t8, $t8, 4	#increment for the next word
libplp_uart_write_string_done:
	jr $t9			#go home
	nop
