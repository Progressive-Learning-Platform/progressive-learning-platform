package edu.asu.plp.tool.backend.isa;

public interface Simulator
{
	boolean isRunning();
	
	boolean run();
	
	boolean step();
	
	void reset();
	
	boolean isPaused();
	
	boolean pause();
	
	boolean isSimModeEnabled();
	
	boolean toggleSimMode();
	
	boolean isProgramLoaded();
	
	boolean loadProgram(ASMImage assembledImage);
}
