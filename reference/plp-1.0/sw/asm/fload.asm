#David Fritz
#
#Bootloader for the S3e starter board
#
#v0

#starts in the ROM
.org 0x0 
j startup
nop

welcome_string:
	.asciiz "PLP-0"

.include bsl_wait.asm
.include bsl_leds.asm
.include bsl_switches.asm
.include bsl_uart.asm

#startup 
#
#this is the entrypoint for the board at power-up.
#we'll do some initialization here (write the board version and frequency to the rs232,
#flash the leds), and check the switch position. the switches tell us how to boot.
#

startup:
	lui $s0, 0x8000		#address for the board id	
	lw  $s2, 4($s0)		#get the board frequency into $s2
flash_leds:
	srl $s3, $s2, 3		#one eighth of the frequency into $a0
	ori $s5, $zero, 0xff00
	ori $s4, $zero, 16
flash_leds_loop:		#scroll the leds through
	move $a0, $s5
	jal bsl_leds_write
	addi $s4, $s4, -1
	move $a0, $s3		#wait length
	jal bsl_wait
	srl $s5, $s5, 1
	bne $s4, $zero, flash_leds_loop
	nop
rs232_hello:
	jal bsl_uart_write_string
	li $a0, welcome_string
	ori $a0, $zero, 0x0d0a	#line feed, carriage return
	jal bsl_uart_write
	nop
	srl $a0, $a0, 8
	jal bsl_uart_write
	nop	
check_switches:
	jal bsl_switches_read	#get the switch parameters
	ori $s0, $zero, 1
	beq $zero, $v0, startup	#boot from flash (not implemented yet)
	beq $s0, $v0, rs232	#boot from rs232
	nop
j startup
	nop


#rs232 handler
#there are only 4 commands that can be sent to this routine:
#identify, org, data, and jump. 
#
#identify - makes the board respond with deadbeef
#[i]
#
#org - set the address counter to the incoming address
#[o][4 byte address]
#
#data - write the incoming data to the address in the address counter and increment
#[d][4 byte data]
#
#jump - to the org address
#[j]

#we'll use the following registers
#s0 for the org value
#s1 for the address counter
#s2 for the incoming address/data
#s3,4,5 for the commands to compare with
#s6 for the get 4 bytes counter
#s7 for the return address backup
#at for the identify command
rs232:
	li $s3, 0x0000006f	#org command
	li $s4, 0x00000064	#data command
	li $s5, 0x0000006a	#jump command
	li $at, 0x00000069	#id command
rs232_run:	
	jal bsl_uart_read	#get a byte of data
	nop
	move $a0, $v0
	jal bsl_leds_write
	nop
	beq $v0, $at, rs232_id   #got an identify command
	beq $v0, $s3, rs232_org	 #got an org command
	beq $v0, $s4, rs232_data #got a data command
	beq $v0, $s5, rs232_jump #got a jump command
	nop
	j rs232_run		#error on the incoming data, start over
rs232_get_4_bytes:
	move $s7, $31		#save the return address
	ori $s6, $zero, 4	#counter for the for loop
	or $s2, $zero, $zero	#clear s2
rs232_get_4_bytes_loop:
	sll $s2, $s2, 8		#shift over one byte
	jal bsl_uart_read	#get a byte
	addi $s6, $s6, -1	#decrement the counter
	jal bsl_leds_write	#for extra shinyness
	move $a0, $v0
	or $s2, $s2, $v0
	bne $s6, $zero, rs232_get_4_bytes_loop
	nop
	jr $s7			#go home
	nop
rs232_id:
	li $a0, 0x30706c70	# "plp0" in hex (backwards)
	jal bsl_uart_write	#send a byte
	nop
	jal bsl_uart_write	#send a byte
	srl $a0, $a0, 8
	jal bsl_uart_write	#send a byte
	srl $a0, $a0, 8	
	jal bsl_uart_write	#send a byte
	srl $a0, $a0, 8	
	j rs232_run
	nop
rs232_org:			#org command
	jal rs232_get_4_bytes	#get 4 bytes into $t2
	nop
	move $s0, $s2
	move $s1, $s2		#make a copy for counting
	j rs232_run
	nop
rs232_data:
	jal rs232_get_4_bytes	#get 4 bytes into $t2
	nop
	sw $s2, 0($s1)		#write the data
	addi $s1, $s1, 4	#increment the address counter
	j rs232_run
	nop
rs232_jump:			#start the program
	jr $s0
	nop	

