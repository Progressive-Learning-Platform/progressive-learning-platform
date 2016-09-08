package edu.asu.plp.tool.backend.plpisa.sim.stages.events;

import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class MemoryCompletion
{
	private CpuState postMemoryState;
	
	public void setPostState(CpuState postMemoryStageState)
	{
		this.postMemoryState = postMemoryStageState;
	}

	public CpuState getPostMemoryState()
	{
		return postMemoryState;
	}
}
