package edu.asu.plp.tool.backend.plpisa.sim.stages.events;

import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class MemoryStageStateResponse
{
	private CpuState state;
	
	public MemoryStageStateResponse(CpuState state)
	{
		this.state = state;
	}

	public CpuState getMemoryStageState()
	{
		return state;
	}

}
