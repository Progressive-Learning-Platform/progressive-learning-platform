package main

/* a module must satisfy:
 * 
 * func mod_read(address int32) int32
 * func mod_write(address, data int32)
 */

const (
	mod_leds_size  = 4
	mod_leds_start = 0xf0200000
)

var leds uint32 = 0

// modules register themselves in the leds map :)
func init() {
	map_register(mod_leds_start, mod_leds_start+mod_leds_size-4, mod_leds_read, mod_leds_write)
}

func mod_leds_read(address uint32) uint32 {
	return leds
}

func mod_leds_write(address, data uint32) {
	leds = data & 0x000000ff
}
