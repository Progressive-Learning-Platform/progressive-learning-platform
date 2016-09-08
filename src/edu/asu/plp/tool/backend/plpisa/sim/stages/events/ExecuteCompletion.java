package edu.asu.plp.tool.backend.plpisa.sim.stages.events;

import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class ExecuteCompletion
{
	private CpuState postMemoryState;
	
	public void setPostMemoryStageState(CpuState postMemoryStageState)
	{
		this.postMemoryState = postMemoryStageState;
	}

	public CpuState getPostMemoryState()
	{
		return postMemoryState;
	}

}
