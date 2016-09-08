package edu.asu.plp.tool.backend.isa;

import java.io.File;
import java.io.IOException;

import edu.asu.plp.tool.prototype.model.SimpleASMFile;
import edu.asu.plp.tool.prototype.model.Project;
import javafx.beans.property.StringProperty;

public interface ASMFile
{
	public static final String DEFAULT_FILE_EXTENSION = ".asm";
	
	boolean writeToFile(File file) throws IOException;
	
	boolean writeToFile(File file, boolean overwrite) throws IOException;
	
	/**
	 * Writes the contents of this {@link SimpleASMFile}, specified by {@link #getContent()}
	 * , to the specified path, overwriting the current contents.
	 * <p>
	 * This method is equivalent to {@link #writeToFile(String, boolean)} with the
	 * parameters (path, true).
	 * <p>
	 * See {@link #writeToFile(String, boolean)} for more details.
	 * 
	 * @param path
	 *            Path to a directory or file at which to save this file.
	 * @return True if the file was written, false otherwise.
	 * @throws IOException
	 *             if the file cannot be overwritten, the path is invalid, the path is
	 *             restricted, or the write fails from another IO issue.
	 */
	boolean writeToFile(String path) throws IOException;
	
	/**
	 * Writes the contents of this {@link SimpleASMFile}, specified by {@link #getContent()}
	 * , to the specified path.
	 * <p>
	 * If the path references a file, the specified name will be used. If the specified
	 * file already exists, this operation will overwrite it only if the "overwrite"
	 * parameter is set to true. If it is set to false, this method will not overwrite the
	 * file, and return false.
	 * <p>
	 * If the path references a directory, this file will be named according to this
	 * file's {@link #nameProperty()}, with the extension "{@value #FILE_EXTENSION}". If
	 * this files name is null, {@link IllegalStateException} will be thrown.
	 * <p>
	 * If the file cannot be overwritten, the path is invalid, the path is restricted, or
	 * the write fails from another IO issue, and {@link IOException} will be thrown.
	 * 
	 * @param path
	 *            Path to a directory or file at which to save this file.
	 * @param overwrite
	 *            True if this operation should overwrite a pre-existing file, false
	 *            otherwise.
	 * @return True if the file was written, false otherwise.
	 * @throws IOException
	 *             if the file cannot be overwritten, the path is invalid, the path is
	 *             restricted, or the write fails from another IO issue.
	 */
	boolean writeToFile(String path, boolean overwrite) throws IOException;
	
	public default String constructFileName()
	{
		// TODO: check name for illegal characters
		String name = getName();
		if (name == null)
			throw new IllegalStateException("Null file name is not allowed");
		else
			return name + DEFAULT_FILE_EXTENSION;
	}
	
	String getName();
	
	void setName(String name);
	
	StringProperty nameProperty();
	
	String getContent();
	
	void setContent(String content);
	
	StringProperty contentProperty();
	
	Project getProject();
}
