#
#david fritz
#
#3.29.2010
#
#a program that displays "hello world!" and then just echoes any user input
#

.org 0x10000000			#start in RAM
j entrypoint			#jump to our entry point
nop

.include bsl_uart.asm		#uart bsl, found in /opt/plp/software/asm

hello_world:
.asciiz "hello world!"

entrypoint:
	#first load a pointer to hello world and write it to the uart
	li	$a0, hello_world
	jal	bsl_uart_write_string
	nop
	ori 	$a0, $zero, 0x0d	#carriage return
	jal	bsl_uart_write
	nop
	ori	$a0, $zero, 0x0a	#line feed
	jal	bsl_uart_write
	nop

	#now just fall into an infinite loop, echoing any input
loop:
	jal	bsl_uart_read
	nop
	move	$a0, $v0
	jal	bsl_uart_write
	nop
	j	loop
	nop

	#we shouldn't ever get here
