#switch routines
#
#

bsl_switches_read:
	lui $t0, 0x3000		#led address
	jr  $31			#return with delay slot
	lw  $v0, 0($t0)		#read the leds into $v0