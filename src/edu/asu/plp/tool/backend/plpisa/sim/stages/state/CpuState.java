package edu.asu.plp.tool.backend.plpisa.sim.stages.state;

public class CpuState implements Cloneable
{
	public int count;
	public int ifCount;
	public int idCount;
	
	public long currentInstruction;
	public long currentInstructionAddress;
	
	public long forwardCt1Memtoreg;
	public long forwardCt1Regwrite;
	
	public long forwardCt1Memwrite;
	public long forwardCt1Memread;
	public long forwardCt1Jal;
	public long forwardCt1Linkaddress;
	
	public long forwardCt1DestRegAddress;

	public long forwardDataAluResult;
	
	public long ct1Pcplus4;
	
	public long ct1Alusrc;
	public long ct1Aluop;
	public long ct1Regdest;
	
	public long ct1Jump;
	public long ct1Pcsrc;
	public long ct1Branch;
	public long ct1Branchtarget;
	public long ct1JumpTarget;
	
	public long ct1Forwardx;
	public long ct1Forwardy;
	
	public long ct1Memwrite;
	public long ct1Memread;
	public long ct1ForwardMemMem;
	
	public long ct1Memtoreg;
	public long ct1Regwrite;
	public long ct1DestRegAddress;
	public long ct1Linkaddress;
	public long ct1Jal;
	
	public long dataRs;
	public long dataRt;
	
	public long dataX;
	public long dataEffY;
	public long dataY;
	
	public long dataMemwritedata;
	public Long dataMemLoad;
	public long dataMemStore;
	
	public long dataMemreaddata;
	public long dataAluResult;
	public long dataRegwrite;
	
	public long dataImmediateSignextended;
	public long ct1RtAddress;
	public long ct1RdAddress;
	
	public long nextInstruction;
	public long nextInstructionAddress;
	
	public long nextForwardCt1Jal;
	public long nextForwardCt1LinkAddress;
	public long nextForwardCt1Regwrite;
	public long nextForwardCt1Memtoreg;
	public long nextForwardCt1Memread;
	public long nextForwardCt1Memwrite;
	
	public long nextForwardCt1DestRegAddress;
	
	public long nextForwardDataAluResult;
	
	public long nextCt1Pcplus4;
	
	public long nextCt1RdAddress;
	public long nextCt1RtAddress;
	public long nextCt1AluOp;
	public long nextCt1AluSrc;
	public long nextCt1BranchTarget;
	public long nextCt1Branch;
	public long nextCt1Jump;
	public long nextCt1Regdest;
	
	public long nextCt1Memwrite;
	public long nextCt1Memread;
	
	public long nextCt1Memtoreg;
	public long nextCt1Regwrite;
	public long nextCt1DestRegAddress;
	public long nextCt1Linkaddress;
	public long nextCt1Jal;
	
	public long nextDataMemwritedata;

	public long nextDataRs;
	public long nextDataRt;
	
	public long nextDataMemreaddata;
	public long nextDataAluResult;
	
	public long nextDataImmediateSignExtended;
	
	public long internalAluOut;
	
	public boolean hot;
	public boolean bubble;
	public boolean nextBubble;
	
	public boolean instructionRetired;
	
	public CpuState()
	{
		count = 0;
		ifCount = 0;
		idCount = 0;
		
		hot = false;
		bubble = false;
		nextBubble = false;
		instructionRetired = false;
	}
	
