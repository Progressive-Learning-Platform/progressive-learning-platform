package edu.asu.plp.tool.backend.plpisa.sim.stages.events;

import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class WriteBackStageStateResponse
{
	private CpuState currentWriteBackState;
	
	public WriteBackStageStateResponse(CpuState state)
	{
		this.currentWriteBackState = state;
	}
	
	public CpuState getMemoryStageState()
	{
		return currentWriteBackState;
	}

}
