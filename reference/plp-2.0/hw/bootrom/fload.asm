#David Fritz
#
#Bootloader for the S3e starter board
#
#v0

#starts in the ROM
.org 0x0 
j startup
nop

.include bsl_wait.asm
.include bsl_leds.asm

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
	addiu $s4, $s4, -1
	move $a0, $s3		#wait length
	jal bsl_wait
	srl $s5, $s5, 1
	bne $s4, $zero, flash_leds_loop
	nop
j startup
	nop

