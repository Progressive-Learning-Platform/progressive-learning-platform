package edu.asu.plp.tool.backend.plpisa.sim.stages;

import com.google.common.eventbus.EventBus;

import edu.asu.plp.tool.backend.plpisa.InstructionExtractor;
import edu.asu.plp.tool.backend.plpisa.sim.SimulatorStatusManager;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.MemoryCompletion;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.WriteBackStageStateRequest;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.WriteBackStageStateResponse;
import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class WriteBackStage implements Stage
{
	private EventBus bus;
	private WriteBackEventHandler eventHandler;
	private SimulatorStatusManager statusManager;
	
	private CpuState state;
	
	public WriteBackStage(EventBus simulatorBus, SimulatorStatusManager statusManager)
	{
		this.bus = simulatorBus;
		this.eventHandler = new WriteBackEventHandler();
		this.statusManager = statusManager;
		
		this.bus.register(eventHandler);
		
		this.state = new CpuState();
		
		reset();
	}
	
	@Override
	public void evaluate()
	{
		if (state.hot)
		{
			state.hot = false;
			state.instructionRetired = true;
		}
		
		if (!state.bubble)
			state.count++;
			
		long internal2x1 = (state.ct1Jal == 0) ? state.dataAluResult
				: state.ct1Linkaddress;
				
		state.dataRegwrite = (state.ct1Memtoreg == 0) ? internal2x1
				: state.dataMemreaddata;
				
		if (state.ct1Regwrite == 1 && state.ct1DestRegAddress != 0)
		{
			// TODO memorymodule
			// regFile.write(state.ct1DestRegAddress, state.dataRegwrite, false);
		}
	}
	
	@Override
	public void clock()
	{
		state.bubble = state.nextBubble;
		state.currentInstruction = state.nextInstruction;
		state.currentInstructionAddress = state.nextInstructionAddress;
		
		state.ct1Memtoreg = state.nextCt1Memtoreg;
		state.ct1Regwrite = state.nextCt1Regwrite;
		state.ct1DestRegAddress = state.nextCt1DestRegAddress;
		state.ct1Linkaddress = state.nextCt1Linkaddress;
		state.ct1Jal = state.nextCt1Jal;
		
		state.dataMemreaddata = state.nextDataMemreaddata;
		state.dataAluResult = state.nextDataAluResult;
	}
	
	@Override
	public void printVariables()
	{
		int spaceSize = -35;
		
		System.out.println("MEM vars");
		System.out.println(String.format("%" + spaceSize + "s %08x %s", "\tInstruction",
				state.currentInstruction,
				InstructionExtractor.format(state.currentInstruction)));
				
		String formattedInstructionAddress = ((state.currentInstructionAddress == -1
				|| state.bubble) ? "--------"
						: String.format("%08x", state.currentInstructionAddress));
		System.out.println(String.format("%" + spaceSize + "s %s", "\tInstructionAddress",
				formattedInstructionAddress));
				
		System.out.println(String.format("%" + spaceSize + "s %x", "\tCt1MemToReg",
				state.ct1Memtoreg));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tCt1Regwrite",
				state.ct1Regwrite));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tCt1DestRegAddress",
				state.ct1DestRegAddress));
		System.out.println(String.format("%" + spaceSize + "s %08x", "\tCt1LinkAddress",
				state.ct1Linkaddress));
		System.out.println(
				String.format("%" + spaceSize + "s %x", "\tCt1Jal", state.ct1Jal));
				
		System.out.println(String.format("%" + spaceSize + "s %08x", "\tDataMemreadData",
				state.dataMemreaddata));
		System.out.println(String.format("%" + spaceSize + "s %08x", "\tDataAluResult",
				state.dataAluResult));
		System.out.println(String.format("%" + spaceSize + "s %08x", "\tDataRegWrite",
				state.dataRegwrite));
		System.out.println();
	}
	
	@Override
	public void printNextVariables()
	{
		int spaceSize = -35;
		
		System.out.println("MEM Next vars");
		System.out.println(String.format("%" + spaceSize + "s %08x %s",
				"\tNextInstruction", state.nextInstruction,
				InstructionExtractor.format(state.nextInstruction)));
				
		String formattedInstructionAddress = ((state.nextInstructionAddress == -1)
				? "--------" : String.format("%08x", state.nextInstructionAddress));
		System.out.println(String.format("%" + spaceSize + "s %s",
				"\tNextInstructionAddress", formattedInstructionAddress));
				
		System.out.println(String.format("%" + spaceSize + "s %x", "\tNextCt1MemToReg",
				state.nextCt1Memtoreg));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tNextCt1Regwrite",
				state.nextCt1Regwrite));
		System.out.println(String.format("%" + spaceSize + "s %x",
				"\tNextCt1DestRegAddress", state.nextCt1DestRegAddress));
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tNextCt1LinkAddress", state.nextCt1Linkaddress));
		System.out.println(
				String.format("%" + spaceSize + "s %x", "\tCt1Jal", state.nextCt1Jal));
				
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tNextDataMemreadData", state.nextDataMemreaddata));
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tNextDataAluResult", state.nextDataAluResult));
		System.out.println();
	}
	
	@Override
	public String printInstruction()
	{
		String formattedInstructionAddress = (state.currentInstructionAddress == -1
				|| state.bubble) ? "--------"
						: String.format("08x", state.currentInstructionAddress);
						
		// TODO add MIPSInstr format like ability
		String instruction = String.format("%s %s %s %08x %s", "WriteBack:",
				formattedInstructionAddress, "Instruction:", state.currentInstruction,
				" : " + InstructionExtractor.format(state.currentInstruction));
				
		return instruction;
	}
	
	@Override
	public void reset()
	{
		state.count = 0;
	}
	
	public void retireInstruction()
	{
		state.instructionRetired = false;
	}
	
	@Override
	public boolean isHot()
	{
		return state.hot;
	}
	
	@Override
	public CpuState getState()
	{
		return state;
	}
	
	public class WriteBackEventHandler
	{
		private WriteBackEventHandler()
		{
		
		}
		
		public void memoryCompletionEvent(MemoryCompletion event)
		{
			CpuState postState = event.getPostMemoryState();
			
			state.nextBubble = postState.nextBubble;
			state.nextInstruction = postState.nextInstruction;
			state.nextInstructionAddress = postState.nextInstructionAddress;
			
			state.hot = (postState.hot) ? postState.hot : state.hot;
			
			state.nextCt1Memtoreg = postState.nextCt1Memtoreg;
			state.nextCt1Regwrite = postState.nextCt1Regwrite;
			state.nextCt1DestRegAddress = postState.nextCt1DestRegAddress;
			state.nextCt1Jal = postState.nextCt1Jal;
			state.nextCt1Linkaddress = postState.nextCt1Linkaddress;
			
			state.nextDataAluResult = postState.nextDataAluResult;
			
			state.nextDataMemreaddata = postState.nextDataMemreaddata;
		}
		
		public void stateRequested(WriteBackStageStateRequest event)
		{
			bus.post(new WriteBackStageStateResponse(state.clone()));
		}
	}

}
