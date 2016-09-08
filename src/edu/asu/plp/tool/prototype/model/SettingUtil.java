package edu.asu.plp.tool.prototype.model;

import edu.asu.plp.tool.prototype.ApplicationSettings;

import java.util.Optional;

/**
 * Methods for accessing the {@link ApplicationSettings} class easier.
 *
 * @author by Morgan on 3/5/2016.
 */
public class SettingUtil
{
	/**
	 * For use in loading only saved settings that are have a default counterpart.
	 * <p>
	 * If the saved version of the setting is not present it will default to the {@link ApplicationSetting} parameter.
	 * <p>
	 * If the default setting is not present in the settings it will throw an illegal state exception.
	 * <p>
	 * NOTE: This is reliant on the fact that any setting in {@link ApplicationSetting} must me present in {@link
	 * ApplicationSettings}.
	 * <p>
	 * If the setting you are trying to retrieve is not present in {@link ApplicationSetting}, this is not the method
	 * you are looking for.
	 * <p>
	 * See {@link SettingUtil#loadSavedSettingDefaultIfNotPresent(Setting)} for settings not natively provided by
	 * PLPTool
	 *
	 * @param setting
	 *
	 * @return Saved setting if present, or default otherwise
	 */
	public static String loadRequiredSavedSettingDefaultIfNotPresent( ApplicationSetting setting )
	{
		Optional<String> retrievedSetting = loadSavedSettingDefaultIfNotPresent(setting);
		if(retrievedSetting.isPresent())
			return retrievedSetting.get();
		else
			throw new IllegalStateException(
					"Default ApplicationSetting was not present. ApplicationSettings must resolve all " +
							"default application settings if not present in loading.");
	}

	/**
	 * For use in loading only saved settings that are have a default counterpart.
	 * <p>
	 * If the saved version of the setting is not present it will default to the {@link Setting} parameter.
	 * <p>
	 * If the default version is not present it will return an empty optional.
	 *
	 * @param setting
	 *
	 * @return Saved setting if present, default if saved is not present, empty optional if default is not present.
	 */
	public static Optional<String> loadSavedSettingDefaultIfNotPresent( Setting setting )
	{
		Optional<String> savedSetting = ApplicationSettings.getSetting(SettingUtil.prependSaveLabel(setting));
		if ( savedSetting.isPresent() )
			return savedSetting;
		else
		{
			Optional<String> defaultSetting = ApplicationSettings.getSetting(setting);
			if ( defaultSetting.isPresent() )
				return defaultSetting;
			else
				return Optional.empty();
		}
	}

	/**
	 * Prepends the save label to provided {@link Setting} parameter.
	 * <p>
	 * NOTE: This expects you to not create {@link Setting} for saved variants.
	 * <p>
	 * {@link ApplicationSettings} will load saved settings if present in the settings file. Saved settings share the
	 * same name as their default counterpart, but are prefixed with SAVED_.
	 * <p>
	 * e.g EDITOR_FONT vs SAVED_EDITOR_FONT
	 *
	 * @param setting
	 *
	 * @return
	 */
	public static String prependSaveLabel( Setting setting )
	{
		return prependSaveLabel(setting.toString());
	}

	public static String prependSaveLabel( String setting )
	{
		return "SAVED_" + setting;
	}

}
