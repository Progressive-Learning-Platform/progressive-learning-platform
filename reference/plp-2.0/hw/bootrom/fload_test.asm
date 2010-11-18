#working on the pipelined machine so i'll write something that has no hazards and won't depend on forwarding


.org 0
top:
ori $1, $0, 0x4f
nop
nop
nop
nop
nop
nop

lui $2, 0x4000
nop
nop
nop
nop
nop
nop

sw $1, 0($2)
nop
nop
nop
nop
nop
nop

j top
nop
nop
nop
nop
nop
nop
