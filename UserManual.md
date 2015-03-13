# Introduction #

The Progressive Learning Platform (PLP) utilized on the PLP Board provides a unique learning platform designed to be simple, open, and useful for education.

This document serves as a detailed guide for developers for the Progressive Learning Platform System on a Chip (SoC).

# Software Tools (PLPTool) #

PLPTool is the software suite for the Progressive Learning Platform that incorporates an assembler, a board simulator, and a board programming interface.

## Getting PLPTool ##

Always make sure to run the latest version of PLPTool. Not only does it offer new features or bug fixes, this manual is also tailored to the newest version and certain sections may not be applicable to older versions.

The latest version of PLPTool (4.1.2) is available in the [Downloads](http://code.google.com/p/progressive-learning-platform/downloads/list) section. The .zip file downloaded contains the following directories:
  * **hw** - Hardware images for the CPU (for all 3 supported boards)
  * **sw** - Software tools (including PLPTool), example programs, and the software libraries
    * **PLPTool**  - PLPTool (Windows 32/64-bit, and Mac/Linux)
    * **libplp**   - PLP Software Libraries
    * **examples** - Example Programs

[Back to the top](UserManual.md)

## Running PLPTool ##

There are several requirements for running PLPTool:
  * A Java Runtime Environment (JRE) that compiles with at least Java 2 Platform SE 5 (1.5)
    * If you aren't sure if you meet this requirement, or you do not have a compatible JRE, you can download the latest version [here](http://www.oracle.com/technetwork/java/javase/downloads/index.html).
  * RXTX Library for Serial Communication
    * If you are using Windows, RXTX Library is bundled with PLPTool.
    * If you are using Mac/Linux, you must install the library manually.
      * If you are on Mac OS, follow the instructions [here](http://rxtx.qbang.org/wiki/index.php/Installation_on_MacOS_X).
      * If you are running Ubuntu, open a terminal window and type `apt-get install librxtx-java`. This may require root access on some machines.
      * If you are running another distribution of Linux, go [here](http://rxtx.qbang.org/wiki/index.php/Installation_on_Linux) for instructions on how to install the RXTX library.
  * If you are running Windows, you must know whether it is 32-bit or 64-bit.
    * To do this, press the "Start" button, right click on "Computer", and click "Properties". From there, you should see either a 32-bit or 64-bit system type.

Once the above requirements are met, you are ready to run PLPTool.

First, you must extract the .zip file downloaded earlier containing PLPTool. After that, navigate to /sw/PLPTool/.

To run PLPTool:
  * If you are using Windows 32-bit, you need to run the batch file _PLPToolWin32.bat_.
  * If you are using Windows 64-bit, you need to run the batch file _PLPToolWin64.bat_.
  * If you are using Mac/Linux, you must use the Command Line to launch PLPTool.
    * You can also do this in Windows.

[Back to the top](UserManual.md)



## Launching PLPTool with the Command Line ##

In order to launch PLPTool, open a terminal (or Command Prompt) and navigate to where you previously extracted the PLP .zip file. Once you get to the PLP folder, navigate to /sw/PLPTool. From there, you can launch PLPTool with several options.

  * To launch PLPTool:
    * For Windows 32-Bit, type:
```
   PLPToolWin32.bat
```
    * For Windows 64-Bit, type:
```
   PLPToolWin64.bat
```
    * For Mac/Linux, type:
```
   java -jar PLPToolStatic.jar
```


  * To open a .plp project with PLPTool:
    * For Windows 32-Bit, type:
```
   PLPToolWin32.bat <.plp file to open>
```
    * For Windows 64-Bit, type:
```
   PLPToolWin64.bat <.plp file to open>
```
    * For Mac/Linux, type:
```
   java -jar PLPToolStatic.jar <.plp file to open>
```

  * To list the source files contained in a .plp file, type:
```
   java -jar PLPToolStatic.jar -plp <.plp file>
```

The -plp `<plpfile>` command can also take additional arguments that can be used to manipulate the project file without launching PLPTool. These additional arguments are:

| **Command Line Argument** | **Description** |
|:--------------------------|:----------------|
| -c `<asm 1>` `<asm 2>` ...| Creates `<plpfile>` and imports `<asm 1>`, `<asm 2>`, ... to the project |
| -p `<port>` | Programs `<plpfile>` to the serial port |
| -a | Performs an assembly of the source files inside `<plpfile>` |
| -i `<asm 1>` `<asm 2>` ... | Imports <asm 1>, <asm 2>, ... into `<plpfile>` project file |
| -d `<directory>` | Import all files in `<directory>` to the `<plpfile>` project file |
| -e `<index>` `<file>` | Exports the source file with the index `<index>` as `<file>` |
| -r `<index>` | Removes the source file with the index `<index>` |
| -s `<index>` | Set the source file with the index `<index>` as the main program |
| -m `<index>` `<new index>` | Set `<new index>` for the source file with the index `<index>` |


[Back to the top](UserManual.md)


## PLPTool Graphical User Interface (GUI) ##

PLPTool starts in the development environment view, displaying the current open project, files in the project, and a status/console window. From here, you can import, remove, and create new assembly files, assemble the current project, go into simulation mode, and program the PLP board.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_panes.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_panes.png)

The **Project Pane** contains all the source files in the project. The **Editor Pane** displays the contents of the currently open source file. The **Output Pane** displays status, warning, and error messages.



[Back to the top](UserManual.md)


## Simulator ##

PLPTool includes a cycle-accurate simulator that can be accessed through the GUI or via a command-line argument.

  * To launch the simulator from within PLPTool, press the "Simulator" toggle button.
  * To launch a text-based simulator from the command line, type:
```
   java -jar PLPToolStatic.jar -s <.plp file>
```

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_buttons.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_buttons.png)

Simulation mode adds additional controls to the GUI window.
  * The first three buttons are the the single cycle increment (step), run, and reset buttons.
    * Step (F5) will advance the simulation by one cycle.
    * Run (F7) will continuously run the simulation.
    * Reset (F9) will return the CPU to the reset state, as well as reset all the registers to zero.
  * The Floating Simulator Control Window allows you to stop and start/step the simulation, as well as control the step size.
  * The CPU View button will display the CPU window where you can view and modify register file contents, see disassembly listing, and access the debug console.
  * The Watcher Window button will display a window that allows the user to monitor the content of individual register addresses and buses.


[Back to the top](UserManual.md)


### I/O Device Toggles ###

In addition to the step, run, reset, floating simulator control, CPU view, and Watcher window buttons, there are eight input/output (I/O) device toggles that will appear.

http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/IO_devices.PNG

_LEDs_

Pressing the LEDs toggle button will bring up the window shown below. It displays the LEDs that are currently lit as a result of your code. It also displays the memory address of the LEDs, as well as the value stored at that location.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_led.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_led.png)

