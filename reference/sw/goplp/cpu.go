package main

import (
	"fmt"
	"time"
)

// the memory map
type memory_map_item struct {
	start uint32
	stop  uint32
	read  func(uint32) uint32
	write func(uint32, uint32)
}

var memory_map []*memory_map_item

func map_register(start, stop uint32, read func(uint32) uint32, write func(uint32, uint32)) {
	log("registering:", start, ":", stop)
	memory_map = append(memory_map, &memory_map_item{
		start: start,
		stop:  stop,
		read:  read,
		write: write,
	})
}

func map_find(address uint32) *memory_map_item {
	for _, i := range memory_map {
		if i.start <= address && address <= i.stop {
			//log(fmt.Sprintf("found module at %#08x", i.start))
			return i
		}
	}
	return nil
}

// the cpu
var pc uint32 = 0
var rf = make([]uint32, 32)
var j bool = false
var jpc uint32 = 0

func step(n int) {
	now := time.Now()
	for i := 0; i < n; i++ {
		// check the trace cache first
		inst, ok := trace_lookup(pc)
		if !ok {
			raw, ok := cpu_read(pc)
			if ok {
				inst = disassemble(raw)
				trace_insert(pc, inst)
			}
		}
		//log("decoding:", inst)
		if j {
			calculate(inst)
			pc = jpc
			j = false
		} else {
			pc = calculate(inst)
		}
		mod_timer_eval()
	}
	t := time.Since(now)
	raw, ok := cpu_read(pc)
	if ok {
		inst := disassemble(raw)
		fmt.Printf("%v steps in %v | %#08x : %v\n", n, t, pc, inst)
	}
}

func cpu_read(address uint32) (uint32, bool) {
	if address%4 != 0 {
		fmt.Println("misaligned read:", address, ", pc:", pc)
		return 0, false
	}
	map_item := map_find(address)
	if map_item == nil {
		fmt.Printf("unmapped address: %#08x, pc: %#08x\n", address, pc)
		return 0, false
	}
	return map_item.read(address), true
}

func cpu_write(address, data uint32) bool {
	if address%4 != 0 {
		fmt.Println("misaligned write:", address, ", pc:", pc)
		return false
	}
	map_item := map_find(address)
	if map_item == nil {
		fmt.Printf("unmapped address: %#08x, pc: %#08x\n", address, pc)
		return false
	}
	map_item.write(address, data)
	watched_m(address)
	return true
}

func calculate(i *instruction) uint32 {
	// just a big case statement for each instruction

	switch i.typ {
	case data:
		fmt.Println("attempt to execute data:", i)
	case rtype:
		switch i.function {
		case 0x21:
			rf[i.rd] = rf[i.rs] + rf[i.rt]
		case 0x23:
			rf[i.rd] = rf[i.rs] - rf[i.rt]
		case 0x24:
			rf[i.rd] = rf[i.rs] & rf[i.rt]
		case 0x25:
			rf[i.rd] = rf[i.rs] | rf[i.rt]
		case 0x27:
			rf[i.rd] = ^(rf[i.rs] | rf[i.rt])
		case 0x2a:
			if int32(rf[i.rs]) < int32(rf[i.rt]) {
				rf[i.rd] = 1
			} else {
				rf[i.rd] = 0
			}
		case 0x2b:
			if rf[i.rs] < rf[i.rt] {
				rf[i.rd] = 1
			} else {
				rf[i.rd] = 0
			}
		case 0x10:
			rf[i.rd] = rf[i.rs] * rf[i.rt]
		case 0x11:
			rf[i.rd] = uint32((uint64(rf[i.rs]) * uint64(rf[i.rt])) >> 32)
		case 0x00:
			rf[i.rd] = rf[i.rt] << i.shamt
		case 0x02:
			rf[i.rd] = rf[i.rt] >> i.shamt
		case 0x08:
			j = true
			jpc = rf[i.rs]
		case 0x09:
			j = true
			jpc = rf[i.rs]
			rf[i.rd] = pc + 8
		}
		watched_r(i.rd)
	default: // itype or jtype
		switch i.opcode {
		case 0x04:
			if rf[i.rt] == rf[i.rs] {
				j = true
				jpc = uint32(int32(pc) + 4 + (i.imm << 2))
			}
		case 0x05:
			if rf[i.rt] != rf[i.rs] {
				j = true
				jpc = uint32(int32(pc) + 4 + (i.imm << 2))
			}
		case 0x09:
			rf[i.rt] = uint32(int32(rf[i.rs]) + i.imm)
		case 0x0c:
			rf[i.rt] = rf[i.rs] & i.uimm
		case 0x0d:
			rf[i.rt] = rf[i.rs] | i.uimm
		case 0x0a:
			if int32(rf[i.rs]) < i.imm {
				rf[i.rt] = 1
			} else {
				rf[i.rt] = 0
			}
		case 0x0b:
			if rf[i.rs] < i.uimm {
				rf[i.rt] = 1
			} else {
				rf[i.rt] = 0
			}
		case 0x0f:
			rf[i.rt] = i.uimm << 16
		case 0x23:
			raw, _ := cpu_read(uint32(int32(rf[i.rs]) + i.imm))
			rf[i.rt] = raw
		case 0x2b:
			cpu_write(uint32(int32(rf[i.rs])+i.imm), rf[i.rt])
		case 0x02:
			j = true
			jpc = (pc & 0xf0000000) | (i.jaddr << 2)
		case 0x03:
			rf[31] = pc + 8
			j = true
			jpc = (pc & 0xf0000000) | (i.jaddr << 2)
		}
		rf[0] = 0
		watched_r(i.rt)
	}
	return pc + 4
}

// trace cache
var trace_cache = make([]*instruction, mod_memory_size / 4)

func trace_lookup(a uint32) (*instruction, bool) {
	e := (a - mod_memory_start) / 4
	if trace_cache[e] != nil {
		return trace_cache[e], true
	}
	return nil, false
}

func trace_insert(a uint32, i *instruction) {
	e := (a - mod_memory_start) / 4
	trace_cache[e] = i
}
