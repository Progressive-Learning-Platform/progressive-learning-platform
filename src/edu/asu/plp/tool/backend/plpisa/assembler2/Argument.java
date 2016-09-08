package edu.asu.plp.tool.backend.plpisa.assembler2;

import edu.asu.plp.tool.backend.plpisa.assembler2.arguments.ArgumentType;

public interface Argument
{
	int encode();

	String raw();

	ArgumentType getType();
}
