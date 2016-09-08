package edu.asu.plp.tool.backend.plpisa.assembler2.instructions;

import static edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType.NUMBER_LITERAL;
import static edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType.REGISTER;

import java.text.ParseException;

import edu.asu.plp.tool.backend.plpisa.assembler2.Argument;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType;

public class ITypeInstruction extends AbstractInstruction
{
	public static final int MASK_5BIT = 0b011111;
	public static final int MASK_6BIT = 0b111111;
	public static final int MASK_16BIT = 0xFFFF;
	public static final int OP_CODE_POSITION = 26;
	public static final int RS_POSITION = 21;
	public static final int RT_POSITION = 16;
	public static final int IMMEDIATE_POSITION = 0;
	
	private int opCode;
	
	public ITypeInstruction(int opCode)
	{
		super(new ArgumentType[] { REGISTER, REGISTER, NUMBER_LITERAL });
		this.opCode = opCode;
	}
	
	@Override
	protected int safeAssemble(Argument[] arguments) throws ParseException
	{
		Argument rtRegisterArgument = arguments[0];
		Argument rsRegisterArgument = arguments[1];
		Argument immediateArgument = arguments[2];
		
		return assembleEncodings(rtRegisterArgument.encode(),
				rsRegisterArgument.encode(), immediateArgument.encode());
	}
	
	private int assembleEncodings(int encodedRTArgument, int encodedRSArgument,
			int encodedImmediateArgument)
	{
		int encodedBitString = 0;
		encodedBitString |= (encodedRSArgument & MASK_5BIT) << RS_POSITION;
		encodedBitString |= (encodedRTArgument & MASK_5BIT) << RT_POSITION;
		encodedBitString |= (encodedImmediateArgument & MASK_16BIT) << IMMEDIATE_POSITION;
		encodedBitString |= (opCode & MASK_6BIT) << OP_CODE_POSITION;
		
		return encodedBitString;
	}
}
