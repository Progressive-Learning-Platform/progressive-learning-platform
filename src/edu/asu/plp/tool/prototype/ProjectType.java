package edu.asu.plp.tool.prototype;

import java.io.File;

import edu.asu.plp.tool.prototype.model.Project;

/**
 * A {@link ProjectType} specifies how a {@link Project} is represented on the <i>disk</i>
 * (i.e. <i>file system</i>). That is, it specifies how a {@link Project} is saved and
 * loaded, <b>not</b> how it is assembled and simulated. For the latter, refer to
 * "isaType"
 * 
 * @author Moore, Zachary
 * @see ASMType
 *
 */
public interface ProjectType
{
	public static enum Target
	{
		FILE,
		DIRECTORY
	}
	
	String getName();
	
	Target getSaveTarget();
	
	boolean canLoadFrom(Target target);
	
	/**
	 * Returns true if the {@link ProjectType} supports loading files from the given
	 * extension. If the extension is null, this method will return true only if the
	 * {@link ProjectType} supports loading from <i>directories</i>.
	 * <p>
	 * Note that the save method will save projects indiscriminately to the given path,
	 * regardless of the files extension. Also note that some {@link ProjectType}s will
	 * load indiscriminately, although it is suggested to enforce a particular extension.
	 * 
	 * @param extension
	 *            The file extension, without the preceding ".", or <i>null</i> if the
	 *            {@link File} is a directory. Example: "foo.txt" has the extension "txt"
	 * @return true if this {@link ProjectType} can load files with the given extension
	 */
	boolean isFileExtensionSupported(String extension);
	
	Project load(File file);
	
	default void save(Project project, File file)
	{
		save(project, file, true);
	}
	
	void save(Project project, File file, boolean overwrite);
}
