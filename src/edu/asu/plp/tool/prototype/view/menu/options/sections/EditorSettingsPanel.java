package edu.asu.plp.tool.prototype.view.menu.options.sections;

import edu.asu.plp.tool.prototype.model.Submittable;
import edu.asu.plp.tool.prototype.util.UIConstants;
import edu.asu.plp.tool.prototype.util.UIStyle;
import edu.asu.plp.tool.prototype.util.VerifyUtil;
import edu.asu.plp.tool.prototype.view.menu.options.details.EditorSettingDetails;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.ComboBox;
import javafx.scene.control.SingleSelectionModel;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;

import java.util.Arrays;
import java.util.List;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class EditorSettingsPanel extends BorderPane implements Submittable
{
	SingleSelectionModel<String> fontSelectionModel;
	SingleSelectionModel<String> fontSizeSelectionModel;
	SingleSelectionModel<String> editorModeSelectionModel;

	public EditorSettingsPanel( ObservableList<String> fontList, ObservableList<String> editorModes )
	{
		this(fontList, editorModes, EditorSettingDetails.DEFAULT);
	}

	public EditorSettingsPanel( ObservableList<String> fontList, ObservableList<String> editorModes,
			EditorSettingDetails details )
	{
		EditorSettingDetails settingDetails = ( details != null ) ? details : EditorSettingDetails.DEFAULT;
		VBox settingsColumn = new VBox();

		settingsColumn.getChildren().add(fontListSelection(settingDetails, fontList));
		settingsColumn.getChildren().add(fontSizeSelection(settingDetails));
		settingsColumn.getChildren().add(editorModeSelection(settingDetails, editorModes));

		settingsColumn.setPadding(new Insets(10));
		settingsColumn.setSpacing(8);

		setCenter(settingsColumn);
	}

	private Node fontListSelection( EditorSettingDetails settingDetails, ObservableList<String> fontList )
	{
		HBox hBox = new HBox();

		Text fontLabel = new Text("Font: ");
		fontLabel.setId(UIConstants.TEXT_COLOR);

		ComboBox<String> fonts = new ComboBox<>(fontList);
		fonts.getSelectionModel().select(settingDetails.getFontName());

		fontSelectionModel = fonts.getSelectionModel();


		hBox.getChildren().addAll(fontLabel, fonts);
		hBox.setAlignment(Pos.CENTER_LEFT);

		return hBox;
	}

	private Node fontSizeSelection( EditorSettingDetails settingDetails )
	{
		HBox hBox = new HBox();

		Text fontSizeLabel = new Text("Font Size: ");
		fontSizeLabel.setId(UIConstants.TEXT_COLOR);

		List<String> fontSizes = Arrays.asList("6", "8", "12", "14", "16", "20", "32", "48", "60", "72");

		ObservableList<String> fontSizeList = FXCollections.observableArrayList(fontSizes);

		ComboBox<String> fontSizesComboBox = new ComboBox<>(fontSizeList);
		fontSizesComboBox.setEditable(true);
		fontSizesComboBox.editorProperty().getValue().textProperty().addListener(( observable, oldValue, newValue )
																						 -> {
			if ( !newValue.isEmpty() )
				UIStyle.applyError(VerifyUtil.simpleIntegerCheck(newValue), fontSizesComboBox);
		});

		fontSizesComboBox.getSelectionModel().select(settingDetails.getFontSize());
		fontSizesComboBox.autosize();

		fontSizeSelectionModel = fontSizesComboBox.getSelectionModel();

		hBox.getChildren().addAll(fontSizeLabel, fontSizesComboBox);
		hBox.setAlignment(Pos.CENTER_LEFT);

		return hBox;
	}

	private Node editorModeSelection( EditorSettingDetails settingDetails, ObservableList<String> editorModes )
	{
		HBox hBox = new HBox();

		Text editorModeLabel = new Text("Editor Mode: ");
		editorModeLabel.setId(UIConstants.TEXT_COLOR);

		ComboBox<String> editorModeComboBox = new ComboBox<>(editorModes);
		editorModeComboBox.getSelectionModel().select(settingDetails.getEditorMode());

		editorModeSelectionModel = editorModeComboBox.getSelectionModel();

		hBox.getChildren().addAll(editorModeLabel, editorModeComboBox);
		hBox.setAlignment(Pos.CENTER_LEFT);

		return hBox;
	}

	public EditorSettingDetails getResults()
	{
		String fontName = fontSelectionModel.getSelectedItem();
		String fontSize = fontSizeSelectionModel.getSelectedItem();
		String editorMode = editorModeSelectionModel.getSelectedItem();

		return new EditorSettingDetails(fontName, fontSize, editorMode);
	}

	@Override
	public boolean isValid()
	{
		return VerifyUtil.simpleIntegerCheck(fontSizeSelectionModel.getSelectedItem());
	}

	@Override
	public void submit()
	{
		//TODO waiting for editor changes to fufill all editor events
	}
}
