.org 0x10000000

j start
nop

memory_test:
        .asciiz "starting memory test..."
memory_done:
        .asciiz "done."

start:

#walk across memory and make sure we can read/write to all locations
boot_memory_test:
        li $a0, memory_test
        jal libplp_uart_write_string
        nop
        jal libplp_uart_write
        ori $a0, $zero, 0x000d  #newline
        jal libplp_uart_write
        ori $a0, $zero, 0x000a  #linefeed

        li $s0, end_of_program  #base address of memory
        li $s1, 0x11000000      #upper address of memory
        li $s2, 0xdeadbeef      #value to write to memory

boot_memory_test_write_loop:
        sw $s0, 0($s0)
        addiu $s0, $s0, 4
nop
nop
nop
nop
nop
nop
nop
nop
nop

        bne $s0, $s1, boot_memory_test_write_loop
        nop

        li $s0, end_of_program
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
        ori $a0, $zero, 0x003a  #colon

        jal libplp_uart_write_value_b2
        move $a0, $s3
        jal libplp_uart_write
        ori $a0, $zero, 0x000d  #newline
        jal libplp_uart_write
        ori $a0, $zero, 0x000a  #linefeed

        j boot_memory_test_read_loop_fail_done
        nop

boot_memory_test_halt:
        j start
        nop




