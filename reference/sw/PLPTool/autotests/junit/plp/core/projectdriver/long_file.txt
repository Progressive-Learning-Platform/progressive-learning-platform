# main source file

.org 0x10000000

j main
nop

s_uart:
	.asciiz "uart interrupt: "
s_button:
	.asciiz "button interrupt\n"

main:
	li $t0, 0xf0200000
	sw $zero, 0($t0)

	#k0 should have the isr
	li $iv, isr
	li $t0, 0xffffffff
	li $t1, 25000000 #1 second

	subu $a0, $t0, $t1
	jal libplp_timer_write
	nop

	#enable interrupts
	li $t0, 0xf0700000
	li $t1, 15
	sw $zero, 4($t0)
	sw $t1, 0($t0)

loop:
	j loop
	nop

isr:
	#what kind of interrupt is this?
	li $t0, 0xf0700000
	lw $t1, 4($t0)
	li $t2, 8
	and $t5, $t2, $t1 #button interrupt
	li $t2, 4
	and $t3, $t2, $t1 #uart interrupt
	li $t2, 2
	and $t4, $t2, $t1 #timer interrupt
	bne $zero, $t3, isr_uart
	nop
	bne $zero, $t4, isr_timer
	nop
	bne $zero, $t5, isr_button
	
isr_return:
	#reenable interrupts
	li $i0, 0xf0700000
	li $i1, 15
	
	jr $ir
	sw $i1, 0($i0)

isr_button:
	#sleep for a while to debounce (without using the timer, since it's already in use)
	move $t0, $zero
	li $t1, 100
isr_button_loop:
	addiu $t0, $t0, 1
	bne $t0, $t1, isr_button_loop
	nop

	li $a0, s_button
	jal libplp_uart_write_string
	nop

	#clear the button status bit
	li $t0, 0xf0700000
	li $t1, 0xfffffff7
	lw $t2, 4($t0)
	and $t2, $t2, $t1
	sw $t2, 4($t0)

	j isr_return
	nop	

isr_uart:
	jal libplp_uart_read
	nop
	move $s0, $v0
	li $a0, s_uart
	jal libplp_uart_write_string
	nop
	move $a0, $s0
	jal libplp_uart_write
	nop
	jal libplp_uart_newline
	nop

	#clear the uart status bit
	li $t0, 0xf0700000
	li $t1, 0xfffffffb
	lw $t2, 4($t0)
	and $t2, $t2, $t1
	sw $t2, 4($t0)

	j isr_return
	nop

isr_timer:
	li $t0, 0xf0200000
	lw $t1, 0($t0)
	addiu $t1, $t1, 1
	sw $t1, 0($t0)
	li $t0, 0xffffffff
	li $t1, 25000000 #1 second
	subu $a0, $t0, $t1
	jal libplp_timer_write
	nop

	#clear the timer status bit
	li $t0, 0xf0700000
	li $t1, 0xfffffffd
	lw $t2, 4($t0)
	and $t2, $t2, $t1
	sw $t2, 4($t0)

	j isr_return
	nop