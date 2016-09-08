package edu.asu.plp.tool.backend.plpisa.sim.stages;

import com.google.common.eventbus.EventBus;

import edu.asu.plp.tool.backend.plpisa.InstructionExtractor;
import edu.asu.plp.tool.backend.plpisa.sim.SimulatorStatusManager;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.ExecuteCompletion;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.MemoryCompletion;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.MemoryStageStateRequest;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.MemoryStageStateResponse;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.WriteBackStageStateRequest;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.WriteBackStageStateResponse;
import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class MemoryStage implements Stage
{
	private EventBus bus;
	private MemoryEventHandler eventHandler;
	private SimulatorStatusManager statusManager;
	
	private CpuState state;
	private CpuState currentWriteBackStageState;
	
	public MemoryStage(EventBus simulatorBus, SimulatorStatusManager statusManager)
	{
		this.bus = simulatorBus;
		this.eventHandler = new MemoryEventHandler();
		this.statusManager = statusManager;
		
		this.bus.register(eventHandler);
		
		this.state = new CpuState();
		
		reset();
	}
	
	@Override
	public void evaluate()
	{
		//@formatter:off
		MemoryCompletion writeBackPackage = new MemoryCompletion();
		CpuState postWriteBackStageState = new CpuState();
		
		writeBackPackage.setPostState(postWriteBackStageState);
		
		currentWriteBackStageState = null;
		
		bus.post(new WriteBackStageStateRequest());
		
		if(currentWriteBackStageState == null)
			throw new IllegalStateException("Could not retrieve write back stage state.");
		
		postWriteBackStageState.nextBubble = state.bubble;
		postWriteBackStageState.nextInstruction = state.currentInstruction;
		postWriteBackStageState.nextInstructionAddress = state.currentInstructionAddress;
		
		if(state.hot)
		{
			state.hot = false;
			postWriteBackStageState.hot = true;
		}
		
		if(!state.bubble)
			state.count++;
		
		boolean ct1MemtoRegOne = currentWriteBackStageState.ct1Memtoreg == 1;
		boolean memWriteOne = state.ct1Memwrite == 1;
		boolean destRegAddress = currentWriteBackStageState.ct1DestRegAddress == InstructionExtractor.rt(state.currentInstruction);
		boolean rtInstruction = InstructionExtractor.rt(state.currentInstruction) != 0;
		
		state.ct1ForwardMemMem = (ct1MemtoRegOne && memWriteOne && destRegAddress && rtInstruction) ? 1: 0;// && mem_mem 
		//sim_flags |= (state.ct1ForwardMemMem == 1 ? SimulatorFlag.PLP_SIM_FWD_MEM_MEM : 0);
		state.dataMemStore = state.ct1ForwardMemMem == 1 ? postWriteBackStageState.dataRegwrite : state.dataMemwritedata;
		
		postWriteBackStageState.nextInstruction = state.currentInstruction;
		postWriteBackStageState.nextInstructionAddress = state.currentInstructionAddress;
		
		postWriteBackStageState.nextCt1Memtoreg = state.forwardCt1Memtoreg;
		postWriteBackStageState.nextCt1Regwrite = state.forwardCt1Regwrite;
		postWriteBackStageState.nextCt1DestRegAddress = state.forwardCt1DestRegAddress;
		postWriteBackStageState.nextCt1Jal = state.forwardCt1Jal;
		postWriteBackStageState.nextCt1Linkaddress = state.forwardCt1Linkaddress;
		
		postWriteBackStageState.nextDataAluResult = state.forwardDataAluResult;
		
		//state.dataMemLoad = (state.ct1Memread == 1) ? (Long) simBus.read(state.forwardDataAluResult) : 0;
		if(state.dataMemLoad == null)
			throw new IllegalArgumentException("Bus returned no data. Sim Bus Error: Memory Stage");
		
		postWriteBackStageState.nextDataMemreaddata = state.dataMemLoad;
		
		if(state.ct1Memwrite == 1)
		{
			//simBus.write(state.forwardDataAluResult, state.dataMemStore, false);
		}
		
		bus.post(writeBackPackage);
		//@formatter:on
	}
	
	@Override
	public void clock()
	{
		state.bubble = state.nextBubble;
		state.currentInstruction = state.nextInstruction;
		state.currentInstructionAddress = state.nextInstructionAddress;
		
		state.forwardCt1Memtoreg = state.nextForwardCt1Memtoreg;
		state.forwardCt1Regwrite = state.nextForwardCt1Regwrite;
		state.forwardCt1DestRegAddress = state.nextForwardCt1DestRegAddress;
		state.dataAluResult = state.nextDataAluResult;
		state.forwardCt1Linkaddress = state.nextForwardCt1LinkAddress;
		state.forwardCt1Jal = state.nextForwardCt1Jal;
		
		state.ct1Memwrite = state.nextCt1Memwrite;
		state.ct1Memread = state.nextCt1Memread;
		state.dataMemwritedata = state.nextDataMemwritedata;
	}
	
	@Override
	public void printVariables()
	{
		String writeDataForwarded = ""; // (simFlags & SimulatorFlag.PLP_SIM_FWD_MEM_MEM)
										// == 0 ? "" : " (forwarded)";
		int spaceSize = -35;
		
		System.out.println("MEM vars");
		System.out.println(String.format("%" + spaceSize + "s %08x %s", "\tInstruction",
				state.nextInstruction,
				InstructionExtractor.format(state.currentInstruction)));
				
		String formattedInstructionAddress = ((state.currentInstructionAddress == -1)
				? "--------" : String.format("%08x", state.currentInstructionAddress));
		System.out.println(String.format("%" + spaceSize + "s %s", "\tInstructionAddress",
				formattedInstructionAddress));
				
		System.out.println(String.format("%" + spaceSize + "s %x", "\tForwardCt1MemToReg",
				state.forwardCt1Memtoreg));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tForwardCt1Regwrite",
				state.forwardCt1Regwrite));
		System.out.println(String.format("%" + spaceSize + "s %x",
				"\tForwardCt1DestRegAddress", state.forwardCt1DestRegAddress));
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tForwardCt1LinkAddress", state.forwardCt1Linkaddress));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tForwardCt1Jal",
				state.forwardCt1Jal));
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tnForwardDataAluResult", state.forwardDataAluResult));
				
		System.out.println(String.format("%" + spaceSize + "s %x", "\tCt1Memwrite",
				state.ct1Memwrite));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tCt1Memread",
				state.ct1Memread));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tct1ForwardMemMem",
				state.ct1ForwardMemMem));
				
		System.out.println(String.format("%" + spaceSize + "s %08x", "\tDataMemwritedata",
				state.dataMemwritedata));
		System.out.println(String.format("%" + spaceSize + "s %08x", "\tdataMemLoad*",
				state.dataMemLoad));
		System.out.println(String.format("%" + spaceSize + "s %08x %s", "\tdataMemStore*",
				state.dataMemStore, writeDataForwarded));
		System.out.println();
	}
	
	@Override
	public void printNextVariables()
	{
		int spaceSize = -35;
		
		System.out.println("MEM next vars");
		System.out.println(String.format("%" + spaceSize + "s %08x %s",
				"\tNextInstruction", state.nextInstruction,
				InstructionExtractor.format(state.nextInstruction)));
				
		String formattedInstructionAddress = ((state.currentInstructionAddress == -1)
				? "--------" : String.format("%08x", state.nextInstructionAddress));
		System.out.println(String.format("%" + spaceSize + "s %s",
				"\tNextInstructionAddress", formattedInstructionAddress));
				
		System.out.println(String.format("%" + spaceSize + "s %x",
				"\tNextForwardCt1MemToReg", state.nextForwardCt1Memtoreg));
		System.out.println(String.format("%" + spaceSize + "s %x",
				"\tNextForwardCt1Regwrite", state.nextForwardCt1Regwrite));
		System.out.println(String.format("%" + spaceSize + "s %x",
				"\tNextForwardCt1DestRegAddress", state.nextForwardCt1DestRegAddress));
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tnextForwardCt1LinkAddress", state.nextForwardCt1LinkAddress));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tnextForwardCt1Jal",
				state.nextForwardCt1Jal));
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tnextForwardDataAluResult", state.nextForwardDataAluResult));
				
		System.out.println(String.format("%" + spaceSize + "s %x", "\tnextCt1Memwrite",
				state.nextCt1Memwrite));
		System.out.println(String.format("%" + spaceSize + "s %x", "\tnextCt1Memread",
				state.nextCt1Memread));
				
		System.out.println(String.format("%" + spaceSize + "s %08x",
				"\tnextDataMemwritedata", state.nextDataMemwritedata));
		System.out.println();
	}
	
	@Override
	public String printInstruction()
	{
		String formattedInstructionAddress = (state.currentInstructionAddress == -1
				|| state.bubble) ? "--------"
						: String.format("08x", state.currentInstructionAddress);
						
		// TODO add MIPSInstr format like ability
		String instruction = String.format("%s %s %s %08x %s", "Execute:",
				formattedInstructionAddress, "Instruction:", state.currentInstruction,
				" : " + InstructionExtractor.format(state.currentInstruction));
				
		return instruction;
	}
	
	@Override
	public void reset()
	{
		state.count = 0;
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
	
	public class MemoryEventHandler
	{
		private MemoryEventHandler()
		{
		
		}
		
		public void executeCompletionEvent(ExecuteCompletion event)
		{
			CpuState postState = event.getPostMemoryState();
			
			state.nextBubble = postState.nextBubble;
			state.nextInstruction = postState.nextInstruction;
			state.nextInstructionAddress = postState.nextInstructionAddress;
			
			state.hot = (postState.hot) ? postState.hot : state.hot;
			
			state.nextForwardDataAluResult = postState.nextForwardDataAluResult;
			
			state.nextForwardCt1Memtoreg = postState.nextForwardCt1Memtoreg;
			state.nextForwardCt1Regwrite = postState.nextForwardCt1Regwrite;
			state.nextForwardCt1DestRegAddress = postState.nextForwardCt1DestRegAddress;
			
			state.nextCt1Memwrite = postState.nextCt1Memwrite;
			state.nextCt1Memread = postState.nextCt1Memread;
			state.nextForwardCt1LinkAddress = postState.nextForwardCt1LinkAddress;
			
			state.nextForwardCt1Jal = postState.nextForwardCt1Jal;
			
			state.nextDataMemwritedata = postState.nextDataMemwritedata;
		}
		
		public void stateRequested(MemoryStageStateRequest event)
		{
			bus.post(new MemoryStageStateResponse(state.clone()));
		}
		
		public void writeBackStageStateResponse(WriteBackStageStateResponse event)
		{
			currentWriteBackStageState = event.getMemoryStageState();
		}
	}
	
}
