package edu.asu.plp.tool.prototype;

import static edu.asu.plp.tool.prototype.Main.findDiskObjectForASM;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.nio.file.FileAlreadyExistsException;
import java.util.Collections;
import java.util.List;
import java.util.function.Predicate;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.collections.ObservableList;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.exceptions.DiskOperationFailedException;
import edu.asu.plp.tool.exceptions.ProjectAlreadyOpenException;
import edu.asu.plp.tool.exceptions.ProjectNameConflictException;
import edu.asu.plp.tool.exceptions.UnexpectedFileTypeException;
import edu.asu.plp.tool.exceptions.UnsupportedFileExtensionException;
import edu.asu.plp.tool.exceptions.UnsupportedProjectTypeException;
import edu.asu.plp.tool.prototype.model.Project;
import edu.asu.plp.tool.prototype.model.SimpleASMFile;

public class ProjectManager
{
	private ObservableList<Project> projects;
	private ObjectProperty<Project> activeProjectProperty;
	private ObservableList<ProjectType> supportedProjectTypes;
	
	public ProjectManager(ObservableList<Project> projects)
	{
		this.projects = projects;
		this.activeProjectProperty = new SimpleObjectProperty<>();
	}
	
	public void addProject(Project project) throws ProjectAlreadyOpenException,
			ProjectNameConflictException
	{
		Project existingProject = getProjectByName(project.getName());
		if (existingProject != null)
		{
			if (existingProject.getPath().equals(project.getPath()))
				throw new ProjectAlreadyOpenException(existingProject);
			else
				throw new ProjectNameConflictException(existingProject);
		}
		else
		{
			projects.add(project);
		}
	}
	
	public Project getProjectByName(String name)
	{
		for (Project project : projects)
		{
			String projectName = project.getName();
			boolean namesAreNull = (projectName == null && name == null);
			if (namesAreNull || name.equals(projectName))
				return project;
		}
		
		return null;
	}
	
	public void openProjectFromFile(File file) throws ProjectAlreadyOpenException,
			ProjectNameConflictException, FileNotFoundException,
			UnsupportedFileExtensionException
	{
		if (!file.exists())
			throw new FileNotFoundException();
		
		ProjectType type = getType(file);
		Project project = type.load(file);
		this.addProject(project);
	}
	
	public void saveActiveProject() throws UnsupportedProjectTypeException
	{
		Project activeProject = getActiveProject();
		String filePath = activeProject.getPath();
		File destination = new File(filePath);
		String projectType = activeProject.getType();
		
		saveActiveProjectAs(destination, projectType);
	}
	
	public void saveActiveProjectAs(File destination)
			throws UnsupportedProjectTypeException
	{
		Project activeProject = getActiveProject();
		String projectType = activeProject.getType();
		
		saveActiveProjectAs(destination, projectType);
	}
	
	public void saveActiveProjectAs(File destination, String projectType)
			throws UnsupportedProjectTypeException
	{
		Project activeProject = getActiveProject();
		ProjectType type = getTypeByName(projectType);
		
		type.save(activeProject, destination);
	}
	
	public ProjectType getTypeByName(String typeName)
			throws UnsupportedProjectTypeException
	{
		Predicate<ProjectType> filter = (type) -> typeName.equals(type.getName());
		ObservableList<ProjectType> validTypes = supportedProjectTypes.filtered(filter);
		
		if (validTypes.isEmpty())
			throw new UnsupportedProjectTypeException(typeName);
		else
			return supportedProjectTypes.get(0);
	}
	
	private ProjectType getType(File file) throws UnsupportedFileExtensionException
	{
		String extension = FilenameUtils.getExtension(file.getAbsolutePath());
		Predicate<ProjectType> filter = (type) -> type.isFileExtensionSupported(extension);
		ObservableList<ProjectType> validTypes = supportedProjectTypes.filtered(filter);
		
		if (validTypes.isEmpty())
			throw new UnsupportedFileExtensionException(extension);
		else
			return supportedProjectTypes.get(0);
	}
	
	public void createNewASM(String name, String defaultContent)
	{
		Project activeProject = getActiveProject();
		ASMFile asm = new SimpleASMFile(activeProject, name);
		asm.setContent(defaultContent);
		
		if (activeProject.filtered((e) -> e.getName().equals(name)).isEmpty())
			activeProject.add(asm);
		else
			throw new IllegalStateException("Project {" + activeProject.getName()
					+ "} already contains a file with the name {" + name + "}");
	}
	
	public void importASM(String filePath) throws IOException
	{
		File importTarget = new File(filePath);
		
		String content = FileUtils.readFileToString(importTarget);
		Project activeProject = getActiveProject();
		String name = importTarget.getName();
		
		ASMFile asmFile = new SimpleASMFile(activeProject, name);
		asmFile.setContent(content);
		activeProject.add(asmFile);
		activeProject.save();
	}
	
