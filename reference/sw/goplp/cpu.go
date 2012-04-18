package main

import (
	"fmt"
	"time"
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
	for i:=0; i<n; i++ {
		raw, ok := cpu_read(pc)
		if ok {
			inst := disassemble(raw)
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
	}
	t := time.Since(now)
	raw, ok := cpu_read(pc)
	if ok {
		inst := disassemble(raw)
		fmt.Printf("%v steps in %v | %#08x : %v\n", n, t, pc, inst)
	}
}

func cpu_read(address uint32) (uint32, bool) {
	if address % 4 != 0 {
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
	if address % 4 != 0 {
		fmt.Println("misaligned write:", address, ", pc:", pc)
		return false
	}
	map_item := map_find(address)
	if map_item == nil {
		fmt.Printf("unmapped address: %#08x, pc: %#08x\n", address, pc)
		return false
	}
	map_item.write(address,data)
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
		case "addu":
			rf[i.rd_i] = rf[i.rs_i] + rf[i.rt_i]
		case "subu":
			rf[i.rd_i] = rf[i.rs_i] - rf[i.rt_i]
		case "and":
			rf[i.rd_i] = rf[i.rs_i] & rf[i.rt_i]
		case "or":
			rf[i.rd_i] = rf[i.rs_i] | rf[i.rt_i]
		case "nor":
			rf[i.rd_i] = ^(rf[i.rs_i] | rf[i.rt_i])
		case "slt":
			if int32(rf[i.rs_i]) < int32(rf[i.rt_i]) {
				rf[i.rd_i] = 1
			} else {
				rf[i.rd_i] = 0
			}
		case "sltu":
			if rf[i.rs_i] < rf[i.rt_i] {
				rf[i.rd_i] = 1
			} else {
				rf[i.rd_i] = 0
			}
		case "mullo":
			rf[i.rd_i] = rf[i.rs_i] * rf[i.rt_i]
		case "mulhi":
			rf[i.rd_i] = uint32((uint64(rf[i.rs_i]) * uint64(rf[i.rt_i])) >> 32)
		case "sll":
			rf[i.rd_i] = rf[i.rt_i] << i.shamt
		case "srl":
			rf[i.rd_i] = rf[i.rt_i] >> i.shamt
		case "jr":
			j = true
			jpc = rf[i.rs_i]
		case "jalr":
			j = true
			jpc = rf[i.rs_i]
			rf[i.rd_i] = pc + 8
		}
		watched_r(uint32(i.rd_i))
	default: // itype or jtype
		switch i.opcode {
		case "beq":
			if rf[i.rt_i] == rf[i.rs_i] {
				j = true
				jpc =  uint32(int32(pc) + 4 + (i.imm << 2))
			}
		case "bne":
			if rf[i.rt_i] != rf[i.rs_i] {
				j = true
				jpc = uint32(int32(pc) + 4 + (i.imm << 2))
			}
		case "addiu":
			rf[i.rt_i] = uint32(int32(rf[i.rs_i]) + i.imm)
		case "andi":
			rf[i.rt_i] = rf[i.rs_i] & i.uimm
		case "ori":
			rf[i.rt_i] = rf[i.rs_i] | i.uimm
		case "slti":
			if int32(rf[i.rs_i]) < i.imm {
				rf[i.rt_i] = 1
			} else {
				rf[i.rt_i] = 0
			}
		case "sltu":
			if rf[i.rs_i] < i.uimm {
				rf[i.rt_i] = 1
			} else {
				rf[i.rt_i] = 0
			}
		case "lui":
			rf[i.rt_i] = i.uimm << 16
		case "lw":
			raw,_ := cpu_read(uint32(int32(rf[i.rs_i]) + i.imm))
			rf[i.rt_i] = raw
		case "sw":
			cpu_write(uint32(int32(rf[i.rs_i]) + i.imm), rf[i.rt_i])
		case "j":
			j = true
			jpc = (pc & 0xf0000000) | (i.jaddr << 2)
		case "jal":
			rf[31] = pc + 8
			j = true
			jpc = (pc & 0xf0000000) | (i.jaddr << 2)
		}
		rf[0] = 0
		watched_r(uint32(i.rt_i))
	}
	return pc + 4
}

