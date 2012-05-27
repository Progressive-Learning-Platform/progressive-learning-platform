#
#David Fritz
#
#PLP FLOAD
#

#starts in the ROM
.org 0x00000000
j startup
nop

fritz_propaganda:
	.asciiz "Hurry up Will..."
version_string:
	.asciiz "plp-4.1"
memory_test:
	.asciiz "starting memory test..."
memory_done:
	.asciiz "done."

#
#startup 
#
#this is the entrypoint for the board at power-up.
#

startup:
	jal libplp_plpid_read_frequency
	nop
	move $s2, $v0		#get the board frequency into $s2

#influence the masses
	li $a0, fritz_propaganda
	jal libplp_uart_write_string
	nop
	ori $a0, $zero, 0x000d	#newline
	jal libplp_uart_write
	nop
	ori $a0, $zero, 0x000a  #linefeed
	jal libplp_uart_write
	nop
shiny:

#seven segment
	li $a0, 0x8cc78cff
	jal libplp_sseg_write_raw
	nop

#the led routine, which waits for the switches to be non-zero
flash_leds:
	srl $s3, $s2, 4		#one eighth of the frequency into $a0
	ori $s5, $zero, 0xff00
	ori $s4, $zero, 16
flash_leds_loop:		#scroll the leds through and update the sseg
	ori $t0, $zero, 0x00ff
	beq $s5, $t0, sseg_version
	nop
flash_leds_loop2:
	move $a0, $s5
	jal libplp_leds_write
	addiu $s4, $s4, -1
	move $a0, $s3		#wait length
	jal libplp_timer_wait
	srl $s5, $s5, 1
	bne $s4, $zero, flash_leds_loop
	nop
	j switches
	nop

#more seven segment
sseg_version:
	li $a0, 0x19f9ffff
	jal libplp_sseg_write_raw
	nop
	j flash_leds_loop2
	nop

#check the switches
switches: 
	jal libplp_switches_read
	ori $s0, $zero, 1
	beq $s0, $v0, boot_uart
	nop
	ori $s0, $zero, 2
	beq $s0, $v0, boot_memory_test
	nop
	ori $s0, $zero, 4
	beq $s0, $v0, vga_memory_test
	nop
	j shiny
	nop

#boot from uart
boot_uart:
	ori $s0, $zero, 0x0061 #a
	ori $s1, $zero, 0x0064 #d
	ori $at, $zero, 0x0063 #c
	ori $s2, $zero, 0x006a #j
	ori $s3, $zero, 0x0076 #v
	ori $s4, $zero, 0x0066 #f, for fritz!
	move $s5, $zero	#the address counter

boot_uart_run:
	jal libplp_uart_read
	nop
	move $a0, $v0
	jal libplp_leds_write
	nop
	beq $v0, $s0, boot_uart_address
	nop
	beq $v0, $s1, boot_uart_data
	nop
	beq $v0, $s2, boot_uart_jump
	nop
	beq $v0, $s3, boot_uart_version
	nop
	beq $v0, $at, boot_uart_chunk
	nop
	j boot_uart_run
	nop

boot_uart_get_4_bytes:
	move $s6, $31	#save the return address
	jal libplp_uart_read
	nop
	sll $s7, $v0, 24
	jal libplp_uart_read
	nop
	sll $t0, $v0, 16
	or $s7, $s7, $t0
	jal libplp_uart_read
	nop
	sll $t0, $v0, 8
	or $s7, $s7, $t0
	jal libplp_uart_read
	nop
	or $s7, $s7, $v0
	jr $s6
	move $v0, $s7

boot_uart_address:
	jal boot_uart_get_4_bytes
	nop
	move $s5, $v0
	jal libplp_uart_write
	move $a0, $s4
	j boot_uart_run
	nop

boot_uart_data:
	jal boot_uart_get_4_bytes
	nop
	sw $v0, 0($s5)
	addiu $s5, $s5, 4
	jal libplp_uart_write
	move $a0, $s4
	j boot_uart_run
	nop

boot_uart_jump:
	jal libplp_uart_write
	move $a0, $s4
	jr $s5
	nop

boot_uart_version:
	li $a0, version_string
	jal libplp_uart_write_string
	nop
	j boot_uart_run
	nop

boot_uart_chunk:
	jal boot_uart_get_4_bytes
	nop
	move $t4, $v0 #data size
	move $t5, $zero #count
	boot_uart_chunk_loop:
		jal boot_uart_get_4_bytes
		nop
		sw $v0, 0($s5)
		addiu $s5, $s5, 4
		addiu $t5, $t5, 1
		bne $t5, $t4, boot_uart_chunk_loop
		nop
	jal libplp_uart_write
	move $a0, $s4
	j boot_uart_run
	nop

#does the memory test, but enables the vga module first
vga_memory_test:
	li $t0, 0x10000000 #ram
	li $t1, 1	   #vga module enable
	li $t2, 0xf0400000 #vga module
	sw $t0, 4($t2)
	sw $t1, 0($t2)
	#and we just fall into the memory test now

#walk across memory and make sure we can read/write to all locations
boot_memory_test:
	li $a0, memory_test
	jal libplp_uart_write_string
	nop
        jal libplp_uart_write
	ori $a0, $zero, 0x000d  #newline
        jal libplp_uart_write
        ori $a0, $zero, 0x000a  #linefeed

	li $s0, 0x10000000	#base address of memory
	li $s1, 0x11000000	#upper address of memory 
	li $s2, 0xdeadbeef	#value to write to memory

boot_memory_test_write_loop:
	sw $s0, 0($s0)
	addiu $s0, $s0, 4
	bne $s0, $s1, boot_memory_test_write_loop
	nop

	li $s0, 0x10000000
boot_memory_test_read_loop:
	lw $s3, 0($s0)

	bne $s0, $s3, boot_memory_test_fail
	nop
boot_memory_test_read_loop_fail_done:
	addiu $s0, $s0, 4
	bne $s0, $s1, boot_memory_test_read_loop
	nop

boot_memory_test_done:
	li $a0, memory_done
	jal libplp_uart_write_string
	nop
	j boot_memory_test_halt
	nop

boot_memory_test_fail:
	jal libplp_uart_write_value_b2
	move $a0, $s0

	jal libplp_uart_write
	ori $a0, $zero, 0x003a	#colon

	jal libplp_uart_write_value_b2
	move $a0, $s3
        jal libplp_uart_write
	ori $a0, $zero, 0x000d  #newline
        jal libplp_uart_write
        ori $a0, $zero, 0x000a  #linefeed

	j boot_memory_test_read_loop_fail_done
	nop

boot_memory_test_halt:
	j boot_memory_test_halt
	nop