_Switches_

Pressing the Switches toggle button will bring up the window shown below. It displays an interactive window that allows you to select which switches you want to toggle on or off. It also displays the memory address of the switches, as well as the value stored at that location.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_switch.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_switch.png)

_Seven Segment Displays_

Pressing the Seven Segment Displays toggle button will bring up the window shown below. It displays the segments currently lit on the seven segment displays. It also displays the memory address of the seven segment displays, as well as the value stored at that location.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_sseg.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_sseg.png)

_UART_

Pressing the UART toggle button will bring up the window shown below. It displays a window that allows you to send and receive data through an emulated serial port. It gives you three options of sending the data:

  * ASCII String
  * 1-byte raw
  * Space-delimited raw

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_uart.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_uart.png)

_VGA_

Pressing the VGA toggle button will bring up the window shown below. It displays a VGA window with a resolution of 640x480, where an image is displayed if your code writes to the VGA's memory address. It also displays whether VGA is enabled or disabled, as well as the current frame pointer.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_vga.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_vga.png)

_PLPID_

Pressing the PLPID toggle button will bring up the window shown below. It displays a window that shows the frequency of the PLP Board you are using. This is useful for implementing a wait routine within your program.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_plpid.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_plpid.png)

_GPIO (General Purpose Input/Output)_

Pressing the GPIO toggle button will bring up the window shown below. It displays an interactive window with the 16 enabled GPIO ports (separated into Port A and Port B) for the emulated PLP Board. It also displays the contents of the tristate register.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_gpio.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_gpio.png)

_Button Interrupt_

The Button Interrupt toggle triggers a jump in the program to your code's interrupt service routine (ISR). No window is displayed.

### Breakpoints ###

A breakpoint is a line in the program where execution halts. This useful for testing and debugging.

The breakpoint in PLP can be set by double-clicking the line number where you want the program to halt. However, a breakpoint can only be set on a line where an instruction is present, meaning it cannot be set on a blank line.

To clear an existing breakpoint, you can double-click on the line number where the breakpoint is located. You can also clear all the breakpoints by going to   `Simulation`   on the menu bar, and clicking the   `Clear Breakpoints`   option. This can also be done by pressing   `Ctrl + B`.

[Back to the top](UserManual.md)



## Programming the PLP Board ##

In order to program the PLP Board, the PLP Board must be powered on and connected to the computer via a serial connection. It must also have the correct hardware configuration and bootloader (fload) on the board, as well as be in programming mode.  If your PLP Board is not correctly programmed, refer to this [section](UserManual#Loading_the_Hardware_Configuration_and_Bootloader.md) which details how to program the PLP Board with the proper hardware configuration and `fload` bootloader.  To enter programming mode, switch 0 (indicated by SW0 above the switch) must be up/on.

After this is all completed, press the `Program PLP Board` button. This will bring up the window below.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_program.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_program.png)

This window allows you to select which port your PLP Board is connected to. There are four preloaded options (COM1, COM2, COM3, and COM4) that can be selected via the drop down menu. If the port you are using isn't listed, you can enter in the port by clicking in the text box and typing the port's name.

Once you have selected the correct port, click `Download Program`, and the program will be loaded onto the PLP Board. If there are any errors, they will be displayed in the Output Pane of PLPTool.


[Back to the top](UserManual.md)


# Instruction Set and Assembly Language #

This section describes all the instructions and pseudo-instructions supported by the PLP system. It also gives examples on how to use each instruction and notes on any limitations.

## Syntax ##

This section describes how operations and pseudo-operations should be formatted in order to work within the PLP system.

[Back to the top](UserManual.md)



### Instructions ###

Instructions are written in the format: `<opcode> <destination>, <operands>`.

_Note_: This varies slightly for each type of instruction.

  * R-type, meaning a register type instruction, have an opcode, a destination, and two operands (arguments).
    * Arithmetic and logical r-type instructions have 2 registers as operands.
      * For example, to add register $s0 and $s1 and store the sum in $t0, the instruction would be:
