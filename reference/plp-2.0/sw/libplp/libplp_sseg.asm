#
#sseg routines
#

libplp_sseg_write_raw:
	lui $t0, 0xf0a0		#sseg address
	jr  $31			#return with delay slot
	sw  $a0, 0($t0)		#write to the sseg
	

