#
# a program to sufficiently test hazards in the
# reference design. 
#
# a successful pass through this program will
# result in all 8 LEDs illuminated
#

.org 0x0000000

j start
nop

var:
.word 0xfa17fa17
.word 0xdeadbeef

start:

move $30, $zero

#
# test 1
#
# ex->ex hazard, which should be forwarded
#
# if successful will illuminate led 0

addiu $1, $zero, 5	# $1 = 5
addiu $2, $1, 10	# $2 = 15
addu   $3, $1, $2	# $3 = 20
li    $4, 20
bne   $3, $4, test1_fail
nop
li    $1, 0x01
or    $30, $30, $1
test1_fail:

#
# test 2
#
# mem->mem hazard, using lw/sw, which should forward
#
# if successful will illuminate led 1

li $1, var
lw $2, 0($1)
sw $2, 4($1)
lw $3, 4($1)
bne $2, $3, test2_fail
nop
li $1, 0x02
or $30, $30, $1
test2_fail:

#
# test 3
#
# mem->mem hazard, using lw/sw, which should stall 
#
# if successful will illuminate led 2

li $1, var
li $4, 0xfeedbeef
sw $1, 0($1)	# the address of var is stored at var
lw $2, 0($1)
sw $4, 0($2)	
lw $3, 0($2)	# $3 = feedbeef
bne $3, $4, test3_fail
nop
li $1, 0x04
or $30, $30, $1
test3_fail:

#
# test 4
#
# mem->ex hazard, which should forward
#
# if successful will illuminate led 3

li $1, var
li $2, 10
sw $2, 0($1)	# var = 10
lw $3, 0($1)	# $3 = 10
nop
addiu $4, $3, 10 # $4 = 20
li $5, 20
bne $4, $5, test4_fail
nop
li $1, 0x08
or $30, $30, $1
test4_fail:

#
# test 5
#
# mem->ex hazard, which should stall
#
# if successful will illuminate led 4

li $1, var
li $2, 10
sw $2, 0($1)	# var = 10
lw $3, 0($1)	# $3 = 10
addiu $4, $3, 10 # $4 = 20
li $5, 20
bne $4, $5, test5_fail
nop
li $1, 0x10
or $30, $30, $1
test5_fail:

#
# test 6
#
# floating value test, which is technically a 
# ex->ex hazard.
# 
# this occurs during array summations, etc,
# and causes a constant ex->ex hazard condition
# which essentially leaves the latest value
# "floating" in the ex/mem stage
#
# if successul will illuminate led 5

move $1, $zero
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1
addiu $1, $1, 1		# $1 = 10
li $2, 10
bne $1, $2, test6_fail
li $1, 0x20
or $30, $30, $1
test6_fail:

#
# test 7
#
# which hazard to satisfy? 
# 
# a combination of two hazard events, which
# tests the priority of one hazard over another
# essentially we create two hazard conditions at once,
# an ex->ex, and a mem->ex forward condition.
# the ex->ex should take priority, since it's more 
# recent
# 
# if successful will illuminate led 6

li $1, var
li $2, 10
sw $2, 0($1)    # var = 10
lw $3, 0($1)    # $3 = 10
ori $3, $zero, 5 # $3 = 5
addiu $4, $3, 10 # $4 = 15
li $5, 15
bne $4, $5, test7_fail
nop
li $1, 0x40
or $30, $30, $1
test7_fail:

#
# always illuminate led 7
# and write to the leds
#

li $1, 0x80
or $30, $30, $1
li $2, 0xf0200000
sw $30, 0($2)

halt:
j halt
nop