```
add $t0, $s0, $s1
```
    * Shift r-type instructions have a register and a shift amount as operands.
      * For example, to shift $s0 left 5 bits and store the result into $t0, the instruction would be:
```
sll $t0, $s0, 5
```

  * I-type, meaning immediate type instruction, have an opcode, two register arguments, and an immediate argument.
    * For example, to perform a logical OR on $s0 with the value `0xfeed`, and store the result in $t0, the instruction would be:
```
ori $t0, $s0, 0xfeed
```
      * Notice the immediate field (`0xfeed`) is written in hexadecimal. It can be written in binary, hexadecimal, or decimal format.
        * The prefix for binary is `0b`.
        * The prefix for hexadecimal is `0x`.
        * There is no prefix for decimal. It is the standard base-10 number system.
  * J-type, meaning jump type instruction, only have an opcode followed by one argument. The argument must be a label.
    * For example, to jump to a different function, the code would be:
```
j my_function
```

  * Branch instructions have an opcode, two register arugments, and an immediate field that must be a label.
    * For example, if you want to jump to the `loop` function if the contents of $s0 and $s1 are equal, the could would be:
```
beq $s0, $s1, loop
```

[Back to the top](UserManual.md)



### Memory Organization ###
In order to resolve branch and jump targets, the user must inform the assembler where program starts in memory before any instructions, labels, or includes are written/executed.

The format for this would be `.org` followed by the address in memory desired. The address must be word aligned, meaning it must a 32-bit number that is a multiple of 4.

  * For example, to begin the program at the address 0x10000000, the code would be:
```
.org 0x10000000
```

_**IMPORTANT NOTE:**_ This must be the first non-comment line in the main source file. It is possible however to have multiple .org statements throughout the program.

[Back to the top](UserManual.md)


### Labels ###

Labels allow the programmer to use branch and jump instructions. A label is used to mark sections of code within the program.

To implement a label, type the name of label you wish to use followed by a colon.

  * For example, to create a label called "main", the code would be:
```
main:
<instructions>
```
    * It is the standard convention to have the first label in a program titled "main".

_Note:_ It is possible to load a pointer to a label using the load immediate instruction (li).

[Back to the top](UserManual.md)



### Comments ###

Comments may appear anywhere in the program's code, including on label, instruction, and directive lines.

To use a comment, type `#` before your comment. All text after the `#` until the end of the line are ignored by the assembler.

  * For example, if you wanted to put a comment on a blank line as well as a instruction line, the code would be:
```
#this is the first comment
add $s0, $s0, $s1 #this is the second comment
```

_Note:_ Comments are very helpful for debugging and helping others who read the code to understand what a certain segment of code is supposed to do.

[Back to the top](UserManual.md)



### Data and String Allocation ###

There are three ways to allocate space for data with PLPTool:
  * A single word
  * Space in terms of numbers of words
  * A string

[Back to the top](UserManual.md)



#### Single Word Allocation ####

The `.word` directive allocates a single word with or without an initial value. This is especially useful after a label for ease of access.

  * For example, to allocate a variable and initialize it to the value `4`, the code would be:
```
my_variable:
     .word 4

...

li $t0, my_variable     # get a pointer to my_variable
lw $t1, 0($t0)          # $t1 has the value of my_variable (4) now
```


[Back to the top](UserManual.md)


#### Space Allocation ####

PLPTool supports allocating space by taking the number of words to allocated by using the `.space` directive, as opposed to a single word with the `.word` directive.

  * For example, to allocate a variable with a length of 2 words, the code would be:
```
long_variable:
     .space 2

...

li $t0, long_variable    # get a pointer to the variable
lw $t1, 0($t0)           # get the first word
lw $t2, 4($t0)           # get the second word
```

[Back to the top](UserManual.md)



#### String Allocation ####

PLPTool supports two types of string allocation:
  * `.ascii`
    * This allocates a packed array of characters without a trailing null character (terminator), which indicates the end of the string.
      * For example, if you wanted to allocate a variable with a string using the `.ascii` directive, the code would be:
```
my_string_ascii:
     .ascii "example string"  # no null terminator
```

  * `.asciiz`
    * This allocates a packed array of characters with a trailing null character that indicated the end of the string.
      * For example, if you wanted to allocate a variable with a string using the `.asciiz` directive, the code would be:
```
my_string_asciiz:
     .asciiz "example string" # null terminator inserted at end of string
```
    * This type of string allocation should be used for string operations.

_Note:_ PLPTool also supports escaping newline characters with **`\n`** .

[Back to the top](UserManual.md)



## Operations ##

### R-type Arithmetic and Logical Instructions ###

These instructions receive inputs from two registers, do an arithmetic or logical operation, and write the result to the destination register. The general syntax is:
```
op   $destination_register, $source_register_1, $source_register_2
```

The following are the R-type arithmetic and logical instructions supported by PLP:

