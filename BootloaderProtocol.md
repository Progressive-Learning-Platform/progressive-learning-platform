# `fload` Bootloader #

The `fload` bootloader is a simple bootloader that initializes the machine at power-up, performs a simple built in self test (BIST), and loads executables.

`fload` currently only supports loading executables via the UART. Future versions may support booting from a second stage loader on the flash ROM.

# Initialization and self test #

`fload` begins by zeroing the register file and performing user visible self test routines. Self test includes writing color bars to the VGA module, writing the `plp` version to the seven segment display, and flashing the LEDS. This behavior continues until one of the boot switches is set.

# Boot from UART #

`fload` enters the UART boot routine when switch 0 is set. When set, LED 0 will be set, and all BIST behavior will stop. At this point, `fload` is waiting for a command sequence via the UART.

`fload` accepts five commands:
| **command** | **sequence** | **description** |
|:------------|:-------------|:----------------|
| `address` | `'a'` followed by a 4 byte address | Sets the data pointer. A subsequent data or jump command will use this address |
| `data` | `'d'` followed by a 4 byte data word | Puts data into the address pointed to by the data pointer. A data command will automatically advance the data pointer by 4. |
| `chunk` | `'c'` followed by a 4 byte length (in words), then the data chunk (not to exceed the specified length). | Send a chunk of data. |
| `jump` | `'j'` | Jump to the address pointed to by the data pointer. |
| `preamble` | `'p'` followed by a 1 byte preamble ID and a 3 byte preamble word | Sets board-specific configuration during programming. Hardware implementations can ignore this command if the specified preamble ID is not supported. |
| `version` | `'v'` | Print the PLP system version, in the format of `PLP-5.0` |

An `address`, `data`, `chunk`, `jump`, or `preamble` command will return with the acknowledge byte: `f` (`chunk` will return a single `f` at the end of the transfer). A `version` command will only return the `plp` version string.

# Memory Test #

`fload` enters a memory test routine when switch 1 is set. When set, LED 0 will be set, and all BIST behavior will stop. On the UART, the string `starting memory test...` will be printed, and the memory test will write and read to and from each memory location in SRAM. Any unsuccessful read cycles will print the memory location and invalid data. When the memory test is finished, the string "done" will be printed and `fload` will halt.