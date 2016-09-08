package edu.asu.plp.tool.prototype.view.menu.options.details;

import edu.asu.plp.tool.prototype.model.ApplicationSetting;
import edu.asu.plp.tool.prototype.model.SettingUtil;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class ApplicationSettingDetails
{
	public static final ApplicationSettingDetails DEFAULT = ApplicationSettingDetails.defaultDetails();

	private String applicationTheme;
	private String editorTheme;

	private ApplicationSettingDetails()
	{
	}

	public ApplicationSettingDetails( String applicationTheme, String editorTheme )
	{
		this.applicationTheme = applicationTheme;
		this.editorTheme = editorTheme;
	}

	public ApplicationSettingDetails( ApplicationSettingDetails details )
	{
		this.applicationTheme = details.applicationTheme;
		this.editorTheme = details.editorTheme;
	}

	private static ApplicationSettingDetails defaultDetails()
	{
		ApplicationSettingDetails details = new ApplicationSettingDetails();

		ApplicationSetting setting = ApplicationSetting.APPLICATION_THEME;
		details.applicationTheme =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.EDITOR_THEME;
		details.editorTheme =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		return details;
	}

	public String getApplicationTheme()
	{
		return applicationTheme;
	}

	public String getEditorTheme()
	{
		return editorTheme;
	}


}