|Syntax|Expression|Sample Usage|Notes|Opcode/Function|
|:-----|:---------|:-----------|:----|:--------------|
|addu  $rd, $rs, $rt|rd = rs + rt;|addu  $v0, $a0, $a1|Unsigned addition|0x00 / 0x21|
|subu  $rd, $rs, $rt|rd = rs - rt;|subu  $v0, $a0, $a1|Unsigned subtraction|0x00 / 0x23|
|and   $rd, $rs, $rt|rd = rs & rt;|and   $v0, $a0, $a1|Bitwise logical AND|0x00 / 0x24|
|or    $rd, $rs, $rt|rd = rs | rt;|or    $v0, $a0, $a1|Bitwise logical OR|0x00 / 0x25|
|nor   $rd, $rs, $rt|rd = ~(rs | rt);|nor   $v0, $a0, $a1|Bitwise logical NOR|0x00 / 0x27|
|slt   $rd, $rs, $rt|rd = (rs < rt) ? 1 : 0;|slt   $v0, $a0, $a1|Signed compare|0x00 / 0x2a|
|sltu  $rd, $rs, $rt|rd = (rs < rt) ? 1 : 0;|sltu  $v0, $a0, $a1|Unsigned compare|0x00 / 0x2b|
|mullo $rd, $rs, $rt|`rd = (rs * rt) & 0xFFFFFFFF;`|mullo $v0, $a0, $a1|Multiply (return low order bits)|0x00 / 0x10|
|mulhi $rd, $rs, $rt|`rd = (rs * rt) >>> 32;`|mulhi $v0, $a0, $a1|Multiply (return high order bits)|0x00 / 0x11|

[Back to the top](UserManual.md)



### R-type Shift Instructions ###

PLP supports logical shift left and logical shift right instructions. The general syntax is:
```
op    $destination_register, $source_register, shift_amount
```

The shift amount is a 5-bit integer that can be represented in either decimal or hexadecimal (with the `0x` prefix).

**IMPORTANT NOTE:** If the shift amount value is greater than 5 bits, the assembler will truncate the more significant positions beyond the fifth bit.

The following are the R-type shift instructions supported by PLP:

|Syntax|Expression|Sample Usage|Notes|Opcode/Function|
|:-----|:---------|:-----------|:----|:--------------|
|sll $rd, $rt, shamt|rd = rt << shamt;|sll $v0, $a0, 0x12|Shift $a0 by 18 to the left and store the result in $v0|0x00 / 0x00|
|srl $rd, $rt, shamt|rd = rt >> shamt;|srl $v0, $a0, 18|Shift $a0 by 18 to the right and store the result in $v0|0x00 / 0x02|

[Back to the top](UserManual.md)



### R-type Jump Register Instructions ###

Jump register loads the content of the specified register $rs into the program counter.


|Syntax|Expression|Sample Usage|Notes|Opcode/Function|
|:-----|:---------|:-----------|:----|:--------------|
|jr $rs|PC = rs;|jr $ra|Load the content of $ra into PC register|0x00 / 0x08|
|jalr $rd, $rs|rd = PC + 4; PC = rs;|jalr $s5, $t0|Jump to location specified by the contents of `rs`, save return address in `rd`.|0x00 / 0x09|

**_IMPORTANT NOTE:_** After every jump/branch instruction, there is a "branch delay slot" immediately after. The next line of code following the jump/branch will also get executed along with the jump/branch. To avoid complications, it is generally advisable to put a no operation instruction (nop) immediately after the jump/branch instruction, unless the branch delay slot needs to be utilized.

[Back to the top](UserManual.md)



### I-type Branch Instructions ###

PLP supports two branch instructions:
  * Branch on Equal (**`beq`**)
  * Branch on Not Equal (**`bne`**)

This instruction compares two registers, and takes the appropriate action depending on the instruction used (**`beq`** or **`bne`**).

The general syntax is:
```
op    $source_register_1, $source_register_2, branch_target
```

The branch\_target must be a valid label in the program. The assembler takes the address of the label and will calculate the branch offset to be put into the immediate\_field.

The following I-type branch instructions are supported by PLP:

|Syntax|Expression|Sample Usage|Notes|Opcode|
|:-----|:---------|:-----------|:----|:-----|
|beq $rt, $rs, label|if(rt == rs) PC = PC + 4 + imm;|beq $a0, $a1, done|Branch to done if $a0 and $a1 are equal|0x04|
|bne $rt, $rs, label|if(rt != rs) PC = PC + 4 + imm;|bne $a0, $a1, error|Branch to error if $a0 and $a1 are NOT equal|0x05|

**_IMPORTANT NOTE:_** After every jump/branch instruction, there is a "branch delay slot" immediately after. The next line of code following the jump/branch will also get executed along with the jump/branch. To avoid complications, it is generally advisable to put a no operation instruction (nop) immediately after the jump/branch instruction, unless the branch delay slot needs to be utilized.

[Back to the top](UserManual.md)



### I-type Arithmetic and Logical Instructions ###

These instructions receive input from a source register ($rs) and the immediate field, and store the result in the target register ($rt).

The general syntax is:
```
op    $target_register, $source_register, immediate_field
```

The `immediate_field` is a 16-bit integer that can be represented in either decimal or hexadecimal (with the `0x` prefix) form.

_**IMPORTANT NOTE:**_ If the `immediate_field` value is greater than 16 bits, the assembler will truncate the more significant bit positions beyond the sixteenth place.

The processor extends the sign bit of `immediate_field` to make it a 32-bit number.

The following I-type arithmetic and logical instructions are supported by PLP:

|Syntax|Expression|Sample Usage|Notes|Opcode|
|:-----|:---------|:-----------|:----|:-----|
|addiu $rt, $rs, imm|`rt = rs + SignExtend(imm);`|addiu $v0, $a0, 0xFEED|Unsigned addition, add $a0 with 65261|0x09|
|andi  $rt, $rs, imm|`rt = rs & ZeroExtend(imm);`|andi  $v0, $a0, 1337|Bitwise Logical AND|0x0c|
|ori   $rt, $rs, imm|`rt = rs | ZeroExtend(imm);`|ori   $v0, $a0, 0x0539|Bitwise Logical OR|0x0d|
|slti  $rt, $rs, imm|`rt = (rs < SignExtend(imm)) ? 1 : 0;`|slti  $v0, $a0, 0xDEAD|Signed compare|0x0a|
|sltiu $rt, $rs, imm|`rt = (rs < SignExtend(imm)) ? 1 : 0;`|sltiu $v0, $a0, 0xDEAD|Unsigned compare|0x0b|

