package edu.asu.plp.tool.prototype;

import edu.asu.plp.tool.prototype.model.Setting;
import edu.asu.plp.tool.prototype.model.SettingUtil;
import org.apache.commons.io.FileUtils;
import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Optional;

/**
 * Global map that holds settings configurations. Derives from a .settings file (JSON Format). If the file is not
 * loaded
 * or cannot be loaded, then the default settings will be used.
 * <p>
 * See {@link edu.asu.plp.tool.prototype.model.ApplicationSetting} for the base required values.
 * <p>
 * To use this settings class, you must call {@link ApplicationSettings#initialize()} before attempting any usage.
 * <p>
 * You must also provide it with a settings file by {@link ApplicationSettings#loadFromFile(String)} or {@link
 * ApplicationSettings#loadFromFile(File)}. Or load from the {@link ApplicationSettings#DEFAULT_SETTINGS_FILE} by
 * calling {@link ApplicationSettings#loadFromFile()}.
 * <p>
 * This will populate the map with all the given values. To retrieve a value from the map call {@link
 * ApplicationSettings#getSetting(String)} with the respective key and it will return the value wrapped in an Optional
 * if it exists or return an empty Optional otherwise.
 * <p>
 * HOWEVER, it is highly suggested that you query for keys using {@link ApplicationSettings#getSetting(Setting)}.
 * Please
 * see {@link Setting} for information on how to use/implement {@link Setting}.
 * <p>
 * Section of the file might look something like this:
 * <p>
 * {
 * <p>
 * <p style="padding-left: 4em;">"Application" : {</p>
 * <p style="padding-left: 8em;">"theme base path" : "resources/application/styling/",</p>
 * <p style="padding-left: 8em;">"theme" : "light"</p>
 * <p style="padding-left: 4em;">}</p>
 * }
 * <p>
 * The keys for accessing those two inner values would be stored as:
 * APPLICATION_THEME_BASE_PATH
 * and
 * APPLICATION_THEME
 * <p>
 * This representation leads great for how enums are typically written
 * {@link edu.asu.plp.tool.prototype.model.ApplicationSetting}
 *
 * @author Nesbitt, Morgan Created on 2/23/2016.
 */
public class ApplicationSettings
{
	/**
	 * Default settings file path.
	 */
	public static final String DEFAULT_SETTINGS_FILE = "settings/plp-tool.settings";

	/**
	 * Global (static) instance, for singleton pattern.
	 */
	private static ApplicationSettings instance;

	/**
	 * Settings map
	 */
	private static HashMap<String, String> settings;

	/**
	 * Location of settings file loaded. Can be the same as {@link ApplicationSettings#DEFAULT_SETTINGS_FILE}.
	 */
	private String settingsSaveLocation;

	/**
	 * Private constructor to enforce singleton pattern.
	 */
	private ApplicationSettings()
	{
		settings = new HashMap<>();
	}

	/**
	 * Global accessor for retrieving a setting from the map.
	 * <p>
	 * Takes in a string key to retrieve a value from the map.
	 * <p>
	 * NOTE: Suggested you use {@link ApplicationSettings#getSetting(Setting)} instead.
	 *
	 * @param key
	 * 		Key query for retrieving a value.
	 *
	 * @return Optional with a value if present and empty otherwise.
	 */
	public static Optional<String> getSetting( String key )
	{
		if ( settings.containsKey(key) )
			return Optional.of(settings.get(key));

		return Optional.empty();
	}

	/**
	 * Global accessor for retrieving a setting from the map.
	 * <p>
	 * Takes in any variant of the {@link Setting} interface. Uses the {@link Setting#toString()} as the key for
	 * searching for the setting.
	 * <p>
	 * NOTE: This is the suggested way to access values from the {@link ApplicationSettings}.
	 *
	 * @param setting
	 * 		Key query for retrieving a value.
	 *
	 * @return Optional with a value if present and empty otherwise.
	 */
	public static Optional<String> getSetting( Setting setting )
	{
		return getSetting(setting.toString());
	}