	public void exportASM(String asmName, String filePath, boolean overwrite)
			throws FileAlreadyExistsException, DiskOperationFailedException
	{
		if (asmName == null || filePath == null)
			throw new IllegalArgumentException("No file was specified");
		else
			exportASM(asmName, new File(filePath), overwrite);
	}
	
	private void exportASM(String asmName, File destination, boolean overwriteAllowed)
			throws FileAlreadyExistsException, DiskOperationFailedException
	{
		if (destination == null)
			throw new IllegalStateException("Failed to create file object");
		else if (destination.exists() && !overwriteAllowed)
			throw new FileAlreadyExistsException(destination.getAbsolutePath());
		
		ASMFile asmFile = getASMByName(asmName);
		ensureExists(destination);
		
		try (PrintWriter writer = new PrintWriter(destination))
		{
			writer.print(asmFile.getContent());
		}
		catch (IOException exception)
		{
			throw new DiskOperationFailedException(exception);
		}
	}
	
	private void ensureExists(File destination) throws DiskOperationFailedException
	{
		try
		{
			if (!destination.exists())
				destination.createNewFile();
		}
		catch (IOException exception)
		{
			throw new DiskOperationFailedException(exception);
		}
	}
	
	public void removeASM(String asmName) throws FileNotFoundException,
			UnexpectedFileTypeException, DiskOperationFailedException
	{
		if (asmName == null)
			throw new IllegalArgumentException("No file was specified");
		
		ASMFile activeFile = getASMByName(asmName);
		File removalTarget = findDiskObjectForASM(activeFile);
		
		Project activeProject = activeFile.getProject();
		activeProject.remove(activeFile);
		
		String failMessage = "The asm \""
				+ activeFile.getName()
				+ "\" will be removed from the project \""
				+ activeFile.getProject().getName()
				+ "\" but it is suggested that you verify the deletion from disk manually.";
		
		if (removalTarget == null || !removalTarget.exists())
		{
			failMessage = "Unable to locate file on disk. " + failMessage;
			throw new FileNotFoundException(failMessage);
		}
		else if (removalTarget.isDirectory())
		{
			failMessage = "The path specified is a directory, but should be a file. "
					+ failMessage;
			throw new UnexpectedFileTypeException(failMessage);
		}
		
		try
		{
			boolean wasRemoved = removalTarget.delete();
			if (!wasRemoved)
				throw new Exception("Operation failed. Received: " + wasRemoved);
		}
		catch (Exception exception)
		{
			throw new DiskOperationFailedException(
					"Failed to delete asm from disk. It is suggested that you verify the deletion from disk manually.");
		}
	}
	
	private ASMFile getASMByName(String asmName)
	{
		Project activeProject = getActiveProject();
		Predicate<ASMFile> filter = (asm) -> asm.getName().equals(asmName);
		List<ASMFile> list = activeProject.filtered(filter);
		
		if (list.isEmpty())
		{
			throw new IllegalArgumentException("Active project {"
					+ activeProject.getName() + "} does not contain the given file {"
					+ asmName + "}");
		}
		else if (list.size() == 1)
		{
			return list.get(0);
		}
		else
		{
			throw new IllegalStateException("Project {" + activeProject.getName()
					+ "} contains duplicate file names.");
		}
	}
	
	public void setMainASMFile(String asmName)
	{
		ASMFile targetASM = getASMByName(asmName);
		setMainASMFile(targetASM);
	}
	
	public void setMainASMFile(ASMFile asmFile)
	{
		Project activeProject = getActiveProject();
		int index = activeProject.indexOf(asmFile);
		
		if (index < 0)
		{
			throw new IllegalArgumentException("Active project {"
					+ activeProject.getName() + "} does not contain the given file {"
					+ asmFile.getName() + "}");
		}
		
		Collections.swap(activeProject, 0, index);
	}
	
	public String[] getSupportedProjectTypeNames()
	{
		int length = supportedProjectTypes.size();
		String[] typeNames = new String[length];
		
		for (int index = 0; index < length; index++)
		{
			ProjectType type = supportedProjectTypes.get(index);
			typeNames[index] = type.getName();
		}
		
		return typeNames;
	}
	
	public ObjectProperty<Project> getActiveProjectProperty()
	{
		return activeProjectProperty;
	}
	
	public Project getActiveProject()
	{
		return activeProjectProperty.get();
	}
	
	public void setActiveProject(String projectName) throws IllegalArgumentException
	{
		Project project = getProjectByName(projectName);
		if (project == null)
			throw new IllegalArgumentException("Project does not exist: " + projectName);
		activeProjectProperty.set(project);
	}
	
	public ObservableList<ProjectType> getSupportedProjectTypesProperty()
	{
		return supportedProjectTypes;
	}
}
