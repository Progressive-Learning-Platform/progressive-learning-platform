package edu.asu.plp.tool.backend.plpisa;

public enum PLPInstruction
{
	SHIFT_LEFT_LOGICAL("sll", "function", 0x00, 1),
	SHIFT_LEFT_LOGICAL_VARIABLE("sllv", "function", 0x01, 0),
	SHIFT_RIGHT_LOGICAL("srl", "function", 0x02, 1),
	SHIFT_RIGHT_LOGICAL_VARIABLE("srlv", "function", 0x03, 0),
	JUMP_RETURN("jr", "function", 0x08, 2),
	JUMP_AND_LINK_RETURN("jalr", "function", 0x09, 9),
	MULTIPLY_LOW("mullo", "function", 0x10, 8),
	MULTIPLY_HIGH("mulhi", "function", 0x11, 8),
	//ADD("add", "function", 0x20),
	ADD_UNSIGNED("addu", "function", 0x21, 0),
	SUBTRACT_UNSIGNED("subu", "function", 0x23, 0),
	//SUBTRACT("sub", "function", 0x22),
	AND("and", "function", 0x24, 0),
	OR("or", "function", 0x25, 0),
	NOR("nor", "function", 0x27, 0),
	SET_ON_LESS("slt", "function", 0x2A, 0),
	SET_ON_LESS_UNSIGNED("sltu", "function", 0x2B, 0),
	
	JUMP("j", "opcode", 0x02, 7),
	JUMP_AND_LINK("jal", "opcode", 0x03, 7),
	BRANCH_EQUALS("beq", "opcode", 0x04, 3),
	BRANCH_NOT_EQUALS("bne", "", 0x05, 3),
	ADD_IMMEDIATE_UNSIGNED("addiu", "opcode", 0x09, 4),
	SET_ON_LESS_IMMEDIATE("slti", "opcode", 0x0A, 4),
	SET_ON_LESS_IMMEDIATE_UNSIGNED("sltiu", "opcode", 0x0B, 4),
	AND_IMMEDIATE("andi", "opcode", 0x0C, 4),
	OR_IMMEDIATE("ori", "opcode", 0x0D, 4),
	LOAD_UPPER_IMMEDIATE("lui", "opcode", 0x0F, 5),
	LOAD_WORD("lw", "opcode", 0x23, 6),
	STORE_WORD("sw", "opcode", 0x2B, 6);
	
	private String identifier;
	private String type;
	private int byteCode;
	private int instructionType;
	
	// TODO: consider removing "type"
	private PLPInstruction(String identifier, String type, int byteCode, int instructionType)
	{
		this.identifier = identifier;
		this.type = type;
		this.byteCode = byteCode;
		this.instructionType = instructionType;
	}

	public String getMnemonic()
	{
		return identifier;
	}

	public String getType()
	{
		return type;
	}

	public int getByteCode()
	{
		return byteCode;
	}

	public int getInstructionType()
	{
		return instructionType;
	}
	
}