[Back to the top](UserManual.md)



### I-type Load Upper Immediate Instruction ###

This instruction loads the immediate field to the upper 2 bytes of the specified target register ($rt).

The following I-type Load Upper Immediate instruction is supported by PLP:

|Syntax|Expression|Sample Usage|Notes|Opcode|
|:-----|:---------|:-----------|:----|:-----|
|lui $rt, imm|rt = imm << 16;|lui $a0, 0xFEED|Write 0xFEED0000 to $a0 register. Note that the lower 16 bits are filled with zeroes|0x0f|

[Back to the top](UserManual.md)



### I-type Load and Store Word Instructions ###

These instructions read from or write to memory. The general syntax is:
```
op    $register, immediate_field($offset_register)
```

`immediate_field` is a 16-bit wide value, and it is sign extended to 32-bit by the processor to resolve the memory address. If the value of `immediate_field` is larger than 16-bit, it will be truncated by the assembler. `$offset_register` acts as an index to the memory address, similar to the index of an array.

|Syntax|Expression|Sample Usage|Notes|Opcode|
|:-----|:---------|:-----------|:----|:-----|
|lw $rt, imm($rs)|`rt = SignExtend(imm)[rs];`|lw $v0, 0x4000($a1)|Load contents of 0x4000 + $a1 into $v0|0x23|
|sw $rt, imm($rs)|`SignExtend(imm)[rs] = rt;`|sw $a0, 128($v0)|Store contents of register $a0 to 128 + $v0|0x2b|

[Back to the top](UserManual.md)



### J-type Instructions ###

There are two J-type instructions:
  * Jump (**`j`**)
    * This jumps to the specified label in the program without writing the return address to $ra.
  * Jump and Link (**`jal`**)
    * This jumps to the specified label in the program and writes the return address to $ra.

The general syntax is:
```
op    jump_target
```

`jump_target` must be a valid label.

The following J-type instructions are supported by PLP:

|Syntax|Expression|Sample Usage|Notes|Opcode|
|:-----|:---------|:-----------|:----|:-----|
|j label|PC = jump\_target;|j loop|Jump to loop label|0x02|
|jal label|ra = PC + 4; PC = jump\_target;|jal read\_serial|Jump to read\_serial after saving return address to $ra|0x03|

**_IMPORTANT NOTE:_** After every jump/branch instruction, there is a "branch delay slot" immediately after. The next line of code following the jump/branch will also get executed along with the jump/branch. To avoid complications, it is generally advisable to put a no operation instruction (nop) immediately after the jump/branch instruction, unless the branch delay slot needs to be utilized.

[Back to the top](UserManual.md)



## Pseudo-Operations ##

The PLP assembler supports several pseudo-operations to make programming easier. The following pseudo-operations are supported by PLP:

|Pseudo-op|Equivalent instruction(s)|Notes|
|:--------|:------------------------|:----|
|nop|sll $0, $0, 0|No-operation. Can be used for branch delay slots|
|b label|beq $0, $0, label|Branch always to label|
|move $rd, $rs|add $rd, $0, $rs|Copy $rs to $rd|
|push $rt|addiu $sp, $sp, -4; sw $rt, 0($sp)|Push $rt into the stack|
|pop $rt|lw $rt, 0($sp); addiu $sp, $sp, 4|Pop data from the top of the stack to $rt|
|li $rd, imm|lui $rd, (imm & 0xff00) >> 16; ori $rd, imm & 0x00ff|Load a 32-bit number to $rd|
|li $rd, label|lui $rd, (imm & 0xff00) >> 16; ori $rd, imm & 0x00ff|Load the address of a label to a register to be used as a pointer.|
|call label|  |Save $aX, $tX, $sX, and $ra to stack and call function|
|return|  |Restore $aX, $tX, $sX, and $ra from stack and return|
|save|  |Save all registers except for $zero to stack|
|restore|  |Restore all registers saved by 'save' in reverse order|
|lwm $rt, imm32/label|  |Load the value from a memory location into $rt|
|swm $rt, imm32/label|  |Store the value in $rt to a memory location|
[Back to the top](UserManual.md)



## Notes on Register Usage ##

Aside from $zero, $i0, $i1, and $ra, PLP does not explicitly assign special functions to a register. This section lays down some conventions on how the other registers should be used. All the supplied libraries adhere to this guideline.

|Register|Usage|Notes|
|:-------|:----|:----|
|$zero|Constant value 0|This register can not be written to and always returns the value 0|
|$at|Assembler temporary|Assembler reserved, do not use|
|$v0 - $v1|Values for results|Use for return values of functions|
|$a0 - $a3|Arguments|Use for arguments of functions|
|$t0 - $t9|Temporaries|Do not use these registers across function calls, as they will most likely be corrupted|
|$s0 - $s7|Saved temporaries|- |
|$i0|Interrupt vector|The CPU jumps to the address pointed by this register when an interrupt occurs|
|$i1|Interrupt return address|Written by the CPU when an interrupt occurs|
|$gp|Global pointer|- |
|$sp|Stack pointer|Use this register to implement a stack|
|$fp|Frame pointer|- |
|$ra|Return address|Do not manually write to this register unless restoring from the stack for nested function calls. Use this register to return from a function using the jump register instruction|

