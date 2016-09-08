package edu.asu.plp.tool.prototype.view.menu.options.sections;

import edu.asu.plp.tool.backend.EventRegistry;
import edu.asu.plp.tool.prototype.model.Submittable;
import edu.asu.plp.tool.prototype.model.ThemeRequestEvent;
import edu.asu.plp.tool.prototype.util.UIConstants;
import edu.asu.plp.tool.prototype.view.menu.options.details.ApplicationSettingDetails;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.ComboBox;
import javafx.scene.control.SingleSelectionModel;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class ApplicationSettingsPanel extends BorderPane implements Submittable
{
	SingleSelectionModel<String> applicationThemeSelectionModel;
	SingleSelectionModel<String> editorThemeSelectionModel;

	public ApplicationSettingsPanel( ObservableList<String> applicationThemes, ObservableList<String> editorsThemes )
	{
		this(applicationThemes, editorsThemes, ApplicationSettingDetails.DEFAULT);
	}

	public ApplicationSettingsPanel( ObservableList<String> applicationThemes, ObservableList<String> editorsThemes,
			ApplicationSettingDetails details )
	{
		ApplicationSettingDetails settingDetails = ( details != null ) ? details : ApplicationSettingDetails.DEFAULT;

		VBox settingsColumn = new VBox();

		settingsColumn.getChildren().add(applicationThemeSelection(settingDetails, applicationThemes));
		settingsColumn.getChildren().add(editorThemeSelection(settingDetails, editorsThemes));

		settingsColumn.setPadding(new Insets(10));
		settingsColumn.setSpacing(8);

		setCenter(settingsColumn);
	}

	private HBox applicationThemeSelection( ApplicationSettingDetails details,
			ObservableList<String> applicationThemes )
	{
		HBox hbox = new HBox();

		Text themeLabel = new Text("Application Theme: ");
		themeLabel.setId(UIConstants.TEXT_COLOR);

		ObservableList<String> list = applicationThemes;

		ComboBox<String> themes = new ComboBox<>(list);
		themes.getSelectionModel().select(details.getApplicationTheme());

		applicationThemeSelectionModel = themes.getSelectionModel();


		hbox.getChildren().addAll(themeLabel, themes);
		hbox.setAlignment(Pos.CENTER_LEFT);

		return hbox;
	}

	private HBox editorThemeSelection( ApplicationSettingDetails details, ObservableList<String> editorsThemes )
	{
		HBox hbox = new HBox();

		Text themeLabel = new Text("Editor Theme: ");
		themeLabel.setId(UIConstants.TEXT_COLOR);

		ObservableList<String> list = editorsThemes;

		ComboBox<String> themes = new ComboBox<>(list);
		themes.getSelectionModel().select(0);

		editorThemeSelectionModel = themes.getSelectionModel();

		hbox.getChildren().addAll(themeLabel, themes);
		hbox.setAlignment(Pos.CENTER_LEFT);

		return hbox;
	}

	public ApplicationSettingDetails getResults()
	{
		String appTheme = applicationThemeSelectionModel.getSelectedItem();
		String editorTheme = editorThemeSelectionModel.getSelectedItem();

		return new ApplicationSettingDetails(appTheme, editorTheme);
	}

	@Override
	public boolean isValid()
	{
		return true;
	}

	@Override
	public void submit()
	{
		ApplicationSettingDetails details = getResults();
		EventRegistry.getGlobalRegistry().post(new ThemeRequestEvent(details.getApplicationTheme()));

		//TODO fire editor theme event
	}
}
