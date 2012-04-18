package main

import (
	"fmt"
)

type instruction struct {
	typ      instructionType
	opcode   string
	function string
	rd       string
	rd_i     int
	rs       string
	rs_i     int
	rt       string
	rt_i     int
	shamt    uint32
	imm      int32
	uimm     uint32
	jaddr    uint32
	raw      uint32
}

func (i *instruction) String() string {
	switch i.typ {
	case data:
		return fmt.Sprintf("data: %#08x | %q", i.raw, i.raw)
	case rtype:
		// there are special cases
		switch i.function {
		case "srl", "sll":
			// special case for a nop
			if i.raw == 0 {
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

	ret.opcode = opcodes[int((i&0xfc000000)>>26)]
	ret.function = functions[int(i&0x0000003f)]
	ret.rd = registers[int((i&0x0000f800)>>11)]
	ret.rd_i = int((i & 0x0000f800) >> 11)
	ret.rt = registers[int((i&0x001f0000)>>16)]
	ret.rt_i = int((i & 0x001f0000) >> 16)
	ret.rs = registers[int((i&0x03e00000)>>21)]
	ret.rs_i = int((i & 0x03e00000) >> 21)
	ret.shamt = uint32((i & 0x000007c0) >> 6)
	ret.imm = int32(int16(i & 0x0000ffff))
	ret.uimm = uint32(i & 0x0000ffff)
	ret.jaddr = uint32(i & 0x03ffffff)
	ret.raw = i

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
