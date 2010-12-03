#starts in the RAM
.org 0x10000000 
j startup
nop

.include bsl_wait.asm
.include bsl_leds.asm

startup:
	lui $s0, 0x8000		#address for the board id	
	lw  $s2, 4($s0)		#get the board frequency into $s2
loop:
	ori $a0, $zero, 0xaa
	jal bsl_leds_write
	nop
	srl $a0, $s2, 4		#quarter second
	jal bsl_wait
	nop
	move $a0, $zero
	jal bsl_leds_write
	nop
	srl $a0, $s2, 4
	jal bsl_wait
	nop
	j loop
	nop
