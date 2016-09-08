package edu.asu.plp.tool.prototype.model;

import java.io.File;
import java.net.MalformedURLException;

/**
 * Implementation of {@link Theme}.
 *
 * Should be used if all theming is done in plain CSS.
 *
 * @author Nesbitt, Morgan Created on 2/24/2016.
 */
public class CSSTheme implements Theme
{
	private String name;
	private String path;

	public CSSTheme(String name, String pathToCssFile)
	{
		this.name = name;
		this.path = pathToCssFile;
	}

	@Override
	public String getPath() throws MalformedURLException
	{
		return new File(path).toURI().toURL().toString();
	}

	@Override
	public String getName ()
	{
		return name;
	}

}