[Back to the top](UserManual.md)



# Hardware Description #

The following sections describe the individual hardware components of the PLP Board that can be accessed and utilized. These components are memory mapped.

[Back to the top](UserManual.md)



## Memory Map ##

The table below indicates where a certain memory mapped device begins, how many bytes are allocated to that device, and the name of the device.

| **Beginning Address** | **Length in Bytes** | Device |
|:----------------------|:--------------------|:-------|
| `0x00000000` | 2048 | Boot/ROM |
| `0x10000000` | 16777216 | RAM |
| `0xf0000000` | 16 | UART |
| `0xf0100000` | 4 | Switches |
| `0xf0200000` | 4 | LEDs |
| `0xf0300000` | 12 | GPIO |
| `0xf0400000` | 8 | VGA |
| `0xf0500000` | 8 | PLPID |
| `0xf0600000` | 4 | Timer |
| `0xf0700000` | 8 | Interrupt Controller |
| `0xf0800000` | ? | Performance Counter Hardware |
| `0xf0a00000` | 4 | Seven Segment Displays |

Each of these devices will be discussed in the following sections.


[Back to the top](UserManual.md)



## ROM ##

The ROM module is a non-volatile, read-only memory that stores the bootloader (fload). The bootloader is used with PLPTool to load programs over the serial port. The PLP Board starts at the memory address 0x00000000 on start up and upon reset, causing the bootloader to execute.

[Back to the top](UserManual.md)



## RAM ##

The RAM module is a volatile, random access memory that stores all the downloaded program code and data. Generally, the programmer with place their program at the beginning of the RAM using the directive `.org 0x10000000` . Additionally, the stack is generally initialized at the "top" of RAM by using the directive `$sp = 0x10fffffc`  .

[Back to the top](UserManual.md)



## UART ##

The UART module is running at 57600 baud, with 8 data bits, 1 stop bit, and no parity. The UART module is connected to the serial port on the PLP Board.

The UART module is designed to send or receive a single byte at a time, and can only store one byte in the send and receive buffer. This means that you must first either send the data in the buffer before reloading the buffer and you must retrieve the data in the receive buffer (by polling) before the next byte is available.

There are four registers that are memory mapped that the UART module uses:
| Address | Description |
|:--------|:------------|
|`0xf0000000` | Command Register |
|`0xf0000004` | Status Register|
|`0xf0000008` | Receive Buffer|
|`0xf000000c` | Send Buffer|

The command register's default value is 0. Writing the value `0x01` will initiate a send operation using the lowest byte in the send buffer. Writing `0x02` will clear the ready flag in the status register, thus preparing for the cycle again.

The status register uses only the bottom two bits, with the remaining bits reading 0.
  * The value `0x00` in the status register is the **clear to send bit** (CTS bit), which is set after a successful transfer of data from the send buffer. It indicates that another transmission can now be made.
    * The CTS bit is `0x00` is during a transmission, and the data in the send buffer must not be modified during the transmission.
  * The value `0x01` in the status register is the **ready bit**, which is set when a new byte has been successfully received.
    * The ready bit can be cleared by writing `0x02` to the command register.

The receive buffer holds the last received byte. On a successful receive, the ready bit will be set, allowing the user to poll the status register for incoming data. When the ready bit is not set, the receive buffer should not be read as any data contained within is invalid.

The send buffer holds the byte that will be sent or is ready to be sent. During a send operation (CTS bit = `0x00`), the data in the send buffer **MUST NOT** be modified. The user should only update the send buffer when the CTS bit is reset.

The UART module supports interrupts, and will trigger an interrupt whenever data is available in the receive buffer.

**_IMPORTANT NOTE:_** The user must complete a receive cycle (including clearing the ready bit) before clearing the interrupt status bit for the UART.


[Back to the top](UserManual.md)



## Switches ##

The Switches module is a read-only register that always holds the current value of the switch positions on the PLP Board. There are 8 switches on the PLP Board, which are mapped to the lowest byte of the register. Writing to this register will have no effect.


[Back to the top](UserManual.md)



## LEDs ##

The LEDs module is a read-write register that stores the value of the LEDs on the PLP Board. There are 8 LEDs, mapped to the lowest byte of the register. Reading the register will provide the current status of the LEDs, and writing to the register will update the LEDs' status.


[Back to the top](UserManual.md)



## GPIO ##

The GPIO module connects two of the PLP Board's I/O connectors to the PLP System, enabling 16 GPIO ports.

There are three registers that are used with the GPIO module:

| Address | Description |
|:--------|:------------|
| `0xf0300000` | Tristate register |
| `0xf0300004` | GPIO Block A |
| `0xf0300008` | GPIO Block B |

Each block of GPIO ports on the PLP Board has 12 pins: 8 I/O, 2 ground, and 2 Vdd.

The tristate register controls the direction of data on each of the GPIO pins. At startup and on reset, all GPIO are set to be inputs (the tristate register is zeroed). This protects circuits that are driving any pins on the GPIO ports. The user can set GPIO to be outputs by enabling the tristate pins for those outputs. The tristate bits map to GPIO pins in the following table.

