org 0
.entrypoint test1
test1:
	lui $11,0x4000
	ori $11,$11,0x0000	# leds are at 0x40000000
	lui $1,0xdead
	ori $1,$1,0xbeef
	add $2,$1,$0
	addi $2,$2,1		# 0xdeadbeef+1
	sub $3,$2,$1
	ori $10,$3,0x10	# test 0
	sw $10,0($11)		# output to the leds
	jal wait_1_second
	nop
test2:
	add $1,$0,$0		# clear reg 1
	add $2,$0,$0		# clear reg 2
	add $3,$0,$0		# clear reg 3
	add $10,$0,$0		# clear reg 10
	ori $1,$0,0x000f
	ori $2,$0,0x00aa
	and $3,$1,$2		# $3 should = 0x0000000a	
	ori $10,$3,0x20	# test 2
	sw $10,0($11)		# output to the leds
	jal wait_1_second
	nop
test3:
	add $1,$0,$21		# the one second register value
	srl $1,$1,1		# divide by 2 for a half second delay
	addi $2,$0,0xaa	
	sll $2,$2,4		# we'll flash this on the leds
	addi $7,$0,0
	addi $8,$0,10		# 10 outer loops
test3_loop_outer:
	addi $7,$7,1
	nor $2,$2,$0		# invert $2
	sw $2,0($11)
	jal test3_loop_inner
	nop
	bne $7,$8,test3_loop_outer
	nop
	j test4
	nop
test3_loop_inner:
	add $3,$0,$0		# clear reg 3
test3_loop_inner_t:
	addi $3,$3,1
	beq $3,$1,test3_loop_inner_done
	nop
	j test3_loop_inner_t
	nop
test3_loop_inner_done:
	jr $31
	nop
test4:		
	ori $12,$0,0xa
	srl $13,$12,0x2
	or $14,$12,$13	# $14 should be 0x0f
	ori $14,$14,0x40	# test 4
	sw $14,0($11)
	jal wait_1_second
	nop
test5:
	ori $1,$0,5
	ori $2,$0,-6
	slt $3,$2,$1		# this SHOULD set
	sltu $4,$1,$2		# this SHOULD set
	slt $5,$1,$2		# this SHOULDN'T set
	sltu $6,$2,$1		# this SHOULDN'T set
	sll $4,$4,1
	sll $5,$5,2
	sll $6,$6,3
	or $3,$3,$4
	or $3,$3,$5
	or $3,$3,$6		# $3 should be 0x03
	ori $3,$3,0x50	# test 5
	sw $3,0($11)
	jal wait_1_second
	nop
test6:
	slti $3,$1,-6		# this SHOULDN'T set
	sltiu $4,$2,5		# this SHOULDN'T set
	slti $5,$2,5		# this SHOULD set
	sltiu $6,$1,-6	# this SHOULD set
	sll $4,$4,1
	sll $5,$5,2
	sll $6,$6,3
	or $3,$3,$4
	or $3,$3,$5
	or $3,$3,$6		# $3 should be 0x0c
	ori $3,$3,0x60	# test 6
	sw $3,0($11)
	jal wait_1_second
	nop	
done_and_start_over:
	j test1	
	nop
wait_1_second:
	add $20,$0,$0
	lui $21,0x7f		# 25e6 / 3 (top 16 bits)
wait_loop:
	addi $20,$20,1
	bne $20,$21,wait_loop
	nop
	jr $31
	nop	
