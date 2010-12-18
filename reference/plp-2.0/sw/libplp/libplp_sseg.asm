#
#sseg routines
#

#base 16 font
libplp_sseg_0:
	jr $31
	ori $v0, $zero, 0x00c0
libplp_sseg_1:
	jr $31
	ori $v0, $zero, 0x00f9
libplp_sseg_2:
	jr $31
	ori $v0, $zero, 0x00a4
libplp_sseg_3:
	jr $31
	ori $v0, $zero, 0x00b0
libplp_sseg_4:
	jr $31
	ori $v0, $zero, 0x0099
libplp_sseg_5:
	jr $31
	ori $v0, $zero, 0x0092
libplp_sseg_6:
	jr $31
	ori $v0, $zero, 0x0082
libplp_sseg_7:
	jr $31
	ori $v0, $zero, 0x00f8
libplp_sseg_8:
	jr $31
	ori $v0, $zero, 0x0080
libplp_sseg_9:
	jr $31
	ori $v0, $zero, 0x0090
libplp_sseg_a:
	jr $31
	ori $v0, $zero, 0x0088
libplp_sseg_b:
	jr $31
	ori $v0, $zero, 0x0083
libplp_sseg_c:
	jr $31
	ori $v0, $zero, 0x00a7
libplp_sseg_d:
	jr $31
	ori $v0, $zero, 0x00a1
libplp_sseg_e:
	jr $31
	ori $v0, $zero, 0x0086
libplp_sseg_f:
	jr $31
	ori $v0, $zero, 0x008e

libplp_sseg_write_raw:
	lui $t0, 0xf0a0		#sseg address
	jr  $31			#return with delay slot
	sw  $a0, 0($t0)		#write to the sseg
	
libplp_sseg_write:
	move $t1, $ra		#save the return address
	move $t2, $a0		#save the word to write
	ori $t3, $zero, 0x00ff
	jal libplp_sseg_decode
	and $a0, $t2, $t3	#low order byte
	move $t4, $v0
	srl $t2, $t2, 8
	jal libplp_sseg_decode
	and $a0, $t2, $t3	#second byte
	move $t5, $v0
	srl $t2, $t2, 8
	jal libplp_sseg_decode
	and $a0, $t2, $t3	#third byte
	move $t6, $v0
	srl $t2, $t2, 8
	jal libplp_sseg_decode
	and $a0, $t2, $t3	#high order byte
	
	#bytes are in: v0, t6, t5, t4
	sll $a0, $v0, 24
	sll $t6, $t6, 16
	sll $t5, $t5, 8
	or  $a0, $a0, $t6
	or  $a0, $a0, $t5
	or  $a0, $a0, $t4
	
	#restore the return address and jump to the raw function
	#without linking
	j libplp_sseg_write_raw
	move $ra, $t1

libplp_sseg_decode:
	ori $t0, $zero, 0x00
	beq $a0, $t0, libplp_sseg_0
	ori $t0, $zero, 0x01
	beq $a0, $t0, libplp_sseg_1
	ori $t0, $zero, 0x02
	beq $a0, $t0, libplp_sseg_2
        ori $t0, $zero, 0x03
	beq $a0, $t0, libplp_sseg_3
        ori $t0, $zero, 0x04
	beq $a0, $t0, libplp_sseg_4
        ori $t0, $zero, 0x05
	beq $a0, $t0, libplp_sseg_5
        ori $t0, $zero, 0x06
	beq $a0, $t0, libplp_sseg_6
        ori $t0, $zero, 0x07
	beq $a0, $t0, libplp_sseg_7
        ori $t0, $zero, 0x08
	beq $a0, $t0, libplp_sseg_8
        ori $t0, $zero, 0x09
	beq $a0, $t0, libplp_sseg_9
        ori $t0, $zero, 0x0a
	beq $a0, $t0, libplp_sseg_a
        ori $t0, $zero, 0x0b
	beq $a0, $t0, libplp_sseg_b
        ori $t0, $zero, 0x0c
	beq $a0, $t0, libplp_sseg_c
        ori $t0, $zero, 0x0d
	beq $a0, $t0, libplp_sseg_d
        ori $t0, $zero, 0x0e
	beq $a0, $t0, libplp_sseg_e
        ori $t0, $zero, 0x0f
	beq $a0, $t0, libplp_sseg_f
	nop
	jr $31
	ori $v0, $zero, 0xff

