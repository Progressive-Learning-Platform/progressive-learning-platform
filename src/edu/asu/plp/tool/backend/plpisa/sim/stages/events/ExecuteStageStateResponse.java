package edu.asu.plp.tool.backend.plpisa.sim.stages.events;

import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class ExecuteStageStateResponse
{
	private CpuState state;
	
	public ExecuteStageStateResponse(CpuState state)
	{
		this.state = state;
	}

	public CpuState getExecuteStageState()
	{
		return state;
	}

}
