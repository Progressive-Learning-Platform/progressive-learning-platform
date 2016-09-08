#
# simple program to write a value to the leds
#

#start in RAM
.org 0x10000000 

start:
	ori $a0, $zero, 0xdf	#put 0xdf in the argument register
	jal libplp_leds_write	#write it to the leds
	nop
	
halt:				#halt
	j halt
	nop
