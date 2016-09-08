package edu.asu.plp.tool.prototype.view.menu.options.details;

import edu.asu.plp.tool.prototype.model.ApplicationSetting;
import edu.asu.plp.tool.prototype.model.SettingUtil;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class EditorSettingDetails
{
	public static final EditorSettingDetails DEFAULT = EditorSettingDetails.defaultDetails();

	private String editorMode;
	private String fontName;
	private String fontSize;
	//TODO Consider new below EditorSettingDetails
	//Soft wrapping, scroll behavior, show line numbers, print margin length, use soft tabs

	private EditorSettingDetails()
	{

	}

	public EditorSettingDetails( String editorMode, String fontName, String fontSize )
	{
		this.editorMode = editorMode;
		this.fontName = fontName;
		this.fontSize = fontSize;
	}

	public EditorSettingDetails( EditorSettingDetails details )
	{
		this(details.editorMode, details.fontName, details.fontSize);
	}

	private static EditorSettingDetails defaultDetails()
	{
		EditorSettingDetails details = new EditorSettingDetails();

		ApplicationSetting setting = ApplicationSetting.EDITOR_MODE;
		details.editorMode =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.EDITOR_FONT;
		details.fontName =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		setting = ApplicationSetting.EDITOR_FONT_SIZE;
		details.fontSize =
				SettingUtil.loadRequiredSavedSettingDefaultIfNotPresent(setting);

		return details;
	}

	public String getEditorMode()
	{
		return editorMode;
	}

	public String getFontName()
	{
		return fontName;
	}

	public String getFontSize()
	{
		return fontSize;
	}
}
