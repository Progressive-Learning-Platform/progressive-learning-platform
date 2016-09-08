package edu.asu.plp.tool.backend.plpisa.sim.stages;

import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public interface Stage
{
	void evaluate();
	
	void clock();
	
	void printVariables();
	
	void printNextVariables();
	
	String printInstruction();
	
	void reset();
	
	boolean isHot();
	
	CpuState getState();
}
