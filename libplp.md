

# Introduction #

`libplp` is a set of board support libraries intended for use with user generated programs. `libplp` consists of functions written to abstract the use of all standard hardware modules, as well as other support libraries for common functions and tasks such as math functions.

# Calling convention #

All `libplp` functions follow the register usage conventions outlined in the [user manual](UserManual.md).

When calling `libplp` functions, arguments are placed in the argument registers (`$a0-$a3`). Return values are in the return value registers (`$v0-$v1`).

`libplp` functions **do not** modify saved temporaries (`$s0-$s7`) or any of the pointer registers (`$gp, $sp, $fp`, unless the function is one that explicitly modifies them). All other registers, including the temporaries (`$t0-$t9`) are subject to modification. It is up to the user to ensure that such register values are saved across `libplp` function calls.

The particular argument/return value structure is defined by each function definition below.

# Using `libplp` #

`libplp` functions are available in the release package, under `sw/libplp`. Module wrappers each have their own `.asm` file, as well as each support library. To gain access to these functions, simply include the needed files in your assembly program.

All `libplp` function calls begin with `libplp_`.

Additionally, you may include all `libplp` functions by including the `libplp.asm` file.

## Example usage ##

Below is an example of a UART based loopback program, which waits for user input, and simply writes the value back to the UART. Two `libplp` function calls are made - `libplp_uart_read` and `libplp_uart_write`. The `libplp` calling convention is used, where `libplp_uart_read` returns the read value into the return value register `$v0`, and `libplp_uart_write` uses the argument register `$a0`.

```
.org 0x10000000
  j start
  nop

.include sw/libplp/libplp_uart.asm

start:
  #get a character from the UART
  jal libplp_uart_read
  nop

  #character is in $v0, echo it back
  move $a0, $v0
  jal libplp_uart_write
  nop

  #loop
  j start
  nop
```

# `libplp` #

## Math functions ##

`libplp_math` provide a handful of commonly used math functions that the `plp` hardware does not provide natively.

#### `libplp_math_imult` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | multiplicand a |
| `$a1` | multiplicand b |
| `$v0` | product |

Simple integer multiplication. Works with both signed and unsigned values.

#### `libplp_math_abs` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed integer |
| `$v0` | abs($a0) |

Returns the absolute value of a signed integer input.

#### `libplp_math_parity` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | input value |
| `$v0` | parity of $a0 |

Returns the parity of the input value. 0 = even parity, 1 = odd parity.

#### `libplp_math_min` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed value a |
| `$a1` | signed value b |
| `$v0` | min(a,b) |

Returns the minimum of the two inputs.

#### `libplp_math_man` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed value a |
| `$a1` | signed value b |
| `$v0` | max(a,b) |

Returns the maximum of the two inputs.

#### `libplp_math_floor**` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed value a |
| `$v0` | floor($a0) |

The result is the "floor" value of the input.


#### `libplp_math_ceil**` ####

_**Uses stack!!**_

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed value a |
| `$v0` | ceiling($a0) |

The result is the "ceiling" value of the input.

#### `libplp_math_fix_round**` ####

_**Uses stack!!**_

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed fixed point a |
| `$v0` | round($a0) |

Rounds the fixed point value up to the nearest integer.

#### `libplp_math_negative**` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed value a |
| `$v0` | negate($a0) |

Returns the negative of the input.

#### `libplp_math_sign**` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | value a |
| `$v0` | sign($a0) |

Returns the sign of the input.

#### `libplp_math_absval**` ####

_**Uses stack!!**_

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed integer/fixed point a |
| `$v0` | absolute\_value($a0) |

Returns the absolute value of the input.


#### `libplp_math_add**` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed integer/fixed point a |
| `$a1` | signed integer/fixed point b |
| `$v0` | add($a0,$a1) |

Adds the two inputs together and returns result.

#### `libplp_math_sub**` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | signed integer/fixed point a |
| `$a1` | signed integer/fixed point b |
| `$v0` | subtract($a0,$a1) |

Subtracts $a0 from $a1 and returns the result.



`**`Not implemented

## Module Wrappers ##

### UART ###


---


#### `libplp_uart_read` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | ASCII encoded character from UART. Upper 24 bits are zeroed |

`libplp_uart_read` blocks on waiting for a byte to be sent to the board via the UART. Upon reading, the byte is placed in `$v0` and returned.


