package edu.asu.plp.tool.backend.isa;

import java.util.Collections;
import java.util.List;

import edu.asu.plp.tool.backend.isa.exceptions.AssemblerException;

public interface Assembler
{
	ASMImage assemble(List<ASMFile> asmFiles) throws AssemblerException;
	
	default ASMImage assemble(ASMFile asmFile) throws AssemblerException
	{
		List<ASMFile> asmFiles = Collections.singletonList(asmFile);
		return assemble(asmFiles);
	}
}