| Tristate Register Bit | GPIO Pin |
|:----------------------|:---------|
| 0 | A0 |
| 1 | A1 |
| 2 | A2 |
| 3 | A3 |
| 4 | A4 |
| 5 | A5 |
| 6 | A6 |
| 7 | A7 |
| 8 | B0 |
| 9 | B1 |
| 10 | B2 |
| 11 | B3 |
| 12 | B4 |
| 13 | B5 |
| 14 | B6 |
| 15 | B7 |

The GPIO registers use the bottom 8 bits of the data word. The other bits are always read `0`.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_gpio2.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_gpio2.png)

The figure above shows the pin mappings to the below table.

| Pin | Mapping/Bit Position |
|:----|:---------------------|
| 1 | 0 |
| 2 | 1 |
| 3 | 2 |
| 4 | 3 |
| 5 | GND |
| 6 | Vdd |
| 7 | 4 |
| 8 | 5 |
| 9 | 6 |
| 10 | 7 |
| 11 | GND |
| 12 | Vdd |


[Back to the top](UserManual.md)



## VGA ##

The VGA module controls a 640x480 display with an 8-bit color depth.

An 8-bit color depth provides 3 red bits, 3 green bits, and 2 blue bits per pixel. The blue channel only has two bits because of a bit-depth limitation as well as the human eye's poor sensitivity to blue intensity.

The VGA module has two registers:

| Address | Description |
|:--------|:------------|
| `0xf0400000` | Control |
| `0xf0400004` | Frame Buffer Pointer |

The control register uses only the least significant bit, which enables or disables the VGA controller output. When the control register is 0, the VGA module is disabled. When the control register is 0x1, the VGA module is enabled.

_Enabling the VGA module has significant impact on memory performance._ The VGA module uses RAM as VGA memory, and has a higher priority to the RAM bus than the CPU. During a draw cycle, the CPU will not be able to access the SRAM for a short period of time.

The frame buffer pointer is a pointer to the pixel data to draw in memory. For example, if your pixel data begins at 0x100f0000, you would set the frame buffer pointer to that location. The frame buffer must be 307,200 bytes long. The pixel data is arranged as starting from the upper left hand corner of the screen (0,0), and drawing to the right, one row at a time (like reading a book). That is, the zeroth pixel in the pixel data is the upper left hand corner. The upper right hand corner is the 639th pixel, and the left most pixel of the second row is the 640th pixel.

A pixel can be indexed by its row and column address (with 0,0 being the upper left hand corner) with : address = base\_address + (640\*row) + column.

The chart below displays the order of the color bits in the byte.

| Bit | Color |
|:----|:------|
| 7 | `red[2]` |
| 6 | `red[1] `|
| 5 | `red[0]` |
| 4 | `green[2]` |
| 3 | `green[1]` |
| 2 | `green[0]` |
| 1 | `blue[1]` |
| 0 | `blue[0]` |

  * For example, to create a purely red pixel, the code would be `0b11100000` in binary, or `0xE0` in hexadecimal.


[Back to the top](UserManual.md)



## PLPID ##

The PLPID module contains two registers that describe the board identity and frequency. Writing to either register has no effect.

`0xf0500000` - PLPID (0xdeadbeef for this version)
`0xf0500004` - Board frequency (50MHz, 0x2faf080, for the reference design)

The CPUID module is useful for dynamically calculating wait time in a busy-wait loop. For example, if you wanted to wait .5 seconds, you could read the board frequency, shift right by 1 bit, and call the libplp\_wait function.


[Back to the top](UserManual.md)



## Timer ##

The timer module is a single 32-bit counter that increments by one every clock cycle. It can be written to at any time. At overflow, the timer will continue counting. The timer module is useful for waiting a specific amount of time with high resolution (20ns on the reference design).

The timer module supports interrupts, and will trigger an interrupt when the timer overflows. The user can configure a specific timed interrupt by presetting the timer value to N cycles before the overflow condition.


[Back to the top](UserManual.md)



## Seven Segment Displays ##

The Seven Segment Displays module exposes the raw seven segment LEDs to the user, allowing for specialized output. There are `libplp` wrappers that exist for various abstractions.

There are 4 seven segment displays (seven segments plus a dot), mapped to four bytes in the register listed in the [Memory Map](UserManual#Memory_Map.md) section.

The byte order is:

| `[31:24]` | `[23:16]` | `[15:8]` | `[7:0]` |
|:----------|:----------|:---------|:--------|
| Left-most  |  |  | Right-most |

The bits of each byte map to each of the segments as indicated by the figure below.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_sseg2_fixed.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_sseg2_fixed.png)

**_IMPORTANT NOTE:_** The seven segment displays have a built-in inverter that requires the user to invert the bits in the byte before converting to hexadecimal.

  * For example, to program the seven segment displays to display the letter "F", the portions of the seven segment display that would light up would be 0, 4, 5, and 6. This would make the byte `0b01110001`. Remember, the bits must be inverted before converting to hexadecimal. This makes the byte `0b10001110`, which in hexadecimal is `0x8E`. Thus, the code for "F" is `0x8E`.


[Back to the top](UserManual.md)



## Interrupt Controller ##

| Register | Description |
|:---------|:------------|
| `0xf0700000` | Mask |
| `0xf0700004` | Status |

Mask Register:
| bit | Description |
|:----|:------------|
| 31-4 | Reserved |
| 3 | Button Interrupt |
| 2 | UART Interrupt |
| 1 | Timer Interrupt |
| 0 | Global Interrupt Enable |

