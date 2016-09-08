package edu.asu.plp.tool.backend.plpisa.assembler2.instructions;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;
import java.util.function.Function;

import edu.asu.plp.tool.backend.plpisa.assembler2.Argument;
import edu.asu.plp.tool.backend.plpisa.assembler2.PLPInstruction;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType;

public class PsuedoOp implements PLPInstruction
{
	private ArgumentType[] argumentTypes;
	private List<Function<String[], String>> instuctionMappings;
	
	public PsuedoOp(ArgumentType... argumentTypes)
	{
		this.argumentTypes = argumentTypes;
		this.instuctionMappings = new ArrayList<>();
	}
	
	public void addInstructionMapping(Function<String[], String> instuctionMapping)
	{
		this.instuctionMappings.add(instuctionMapping);
	}
	
	public void mapInstruction(Function<String[], String> instuctionMapping)
	{
		addInstructionMapping(instuctionMapping);
	}
	
	@Override
	public int assemble(Argument[] arguments) throws ParseException
	{
		throw new UnsupportedOperationException("PsuedoOps must be preprocessed instead");
	}
	
	public String[] preprocess(Argument[] arguments)
	{
		validateArguments(arguments);
		String[] args = toStrings(arguments, (argument) -> argument.raw());
		
		List<String> preprocessedInstructions = new ArrayList<>();
		for (Function<String[], String> preprocessFunction : instuctionMappings)
		{
			String instruction = preprocessFunction.apply(args);
			preprocessedInstructions.add(instruction);
		}
		
		int size = preprocessedInstructions.size();
		return preprocessedInstructions.toArray(new String[size]);
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
	
	public static String instruction(String name, String... arguments)
	{
		String argumentString = String.join(", ",  arguments);
		return (arguments.length == 0) ? name : name + " " + argumentString;
	}
}
