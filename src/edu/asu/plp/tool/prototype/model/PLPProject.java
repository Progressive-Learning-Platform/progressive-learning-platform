package edu.asu.plp.tool.prototype.model;

import java.awt.geom.IllegalPathStateException;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.FileAlreadyExistsException;
import java.nio.file.Path;
import java.util.Optional;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.FilenameUtils;
import org.json.JSONObject;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.backend.util.PLP5ProjectParser;
import edu.asu.plp.tool.core.ISAModule;
import edu.asu.plp.tool.core.ISARegistry;

/**
 * A {@link PLPProject} represents an ordered, observable collection of
 * {@link SimpleASMFile}s that can be assembled collectively as a single unit.
 * 
 * @author Moore, Zachary
 */
public class PLPProject extends ArrayListProperty<ASMFile> implements Project
{
	public static final String FILE_EXTENSION = ".project";
	private static final String PROJECT_FILE_NAME = "" + FILE_EXTENSION;
	private static final String NAME_KEY = "projectName";
	private static final String TYPE_KEY = "projectType";
	private static final String SOURCE_NAME_KEY = "sourceDirectoryName";
	
	/**
	 * Path to this project in the file system. If the this project exists in memory only
	 * (it has not yet been written to disk), then the value contained by
	 * {@link #pathProperty} should be null.
	 * <p>
	 * Note that {@link #pathProperty} itself should always be non-null.
	 * <p>
	 * Also note that the path should point to a directory, unless the project was loaded
	 * from a legacy source. The actual project FILE will be located in the given
	 * directory, with the name "{@value #PROJECT_FILE_NAME}"
	 * <p>
	 * The files contained by this project will be located in a subdirectory named "src"
	 * <p>
	 * In the case of a legacy file, the path will point to the project file directly, and
	 * no src directory will exist.
	 */
	private StringProperty pathProperty;
	private StringProperty nameProperty;
	private StringProperty typeProperty;
	
	/**
	 * Loads a {@link PLPProject} from the given project file. This method auto-detects
	 * the project version, and is therefore capable of loading both PLP6 and legacy (PLP5
	 * and prior) projects.
	 * 
	 * @param file
	 *            file or directory of the specified project
	 * @return A {@link PLPProject} representative of the information stored in the given
	 *         file.
	 * @throws IOException
	 *             if an IO problem occurs while opening the specified file.
	 */
	public static PLPProject load(File file)
			throws IOException
	{
		
		if (file.isFile())
			return loadLegacy(file);
		else
			return loadCurrent(file);
	}
	
	/**
	 * Reads the asm file content. It is used while creating the asm file object and associating to PLPProject.{@link #AsmFileContent()}
	 * @param fileName
	 * 				The full path name of the asm file whose content needs to be read.
	 * @return A String object containing the asm file contents.
	 * @throws IOException
	 */
	private static String AsmFileContent(String fileName) throws IOException
	{
		BufferedReader br = new BufferedReader(new FileReader(fileName));
		try
		{
			StringBuilder sb = new StringBuilder();
			String line = br.readLine();
			
			while(line != null)
			{
				sb.append(line);
				sb.append("\n");
				line = br.readLine();
			}
			return sb.toString();
		}
		finally
		{
			br.close();
		}
		
	}
	
	/**
	 * Loads a PLP6 project. The given file must be a directory, and have a structure as
	 * specified by {@link #save()}
	 * 
	 * @param projectDirectory
	 *            The directory of the project to load. This file must be a DIRECTORY, and
	 *            have a structure as specified by {@link #save()}
	 * @return A {@link PLPProject} representative of the information stored in the given
	 *         directory.
	 * @throws IOException
	 */
	private static PLPProject loadCurrent(File projectDirectory) throws IOException
	{
		validateProjectDirectory(projectDirectory);
		File projectFile = validateAndFilizeProjectFile(projectDirectory);
		
		if (!projectFile.exists())
			throw new IllegalArgumentException("Project file not found.");
			
		String fileString = FileUtils.readFileToString(projectFile);
		JSONObject projectDetails = new JSONObject(fileString);
		String name = projectDetails.optString(NAME_KEY);
		//String type = projectDetails.optString(NAME_KEY);
		String type = projectDetails.optString(TYPE_KEY);
		String sourceDirectoryName = projectDetails.optString(SOURCE_NAME_KEY, "src");
		
		Path projectPath = projectDirectory.toPath();
		Path sourcePath = projectPath.resolve(sourceDirectoryName);
		File sourceDirectory = sourcePath.toFile();
		
		//PLPProject project = new PLPProject(name, type);
		PLPProject project = new PLPProject(name, type, projectPath.toString());
		for (File file : sourceDirectory.listFiles())
		{
			String sourceName = file.getName();
			
			sourceName = FilenameUtils.removeExtension(sourceName);
			
			//While associating asmfile to the project, we need to pass the content of the file also, otherwise when 
			//user tries to open the respective file from project explorer, Editor pane will show empty file - Harsha
			SimpleASMFile sourceFile = new SimpleASMFile(project, sourceName, AsmFileContent(file.getAbsolutePath()));
			//SimpleASMFile sourceFile = new SimpleASMFile(project, sourceName);
			project.add(sourceFile);
		}
		
		return project;
	}
	
