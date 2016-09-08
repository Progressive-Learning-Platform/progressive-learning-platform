package edu.asu.plp.tool.backend.plpisa.assembler2;

import java.text.ParseException;
import java.util.List;

import edu.asu.plp.tool.backend.BiDirectionalOneToManyMap;
import edu.asu.plp.tool.backend.OrderedBiDirectionalOneToManyHashMap;
import edu.asu.plp.tool.backend.isa.ASMDisassembly;
import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.backend.isa.ASMImage;
import edu.asu.plp.tool.backend.isa.ASMInstruction;
import edu.asu.plp.tool.backend.isa.Assembler;
import edu.asu.plp.tool.backend.isa.exceptions.AssemblerException;
import edu.asu.plp.tool.backend.plpisa.PLPASMImage;
import edu.asu.plp.tool.backend.plpisa.PLPAssemblyInstruction;
import edu.asu.plp.tool.backend.plpisa.assembler.PLPDisassembly;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.RegisterArgument;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.StringLiteral;
import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.Value;

public class PLPAssembler implements Assembler
{
	private static InstructionMap plpInstructions = getPLPSpecification();
	private BiDirectionalOneToManyMap<ASMInstruction, ASMDisassembly> assemblyToDisassemblyMap;
	private long programLocation;
	private int lineNumber;
	
	@Override
	public ASMImage assemble(List<ASMFile> asmFiles) throws AssemblerException
	{
		assemblyToDisassemblyMap = new OrderedBiDirectionalOneToManyHashMap<>();
		
		for (ASMFile asmFile : asmFiles)
		{
			assembleFile(asmFile.getContent());
		}
		
		return new PLPASMImage(assemblyToDisassemblyMap);
	}
	
	private static InstructionMap getPLPSpecification()
	{
		InstructionMap instructions = new InstructionMap();
		
		instructions.addRTypeInstruction("addu", 0x21);
		instructions.addRTypeInstruction("subu", 0x23);
		instructions.addRTypeInstruction("and", 0x24);
		instructions.addRTypeInstruction("or", 0x25);
		instructions.addRTypeInstruction("nor", 0x27);
		instructions.addRTypeInstruction("slt", 0x2a);
		instructions.addRTypeInstruction("sltu", 0x2b);
		instructions.addRTypeInstruction("mullo", 0x10);
		instructions.addRTypeInstruction("mulhi", 0x11);
		instructions.addRTypeInstruction("sllv", 0x01);
		instructions.addRTypeInstruction("slrv", 0x03);
		
		instructions.addRITypeInstruction("sll", 0x00);
		instructions.addRITypeInstruction("srl", 0x02);
		
		instructions.addRJTypeInstruction("jr", 0x08);
		instructions.addRJTypeInstruction("jalr", 0x09);
		
		instructions.addITypeInstruction("addiu", 0x09);
		instructions.addITypeInstruction("andi", 0x0c);
		instructions.addITypeInstruction("ori", 0x0d);
		instructions.addITypeInstruction("slti", 0x0a);
		instructions.addITypeInstruction("sltiu", 0x0b);
		instructions.addITypeInstruction("lui", 0x0f);
		
		return instructions;
	}
	
	private void assembleFile(String content) throws AssemblerException
	{
		String[] lines = content.split("\\n\\r?");
		lineNumber = 1;
		try
		{
			for (String line : lines)
			{
				String source = line.trim();
				String instruction = source.split("\\s+")[0];
				String remainder = source.substring(instruction.length());
				remainder = remainder.trim();
				String[] argumentStrings = remainder.split(",\\s*");
				
				Argument[] arguments = parseArguments(argumentStrings);
				PLPDisassembly disassembly = process(instruction, arguments);
				ASMInstruction key = new PLPAssemblyInstruction(lineNumber, source);
				assemblyToDisassemblyMap.put(key, disassembly);
				
				lineNumber++;
			}
		}
		catch (ParseException exception)
		{
			throw new AssemblerException(exception);
		}
		catch (Exception exception)
		{
			throw new AssemblerException(exception);
		}
	}
	
	private PLPDisassembly process(String instructionName, Argument[] arguments)
			throws ParseException
	{
		PLPInstruction instruction = plpInstructions.get(instructionName);
		int codedInstruction = instruction.assemble(arguments);
		long address = programLocation++;
		PLPDisassembly disassembly = new PLPDisassembly(address, codedInstruction);
		
		return disassembly;
	}
	
	private Argument[] parseArguments(String[] argumentStrings) throws ParseException
	{
		int size = argumentStrings.length;
		Argument[] arguments = new Argument[size];
		
		for (int index = 0; index < size; index++)
		{
			String argumentString = argumentStrings[index];
			arguments[index] = parseArgument(argumentString);
		}
		
		return arguments;
	}
	
	private Argument parseArgument(String argumentString) throws ParseException
	{
		argumentString = argumentString.trim();
		if (argumentString.startsWith("'") || argumentString.startsWith("\""))
		{
			boolean valid = argumentString.endsWith("" + argumentString.charAt(0));
			if (!valid)
			{
				throw new ParseException(
						"String literals must be enclosed in single or double quotes.",
						lineNumber);
			}
			
			return new StringLiteral(argumentString);
		}
		else if (argumentString.startsWith("\\$"))
		{
			return new RegisterArgument(argumentString);
		}
		else if (argumentString.startsWith("0x"))
		{
			boolean valid = argumentString.matches("0x[0-9]+");
			if (!valid)
			{
				throw new ParseException(
						"Expected an integer value to follow '0x' but found '"
								+ argumentString + "'", lineNumber);
			}
			
			String number = argumentString.substring(2);
			int value = Integer.parseInt(number, 16);
			return new Value(value);
		}
		else if (argumentString.startsWith("0b"))
		{
			boolean valid = argumentString.matches("0b[0-9]+");
			if (!valid)
			{
				throw new ParseException(
						"Expected an integer value to follow '0b' but found '"
								+ argumentString + "'", lineNumber);
			}
			
			String number = argumentString.substring(2);
			int value = Integer.parseInt(number, 2);
			return new Value(value);
		}
		else if (argumentString.matches("[0-9]+"))
		{
			int value = Integer.parseInt(argumentString);
			return new Value(value);
		}
		else
		{
			throw new ParseException("Expected argument but found '" + argumentString
					+ "'", lineNumber);
		}
	}
}
