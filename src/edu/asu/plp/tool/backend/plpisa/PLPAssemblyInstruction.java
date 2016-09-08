package edu.asu.plp.tool.backend.plpisa;

import edu.asu.plp.tool.backend.isa.ASMInstruction;

public class PLPAssemblyInstruction implements ASMInstruction
{
	private final int lineNumber;
	private final String instructionContents;
	
	public PLPAssemblyInstruction(int lineNumber, String instructionContents)
	{
		this.lineNumber = lineNumber;
		this.instructionContents = instructionContents;
	}

	@Override
	public int getLineNumber()
	{
		return lineNumber;
	}

	@Override
	public String getInstructionContents()
	{
		return instructionContents;
	}
}
