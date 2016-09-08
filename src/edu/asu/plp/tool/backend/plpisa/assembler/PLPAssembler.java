package edu.asu.plp.tool.backend.plpisa.assembler;

import java.util.List;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.backend.isa.ASMImage;
import edu.asu.plp.tool.backend.isa.Assembler;
import edu.asu.plp.tool.backend.isa.exceptions.AssemblerException;

public class PLPAssembler implements Assembler
{
	@Override
	public ASMImage assemble(List<ASMFile> asmFiles) throws AssemblerException
	{
		DisposablePLPAssembler assembler = new DisposablePLPAssembler(asmFiles);
		return assembler.assemble();
	}
}
