package edu.asu.plp.tool.backend.plpisa.assembler2.instructions;

import static edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType.NUMBER_LITERAL;
import static edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType.REGISTER;
import static edu.asu.plp.tool.backend.plpisa.assembler2.instructions.RTypeInstruction.FUNCT_CODE_POSITION;
import static edu.asu.plp.tool.backend.plpisa.assembler2.instructions.RTypeInstruction.MASK_5BIT;
import static edu.asu.plp.tool.backend.plpisa.assembler2.instructions.RTypeInstruction.MASK_6BIT;
import static edu.asu.plp.tool.backend.plpisa.assembler2.instructions.RTypeInstruction.RD_POSITION;
import static edu.asu.plp.tool.backend.plpisa.assembler2.instructions.RTypeInstruction.RT_POSITION;
import static edu.asu.plp.tool.backend.plpisa.assembler2.instructions.RTypeInstruction.SHAMT_POSITION;

import java.text.ParseException;

import edu.asu.plp.tool.backend.plpisa.assembler2.Argument;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType;

public class RITypeInstruction extends AbstractInstruction
{
	private int functCode;
	
	public RITypeInstruction(int functCode)
	{
		super(new ArgumentType[] { REGISTER, REGISTER, NUMBER_LITERAL });
		this.functCode = functCode;
	}
	
	@Override
	protected int safeAssemble(Argument[] arguments) throws ParseException
	{
		Argument rdRegisterArgument = arguments[0];
		Argument rtRegisterArgument = arguments[1];
		Argument shamtArgument = arguments[2];
		
		return assembleEncodings(rdRegisterArgument.encode(),
				rtRegisterArgument.encode(), shamtArgument.encode());
	}
	
	private int assembleEncodings(int encodedRDArgument, int encodedRTArgument,
			int encodedShamtArgument)
	{
		int encodedBitString = 0;
		encodedBitString |= (encodedShamtArgument & MASK_5BIT) << SHAMT_POSITION;
		encodedBitString |= (encodedRTArgument & MASK_5BIT) << RT_POSITION;
		encodedBitString |= (encodedRDArgument & MASK_5BIT) << RD_POSITION;
		encodedBitString |= (functCode & MASK_6BIT) << FUNCT_CODE_POSITION;
		
		return encodedBitString;
	}
}
