.org 0x10000000
j start
nop

.include ../libplp/libplp_vga.asm

start:
	jal libplp_vga_enable
	nop
	
	li $a0, 0x10f00000
	jal libplp_vga_frame_buffer
	nop

loop:
	li $a0, 0xffffffff
	jal libplp_vga_clear

	li $a0, 0x02020202
	jal libplp_vga_clear
	nop
	
	j loop
	nop

	

