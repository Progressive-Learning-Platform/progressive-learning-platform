#
#uart routines
#

libplp_uart_read_word:
	move $t2, $zero
	move $t3, $ra
	jal libplp_uart_read
	nop
	sll $t2, $v0, 24
	jal libplp_uart_read
	nop
	sll $t4, $v0, 16
	or $t2, $t2, $t4
	jal libplp_uart_read
	nop
	sll $t4, $v0, 8
	or $t2, $t2, $t4
	jal libplp_uart_read
	nop
	or $t2, $t2, $v0
	jr $t3
	nop

libplp_uart_write_word:
	move $t3, $ra
	move $t2, $a0
	srl $a0, $t2, 24
	jal libplp_uart_write
	nop
	srl $a0, $t2, 16
	jal libplp_uart_write
	nop
	srl $a0, $t2, 8
	jal libplp_uart_write
	nop
	move $a0, $t2
	jal libplp_uart_write
	nop
	jr $t3
	nop

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
	ori $t0, $zero, 0x00ff  #reverse the word to make it big endian
	and $t1, $t0, $a0	#least significant byte
	sll $t1, $t1, 24
	srl $a0, $a0, 8
	and $t2, $t0, $a0	#second byte
	sll $t2, $t2, 16
	srl $a0, $a0, 8
	and $t3, $t0, $a0	#third byte
	sll $t3, $t3, 8
	srl $a0, $a0, 8		#last byte in a0
	or $a0, $t1, $a0
	or $a0, $t2, $a0
	or $a0, $t3, $a0
	beq $a0, $zero, libplp_uart_write_string_done
	nop
	ori $t7, $zero, 4
libplp_uart_write_string_loop:
	jal libplp_uart_write	#write this byte
	addiu $t7, $t7, -1
	srl $a0, $a0, 8
	bne $a0, $zero, libplp_uart_write_string_loop
	nop
	beq $t7, $zero, libplp_uart_write_string_multi_word
	addiu $t8, $t8, 4	#increment for the next word
libplp_uart_write_string_done:
	jr $t9			#go home
	nop

libplp_uart_write_value_b2:
	move $t8, $ra
	move $t7, $a0
	li $t6, 0x30
	li $t5, 0x31
	li $t4, 0x80000000
	li $t3, 33
libplp_uart_write_value_b2_loop:
	and $t0, $t7, $t4
	sll $t7, $t7, 1
	addiu $t3, $t3, -1
	beq $t3, $zero, libplp_uart_write_value_b2_done
	nop
	beq $t0, $zero, libplp_uart_write_zero
	nop
	j libplp_uart_write_one
	nop

libplp_uart_write_value_b2_done:
	jr $t8
	nop

libplp_uart_write_zero:
	li $a0, 0x30
	jal libplp_uart_write
	nop
	j libplp_uart_write_value_b2_loop
	nop
libplp_uart_write_one:
	li $a0, 0x31
	jal libplp_uart_write
	nop
	j libplp_uart_write_value_b2_loop
	nop

libplp_uart_newline:
	move $t9, $ra #save the return address
	ori $a0, $zero, 0x000d	#newline
	jal libplp_uart_write
	nop
	ori $a0, $zero, 0x000a  #linefeed
	jal libplp_uart_write
	nop
	jr $t9
	nop

