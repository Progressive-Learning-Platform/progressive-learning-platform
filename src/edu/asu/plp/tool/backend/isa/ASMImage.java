package edu.asu.plp.tool.backend.isa;

import edu.asu.plp.tool.backend.BiDirectionalOneToManyMap;

/**
 * Image of an assembled program. This class needs to be re-instantiated
 * everytime the assembly changes.
 * 
 * @author Nesbitt, Morgan
 *
 */
public interface ASMImage
{
	BiDirectionalOneToManyMap<ASMInstruction, ? extends ASMDisassembly> getAssemblyDisassemblyMap();
}
