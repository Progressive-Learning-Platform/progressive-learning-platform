package edu.asu.plp.tool.backend.plpisa.assembler2.instructions;

import java.text.ParseException;
import java.util.function.Function;

import edu.asu.plp.tool.backend.plpisa.assembler2.Argument;
import edu.asu.plp.tool.backend.plpisa.assembler2.PLPInstruction;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType;

public abstract class AbstractInstruction implements PLPInstruction
{
	private ArgumentType[] argumentTypes;
	
	public AbstractInstruction(ArgumentType[] argumentTypes)
	{
		this.argumentTypes = argumentTypes;
	}
	
	protected abstract int safeAssemble(Argument[] arguments) throws ParseException;
	
	@Override
	public final int assemble(Argument[] arguments) throws ParseException
	{
		validateArguments(arguments);
		return safeAssemble(arguments);
	}
	
	private <T> String[] toStrings(T[] arguments, Function<T, String> toString)
	{
		String[] strings = new String[arguments.length];
		
		for (int index = 0; index < arguments.length; index++)
			strings[index] = toString.apply(arguments[index]);
		
		return strings;
	}
	
	private void validateArguments(Argument[] arguments)
	{
		boolean valid = true;
		if (arguments.length != argumentTypes.length)
		{
			valid = false;
		}
		else
		{
			for (int index = 0; index < arguments.length; index++)
			{
				Argument argument = arguments[index];
				ArgumentType actualType = argument.getType();
				ArgumentType expectedType = argumentTypes[index];
				
				if (!actualType.equals(expectedType))
				{
					valid = false;
					break;
				}
			}
		}
		
		if (!valid)
		{
			String[] expectedArgumentTypeNames = toStrings(argumentTypes,
					(type) -> type.name());
			String[] actualArgumentTypeNames = toStrings(arguments,
					(argument) -> argument.getType().name());
			String expectedArguments = String.join(", ", expectedArgumentTypeNames);
			String actualArguments = String.join(", ", actualArgumentTypeNames);
			throw new IllegalArgumentException(
					"Instruction expected the argument pattern [" + expectedArguments
							+ "] but found [" + actualArguments + "]");
		}
	}
}