Status Register:
| bit | Interrupt Reason |
|:----|:-----------------|
| 31-4 | Reserved (Always 0) |
| 3 | Button Interrupt |
| 2 | UART Interrupt |
| 1 | Timer Interrupt |
| 0 | Always 1 |

The interrupt controller marshals the interrupt behavior of the PLP system.

The user uses the two registers in the interrupt controller, mask and status, along with the interrupt registers, $i0 and $i1, to control all interrupt behavior.

Before enabling interrupts, the user must supply a pointer to the interrupt vector in register $i0.

```
main:
  li $i0, isr # put a pointer to our isr in $i0

isr: ...
```

When an interrupt occurs, the interrupt controller sets the corresponding bit in the status register. Before returning from an interrupt the user must clear any status bits that are resolved or unwanted.

The user enables interrupts by setting any desired interrupts in the mask register, as well as setting the global interrupt enable (GIE) bit. When an interrupt occurs, the GIE bit is automatically cleared and must be set on interrupt exit.

**_IMPORTANT NOTE:_** When returning from an interrupt, set the Global Interrupt Enable (GIE) bit in the delay slot of the returning jump instruction. This is necessary to prevent any interrupts from occurring while still in the interrupt vector.

When an interrupt occurs, the return address is stored in $i1.

A typical interrupt vector:

```
isr:
  li $t0, 0xf0700000
  lw $t1, 4($t0)     # read the status register
  
  #check status bits and handle any pending interrupts
  #clear any handled interrupts in $t1

  sw $t1, 4($t0)     # clear any handled interrupts in the status register
  lw $t1, 0($t0)     # get the mask register
  ori $t1, $t1, 1    # set GIE

  jr $i1
  sw $t1, 0($t0)     # store the mask register in the delay slot to guarantee proper exit
```


[Back to the top](UserManual.md)



## Performance Counters ##

| Address | Description |
|:--------|:------------|
| `0xf0800000` | Interrupts |
| `0xf0800004` | I-cache Misses |
| `0xf0800008` | I-cache Accesses |
| `0xf080000c` | D-cache Misses |
| `0xf0800010` | D-cache Accesses |
| `0xf0800014` | UART Bytes Received |
| `0xf0800018` | UART Bytes Sent |

The performance counter module stores a number of registers that keep count of various events, as shown above. Performance counters are read-only and reset only during board reset.


[Back to the top](UserManual.md)



# Hardware Configuration and Bootloader #

The PLP Board comes with the `fload` bootloader programmed to the board's ROM, which starts at power-on and board reset.

The bootloader currently supports three functions:
  * Loading data from the UART
  * Memory test
  * Memory test with VGA module enabled

These three functions are initiated by setting the appropriate switch after power-up. When all switches are unset, the LEDs will scroll indefinitely. When one of the above functions is enabled, only LED 0 will be illuminated.

| Switch | Function |
|:-------|:---------|
| 0 | UART Boot / Programming Mode |
| 1 | Memory Test |
| 2 | Memory Test with VGA Module Enabled |

The board can be reset at any time by pressing button 0 or BTNL (depending on model) on the PLP Board . This causes all modules and the CPU to reset, setting the PC to 0. This will restart the bootloader.


[Back to the top](UserManual.md)



## Loading the Hardware Configuration and Bootloader ##


If the PLP Board does not have this programmed to the board's ROM, you must program it before the PLP Board will accept programming from PLPTool or the command line.

To program the PLP Board with the correct hardware configuration and bootloader, you must first download and install the [Adept Software](http://www.digilentinc.com/Products/Detail.cfm?Prod=ADEPT2) required to program the PLP Board.

**Before running Adept**, you must first make sure that your PLP Board has the correct jumper position so that you are programming the ROM of the PLP Board. To do this, make sure the jumper labeled "**MODE**" is set to _ROM_. The picture below displays how it should look.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_jumper.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_jumper.png)

After ensuring the jumper is in the right position, connect the PLP Board to your computer with a micro-USB cable and power on the PLP Board. If the board does not power on once connected and the power switch is in the on position, check to make sure the **POWER SELECT** jumper is in the _USB_ position. The image below shows how that the jumper should be positioned to receive power from the micro-USB cable.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_jumper2.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_jumper2.png)

Next, run the Adept software. Adept will bring a window that looks like the one below.

![http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_adept.png](http://progressive-learning-platform.googlecode.com/hg/images/usersmanual/users_manual_adept.png)

If it does not display the proper board in the "Connect:" dialog box in the upper right-hand corner of the window, then the board has not been connected properly. Make sure that the board is connected and powered on before running Adept.

Next, click _Browse..._ on the line with the box labeled **PROM**. Navigate to your the directory where PLP is located. The bootloader is located in /hw/.

The file you select will depend on the board you have:
  * If you have a Nexys 2 (500k), select the file named `nexys2_500k.bit`
  * If you have a Nexys 2 (1200k), select the file named `nexys2_1200k.bit`
  * If you have a Nexys 3, select the file named `nexys3.bit`

Once you've selected the correct bootloader file, click the _Program_ button on the same line as **PROM**. This will load the bootloader onto the PLP Board.

If there are no error messages, the dialog box at the bottom of the Adept window will display `Programming Successful`. Once the programming is complete, turn the PLP Board off, close Adept, and turn the board back on. Your PLP Board now has the proper hardware configuration and `fload` bootloader loaded into the ROM and you are ready to program the board using PLP.

[Back to the top](UserManual.md)