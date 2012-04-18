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
	mod_plpid_size  = 8
	mod_plpid_start = 0xf0500000
)

var plpid_f uint32 = 20
var plpid_id uint32 = 0x00000401

// modules register themselves in the plpid map :)
func init() {
	map_register(mod_plpid_start, mod_plpid_start+mod_plpid_size-4, mod_plpid_read, mod_plpid_write)
}

func mod_plpid_read(address uint32) uint32 {
	a := (address - mod_plpid_start) / 4
	if a == 0 {
		return plpid_id
	}
	return plpid_f
}

func mod_plpid_write(address, data uint32) {
}
