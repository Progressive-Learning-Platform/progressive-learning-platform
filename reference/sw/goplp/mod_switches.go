package main

/* a module must satisfy:
 * 
 * func mod_read(address int32) int32
 * func mod_write(address, data int32)
 */

//import (
//	"fmt"
//)

const (
	mod_switches_size = 4
	mod_switches_start = 0xf0100000
)

// modules register themselves in the switches map :)
func init() {
	map_register(mod_switches_start, mod_switches_start + mod_switches_size - 4, mod_switches_read, mod_switches_write)
}

func mod_switches_read(address uint32) uint32 {
	return 0
}

func mod_switches_write(address, data uint32) {
	// nothing
}
