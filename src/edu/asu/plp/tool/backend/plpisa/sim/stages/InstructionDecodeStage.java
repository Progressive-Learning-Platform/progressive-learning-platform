package edu.asu.plp.tool.backend.plpisa.sim.stages;

import com.google.common.eventbus.EventBus;

import edu.asu.plp.tool.backend.plpisa.InstructionExtractor;
import edu.asu.plp.tool.backend.plpisa.PLPInstruction;
import edu.asu.plp.tool.backend.plpisa.sim.SimulatorFlag;
import edu.asu.plp.tool.backend.plpisa.sim.SimulatorStatusManager;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.ExecuteStageStateRequest;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.ExecuteStageStateResponse;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.InstructionDecodeCompletion;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.MemoryStageStateRequest;
import edu.asu.plp.tool.backend.plpisa.sim.stages.events.MemoryStageStateResponse;
import edu.asu.plp.tool.backend.plpisa.sim.stages.state.CpuState;

public class InstructionDecodeStage implements Stage
{
	private EventBus bus;
	private InstructionDecodeEventHandler eventHandler;
	private SimulatorStatusManager statusManager;
	
	private CpuState state;
	
	// Get state from other stages thats required (Hard Porting)
	private CpuState currentExecuteStageState;
	private CpuState currentMemoryStageState;
	
	public InstructionDecodeStage(EventBus simulatorBus, SimulatorStatusManager statusManager)
	{
		this.bus = simulatorBus;
		this.eventHandler = new InstructionDecodeEventHandler();
		this.statusManager = statusManager;
		
		this.bus.register(eventHandler);

		this.state = new CpuState();
		
		reset();
	}
	
	@Override
	public void evaluate()
	{
		InstructionDecodeCompletion executePackage = new InstructionDecodeCompletion();
		CpuState postExecuteStageState = new CpuState();
		
		executePackage.setPostExecuteStageState(postExecuteStageState);
		
		currentExecuteStageState = null;
		currentMemoryStageState = null;
		
		bus.post(new ExecuteStageStateRequest());
		bus.post(new MemoryStageStateRequest());
		
		if (currentExecuteStageState == null)
			throw new IllegalStateException("Could not retrieve execute stage state.");
			
		if (currentMemoryStageState == null)
			throw new IllegalStateException("Could not retrieve memory stage state.");
			
		// TODO get from wherever the flag is
		boolean mem_ex_lw = statusManager.mem_ex_lw;
		
		byte opCode = (byte) InstructionExtractor.opcode(state.currentInstruction);
		byte funct = (byte) InstructionExtractor.funct(state.currentInstruction);
		
		long addressRt = InstructionExtractor.rt(state.currentInstruction);
		long addressRs = InstructionExtractor.rs(state.currentInstruction);
		
		long executeStageCurrentInstruction = currentExecuteStageState.currentInstruction;
		
		// The register being written to by load word
		long executeRt = InstructionExtractor.rt(executeStageCurrentInstruction);
		
		if (state.hot)
		{
			state.hot = false;
			postExecuteStageState.hot = true;
		}
		
		if (!state.bubble)
			state.idCount++;
			
		postExecuteStageState.nextBubble = state.bubble;
		postExecuteStageState.nextInstruction = state.currentInstruction;
		postExecuteStageState.nextInstructionAddress = state.currentInstructionAddress;
		
		if (currentMemoryStageState.hot && mem_ex_lw)
		{
			boolean executeEqualsAddressRs = executeRt == addressRs;
			boolean executeEqualsAddressRt = executeRt == addressRt;
			boolean executeForwardCt1Memread = currentExecuteStageState.forwardCt1Memread == 1;
			boolean isCurrentInstructionNotStoreWord = InstructionExtractor.opcode(
					state.currentInstruction) != PLPInstruction.STORE_WORD.getByteCode();
					
			if (executeEqualsAddressRt && (addressRt != 0) && executeForwardCt1Memread && isCurrentInstructionNotStoreWord)
			{
				statusManager.currentFlags |= SimulatorFlag.PLP_SIM_FWD_MEM_EX_LW_RT.getFlag();
				statusManager.isExecuteStalled = true;
			}
			if (executeEqualsAddressRs && (addressRs != 0) && executeForwardCt1Memread)
			{
				statusManager.isExecuteStalled = true;
				statusManager.currentFlags |= SimulatorFlag.PLP_SIM_FWD_MEM_EX_LW_RS.getFlag();	
			}
		}
		
		
		//TODO MEMORY MODULE
		//long rt = (addressRt == 0) ? 0 : (Long) memoryModule.read(addressRt);
		//postExecuteStageState.nextDataRt = rt;

		// long rs = (addressRs == 0) ? 0 : (Long) memoryModule.read(addressRs);
		//postExecuteStageState.nextDataRs = rs;
		
		long immediateField = InstructionExtractor.imm(state.currentInstruction);
		
		boolean isNotAndImmediate = opCode != PLPInstruction.AND_IMMEDIATE.getByteCode();
		boolean isNotOrImmediate = opCode != PLPInstruction.OR_IMMEDIATE.getByteCode();
		
		if (isNotAndImmediate && isNotOrImmediate)
		{
			long value = (short) immediateField & ((long) 0xfffffff << 4 | 0xf);
			postExecuteStageState.nextDataImmediateSignExtended = value;
		}
		else
		{
			postExecuteStageState.nextDataImmediateSignExtended = immediateField;
		}
		
		postExecuteStageState.nextCt1RdAddress = InstructionExtractor
				.rd(state.currentInstruction); // rd
		postExecuteStageState.nextCt1RtAddress = addressRt;
		
		postExecuteStageState.nextCt1AluOp = state.currentInstruction;
		
		postExecuteStageState.nextForwardCt1LinkAddress = state.ct1Pcplus4 + 4;
		
		executePackage.clearLogic();
		
		if (opCode != PLPInstruction.SHIFT_LEFT_LOGICAL.getByteCode())
		{
			switch (InstructionExtractor.instructionType(state.currentInstruction))
			{
				case 3: // beq and bne
					postExecuteStageState.nextCt1Branch = 1;
					break;
				case 4: // i-types
				case 5: // lui
					postExecuteStageState.nextCt1AluSrc = 1;
					postExecuteStageState.nextForwardCt1Regwrite = 1;
					break;
				case 6: // lw and sw
					if (opCode == PLPInstruction.LOAD_WORD.getByteCode())
					{
						postExecuteStageState.nextForwardCt1Memtoreg = 1;
						postExecuteStageState.nextForwardCt1Regwrite = 1;
						postExecuteStageState.nextForwardCt1Memread = 1;
					}
					else if (opCode == PLPInstruction.STORE_WORD.getByteCode())
					{
						postExecuteStageState.nextForwardCt1Memwrite = 1;
					}
					postExecuteStageState.nextCt1AluSrc = 1;
					break;
				case 7: // j and jal
					postExecuteStageState.nextCt1Jump = 1;
					if (InstructionExtractor.mnemonic(state.currentInstruction)
							.equals(PLPInstruction.JUMP_AND_LINK.getMnemonic()))
					{
						postExecuteStageState.nextCt1Regdest = 1;
						postExecuteStageState.nextCt1RdAddress = 31;
						postExecuteStageState.nextForwardCt1Regwrite = 1;
						postExecuteStageState.nextForwardCt1Jal = 1;
					}
					break;
				default:
					throw new IllegalStateException("Unhandled instruction type.");
			}
		}
		else
		{
			switch (InstructionExtractor.instructionType(state.currentInstruction))
			{
				case 0: // r-types
				case 1: // shifts
				case 8: // multiply
					postExecuteStageState.nextCt1Regdest = 1;
					postExecuteStageState.nextForwardCt1Regwrite = 1;
					break;
				case 2: // jr
					postExecuteStageState.nextCt1Jump = 1;
					break;
				case 9: // jalr
					postExecuteStageState.nextCt1Jump = 1;
					postExecuteStageState.nextCt1Regdest = 1;
					postExecuteStageState.nextForwardCt1Regwrite = 1;
					postExecuteStageState.nextForwardCt1Jal = 1;
					break;
				default:
					throw new IllegalStateException("Unhandled instruction type.");
			}
		}
		
		long nextCt1BranchTarget = state.ct1Pcplus4
				+ ((short) postExecuteStageState.nextDataImmediateSignExtended << 2);
		postExecuteStageState.nextCt1BranchTarget = nextCt1BranchTarget;
		
		bus.post(executePackage);
	}
	
