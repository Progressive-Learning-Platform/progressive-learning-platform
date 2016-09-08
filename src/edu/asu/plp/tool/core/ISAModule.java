package edu.asu.plp.tool.core;

import java.util.function.Function;

import edu.asu.plp.tool.backend.isa.Assembler;
import edu.asu.plp.tool.backend.isa.Simulator;

public class ISAModule
{
	private final Assembler assembler;
	private final Simulator simulator;
	private final Function<String, Boolean> supportsProjectType;
	
	public ISAModule(Assembler assembler, Simulator simulator,
			Function<String, Boolean> supportsProjectType)
	{
		if (assembler == null || simulator == null || supportsProjectType == null)
			throw new IllegalArgumentException("All components must be non-null");
		
		this.assembler = assembler;
		this.simulator = simulator;
		this.supportsProjectType = supportsProjectType;
	}
	
	public Assembler getAssembler()
	{
		return assembler;
	}
	
	public Simulator getSimulator()
	{
		return simulator;
	}
	
	public boolean supportsProjectType(String projectType)
	{
		return supportsProjectType.apply(projectType);
	}
}
