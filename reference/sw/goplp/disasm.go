package main

import (
	"fmt"
)

type instruction struct {
	typ      instructionType
	opcode   int
	function int
	rd       int
	rs       int
	rt       int
	shamt    uint32
	imm      int32
	uimm     uint32
	jaddr    uint32
	raw      uint32
}

func (i *instruction) String() string {
	function := functions[i.function]
	opcode := opcodes[i.opcode]
	rd := registers[i.rd]
	rs := registers[i.rs]
	rt := registers[i.rt]

	switch i.typ {
	case data:
		return fmt.Sprintf("data: %#08x | %q", i.raw, i.raw)
	case rtype:
		// there are special cases
		switch function {
		case "srl", "sll":
			// special case for a nop
			if i.raw == 0 {
				return fmt.Sprintf("inst: nop")
			}
			return fmt.Sprintf("inst: %v $%v, $%v, %v", function, rd, rt, i.shamt)
		case "jr":
			return fmt.Sprintf("inst: jr $%v", rs)
		case "jalr":
			return fmt.Sprintf("inst: jalr $%v, $%v", rd, rs)
		default:
			return fmt.Sprintf("inst: %v $%v, $%v, $%v", function, rd, rs, rt)
		}
	case itype:
		// more special cases
		switch opcode {
		case "andi", "ori": // unsigned immediate field
			return fmt.Sprintf("inst: %v $%v, $%v, %#04x", opcode, rt, rs, i.uimm)
		case "lui":
			return fmt.Sprintf("inst: lui $%v, %#04x", rt, i.uimm)
		case "lw", "sw":
			return fmt.Sprintf("inst: %v $%v, %#04x($%v)", opcode, rt, i.imm, rs)
		default:
			return fmt.Sprintf("inst: %v $%v, $%v, %#04x", opcode, rt, rs, i.imm)
		}
	case jtype:
		return fmt.Sprintf("inst: %v %#08x", opcode, i.jaddr)
	}
	return fmt.Sprintf("something went wrong: %#08x", i.raw)
}

type instructionType int

const (
	data  instructionType = iota
	rtype instructionType = iota
	itype instructionType = iota
	jtype instructionType = iota
)

var opcodes = map[int]string{
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

var functions = map[int]string{
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
	0x08: "jr",
	0x09: "jalr",
}

var registers = map[int]string{
	0:  "zero",
	1:  "at",
	2:  "v0",
	3:  "v1",
	4:  "a0",
	5:  "a1",
	6:  "a2",
	7:  "a3",
	8:  "t0",
	9:  "t1",
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

func disassemble(i uint32) *instruction {
	// i should be a 4 byte slice, containing a single instruction
	ret := &instruction{}

	ret.opcode = int((i & 0xfc000000) >> 26)
	ret.function = int(i & 0x0000003f)
	ret.rd = int((i & 0x0000f800) >> 11)
	ret.rt = int((i & 0x001f0000) >> 16)
	ret.rs = int((i & 0x03e00000) >> 21)
	ret.shamt = uint32((i & 0x000007c0) >> 6)
	ret.imm = int32(int16(i & 0x0000ffff))
	ret.uimm = uint32(i & 0x0000ffff)
	ret.jaddr = uint32(i & 0x03ffffff)
	ret.raw = i

	switch ret.opcode {
	case 0:
		/* this may not be an r-type instruction, check the function */
		switch ret.function {
		case 0x21, 0x23, 0x24, 0x25, 0x27, 0x2a, 0x2b, 0x10, 0x11, 0x00, 0x02, 0x08, 0x09:
			ret.typ = rtype
		default:
			ret.typ = data
		}
	case 0x04, 0x05, 0x09, 0x0c, 0x0d, 0x0a, 0x0b, 0x0f, 0x23, 0x2b:
		ret.typ = itype
	case 0x02, 0x03:
		ret.typ = jtype
	default:
		ret.typ = data
	}
	return ret
}
