package main

import (
	"fmt"
)

// the memory map
type memory_map_item struct {
	start	uint32
	stop	uint32
	read	func(uint32) uint32
	write	func(uint32, uint32)
}

var memory_map []*memory_map_item

func map_register(start, stop uint32, read func(uint32) uint32, write func(uint32, uint32)) {
	log("registering:", start, ":", stop)
	memory_map = append(memory_map, &memory_map_item{
		start: start,
		stop: stop,
		read: read,
		write: write,
	})
}

func map_find(address uint32) *memory_map_item {
	for _,i := range memory_map {
		if i.start <= address && address <= i.stop {
			log(fmt.Sprintf("found module at %#08x", i.start))
			return i
		}
	}
	return nil
}

// the cpu
var pc uint32 = 0
var regfile = make([]uint32, 31)

func step(n int) {
	for i:=0; i<n; i++ {
		instruction := cpu_read(pc)
		log("decoding:", disassemble(instruction))
		pc += 4
	}
	fmt.Println(n, "steps:", disassemble(cpu_read(pc)))
}

func cpu_read(address uint32) uint32 {
	if address % 4 != 0 {
		fmt.Println("misaligned read:", address, ", pc:", pc)
		return 0
	}
	map_item := map_find(address)
	if map_item == nil {
		fmt.Printf("unmapped address: %#08x, pc: %#08x\n", address, pc)
	}
	return map_item.read(address)
}

func cpu_write(address, data uint32) {
	if address % 4 != 0 {
		fmt.Println("misaligned write:", address, ", pc:", pc)
		return
	}
	map_item := map_find(address)
	if map_item == nil {
		fmt.Printf("unmapped address: %#08x, pc: %#08x\n", address, pc)
	}
	map_item.write(address,data)
}

