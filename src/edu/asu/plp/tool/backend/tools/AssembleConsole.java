package edu.asu.plp.tool.backend.tools;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.StringJoiner;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.DefaultParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.io.FileUtils;

import com.google.common.base.Joiner;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.backend.isa.Assembler;
import edu.asu.plp.tool.backend.isa.exceptions.AssemblerException;
import edu.asu.plp.tool.backend.plpisa.assembler.PLPAssembler;
import edu.asu.plp.tool.backend.util.FileUtil;
import edu.asu.plp.tool.prototype.model.SimpleASMFile;

//@formatter:off
/**
 * <p>Command Line Interface for a Generic Assembler.</p>
 * <p>Can use any Assembly Assembler that implements {@link edu.asu.plp.tool.backend.isa.Assembler}</p>
 * <p>For examples on usage: use the -h or -help flag.</p>
 * <p>For runnable examples use the -e or -example. You can find the example names in the help menu.</p>
 * 
 * @author Nesbitt, Morgan
 *		
 */
//@formatter:on
public class AssembleConsole
{
	protected static String assemblerName;
	protected static Assembler assembler;
	
	protected static CommandLine commandLine;
	protected static Options options;
	
	protected static List<ASMFile> projectFiles;
	
	protected static boolean isBenchMarking;
	
	// Sample Projects
	protected static HashMap<String, String> exampleProjects;
	
	public static void main(String[] args)
	{
		configureStaticSettings();
		
		initializeCommandLineOptions();
		
		parseCLIArguments(args);
		
		configureEnteredSettings();
		
		long startTime = System.nanoTime();
		
		try
		{
			assembler = new PLPAssembler();
			assembler.assemble(projectFiles);
		}
		catch (AssemblerException e)
		{
			e.printStackTrace();
			System.exit(1);
		}
		
		long endTime = System.nanoTime();
		
		if (isBenchMarking)
			System.out.println(String.format(
					"\nCompleted Assembling process in %.2f seconds",
					(endTime - startTime) * 1e-9));
	}
	
	private static void configureStaticSettings()
	{
		exampleProjects = new HashMap<>();
		projectFiles = new ArrayList<>();
		
		exampleProjects.put("one-sub-directory-project",
				"examples/PLP Projects/leds_test.plp");
		exampleProjects.put("no-sub-directory-project",
				"examples/PLP Projects/memtest.plp");
		exampleProjects.put("file-count",
				"examples/PLP Projects/universe/stress/file_count/file-count.plp");
		exampleProjects
				.put("one-file",
						"examples/Stripped PLP Projects (ASM Only)/universe/encapsulated/one-file.asm");
		exampleProjects
				.put("file-length",
						"examples/Stripped PLP Projects (ASM Only)/universe/stress/file_length/main.asm");
	}
	
	private static void initializeCommandLineOptions()
	{
		options = new Options();
		options.addOption("h", "help", false, "show help");
		options.addOption("b", "benchmark", false, "enable benchmark timing ouput");
		options.addOption("a", "assembler", true, "set assembler from choices: plp, mips");
		options.addOption("p", "project", true, "set project path to assemble");
		options.addOption("f", "file", true, "set path of a single asm file to assemble");
		options.addOption("e", "example", true, "set example from choices: "
				+ keySetExample());
	}
	
	private static String keySetExample()
	{
		return Joiner.on(", ").join(exampleProjects.keySet());
	}
	
	private static void parseCLIArguments(String[] args)
	{
		CommandLineParser parser = new DefaultParser();
		
		try
		{
			commandLine = parser.parse(options, args);
		}
		catch (ParseException e)
		{
			e.printStackTrace();
			System.exit(-1);
		}
	}
	
