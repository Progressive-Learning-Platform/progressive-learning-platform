	.file   1 "hello.c"
        .section .mdebug.abi32
        .previous
        .abicalls
        .text
        .align  2
        .globl  function
        .ent    function
        .type   function, @function
function:
        .frame  $fp,8,$31               # vars= 0, regs= 1/0, args= 0, gp= 0
        .mask   0x40000000,-8
        .fmask  0x00000000,0
        .set    noreorder
        .cpload $25
        .set    reorder
        addiu   $sp,$sp,-8
        sw      $fp,0($sp)
        move    $fp,$sp
        sw      $4,8($fp)
        sw      $5,12($fp)
        lw      $3,8($fp)
        lw      $2,12($fp)
        addu    $2,$3,$2
        move    $sp,$fp
        lw      $fp,0($sp)
        addiu   $sp,$sp,8
        j       $31
        .end    function
        .rdata
        .align  2
$LC0:
        .ascii  "hello world!\n\000"
        .text
        .align  2
        .globl  main
        .ent    main
        .type   main, @function
main:
        .frame  $fp,48,$31              # vars= 16, regs= 2/0, args= 16, gp= 8
        .mask   0xc0000000,-4
        .fmask  0x00000000,0
        .set    noreorder
        .cpload $25
        .set    reorder
        addiu   $sp,$sp,-48
        sw      $31,44($sp)
        sw      $fp,40($sp)
        move    $fp,$sp 
        .cprestore      16
        la      $2,$LC0
        sw      $2,24($fp)
        li      $2,5                    # 0x5
        sw      $2,28($fp)
        lw      $3,28($fp)
        lw      $2,28($fp)
        addu    $2,$3,$2
        sw      $2,32($fp)
        lw      $4,28($fp)
        li      $5,200                  # 0xc8
        jal     function
        sw      $2,36($fp)
        move    $2,$0
        move    $sp,$fp
        lw      $31,44($sp)
        lw      $fp,40($sp)
        addiu   $sp,$sp,48
        j       $31
        .end    main
        .ident  "GCC: (GNU) 3.4.4"

