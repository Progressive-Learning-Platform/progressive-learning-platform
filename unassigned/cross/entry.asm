#entrypoint header for generated C files
#2/2/2011
#fritz

.org 0x10000000

#setup the stack and frame pointer
li $sp, 0x10fffffc
move $fp, $sp

j main
nop
