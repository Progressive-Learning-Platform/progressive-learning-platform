package edu.asu.plp.tool.backend.plpisa.sim;

import edu.asu.plp.tool.backend.plpisa.InstructionExtractor;

public class ALU
{
	public ALU()
	{
	}
	
	public long evaluate(long a, long b, long instruction)
	{
		//@formatter:off
		switch (InstructionExtractor.opcode(instruction))
		{
			case 0: // R-types
				switch (InstructionExtractor.funct(instruction))
				{
					case 0x24: return a & b;
					case 0x25: return a | b;
					case 0x27: return ~(a | b);
					case 0x21: return a + b;
					case 0x23: return a - b;
					case 0x2A:
						int aSigned = (int) a;
						int bSigned = (int) b;
						return (aSigned < bSigned) ? 1 : 0;
					case 0x2B: return (a < b) ? 1 : 0;
					case 0x00: 
						return b << InstructionExtractor.sa(instruction);
					case 0x02:
						return b >>> InstructionExtractor.sa(instruction);
					case 0x10:
						return ((long)(int) a * (long)(int)b) & 0xffffffffL;
					case 0x11:
						return (((long)(int) a * (long)(int)b) & 0xffffffff00000000L) >> 32;
					case 0x01: return a << b;
                    case 0x03: return a >> b;
				}
			case 0x04: return (a - b == 0) ? 1 : 0;
            case 0x05: return (a - b == 0) ? 0 : 1;
            case 0x0c: return a & b;
            case 0x0d: return a | b;
            case 0x0f: return b << 16;
            case 0x0A:
                int aSigned = (int) a;
                int bSigned = (int) b;
                return (aSigned < bSigned) ? 1 : 0;
            case 0x0B: return (a < b) ? 1 : 0;
            case 0x08:
            case 0x09:
            case 0x23:
            case 0x2B: return a + b;
		}
		//@formatter:on
		return -1;
	}
	
}
