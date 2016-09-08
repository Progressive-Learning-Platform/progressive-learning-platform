package edu.asu.plp.tool.prototype.model;

/**
 * Enumeration for accessing required (default) settings from the application settings file.
 * <p>
 * See {@link edu.asu.plp.tool.prototype.ApplicationSettings#getSetting(Setting)} for usage with this enumeration.
 *
 * @author Nesbitt, Morgan Created on 2/23/2016.
 */
public enum ApplicationSetting implements Setting
{
	RESOURCES_PATH,
	ACE_PATH,
	APPLICATION_THEME_BASE_PATH,
	LANGUAGES_BASE_PATH,
	LANGUAGES_MODES_PATH,
	LANGUAGES_DEFAULT_PATH,

	EDITOR_FONT,
	EDITOR_FONT_SIZE,
	EDITOR_MODE,
	EDITOR_THEME,

	APPLICATION_THEME,

	PROGRAMMER_PROGRAM_IN_CHUNKS,
	PROGRAMMER_MAXIMUM_CHUNK_SIZE,
	PROGRAMMER_TIMEOUT_MILLISECONDS,
	PROGRAMMER_AUTO_DETECT_SERIAL_PORTS,

	SIMULATOR_SPEED,
	SIMULATOR_ALLOW_EXECUTION_OF_NON_INSTRUCTION_MEMORY,
	SIMULATOR_ASSUME_ZERO_ON_READS_FROM_UNINITIALIZED_MEMORY
}