package edu.asu.plp.tool.backend.tools;

import java.io.File;
import java.util.List;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.backend.isa.ASMImage;
import edu.asu.plp.tool.backend.isa.Assembler;
import edu.asu.plp.tool.backend.isa.Simulator;
import edu.asu.plp.tool.backend.isa.exceptions.AssemblerException;
import edu.asu.plp.tool.backend.plpisa.assembler.PLPAssembler;
import edu.asu.plp.tool.backend.plpisa.sim.PLPSimulator;
import edu.asu.plp.tool.backend.util.FileUtil;

/**
 * Debugging tool.
 * 
 * @author Nesbitt, Morgan
 * 
 */
public class SimulatorConsole
{
	protected static Assembler assembler;
	protected static Simulator simulator;
	
	protected static List<ASMFile> projectFiles;
	protected static ASMImage currentImage;
	
	public static void main(String[] args)
	{
		configureStaticSettings();
		
		initializeCommandLineOptions();
		
		parseCLIArguments(args);
		
		configureEnteredSettings();
		
		long startTime = System.nanoTime();
		
		try
		{
			simulator = new PLPSimulator();
			simulator.loadProgram(currentImage);
		}
		catch (Exception exception)
		{
			exception.printStackTrace();
			System.exit(-1);
		}
		
		simulator.step();
		
		long endTime = System.nanoTime();
		
		System.out.println(String.format(
				"\nCompleted Simulating process in %.2f seconds",
				(endTime - startTime) * 1e-9));
	}
	
	private static void configureStaticSettings()
	{
		projectFiles = FileUtil
				.openProject(new File("examples/PLP Projects/memtest.plp"));
		if (projectFiles == null)
		{
			System.out.println("Error loading sample project.");
			System.exit(-1);
		}
		assembler = new PLPAssembler();
		
		try
		{
			currentImage = assembler.assemble(projectFiles);
		}
		catch (AssemblerException e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	private static void initializeCommandLineOptions()
	{
	}
	
	private static void parseCLIArguments(String[] args)
	{
	}
	
	private static void configureEnteredSettings()
	{
	}
	
}
