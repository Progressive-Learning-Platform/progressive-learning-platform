#led routines
#
#

bsl_leds_write:
	lui $t0, 0x4000		#led address
	jr  $31			#return with delay slot
	sw  $a0, 0($t0)		#write to the leds
	
bsl_leds_read:
	lui $t0, 0x4000		#led address
	jr  $31			#return with delay slot
	lw  $v0, 0($t0)		#read the leds into $v0
