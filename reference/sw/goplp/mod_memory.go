package main

/* a module must satisfy:
 * 
 * func mod_read(address int32) int32
 * func mod_write(address, data int32)
 */

import (
	"fmt"
)

const (
	mod_memory_size = 16777216 / 4
	mod_memory_start = 0x10000000
)

var memory = make([]uint32,mod_memory_size) // 16MB memory

// modules register themselves in the memory map :)
func init() {
	map_register(mod_memory_start, mod_memory_start + mod_memory_size - 1, mod_memory_read, mod_memory_write)
}

func mod_memory_read(address uint32) uint32 {
	a := (address - mod_memory_start) / 4
	log(fmt.Sprintf("mod_memory read: %#08x : %#08x", address, memory[a]))
	return memory[a]
}

func mod_memory_write(address, data uint32) {
	a := (address - mod_memory_start) / 4
	log(fmt.Sprintf("mod_memory write: %#08x : %#08x", address, data))
	memory[a] = data
}
