#
#vga routines
#

libplp_vga_enable:
	lui $t0, 0xf040
	ori $t1, $t0, 1
	jr  $ra
	sw  $t1, 0($t0)

libplp_vga_disable:
	lui $t0, 0xf040
	jr  $ra
	sw  $zero, 0($t0)

libplp_vga_framebuffer:
	lui $t0, 0xf040
	jr  $ra
	sw  $a0, 4($t0)

libplp_vga_clear:
	lui $t0, 0xf040
	lw  $t1, 4($t0) #framebuffer location
	li  $t2, 0x4b000
	addu $t2, $t2, $t1 #upper bound
libplp_vga_clear_loop:
	sw $a0, 0($t1)
	addiu $t1, $t1, 4
	bne $t1, $t2, libplp_vga_clear_loop
	nop
	jr $ra
	nop