---


#### `libplp_uart_write` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | ASCII encoded character to be sent via UART. Upper 24 bits are don't cares. |

`libplp_uart_write` blocks on waiting for a byte to be sent to the host via the UART.


---


#### `libplp_uart_read_word` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | ASCII encoded word from UART. Bytes are read in most significant byte first. |


---


#### `libplp_uart_write_word` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | ASCII encoded word to be sent via UART. Bytes are sent most significant byte first. |


---


#### `libplp_uart_write_string` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Pointer to a zero terminated character array. |

`libplp_uart_write_string` sends an array of bytes over the UART by using the `libplp_uart_write` function. The string must be zero terminated. This can be accomplished by using the `.asciiz` directive.


---


#### `libplp_uart_write_value_b2` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | 32-bit value |

`libplp_uart_write_value_b2` writes the value in the argument register to the UART as a 32-bit base 2 value. This is especially useful for debugging purposes.


---


#### `libplp_uart_newline` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| N/A | No arguments needed |

`libplp_uart_newline` writes a carriage return and linefeed to the UART.


---


### Switches ###


---


#### `libplp_switches_read` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | Value of switches in lower byte. Upper bytes are zeroed. |

`libplp_switches_read` returns the value of the switches in the lower byte of the return value.


---


### LEDs ###


---


#### `libplp_leds_read` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | Value of LEDs in lower byte. Upper bytes are zeroed. |

`libplp_leds_read` returns the value of the switches in the lower byte of the return value.


---


#### `libplp_leds_write` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Value of LEDs in lower byte. Upper bytes are don't cares. |

`libplp_leds_write` writes to the LED bank using the lower byte of the argument register.


---


### GPIO ###


---


#### `libplp_gpio_tristate_read` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | Value of GPIO tristate register. Upper bytes are zeroed. |

Returns the value of the tristate register in `$v0`. See the [user manual](UserManual.md) for a description of the tristate register.


---


#### `libplp_gpio_tristate_write` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Value of GPIO tristate register. Upper bytes are don't cares. |

Sets the tristate register, and the direction of I/O for the GPIO pins. See the [user manual](UserManual.md) for a description of the tristate register.


---


#### `libplp_gpio_read` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | Value of all GPIO I/Os. Upper two bytes are zeroed. Third byte is Bank A value. Lower byte is Bank B value. |

Returns the value on both GPIO banks as a 16-bit value. If some bits are outputs, their current output value is returned.


---


#### `libplp_gpio_write` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Value of all GPIO I/Os. Upper two bytes are don't cares. Third byte is Bank A value. Lower byte is Bank B value. |

Writes to all GPIO outputs. Bits that are set as inputs are ignored.


---


### VGA ###


---


#### `libplp_vga_enable` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| N/A | No arguments |

Enables the VGA module.


---


#### `libplp_vga_disable` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| N/A | No arguments |

Disables the VGA module.


---


#### `libplp_vga_framebuffer` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Pointer to the frame buffer |

Sets the framebuffer location in RAM. This function can be used for double-buffering techniques.


---


#### `libplp_vga_clear` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Color to write to the framebuffer. Upper bytes are don't cares. |

Writes the color in `$a0` to all pixels in the current framebuffer.


---


### PLPID ###


---


#### `libplp_plpid_read_boardid` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | Board ID parameter |

Returns the board ID.


---


#### `libplp_plpid_read_frequency` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | Clock frequency |

Returns the board clock frequency in Hz. Useful in timer operations.


---


### Timer ###


---


#### `libplp_timer_read` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$v0` | Timer value |

Returns the current timer value.


---


#### `libplp_timer_write` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Timer value |

Sets the timer value.


---


#### `libplp_timer_wait` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Number of cycles to wait |

Blocks for `$a0` number of cycles.


---


### Seven Segment ###


---


#### `libplp_sseg_write_raw` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Seven segment values |

Writes directly to the seven segment register. Useful for creating custom symbols. Byte ordering is described in the [user manual](UserManual.md).


---


#### `libplp_sseg_write` ####

Calling convention:
| Register | Description |
|:---------|:------------|
| `$a0` | Seven segment values |

Writes to the seven segment display with a base 16 font. The upper byte maps to the leftmost seven segment display, the lower byte to the rightmost. Only the lower 4 bits of each byte are used.


---
