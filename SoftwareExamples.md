# Introduction #

This page provides brief descriptions of the example PLP programs included with the release files in `sw/examples`.

These examples are provided for use with the tutorials, for testing the PLP system, and for help with learning the PLP environment.

# Examples #

## vga\_test ##

`vga_test` is a simple test routine for the VGA module that flashes the screen a white color, then a purple color. This loops forever. There is no human input to the program.

## uart\_loopback ##

`uart_loopback` is a test routine for the UART that reads a character from the UART, and writes it back to the UART immediately. This creates a local loopback from the perspective of the host. The host may test this program by opening a terminal application on the board at 57600 bps, 8N1.

## sseg\_test ##

`sseg_test` is a test routine for the seven segment driver. It displays four different sets of values on the seven segment display: `0123`, `4567`, `89ab`, `cdef`. Each set of values is displayed for one second.

## leds\_test ##

`leds_test` is the simplest test routine in the examples directory. It simply writes a value to the LEDs: `0xdf` and halts.

## gpio\_test ##

`gpio_test` provides a command line interface via the UART to control the GPIO ports. The command line interface allows the user to set the value of the tristate register, read inputs, and set outputs. After programming, the user will be prompted for a command. Pressing 't' will set the tristate register value, bit by bit, as prompted. Pressing 'i' will read the current state of the GPIO ports. Pressing 'o' will write a fixed value '0x1234' to the GPIO ports. After each command the user will be prompted to issue another command.

## chutes\_and\_hazards ##

`chutes_and_hazards` is a simple test of many of the hazards that exist in the CPU pipeline. This test was used as a first order test of hazards during development of the hardware and software simulator. If all tests pass, all 8 LEDs will be set. If any one test fails, the LED corresponding to that test (see program code) will not be set.

## clock ##

`clock` is a simple twelve hour clock program. It displays the time on the seven segment display. Time is set using switches 0 and 1. Setting switch 0 will increment the minutes, while setting switch 1 will increment the hours.

## memtest ##

`memtest` is a reimplementation of the memory test routine built into the bootloader. This is used for testing simultaneous reads on the instruction and data busses.