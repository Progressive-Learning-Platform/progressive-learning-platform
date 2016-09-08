package edu.asu.plp.tool.backend.plpisa;

import edu.asu.plp.tool.backend.BiDirectionalOneToManyMap;
import edu.asu.plp.tool.backend.isa.ASMDisassembly;
import edu.asu.plp.tool.backend.isa.ASMImage;
import edu.asu.plp.tool.backend.isa.ASMInstruction;

public class PLPASMImage implements ASMImage
{
	private final BiDirectionalOneToManyMap<ASMInstruction, ? extends ASMDisassembly> assemblyToDisassemblyMap;
	
	public PLPASMImage(
			BiDirectionalOneToManyMap<ASMInstruction, ? extends ASMDisassembly> assemblyToDisassemblyMap)
	{
		this.assemblyToDisassemblyMap = assemblyToDisassemblyMap;
	}
	
	@Override
	public BiDirectionalOneToManyMap<ASMInstruction, ? extends ASMDisassembly> getAssemblyDisassemblyMap()
	{
		return assemblyToDisassemblyMap;
	}
	
}
