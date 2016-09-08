package recyclebin;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;

import edu.asu.plp.tool.backend.util.FileUtil;
import edu.asu.plp.tool.prototype.util.Dialogues;

public class ASMFile
{
	protected String asmFilePath;
	protected String asmContents;
	protected List<String> asmLines;
	private static final char[] ILLEGAL_CHARACTERS = { '/', '\n', '\r', '\t', '\0', '\f',
			'`', '?', '*', '\\', '<', '>', '|', '\"', ':' };
			
	public ASMFile(String asmFilePath) throws IOException
	{
		this(FileUtil.readAllLines(asmFilePath), asmFilePath);
	}
	
	public ASMFile(String asmContents, String asmFilePath) throws IOException
	{
		if (asmContents == null && asmFilePath != null)
		{
			setAsmFilePath(asmFilePath, true);
		}
		else
		{
			this.asmContents = asmContents;
			this.asmFilePath = asmFilePath;
			this.asmLines = Arrays.asList(asmContents.split("\n"));
		}
	}
	
	public boolean loadFromFile()
	{
		return loadFromFile(asmFilePath);
	}
	
	private boolean loadFromFile(String asmFilePath)
	{
		if (FileUtil.isValidFile(asmFilePath))
		{
			File asmFile = new File(asmFilePath);
			
			try
			{
				this.asmLines = Files.readAllLines(asmFile.toPath());
				this.asmContents = String.join("\n", this.asmLines);
				return true;
			}
			catch (IOException e)
			{
				e.printStackTrace();
				return false;
			}
		}
		
		return false;
	}
	
	public String getAsmFilePath()
	{
		return asmFilePath;
	}
	
	/**
	 * Will set the filePath to the one specified and overwrite the current file contents
	 * stored in this class with the ones from the newly specified file.
	 * 
	 * @param asmFilePath
	 */
	public void setAsmFilePath(String asmFilePath)
	{
		setAsmFilePath(asmFilePath, true);
	}
	
	/**
	 * Sets the reference path to the one specified.
	 * 
	 * @param asmFilePath
	 * @param overwriteCurrent
	 *            If overwrite current is true, it will overwrite the current contents of
	 *            this class.
	 */
	public void setAsmFilePath(String asmFilePath, boolean overwriteCurrent)
	{
		boolean validPath;
		validPath = checkValidPath(asmFilePath);
		if (validPath)
		{
			this.asmFilePath = asmFilePath;
			if (overwriteCurrent)
			{
				loadFromFile(asmFilePath);
			}
		}
	}
	
	public String getAsmContents()
	{
		return asmContents;
	}
	
	public void setAsmContents(String asmContents)
	{
		this.asmContents = asmContents;
		this.asmLines = Arrays.asList(asmContents.split("\n"));
	}
	
	/**
	 * Retrieve a Line from the ASM file.
	 * 
	 * @param lineNumber
	 *            Follows the file format, 1-n.
	 * @return If the file contains that line number, returns and that string wrapped in
	 *         an optional. Otherwise it returns an empty optional
	 */
	public Optional<String> getAsmLine(int lineNumber)
	{
		if (lineNumber > asmLines.size() || lineNumber <= 0)
			return Optional.empty();
		return Optional.of(asmLines.get(lineNumber - 1));
	}
	
	public List<String> getAsmLines()
	{
		return asmLines;
	}
	
	public boolean checkValidPath(String path)
	{
		boolean valid = false;
		char tempChar;
		
		for (int i = 0; i < ILLEGAL_CHARACTERS.length; i++)
		{
			tempChar = ILLEGAL_CHARACTERS[i];
			try
			{
				if (!path.contains(Character.toString(tempChar)))
				{
					valid = true;
				}
				else
				{
					String cause = "There was an invalid Character (" + tempChar
							+ ") in the path name.";
					throw new Exception(cause);
				}
			}
			catch (Exception exception)
			{
				String recoveryMessage = "The ASM file could not be saved because of an invalid path name";
				Dialogues.showAlertDialogue(exception, recoveryMessage);
			}
		}
		
		return valid;
	}
}
