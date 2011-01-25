#
# simple program to write a value to the leds
#

#start in RAM
.org 0x10000000 

#jump to the program start
	j start
	nop

#include the LED driver
.include ../libplp/libplp_leds.asm

start:
	ori $a0, $zero, 0xdf	#put 0xdf in the argument register
	jal libplp_leds_write	#write it to the leds
	nop
	
halt:				#halt
	j halt
	nop