	private static void configureEnteredSettings()
	{
		if (commandLine.hasOption("h"))
			printHelp();
		
		if (commandLine.hasOption("b"))
			isBenchMarking = true;
		
		if (commandLine.hasOption("a"))
		{
			assemblerName = commandLine.getOptionValue("a").toLowerCase();
			if (!assemblerName.equalsIgnoreCase("plp"))
			{
				System.out.println("Only assembler currently supported is plp.");
				System.exit(-1);
			}
		}
		
		File assembleFile;
		if (commandLine.hasOption("p"))
		{
			// TODO enforce correct project ending relative to project type (.plp for plp)
			// TODO parse project into list of asm files
			assembleFile = new File(commandLine.getOptionValue("p"));
			parseProject(assembleFile);
		}
		else if (commandLine.hasOption("f"))
		{
			// TODO enforce correct file type (.asm for plp)
			assembleFile = new File(commandLine.getOptionValue("f"));
			parseFile(assembleFile);
		}
		else if (commandLine.hasOption("e"))
		{
			String exampleName = commandLine.getOptionValue("e").toLowerCase();
			if (exampleProjects.containsKey(exampleName))
			{
				assembleFile = new File(exampleProjects.get(exampleName));
				if (FileUtil.isValidFile(assembleFile))
				{
					parseFile(assembleFile);
				}
				else if (FileUtil.isValidProject(assembleFile))
				{
					parseProject(assembleFile);
				}
				else
				{
					System.out
							.println("Oops, something went wrong with the file path of this example!");
					System.exit(-1);
				}
			}
			else
			{
				System.out.println("Unknown example was entered, found: " + exampleName
						+ ".");
				System.out
						.println("Please see the help (via -h or -help) for possible examples.");
				System.exit(-1);
			}
		}
		else
		{
			System.out.println("No project, file, or example specified to assemble.");
			System.exit(0);
		}
	}
	
	private static void parseFile(File assembleFile)
	{
		if (FileUtil.isValidFile(assembleFile))
		{
			try
			{
				String fileContent = FileUtils.readFileToString(assembleFile);
				ASMFile asmFile = new SimpleASMFile(null, assembleFile.getName());
				asmFile.setContent(fileContent);
				projectFiles = Arrays.asList(asmFile);
			}
			catch (IOException e)
			{
				e.printStackTrace();
				System.exit(-1);
			}
			if (projectFiles == null || projectFiles.isEmpty())
			{
				System.out
						.println("An error occured attempting to open: " + assembleFile);
				System.exit(-1);
			}
		}
		else
		{
			System.out.println("Provided file was not valid: " + assembleFile.getPath());
			System.exit(-1);
		}
	}
	
	private static void parseProject(File assembleFile)
	{
		if (FileUtil.isValidProject(assembleFile))
		{
			projectFiles = FileUtil.openProject(assembleFile);
			if (projectFiles == null)
			{
				System.out
						.println("An error occured attempting to open: " + assembleFile);
				System.exit(-1);
			}
		}
		else
		{
			System.out.println("Provided project file was not valid: "
					+ assembleFile.getPath());
			System.exit(-1);
		}
	}
	
	private static void printHelp()
	{
		HelpFormatter formatter = new HelpFormatter();
		formatter.printHelp("Assembler Console",
				"Your one stop shop, when you are lost and don't know what to do!",
				options, "Thank you for using Assembler Console!");
		System.exit(0);
	}
	
	/**
	 * Reads a file into a single string, preserving line breaks.
	 * <p>
	 * All line breaks will be replaced with the newline character (\n) regardless of
	 * whether or not the file uses windows convention or not (\n\r)
	 * 
	 * @param path
	 *            Path to the file that will be read
	 * @return The contents of the specified file, as a String
	 */
	private static String getFileContents(Path path)
	{
		try
		{
			List<String> fileLines = Files.readAllLines(path);
			StringJoiner fileJoiner = new StringJoiner("\n");
			
			for (String line : fileLines)
				fileJoiner.add(line);
			
			return fileJoiner.toString();
		}
		catch (Exception e)
		{
			e.printStackTrace();
			return null;
		}
	}
}
