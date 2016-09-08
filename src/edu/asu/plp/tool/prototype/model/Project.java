package edu.asu.plp.tool.prototype.model;

import java.io.IOException;
import java.nio.file.FileAlreadyExistsException;
import java.util.Optional;

import javafx.beans.property.StringProperty;
import javafx.collections.ObservableList;
import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.core.ISAModule;
import edu.asu.plp.tool.core.ISARegistry;

public interface Project extends ObservableList<ASMFile>
{
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
	 * @throws IOException
	 *             if there is an issue outputting to the specified path
	 */
	void save() throws IOException;
	
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
	void saveAs(String filePath) throws IOException;
	
	StringProperty getNameProperty();
	
	String getName();
	
	void setName(String name);
	
	StringProperty getTypeProperty();
	
	String getType();
	
	void setType(String type);
	
	StringProperty getPathProperty();
	
	String getPath();
	
	String getPathFor(ASMFile child);
	
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
	void setPath(String path);
	
	int getFileCount();
	
	/**
	 * Convenience method for accessing the {@link ISARegistry}
	 * 
	 * @return
	 */
	Optional<ISAModule> getISA();
}