	@Override
	public void clock()
	{
		state.bubble = state.nextBubble;
		state.ct1Pcplus4 = state.nextCt1Pcplus4;
		state.currentInstruction = state.nextInstruction;
		state.currentInstructionAddress = state.nextInstructionAddress;
		
		currentExecuteStageState = null;
		currentMemoryStageState = null;
	}
	
	@Override
	public void printVariables()
	{
		prettyPrintVariables("Instruction Decode Variables", state.currentInstruction,
				state.currentInstructionAddress, state.ct1Pcplus4);
	}
	
	@Override
	public void printNextVariables()
	{
		prettyPrintVariables("Instruction Decode Next Variables", state.nextInstruction,
				state.nextInstructionAddress, state.nextCt1Pcplus4);
	}
	
	@Override
	public String printInstruction()
	{
		String formattedInstructionAddress = (state.currentInstructionAddress == -1
				|| state.bubble) ? "--------"
						: String.format("08x", state.currentInstructionAddress);
						
		// TODO add MIPSInstr format like ability
		String instruction = String.format("%s %s %s %08x", "Instruction Decode:",
				formattedInstructionAddress, "Instruction:", state.currentInstruction);
				
		return instruction;
	}
	
	private void prettyPrintVariables(String title, long instruction,
			long instructionAddress, long ct1Pcplus4)
	{
		System.out.println(title);
		
		// TODO add MIPSInstr format like ability
		String instructionFormatted = String.format("%-25s %08x %s", "\tinstruction",
				instruction);
				
		String instructionAddressTemp = (state.currentInstructionAddress == -1)
				? "--------" : String.format("%08x", instructionAddress);
		String instructionAddressFormatted = String.format("%-25s %s",
				"\tinstruction address", instructionAddressTemp);
				
		String tct1Pcplus4Formatted = String.format("%-25s %08x", "\ttct1 pcplus4",
				ct1Pcplus4);
				
		System.out.println(instructionFormatted);
		System.out.println(instructionAddressFormatted);
		System.out.println(tct1Pcplus4Formatted);
		System.out.println();
	}
	
	@Override
	public void reset()
	{
		state.ifCount = 0;
		state.idCount = 0;
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
	
	public class InstructionDecodeEventHandler
	{
		private InstructionDecodeEventHandler()
		{
		
		}
		
		public void executeStageStateResponse(ExecuteStageStateResponse event)
		{
			currentExecuteStageState = event.getExecuteStageState();
		}
		
		public void memoryStageStateResponse(MemoryStageStateResponse event)
		{
			currentMemoryStageState = event.getMemoryStageState();
		}
	}
	
}
