package edu.asu.plp.tool.backend.plpisa.assembler;

import edu.asu.plp.tool.backend.isa.exceptions.AssemblerException;

@FunctionalInterface
public interface AssemblerStep
{
	public void perform() throws AssemblerException;
}