	/**
	 * Loads a {@link PLPProject} from the given project file. This method calls
	 * {@link #load(File)}, which auto-detects the project version, and is therefore
	 * capable of loading both PLP6 and legacy (PLP5 and prior) projects.
	 * 
	 * @param filePath
	 *            Path to the specified file; may be relative or absolute
	 * @return A {@link PLPProject} representative of the information stored in the given
	 *         file.
	 * @throws IOException
	 *             if an IO problem occurs while opening the specified file.
	 * @see #load(File)
	 */
	public static Project load(String filePath)
			throws IOException
	{
		File file = new File(filePath);
		return load(file);
	}
	
	/**
	 * Loads a project file from PLP5 or earlier. The format for these older versions is
	 * Tarball, and typically have the .plp extension.
	 * <p>
	 * Legacy PLP projects should have a tarball entry called "plp.metafile" and will
	 * specify the file version on the first line of the entry, in the format "PLP-#.#"
	 * 
	 * @param filePath
	 *            Path to the specified file; may be relative or absolute
	 * @throws IOException
	 *             if there was an issue loading the Tarball
	 * @returnA {@link PLPProject} representative of the information stored in the given
	 *          file.
	 */
	private static PLPProject loadLegacy(File file) throws IOException
	{
		PLP5ProjectParser parser = new PLP5ProjectParser();
		return parser.parse(file);
	}
	
	/**
	 * Validates the given file to be a directory, and throws an exception if the given
	 * file is not a directory, or if the file is null.
	 * 
	 * @param projectDirectory
	 *            File to validate
	 */
	private static void validateProjectDirectory(File projectDirectory)
	{
		if (projectDirectory == null)
		{
			throw new IllegalArgumentException("Directory must be non-null");
		}
		// When creating project, the directory has not been created yet so always throws
		// error
		else
		{
			//moved this
			if (!projectDirectory.exists())
				projectDirectory.mkdir();
			
			if (!projectDirectory.isDirectory())
			
			{
				String path = projectDirectory.getAbsolutePath();
				String message = "Path must point to a directory. Found: " + path;
				throw new IllegalPathStateException(message);
			}
		}
	}
	
	/**
	 * Given the project directory, this method locates, validates, and returns the
	 * project file located in the given directory.
	 * <p>
	 * The project file will be a direct child of the given directory, will be a file (not
	 * a directory), and will have the name {@value #PROJECT_FILE_NAME}
	 * <p>
	 * If the above conditions are not met, an {@link IllegalStateException} will be
	 * thrown.
	 * 
	 * @param projectDirectory
	 *            The root directory containing the project
	 * @return A File representing the project file for the given project directory
	 */
	private static File validateAndFilizeProjectFile(File projectDirectory)
	{
		if (projectDirectory == null)
		{
			throw new IllegalArgumentException("Directory must be non-null");
		}
		
		Path rootPath = projectDirectory.toPath();
		Path filePath = rootPath.resolve(PROJECT_FILE_NAME);
		File projectFile = filePath.toFile();
		//if (projectFile.isDirectory())
		//{
		//	throw new IllegalStateException("ProjectFile resolved to a directory: "
		//			+ projectFile.getAbsolutePath());
		//}
		
		return projectFile;
	}
	
