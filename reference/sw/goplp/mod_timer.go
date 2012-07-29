package main

const (
	mod_timer_size  = 4
	mod_timer_start = 0xf0600000
)

var timer uint32 = 0

// modules register themselves in the memory map :)
func init() {
	map_register(mod_timer_start, mod_timer_start+mod_timer_size-4, mod_timer_read, mod_timer_write)
}

func mod_timer_read(address uint32) uint32 {
	return timer
}

func mod_timer_write(address, data uint32) {
	timer = data
}

func mod_timer_eval() {
	timer++
}
