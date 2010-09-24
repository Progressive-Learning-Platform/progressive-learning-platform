start:
	li $1, 0x20000000	#command reg
	li $2, 0x20000004	#status reg
	li $3, 0x20000008	#receive buffer
	li $4, 0x2000000c	#send buffer
	li $5, 0x40000000	#leds
read_poll:
	lw $10, 0($2)		#get status
	andi $10, $10, 0x02	#mask status read data ready
	beq $10, $0, read_poll	#loop if not ready
	nop
get_data:
	lw $11, 0($3)		#data is in reg 11
	sw $11, 0($5)		#output the word on the leds
	li $12, 0x00000002	#clear the data ready flag
	sw $12, 0($1) 		#stalls the cpu for ~2500 cycles
fail_on_ready:
	lw $10, 0($2)		#get status
	andi $10, $10, 0x02	#ready status mask
	bne $10, $0, fail_on_ready
	nop
check_cts:
	lw $10, 0($2)		#get status
	andi $10, $10, 0x01	#mask status with cts
	beq $10, $0, check_cts	#loop if not clear to send
	nop
send_data:
	sw $11, 0($4)		#store to the transmit buffer
	li $13, 0x00000001	#send the data
	sw $13, 0($1)		#stall for ~2500 cycles
	j read_poll		#start over
	nop