	public PLPProject()
	{
		pathProperty = new SimpleStringProperty();
		nameProperty = new SimpleStringProperty();
		typeProperty = new SimpleStringProperty();
	}
	
	public PLPProject(String name, String type)
	{
		this();
		nameProperty.set(name);
		typeProperty.set(type);
	}
	
	//Need to set the path of the project otherwise, it will fail while saving the project - Harsha
	public PLPProject(String name, String type, String path)
	{
		this();
		nameProperty.set(name);
		typeProperty.set(type);
		pathProperty.set(path);
	}
	
	public PLPProject(String name)
	{
		// TODO: remove this constructor and force a filetype to be declared
		this(name, "plp");
	}
	
	/**
	 * Outputs this project and all its files to the directory specified by
	 * {@link #getPath()}, as a PLP6 project file with a src directory of .asm files.
	 * <p>
	 * The project information specified by this object will be saved in a file within the
	 * specified directory named "{@value #PROJECT_FILE_NAME}"
	 * <p>
	 * The files contained by this project will be saved to a subdirectory named "src"
	 * <p>
	 * Source files will be exported to a child directory of the project, called "src" and
	 * each .asm file will be named according to {@link SimpleASMFile#getName()}.
	 * 
	 * @see
	 * @throws IllegalStateException
	 *             if the specified path is null
	 * @throws IllegalPathStateException
	 *             if the path does not point to a directory
	 * @throws IOException
	 *             if there is an issue outputting to the specified path
	 */
	@Override
	public void save() throws IOException
	{
		File directory = validateAndFilizePath();
		// moving this
		// if (!directory.exists())
		// directory.mkdir();
		
		File sourceDirectory = validateAndFilizeSourceDirectory(directory);
		if (!sourceDirectory.exists())
			sourceDirectory.mkdir();
			
		File projectFile = validateAndFilizeProjectFile(directory);
		if (!projectFile.exists())
			projectFile.createNewFile();
		String projectFileContent = createProjectFileContent();
		FileUtils.write(projectFile, projectFileContent);
		
		Path sourcePath = sourceDirectory.toPath();
		for (ASMFile file : this)
		{
			String fileName = file.constructFileName();
			Path asmPath = sourcePath.resolve(fileName);
			File diskFile = asmPath.toFile();
			String asmContent = file.getContent();
			FileUtils.write(diskFile, asmContent);
		}
	}
	
	private String createProjectFileContent()
	{
		JSONObject root = new JSONObject();
		root.put(NAME_KEY, getName());
		root.put(TYPE_KEY, getType());
		// TODO: make "src" a constant
		root.put(SOURCE_NAME_KEY, "src");
		
		return root.toString();
	}
	
	private File validateAndFilizeSourceDirectory(File projectDirectory)
	{
		Path projectPath = projectDirectory.toPath();
		// TODO: make the directory "src" a constant variable
		Path sourcePath = projectPath.resolve("src");
		File sourceDirectory = sourcePath.toFile();
		//if (!sourceDirectory.isDirectory())
		//{
		//	throw new IllegalStateException("Source directory resolved to a file: "
		//			+ sourceDirectory.getAbsolutePath());
		//}
		
		return sourceDirectory;
	}
	
	private File validateAndFilizePath()
	{
		String path = getPath();
		if (path == null)
		{
			throw new IllegalStateException("Path must be non-null");
		}
		
		File directory = new File(path);
		validateProjectDirectory(directory);
		return directory;
	}
	
	/**
	 * Outputs this project and all its files to the path specified by {@link #getPath()},
	 * as a PLP5 (legacy) project file. This method is intended only for backwards
	 * compatibility purposes, and where possible, {@link #save()} should be used instead.
	 * <p>
	 * As a legacy format, some features may not be supported by the format. Thus, <b>if
	 * this method can be triggered by a user interface, the interface should display a
	 * warning indicating this risk.</b>
	 * 
	 * @throws IllegalStateException
	 *             if the specified path is null
	 * @throws IOException
	 *             if there is an issue outputting to the specified path
	 */
	public void saveLegacy() throws IOException
	{
		// TODO: implement
		throw new UnsupportedOperationException("Not Yet Implemented");
	}
	
