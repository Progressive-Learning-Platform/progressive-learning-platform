package main

/* a module must satisfy:
 * 
 * func mod_read(address int32) int32
 * func mod_write(address, data int32)
 */

const (
	mod_sseg_size = 4
	mod_sseg_start = 0xf0a00000
)

var sseg uint32 = 0

// modules register themselves in the sseg map :)
func init() {
	map_register(mod_sseg_start, mod_sseg_start + mod_sseg_size - 4, mod_sseg_read, mod_sseg_write)
}

func mod_sseg_read(address uint32) uint32 {
	return sseg
}

func mod_sseg_write(address, data uint32) {
	sseg = data
}
