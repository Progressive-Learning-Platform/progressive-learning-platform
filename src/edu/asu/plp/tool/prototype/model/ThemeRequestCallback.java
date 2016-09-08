package edu.asu.plp.tool.prototype.model;

import java.util.Optional;

/**
 * Fired to {@link edu.asu.plp.tool.backend.EventRegistry} when a {@link ThemeRequestEvent} is received.
 * <p>
 * Contains an Optional of {@link Theme}, which will have a theme if present or be empty if not found in {@link
 * ApplicationThemeManager}.
 *
 * @author Nesbitt, Morgan on 2/24/2016.
 */
public class ThemeRequestCallback
{
	private Optional<Theme> theme;

	public ThemeRequestCallback( Optional<Theme> theme )
	{
		this.theme = theme;
	}

	public Optional<Theme> requestedTheme()
	{
		return theme;
	}
}