	/**
	 * Outputs this project and all its files, as specified by {@link #save()}, to the
	 * given directory, and updates this project's {@link #pathProperty} if the save was
	 * successful.
	 * <p>
	 * Source files will be exported to a child directory of the project, called "src" and
	 * each .asm file will be named according to {@link SimpleASMFile#getName()}.
	 * <p>
	 * If the specified file already exists, an exception will be raised, and the file
	 * <b>will not</b> be overwritten.
	 * <p>
	 * In order to save over an already existing file, a program must delete the file
	 * <b>before</b> calling this method.
	 * 
	 * @param directoryPath
	 *            The location in the file system to save this project to. This path
	 *            should point to a DIRECTORY.
	 * 			
	 * @see #save()
	 * @throws IllegalArgumentException
	 *             if the specified path is null, or points to a file instead of a
	 *             directory.
	 * @throws FileAlreadyExistsException
	 *             if the file already exists. If you still wish to save to this location,
	 *             delete the file before calling this method.
	 * @throws IOException
	 *             if there is an issue outputting to the specified path
	 */
	@Override
	public void saveAs(String filePath) throws IOException
	{
		// TODO implement
		//throw new UnsupportedOperationException("Not yet implemented");
		File directory = validateAndFilizePath(filePath);
		
		File sourceDirectory = validateAndFilizeSourceDirectory(directory);
		if (!sourceDirectory.exists())
			sourceDirectory.mkdir();
			
		File projectFile = validateAndFilizeProjectFile(directory);
		if (!projectFile.exists())
			projectFile.createNewFile();
		String projectFileContent = createProjectFileContent();
		FileUtils.write(projectFile, projectFileContent);
		
		Path sourcePath = sourceDirectory.toPath();
		for (ASMFile file : this)
		{
			String fileName = file.constructFileName();
			Path asmPath = sourcePath.resolve(fileName);
			File diskFile = asmPath.toFile();
			String asmContent = file.getContent();
			FileUtils.write(diskFile, asmContent);
		}
	}
	
	private File validateAndFilizePath(String path)
	{
		if (path == null)
		{
			throw new IllegalStateException("Path must be non-null");
		}
		
		File directory = new File(path);
		validateProjectDirectory(directory);
		return directory;
	}
	
	@Override
	public StringProperty getNameProperty()
	{
		return nameProperty;
	}
	
	@Override
	public String getName()
	{
		return nameProperty.get();
	}
	
	@Override
	public void setName(String name)
	{
		nameProperty.set(name);
	}
	
	@Override
	public StringProperty getTypeProperty()
	{
		return typeProperty;
	}
	
	@Override
	public String getType()
	{
		return typeProperty.get();
	}
	
	@Override
	public void setType(String type)
	{
		typeProperty.set(type);
	}
	
	@Override
	public StringProperty getPathProperty()
	{
		return pathProperty;
	}
	
	@Override
	public String getPath()
	{
		return pathProperty.get();
	}
	
	@Override
	public String getPathFor(ASMFile child)
	{
		if (!this.contains(child))
		{
			String message = "The specified ASMFile must be a child of this Project. "
					+ "However, the given file {" + child.getName() + "} is not.";
			throw new IllegalArgumentException(message);
		}
		
		String location = getPath();
		if (location == null)
			return null;
			
		File file = new File(location);
		Path path = file.toPath();
		String childFileName = child.constructFileName();
		// TODO: make the directory "src" a constant variable
		if (file.isDirectory() && childFileName != null)
			return path.resolve("src/" + childFileName).toString();
		else
			return null;
	}
	
	/**
	 * Sets the path pointer of this {@link PLPProject}.
	 * <p>
	 * Note that this method alone does not alter disk contents, i.e. it does not move the
	 * project file, or remove the old project file. For these functionalities, the
	 * application responsible for instantiating {@link PLPProject} should add a change
	 * listener to the {@link #pathProperty} (via {@link #getPathProperty()}), that
	 * achieves the desired effect.
	 * 
	 * @param path
	 */
	@Override
	public void setPath(String path)
	{
		pathProperty.set(path);
	}
	
	@Override
	public int getFileCount()
	{
		return this.size();
	}
	
	/**
	 * Convenience method for accessing the {@link ISARegistry}
	 * 
	 * @return
	 */
	@Override
	public Optional<ISAModule> getISA()
	{
		ISARegistry registry = ISARegistry.getGlobalRegistry();
		String type = getType();
		return registry.lookupByProjectType(type);
	}
}
