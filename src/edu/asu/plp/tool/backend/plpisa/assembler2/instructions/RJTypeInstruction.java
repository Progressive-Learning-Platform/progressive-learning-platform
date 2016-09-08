package edu.asu.plp.tool.backend.plpisa.assembler2.instructions;

import static edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType.REGISTER;

import java.text.ParseException;

import edu.asu.plp.tool.backend.plpisa.assembler2.Argument;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.RegisterArgument;

public class RJTypeInstruction extends AbstractInstruction
{
	private RTypeInstruction backingInstruction;
	
	public RJTypeInstruction(int functCode)
	{
		super(new ArgumentType[] { REGISTER });
		this.backingInstruction = new RTypeInstruction(functCode);
	}
	
	@Override
	protected int safeAssemble(Argument[] arguments) throws ParseException
	{
		Argument rdRegisterArgument = new RegisterArgument("$0");
		Argument rsRegisterArgument = arguments[0];
		Argument rtRegisterArgument = new RegisterArgument("$0");
		
		arguments = new Argument[] { rdRegisterArgument, rsRegisterArgument,
				rtRegisterArgument };
		return backingInstruction.assemble(arguments);
	}
}
