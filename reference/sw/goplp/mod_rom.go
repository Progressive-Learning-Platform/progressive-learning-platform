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
	mod_rom_size  = 2048
	mod_rom_start = 0x00000000
)

var rom = make([]uint32, mod_rom_size) // 2048KB rom

// modules register themselves in the rom map :)
func init() {
	map_register(mod_rom_start, mod_rom_start+mod_rom_size-1, mod_rom_read, mod_rom_write)
}

func mod_rom_read(address uint32) uint32 {
	a := (address - mod_rom_start) / 4
	//log(fmt.Sprintf("mod_rom read: %#08x : %#08x", address, rom[a]))
	return rom[a]
}

func mod_rom_write(address, data uint32) {
	a := (address - mod_rom_start) / 4
	//log(fmt.Sprintf("mod_rom write: %#08x : %#08x", address, data))
	rom[a] = data
}
