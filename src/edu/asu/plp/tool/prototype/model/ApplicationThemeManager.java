package edu.asu.plp.tool.prototype.model;

import com.google.common.eventbus.Subscribe;
import edu.asu.plp.tool.backend.EventRegistry;
import edu.asu.plp.tool.prototype.ApplicationSettings;

import java.io.File;
import java.util.*;
import java.util.stream.Stream;

/**
 * Manager that holds all the application level (Non-Editor) themes loaded from the provided theme path from {@link
 * edu.asu.plp.tool.prototype.ApplicationSettings}
 *
 * @author Nesbitt, Morgan Created on 2/23/2016.
 */
public class ApplicationThemeManager
{
	ApplicationThemeManagerEventHandler eventHandler;
	//TODO replace with better exceptions
	private String themeDirectory;
	private HashMap<String, Theme> themes;
	private Theme defaultTheme;

	public ApplicationThemeManager()
	{
		themes = new HashMap<>();
		loadThemeDirectory();

		eventHandler = new ApplicationThemeManagerEventHandler();
	}

	public Theme getDefaultTheme()
	{
		return defaultTheme;
	}

	public boolean hasTheme( String themeName )
	{
		return themes.containsKey(themeName);
	}

	public Theme getTheme( String themeName )
	{
		return themes.get(themeName);
	}

	/**
	 * Checks to make sure the ApplicationSettings provides a path for base application theme path.
	 * <p>
	 * Then calls {@link ApplicationThemeManager#loopDirectory(String)} to create all the themes.
	 */
	private void loadThemeDirectory()
	{
		Optional<String> optionalAppThemePath =
				ApplicationSettings.getSetting(ApplicationSetting.APPLICATION_THEME_BASE_PATH);
		if ( !optionalAppThemePath.isPresent() )
		{
			throw new MissingResourceException(
					ApplicationSetting.APPLICATION_THEME_BASE_PATH.toString() + " was not present in settings.",
					this.getClass().getName(),
					ApplicationSetting.APPLICATION_THEME_BASE_PATH.toString());
		}

		this.themeDirectory = optionalAppThemePath.get();

		loopDirectory(themeDirectory);
	}

	/**
	 * Loops through the provided directory and trys to add all children directories as themes.
	 * <p>
	 * themeDirectoryPath parameter must be a directory.
	 *
	 * @param themeDirectoryPath
	 * 		based parent directory that contains all expected themes
	 */
	private void loopDirectory( String themeDirectoryPath )
	{
		File themes = new File(themeDirectoryPath);

		if ( themes.isDirectory() )
		{
			List<File> themeDirectories = Arrays.asList(themes.listFiles(File::isDirectory));
			for ( File theme : themeDirectories )
			{
				addTheme(theme);
			}

			loadDefaultAndActiveTheme();
		} else
			throw new IllegalArgumentException("Theme Directory path in Application Settings is not a directory.");
	}

	/**
	 * Loads a directory into a Theme object.
	 * <p>
	 * themeDirectory parameter must be a directory
	 *
	 * @param themeDirectory
	 * 		Directory holding a theme, and also the name of the theme
	 */
	private void addTheme( File themeDirectory )
	{
		//TODO add class loaders? So you can define a custom way to get the css (e.g. less compiled to css)
		if ( !themeDirectory.isDirectory() )
			throw new IllegalArgumentException("Theme Directory in addTheme(File themeDirectory) is not a directory.");

		String themeName = themeDirectory.getName();

		List<File> directoryFiles = Arrays.asList(themeDirectory.listFiles(File::isFile));

		long fileCount = directoryFiles.stream().filter(file -> file.getName().equals("app.css")).count();

		if ( fileCount < 1 )
			System.out.println(themeName + " was not added as a theme. No app.css was detected.");
		else if ( fileCount > 1 )
			throw new UnsupportedOperationException(
					"Currently not supporting/able to differentiate more then one app.css in a theme directory at a " +
							"time.");

		String appCssFilePath =
				directoryFiles.stream().filter(file -> file.getName().equals("app.css")).findFirst().get()
						.getAbsolutePath();

		themes.put(themeName, new CSSTheme(themeName, appCssFilePath));
	}

	private void loadDefaultAndActiveTheme()
	{
		Optional<String> optionalDefaultThemePath =
				ApplicationSettings.getSetting(ApplicationSetting.APPLICATION_THEME);
		if ( !optionalDefaultThemePath.isPresent() )
		{
			throw new MissingResourceException(
					ApplicationSetting.APPLICATION_THEME.toString() + " was not present in settings.",
					this.getClass().getName(),
					ApplicationSetting.APPLICATION_THEME.toString());
		}

		//TODO Check if just name or ends with / \
		String tempPath = optionalDefaultThemePath.get();
		String defaultThemeDirectoryPath = (tempPath.endsWith("/|\\")) ? tempPath : tempPath + "/";
		String defaultThemeName = defaultThemeDirectoryPath.substring(0, defaultThemeDirectoryPath.length() - 1);
		String defaultThemePath = themeDirectory + defaultThemeDirectoryPath;

		//TODO ensure no duplicate theme names
		File defaultThemeDirectory = new File(defaultThemePath);
		if ( !defaultThemeDirectory.isDirectory() )
			throw new IllegalArgumentException(
					"Default theme directory was not a directory. Found: " + defaultThemeDirectory.getAbsolutePath());

		Stream<String> themeStream = themes.keySet().stream().filter(themeName -> themeName.equals(defaultThemeName));

		if ( themeStream.count() < 1 )
			throw new UnsupportedOperationException("Default theme not found.");

		themeStream = themes.keySet().stream().filter(themeName -> themeName.equals(defaultThemeName));

		defaultTheme = themes.get(themeStream.findFirst().get());

		//TODO load active/saved theme from settings file saved json object
	}

	public class ApplicationThemeManagerEventHandler
	{
		private ApplicationThemeManagerEventHandler()
		{
			EventRegistry.getGlobalRegistry().register(this);
		}

		//TODO change saved theme to whatever request is
		@Subscribe
		public void processThemeRequest( ThemeRequestEvent event )
		{
			String requestedThemeName = event.getThemeName();
			if ( hasTheme(requestedThemeName) )
				EventRegistry.getGlobalRegistry()
						.post(new ThemeRequestCallback(Optional.of(getTheme(requestedThemeName))));
			else
				EventRegistry.getGlobalRegistry().post(new ThemeRequestCallback(Optional.empty()));
		}
	}

	public Collection<String> getThemeNames()
	{
		return themes.keySet();
	}
}
