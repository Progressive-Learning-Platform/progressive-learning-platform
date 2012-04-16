package main

import (
	"fmt"
)

type instruction struct {
	typ		instructionType
	opcode		string
	function	string
	rd		string
	rs		string
	rt		string
	shamt		uint8
	imm		int16
	uimm		uint16
	jaddr		uint32
	raw		uint32
	rawchars	[]byte
}

func (i *instruction) String() string {
	switch i.typ {
	case data:
		return fmt.Sprintf("data: %#08x | %q", i.raw, i.rawchars)
	case rtype:
		// there are special cases
		switch i.function {
		case "srl", "sll":
			// special case for a nop
			if (i.raw == 0) {
				return fmt.Sprintf("inst: nop")
			}
			return fmt.Sprintf("inst: %v $%v, $%v, %v", i.function, i.rd, i.rt, i.shamt)
		case "jr":
			return fmt.Sprintf("inst: jr $%v", i.rs)
		case "jalr":
			return fmt.Sprintf("inst: jalr $%v, $%v", i.rd, i.rs)
		default:
			return fmt.Sprintf("inst: %v $%v, $%v, $%v", i.function, i.rd, i.rs, i.rt)
		}
	case itype:
		// more special cases
		switch i.opcode {
		case "andi", "ori": // unsigned immediate field
			return fmt.Sprintf("inst: %v $%v, $%v, %#04x", i.opcode, i.rt, i.rs, i.uimm)
		case "lui":
			return fmt.Sprintf("inst: lui $%v, %#04x", i.rt, i.uimm)
		case "lw", "sw":
			return fmt.Sprintf("inst: %v $%v, %#04x($%v)", i.opcode, i.rt, i.imm, i.rs)
		default:
			return fmt.Sprintf("inst: %v $%v, $%v, %#04x", i.opcode, i.rt, i.rs, i.imm)
		}
	case jtype:
		return fmt.Sprintf("inst: %v %#08x", i.opcode, i.jaddr)
	}
	return fmt.Sprintf("something went wrong: %#08x", i.raw)
}

type instructionType int

const (
	data	instructionType = iota
	rtype	instructionType = iota
	itype	instructionType = iota
	jtype	instructionType = iota
)

var opcodes = map[int] string {
	0x00: "rtype",
	0x04: "beq",
	0x05: "bne",
	0x09: "addiu",
	0x0c: "andi",
	0x0d: "ori",
	0x0a: "slti",
	0x0b: "sltiu",
	0x0f: "lui",
	0x23: "lw",
	0x2b: "sw",
	0x02: "j",
	0x03: "jal",
}

var functions = map[int] string {
	0x21: "addu",
	0x23: "subu",
	0x24: "and",
	0x25: "or",
	0x27: "nor",
	0x2a: "slt",
	0x2b: "sltu",
	0x10: "mullo",
	0x11: "mulhi",
	0x00: "sll",
	0x02: "srl",
}

var registers = map[int] string {
	0: "zero",
	1: "at",
	2: "v0",
	3: "v1",
	4: "a0",
	5: "a1",
	6: "a2",
	7: "a3",
	8: "t0",
	9: "t1",
	10: "t2",
	11: "t3",
	12: "t4",
	13: "t5",
	14: "t6",
	15: "t7",
	16: "t8",
	17: "t9",
	18: "s0",
	19: "s1",
	20: "s2",
	21: "s3",
	22: "s4",
	23: "s5",
	24: "s6",
	25: "s7",
	26: "i0",
	27: "i1",
	28: "gp",
	29: "sp",
	30: "fp",
	31: "ra",
}

func disassemble(i []byte) (*instruction) {
	// i should be a 4 byte slice, containing a single instruction
	ret := &instruction{}

	ret.opcode	= opcodes[int((i[0] & 0xfc) >> 2)]
	ret.function	= functions[int(i[3] & 0x3f)]
	ret.rd		= registers[int((i[2] & 0xf8) >> 3)]
	ret.rt		= registers[int((i[1] & 0x1f))]
	ret.rs		= registers[int(((i[1] & 0xe0) >> 5) | ((i[0] & 0x03) << 3))]
	ret.shamt	= uint8(((i[3] & 0xc0) >> 6) | ((i[2] & 0x07) << 2))
	ret.imm		= (int16(i[2]) << 8) | int16(i[3])
	ret.uimm	= (uint16(i[2]) << 8) | uint16(i[3])
	ret.jaddr	= (uint32(i[0] & 0x03) << 24) | (uint32(i[1]) << 16) | uint32(ret.imm)
	ret.raw		= (uint32(i[0]) << 24) | (uint32(i[1]) << 16) | (uint32(i[2]) << 8) | uint32(i[3])
	ret.rawchars	= i

	switch ret.opcode {
	case "rtype":
		/* this may not be an r-type instruction, check the function */
		switch ret.function {
		case "":
			ret.typ = data
		default:
			ret.typ = rtype
		}
	case "beq", "bne", "addiu", "andi", "ori", "slti", "sltiu", "lui", "lw", "sw":
		ret.typ = itype
	case "j", "jal":
		ret.typ = jtype
	default:
		ret.typ = data
	}
	return ret
}
