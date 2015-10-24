# main source file

.org 0x10000000

main:
	li $a0, string
	jal libplp_uart_write_string
	nop

halt:
	j halt
	nop

string:
	.asciiz "This is a test string"