	/**
	 * Stores the value parameter in the map as the save version of key.
	 * <p>
	 * The value will not be saved if the key is not a valid key in the map.
	 * <p>
	 * NOTE: The key map to a key that has a default in the map, and the map will store it as a saved version.
	 * <p>
	 * e.g. APPLICATION_THEME is valid (as it has a default value) while THIS_IS_NEW has no default value.
	 * <p>
	 * Naturally, creating THIS_IS_NEW in the settings file would allow you to store it in the map.
	 * <p>
	 * NOTE: It is suggested you use {@link ApplicationSettings#writeSetting(Setting, String)}.
	 *
	 * @param key
	 * 		Key that has a default value in the map
	 * @param value
	 * 		Value to store as a saved value (Different then default value)
	 *
	 * @return True if key is in map, false otherwise.
	 */
	public static boolean writeSetting( String key, String value )
	{
		if ( !settings.containsKey(key) )
			return false;
		else
			settings.put(SettingUtil.prependSaveLabel(key), value);

		return true;
	}

	/**
	 * Stores the value parameter in the map as the save version of key.
	 * <p>
	 * The value will not be saved if the key is not a valid key in the map.
	 * <p>
	 * NOTE: The key map to a key that has a default in the map, and the map will store it as a saved version.
	 * <p>
	 * e.g. APPLICATION_THEME is valid (as it has a default value) while THIS_IS_NEW has no default value.
	 * <p>
	 * Naturally, creating THIS_IS_NEW in the settings file would allow you to store it in the map.
	 *
	 * @param setting
	 * 		Setting that has a default value in the map
	 * @param value
	 * 		Value to store as a saved value (Different then default value)
	 *
	 * @return True if key is in map, false otherwise.
	 */
	public static boolean writeSetting( Setting setting, String value )
	{
		return writeSetting(setting.toString(), value);
	}

	/**
	 * Initiailizes singleton instance of this class.
	 * <p>
	 * NOTE: This MUST be called to utilize this class.
	 *
	 * @return Global handler to the {@link ApplicationSettings}.
	 */
	public static ApplicationSettings initialize()
	{
		if ( instance == null )
			instance = new ApplicationSettings();

		return instance;
	}

	/**
	 * Attempts to load the settings file from the {@link ApplicationSettings#DEFAULT_SETTINGS_FILE}.
	 * <p>
	 * Using {@link ApplicationSettings#loadFromFile(String)}.
	 * <p>
	 * NOTE: This will load the default settings configuration if it fails to load.
	 *
	 * @return True if successfully loaded file, false otherwise.
	 */
	public static final boolean loadFromFile()
	{
		return loadFromFile(DEFAULT_SETTINGS_FILE);
	}

	/**
	 * Attempts to load the settings file from the filePath parameter.
	 * <p>
	 * Using {@link ApplicationSettings#loadFromFile(File)}.
	 * <p>
	 * NOTE: This will load the default settings configuration if it fails to load.
	 *
	 * @param filePath
	 * 		file path
	 *
	 * @return True if successfully loaded file, false otherwise.
	 */
	public static final boolean loadFromFile( String filePath )
	{
		return loadFromFile(new File(filePath));
	}

	/**
	 * Attempts to load the settings file from the file parameter.
	 * <p>
	 * NOTE: This will load the default settings configuration if it fails to load.
	 *
	 * @param file
	 * 		settings file
	 *
	 * @return True if successfully loaded file, false otherwise.
	 */
	public static final boolean loadFromFile( File file )
	{
		if ( !file.isFile() )
		{
			loadFromDefaultSettings();
			return false;
		}

		try
		{
			String fileContents = FileUtils.readFileToString(file);
			JSONObject jsonFile = new JSONObject(fileContents);

			parseJSONSettings(jsonFile, "");

			if ( settings.isEmpty() )
			{
				loadFromDefaultSettings();
				return false;
			} else
				return true;
		}
		catch ( IOException e )
		{
			loadFromDefaultSettings();
			return false;
		}
	}

	public static final boolean saveToFile()
	{
		return saveToFile(DEFAULT_SETTINGS_FILE);
	}

	public static final boolean saveToFile( String filePath )
	{
		return saveToFile(new File(filePath));
	}

	public static final boolean saveToFile( File file )
	{
		if ( !file.isFile() )
			return false;

		//TODO Fix not maintaining structure after inputting into map

		throw new UnsupportedOperationException("saveToFile is not implemented ");
	}


	/**
	 * Loads internal default settings into settings map.
	 * <p>
	 * NOTE: This is expected to only be used when you cannot find the settings file. Or when you want to reset the
	 * settings file.
	 */
	public static final void loadFromDefaultSettings()
	{
		JSONObject jsonSettings = new JSONObject(generateDefaultSettings());
		parseJSONSettings(jsonSettings);
	}

