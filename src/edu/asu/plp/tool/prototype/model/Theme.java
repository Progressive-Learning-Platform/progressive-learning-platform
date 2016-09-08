package edu.asu.plp.tool.prototype.model;

import java.net.MalformedURLException;

/**
 * Interface to extend if needing to convert some theming scheme to CSS.
 * <p>
 * Standard {@link CSSTheme} is used if all theming is done in base CSS.
 * <p>
 * However, say you wanted to make all the theming with LESS and compile that to CSS at runtime. Create a LESSTheme
 * that
 * implements this class and then you would accept the theme directory. Load in all the LESS files and compile them
 * into
 * an app.css file.
 * <p>
 * {@link Theme#getPath()} should return the theme file path (URL for JavaFX uses).
 * <p>
 * {@link Theme#getName()} should return the name of the parent directory containing the theme file and should
 * represent the themes name.
 *
 * @author Nesbitt, Morgan Created on 2/23/2016.
 */
public interface Theme
{
	/**
	 * Should return the path to the CSS file for JavaFX loading (Using URL as JavaFX requires it).
	 *
	 * @return Path to file
	 *
	 * @throws MalformedURLException
	 * 		can be thrown when calling toURI().toURL().toString()
	 */
	String getPath() throws MalformedURLException;

	/**
	 * Name of both theme and themes parent directory.
	 *
	 * @return Theme name
	 */
	String getName();
}
