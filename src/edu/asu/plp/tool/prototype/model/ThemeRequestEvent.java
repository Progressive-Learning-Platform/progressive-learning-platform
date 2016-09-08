package edu.asu.plp.tool.prototype.model;

/**
 * Event for requesting an {@link Theme} through the {@link edu.asu.plp.tool.backend.EventRegistry}.
 *
 * Used for requesting a change to the Application Theme.
 *
 * Subscribe to {@link ThemeRequestCallback} for retrieving the returned theme.
 *
 * @author Nesbitt, Morgan on 2/24/2016.
 */
public class ThemeRequestEvent
{
	private String themeName;

	public ThemeRequestEvent( String themeName )
	{
		this.themeName = themeName;
	}

	public String getThemeName()
	{
		return themeName;
	}
}
