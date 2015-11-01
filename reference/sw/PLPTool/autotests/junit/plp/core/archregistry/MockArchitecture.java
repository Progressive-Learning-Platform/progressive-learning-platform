package junit.plp.core.archregistry;

import plptool.PLPArchitecture;
import plptool.PLPAsm;
import plptool.PLPSerialProgrammer;
import plptool.PLPSimCore;
import plptool.PLPSimCoreGUI;
import plptool.gui.ProjectDriver;

public class MockArchitecture extends PLPArchitecture
{
	public MockArchitecture(int archID, String identifier, ProjectDriver plp)
	{
		super(archID, identifier, plp);
	}

	@Override
	public PLPAsm createAssembler()
	{
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public PLPSimCore createSimCore()
	{
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public PLPSimCoreGUI createSimCoreGUI()
	{
		// TODO Auto-generated method stub
		return null;
	}
	
	@Override
	public PLPSerialProgrammer createProgrammer()
	{
		// TODO Auto-generated method stub
		return null;
	}
	
}
