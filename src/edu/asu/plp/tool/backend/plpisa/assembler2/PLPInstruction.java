package edu.asu.plp.tool.backend.plpisa.assembler2;

import java.text.ParseException;

public interface PLPInstruction
{
	int assemble(Argument[] arguments) throws ParseException;
}
