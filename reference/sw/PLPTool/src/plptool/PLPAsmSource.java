/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

package plptool;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;

/**
 * This class implements an assembly source and its data structures used by
 * PLPAssembler.
 *
 * @author wira
 */

public class PLPAsmSource
{
	private PLPAsmSource refSource;
	private String asmString;
	private String[] asmLines;
	private String asmFilePath;
	private int lastLineOpen;
	
	/**
	 * Unused variable
	 */
	int recursionLevel;
	
	/**
	 * 
	 * @param strAsm String that represents the whole ASM file.
	 * @param strFilePath If strAsm is null, it will attempt to load file contents from the provided path
	 * @param intLevel Currently not used
	 */
	public PLPAsmSource(String strAsm, String strFilePath, int intLevel)
	{
		lastLineOpen = 0;
		recursionLevel = intLevel;
		
		if (strAsm == null)
		{
			setAsmFilePath(strFilePath, true);
		}
		else
		{
			setAsmFilePath(strFilePath);
			setAsmString(strAsm);
		}
	}
	
	public int setRefSource(PLPAsmSource plpRefSource)
	{
		refSource = plpRefSource;
		
		if (refSource != null)
			return 0;
		else
		{
			return Constants.PLP_ASM_INVALID_REFSOURCE;
		}
	}
	
	public String getAsmFilePath()
	{
		return asmFilePath;
	}
	
	public String getAsmString()
	{
		return asmString;
	}
	
	public void setAsmString(String newStr)
	{
		asmString = newStr;
		asmLines = asmString.split("\\r?\\n");
	}
	
	/**
	 * Sets the file path and does not attempt to immediately 
	 * load contents from the new file path. 
	 * Call loadFromFile() if you want to update the contents to what is in the file.
	 * @param newPath
	 */
	public void setAsmFilePath(String newPath)
	{
		setAsmFilePath(newPath, false);
	}
	
	/**
	 * 
	 * @param newPath new Path to be set to.
	 * @param loadFromPath If true, it will call loadFromFile.
	 * @return If loadFromPath is false, this method will return false.
	 * Else this method will return whatever loadFromFile returns.
	 */
	public boolean setAsmFilePath(String newPath, boolean loadFromPath)
	{
		asmFilePath = newPath;
		
		if(loadFromPath)
			return loadFromFile();
		
		return false;
	}
	
	/**
	 * Load full asm file contents into the source
	 * @return Returns false on file not existing or failing to open. True if it opens properly.
	 */
	public boolean loadFromFile()
	{
		File asmFile = new File(asmFilePath);
		
		if (new File(asmFilePath).exists())
		{
			try
			{
				setAsmString(new String(Files.readAllBytes(new File(asmFilePath).toPath())));
				return true;
			}
			catch (IOException exception)
			{
				Msg.printStackTrace(exception);
			}
		}
		else
		{
			Msg.error("ASM file does not exist!",
					Constants.PLP_IO_FILE_DOES_NOT_EXIST, this);
		}
		return false;
	}
	
	/**
	 * Get the string contents of the asm file at the specified line number.
	 * @param lineNum Line numbers are the actual line numbers. They start at 1.
	 * @return String if the line number is valid, null otherwise
	 */
	public String getAsmLine(int lineNum)
	{
		if (lineNum > asmLines.length || lineNum <= 0)
		{
			Msg.error("Invalid line number: " + lineNum,
					Constants.PLP_ASM_INVALID_LINENUM, this);
			return null;
		}
		
		return asmLines[lineNum - 1];
	}
	
	public void setLastLine(int lineNumber)
	{
		lastLineOpen = lineNumber;
	}
	
	public int getLastLine()
	{
		return lastLineOpen;
	}
	
	@Override
	public String toString()
	{
		return "PLPAsmSource(" + this.asmFilePath + ")";
	}
}