	public CpuState clone()
	{
		CpuState copy = new CpuState();
		
		copy.count = count;
		copy.ifCount = ifCount;
		copy.currentInstruction = currentInstruction;
		
		copy.currentInstruction = currentInstruction;
		copy.currentInstructionAddress = currentInstructionAddress;
		
		copy.forwardCt1Memtoreg = forwardCt1Memtoreg;
		copy.forwardCt1Regwrite = forwardCt1Regwrite;
		
		copy.forwardCt1Memwrite = forwardCt1Memwrite;
		copy.forwardCt1Memread = forwardCt1Memread;
		copy.forwardCt1Jal = forwardCt1Jal;
		copy.forwardCt1Linkaddress = forwardCt1Linkaddress;
		
		copy.forwardCt1DestRegAddress = forwardCt1DestRegAddress;

		copy.forwardDataAluResult = forwardDataAluResult;
		
		copy.ct1Pcplus4 = ct1Pcplus4;
		
		copy.ct1Alusrc = ct1Alusrc;
		copy.ct1Aluop = ct1Aluop;
		copy.ct1Regdest = ct1Regdest;
		
		copy.ct1Jump = ct1Jump;
		copy.ct1Pcsrc = ct1Pcsrc;
		copy.ct1Branch = ct1Branch;
		copy.ct1Branchtarget = ct1Branchtarget;
		copy.ct1JumpTarget = ct1JumpTarget;
		
		copy.ct1Forwardx = ct1Forwardx;
		copy.ct1Forwardy = ct1Forwardy;
		
		copy.ct1Memwrite = ct1Memwrite;
		copy.ct1Memread = ct1Memread;
		copy.ct1ForwardMemMem = ct1ForwardMemMem;
		
		copy.ct1Memtoreg = ct1Memtoreg;
		copy.ct1Regwrite = ct1Regwrite;
		copy.ct1DestRegAddress = ct1DestRegAddress;
		copy.ct1Linkaddress = ct1Linkaddress;
		copy.ct1Jal = ct1Jal;
		
		copy.dataRs = dataRs;
		copy.dataRt = dataRt;
		
		copy.dataX = dataX;
		copy.dataEffY = dataEffY;
		copy.dataY = dataY;
		
		copy.dataMemwritedata = dataMemwritedata;
		copy.dataMemLoad = dataMemLoad.longValue();
		copy.dataMemStore = dataMemStore;
		
		copy.dataMemreaddata = dataMemreaddata;
		copy.dataAluResult = dataAluResult;
		copy.dataRegwrite = dataRegwrite;
		
		copy.dataImmediateSignextended = dataImmediateSignextended;
		copy.ct1RtAddress = ct1RtAddress;
		copy.ct1RdAddress = ct1RdAddress;
		
		copy.nextInstruction = nextInstruction;
		copy.nextInstructionAddress = nextInstructionAddress;
		
		copy.nextForwardCt1Jal = nextForwardCt1Jal;
		copy.nextForwardCt1LinkAddress = nextForwardCt1LinkAddress;
		copy.nextForwardCt1Regwrite = nextForwardCt1Regwrite;
		copy.nextForwardCt1Memtoreg = nextForwardCt1Memtoreg;
		copy.nextForwardCt1Memread = nextForwardCt1Memread;
		copy.nextForwardCt1Memwrite = nextForwardCt1Memwrite;
		
		copy.nextForwardCt1DestRegAddress = nextForwardCt1DestRegAddress;
		
		copy.nextForwardDataAluResult = nextForwardDataAluResult;
		
		copy.nextCt1Pcplus4 = nextCt1Pcplus4;
		
		copy.nextCt1RdAddress = nextCt1RdAddress;
		copy.nextCt1RtAddress = nextCt1RtAddress;
		copy.nextCt1AluOp = nextCt1AluOp;
		copy.nextCt1AluSrc = nextCt1AluSrc;
		copy.nextCt1BranchTarget = nextCt1BranchTarget;
		copy.nextCt1Branch = nextCt1Branch;
		copy.nextCt1Jump = nextCt1Jump;
		copy.nextCt1Regdest = nextCt1Regdest;
		
		copy.nextCt1Memwrite = nextCt1Memwrite;
		copy.nextCt1Memread = nextCt1Memread;
		
		copy.nextCt1Memtoreg = nextCt1Memtoreg;
		copy.nextCt1Regwrite = nextCt1Regwrite;
		copy.nextCt1DestRegAddress = nextCt1DestRegAddress;
		copy.nextCt1Linkaddress = nextCt1Linkaddress;
		copy.nextCt1Jal = nextCt1Jal;
		
		copy.nextDataMemwritedata = nextDataMemwritedata;

		copy.nextDataRs = nextDataRs;
		copy.nextDataRt = nextDataRt;
		
		copy.nextDataMemreaddata = nextDataMemreaddata;
		copy.nextDataAluResult = nextDataAluResult;
		
		copy.nextDataImmediateSignExtended = nextDataImmediateSignExtended;
		
		copy.internalAluOut = internalAluOut;
		
		copy.hot = hot;
		copy.bubble = bubble;
		copy.nextBubble = nextBubble;
		
		copy.instructionRetired = instructionRetired;
		
		return copy;
	}
}