	/**
	 * Helper function for {@link ApplicationSettings#parseJSONSettings(JSONObject, String)} that passes a blank base
	 * path.
	 *
	 * @param jsonSettings
	 * 		JSON Object of settings file
	 */
	private static void parseJSONSettings( JSONObject jsonSettings )
	{
		parseJSONSettings(jsonSettings, "");
	}

	/**
	 * Takes jsonSettings parameter and inserts all Key-Value pairs into the settings map
	 * <p>
	 * Recursively passes child json objects into itself with there path as the new base path. Utilizing {@link
	 * ApplicationSettings#bindPath(String, String)} to link the new paths to the base path.
	 * <p>
	 * NOTE: Expects a json object with keys mapping to JSON objects or Strings ONLY. It will fail fast otherwise.
	 *
	 * @param jsonSettings
	 * 		JSON Object of settings file
	 * @param basePath
	 * 		Starting path to prefix onto new keys
	 */
	private static void parseJSONSettings( JSONObject jsonSettings, String basePath )
	{
		for ( String key : JSONObject.getNames(jsonSettings) )
		{
			Object value = jsonSettings.get(key);
			if ( value instanceof String )
			{
				settings.put(bindPath(basePath, key), (String) value);
			} else
			{
				parseJSONSettings(jsonSettings.getJSONObject(key), bindPath(basePath, key));
			}
		}
	}

	/**
	 * Turns JSON Objects into searchable (Key Based) representations.
	 * <p>
	 * Meant to be used when reading input and mapping Key-Value pairs.
	 *
	 * @param basePath
	 * 		Parent path of currentPath parameter.
	 * @param currentPath
	 * 		Key to an object, whether its another json object or a string value.
	 *
	 * @return Two paths (Made all uppercase) connected together with underscores.
	 */
	private static String bindPath( String basePath, String currentPath )
	{
		if ( basePath.isEmpty() )
			return currentPath.replace(" ", "_").toUpperCase();

		String combinedPath = basePath + "_" + currentPath.replace(" ", "_");
		return combinedPath.toUpperCase();
	}

	/**
	 * Generates a JSON String with all of the required settings and their default values.
	 * <p>
	 * Meant to be used, if a settings file is not located.
	 *
	 * @return JSON String representation of the default settings file.
	 */
	private static final String generateDefaultSettings()
	{
		StringBuilder builder = new StringBuilder();

		final String newLine = System.lineSeparator();

		builder.append("{" + newLine);
		builder.append("\t\"Resources Path\" : \"resources/\"," + newLine);
		builder.append("\t\"Ace Path\" : \"lib/ace/\"," + newLine);
		//Languages
		builder.append("\t\"Languages\" : {" + newLine);
		builder.append("\t\t\"base path\" : \"resources/languages/\"," + newLine);
		builder.append("\t\t\"modes path\" : \"modes/\"," + newLine);
		builder.append("\t\t\"default path\" : \"plp/\"" + newLine);
		builder.append("\t}," + newLine);
		//Application
		builder.append("\t\"Application\" : {" + newLine);
		builder.append("\t\t\"theme base path\" : \"resources/application/styling/\"," + newLine);
		builder.append("\t\t\"theme\" : \"light\"," + newLine);
		builder.append("\t}," + newLine);
		//Editor
		builder.append("\t\"Editor\" : {" + newLine);
		builder.append("\t\t\"font\" : \"inconsolata\"," + newLine);
		builder.append("\t\t\"font size\" : \"14\"," + newLine);
		builder.append("\t\t\"mode\" : \"plp\"," + newLine);
		builder.append("\t\t\"theme\" : \"tomorrow\"" + newLine);
		builder.append("\t}," + newLine);
		//Programmer
		builder.append("\t\"Programmer\" : {" + newLine);
		builder.append("\t\t\"program in chunks\" : \"true\"," + newLine);
		builder.append("\t\t\"maximum chunk size\" : \"2048\"," + newLine);
		builder.append("\t\t\"timeout in milliseconds\" : \"500\"," + newLine);
		builder.append("\t\t\"auto detect serial ports\" : \"true\"" + newLine);
		builder.append("\t}," + newLine);
		//Simulator
		builder.append("\t\"Simulator\" : {" + newLine);
		builder.append("\t\t\"speed\" : \"100\"," + newLine);
		builder.append("\t\t\"allow execution of non instruction memory\" : \"true\"," + newLine);
		builder.append("\t\t\"assume zero on reads from uninitialized memory\" : \"true\"" + newLine);
		builder.append("\t}" + newLine);

		builder.append("}" + newLine);

		//		System.out.println(builder.toString());

		return builder.toString();
	}
}
