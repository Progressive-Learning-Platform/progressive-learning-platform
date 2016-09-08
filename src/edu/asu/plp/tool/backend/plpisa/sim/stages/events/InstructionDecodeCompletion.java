package edu.asu.plp.tool.backend.plpisa.sim.stages.events;

import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class InstructionDecodeCompletion
{
	private boolean willClearLogic;
	private CpuState postExecuteStageState;
	
	public InstructionDecodeCompletion()
	{
		this.willClearLogic = false;
		this.postExecuteStageState = null;
	}

	public void clearLogic()
	{
		willClearLogic = true;
	}

	public void setPostExecuteStageState(CpuState postExecuteStageState)
	{
		this.postExecuteStageState = postExecuteStageState;
	}

	public boolean willClearLogic()
	{
		return willClearLogic;
	}
	
	public CpuState getPostState()
	{
		return postExecuteStageState;
	}

}
