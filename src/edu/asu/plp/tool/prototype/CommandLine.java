package edu.asu.plp.tool.prototype;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

import edu.asu.plp.tool.prototype.model.PLPProject;
import edu.asu.plp.tool.prototype.model.SimpleASMFile;

//This whole Console.java will be updated

public class CommandLine
{
	public static void main(String[] args)
	{
		CommandList commandList = new CommandList();
		Controller controller;
		String[] commandInput;
		
		//for demo to clear console
		for(int x = 0; x< 15; x++){
			System.out.println("");
		}
		
		System.out.println("PLP Command Line:");
		System.out.println("Enter Command or 'Help' for list of commands");
		
		while (true)
		{
			commandInput = getCommand();
			if (commandList.commands.get(commandInput[0]) == null)
			{
				System.out.println("Invalid command");
			}
			else
			{
				if (commandInput.length == 1)
					commandList.commands.get(commandInput[0]).execute();
				else
					commandList.commands.get(commandInput[0]).execute(commandInput[1]);
					
			}
		}
		
	}
	
	public interface Command
	{
		void execute();
		
		void execute(String args);
		
	}
	
	public static class CommandList
	{
		Map<String, Command> commands = new HashMap<String, Command>();
		
		public CommandList()
		{			
			commands.put("Help", new Command() {
				public void execute()
				{
					for (String key : commands.keySet()) {
					    System.out.println(key);
					}
				}
				
				@Override
				public void execute(String args)
				{
					for (String key : commands.keySet()) {
					    System.out.println(key);
					}
				}
			});
			
			commands.put("Edit", new Command() {
				public void execute()
				{
					System.out.println("Please Specify Path Name.");
				}
				
				@Override
				public void execute(String args)
				{
					File file = new File(args); 
					try
					{
						java.awt.Desktop.getDesktop().edit(file);
					}
					catch (IOException e)
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			});
			
			commands.put("NewProject", new Command() {
				public void execute()
				{
					System.out.println("No arg");					
				}
				
				@Override
				public void execute(String args)
				{					
					System.out.println("Enter Project Location for new Project: ");
					String projectLocation = getInput();
					
					System.out.println("Enter Main File Name For " + projectLocation + ": ");
					String mainFileName = getInput();
					
					ProjectCreationDetails details = new ProjectCreationDetails(args, mainFileName, projectLocation, "PLP6");

					PLPProject project = new PLPProject(details.getProjectName());
					project.setPath(details.getProjectLocation());
					
					String sourceName = details.getMainSourceFileName();
					SimpleASMFile sourceFile = new SimpleASMFile(project, sourceName);
					project.add(sourceFile);
					try
					{
						project.save();
						System.out.println("Project Created");
					}
					catch (IOException e)
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			});
			
			commands.put("CreateNewASM", new Command() {
				public void execute()
				{
					System.out.println("Not Yet Iplemented");
				}
				
				@Override
				public void execute(String args)
				{
					System.out.println("Not Yet Iplemented");
				}
			});
			
			commands.put("Assemble", new Command() {
				public void execute()
				{
					System.out.println("Not Yet Iplemented");
				}
				
				@Override
				public void execute(String args)
				{
					System.out.println("Not Yet Iplemented");

				}
			});
			
			commands.put("Simulate", new Command() {
				public void execute()
				{
					System.out.println("Not Yet Iplemented");
				}
				
				@Override
				public void execute(String args)
				{
					System.out.println("Not Yet Iplemented");
				}
			});
			
		}
		
	}
	
	private static String[] getCommand()
	{
		System.out.print("> ");
		Scanner in = new Scanner(System.in);
		String input = in.nextLine();
		String[] arguments = input.split("\\s+");
		return arguments;
	}
	
	private static String getInput()
	{
		System.out.print("> ");
		Scanner in = new Scanner(System.in);
		String input = in.nextLine();
		return input;
	}
	
}
