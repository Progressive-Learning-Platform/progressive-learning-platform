#
# libplp gpio routines
#

libplp_gpio_tristate_read:
	lui $t0, 0xf030
	jr $ra
	lw $v0, 0($t0)

libplp_gpio_tristate_write:
	lui $t0, 0xf030
	jr $ra
	sw $a0, 0($t0)

libplp_gpio_read:
	lui $t0, 0xf030
	lw $t1, 4($t0)
	lw $t2, 8($t0)
	sll $v0, $t1, 8
	or $v0, $v0, $t2
	jr $ra
	nop

libplp_gpio_write:
	lui $t0, 0xf030
	sw $a0, 8($t0)
	srl $a0, $a0, 8
	sw $a0, 4($t0)
	jr $ra
	nop
