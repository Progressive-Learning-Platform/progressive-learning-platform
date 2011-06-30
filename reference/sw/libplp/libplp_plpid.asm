#
#plpid routines
#

libplp_plpid_read_boardid:
	lui $t0, 0xf050
	jr $31
	lw $v0, 0($t0)

libplp_plpid_read_frequency:
	lui $t0, 0xf050
	jr $31
	lw $v0, 4($t0)

