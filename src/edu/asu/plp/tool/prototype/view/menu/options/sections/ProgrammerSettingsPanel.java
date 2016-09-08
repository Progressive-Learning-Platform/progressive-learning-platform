package edu.asu.plp.tool.prototype.view.menu.options.sections;

import edu.asu.plp.tool.prototype.model.Submittable;
import edu.asu.plp.tool.prototype.util.UIConstants;
import edu.asu.plp.tool.prototype.util.UIStyle;
import edu.asu.plp.tool.prototype.util.VerifyUtil;
import edu.asu.plp.tool.prototype.view.menu.options.details.ProgrammerSettingDetails;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.StringProperty;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextField;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class ProgrammerSettingsPanel extends BorderPane implements Submittable
{
	private BooleanProperty programInChunksSelectionModel;
	private StringProperty maximumChunkSizeSelectionModel;
	private StringProperty receiveTimeoutSelectionModel;
	private BooleanProperty autodetectSerialPortsSelectionModel;

	public ProgrammerSettingsPanel()
	{
		this(ProgrammerSettingDetails.DEFAULT);
	}

	public ProgrammerSettingsPanel( ProgrammerSettingDetails details )
	{
		ProgrammerSettingDetails settingDetails = ( details != null ) ? details : ProgrammerSettingDetails.DEFAULT;
		VBox settingsColumn = new VBox();

		settingsColumn.getChildren().add(programInChunksSelection(settingDetails));
		settingsColumn.getChildren().add(maximumChunkSizeSelection(settingDetails));
		settingsColumn.getChildren().add(timeoutSelection(settingDetails));
		settingsColumn.getChildren().add(autodetectSerialPortSelection(settingDetails));

		settingsColumn.setPadding(new Insets(10));
		settingsColumn.setSpacing(8);

		setCenter(settingsColumn);
	}

	private Node programInChunksSelection( ProgrammerSettingDetails settingDetails )
	{
		HBox hBox = new HBox();

		//TODO ensure numerical values only
		CheckBox programInChunksCheckBox = new CheckBox("Program in chunks");
		programInChunksCheckBox.setAllowIndeterminate(false);
		programInChunksCheckBox.setSelected(Boolean.valueOf(settingDetails.getProgramInChunks()));

		programInChunksSelectionModel = programInChunksCheckBox.selectedProperty();

		hBox.getChildren().add(programInChunksCheckBox);

		return hBox;
	}

	private Node maximumChunkSizeSelection( ProgrammerSettingDetails settingDetails )
	{
		HBox hBox = new HBox();

		//TODO ensure numerical values only
		Text maxChunkSizeLabel = new Text("Maximum chunk size: ");
		maxChunkSizeLabel.setId(UIConstants.TEXT_COLOR);

		TextField maxChunkSizeTextField = new TextField();
		maxChunkSizeTextField.setText(settingDetails.getMaximumChunkSize());

		maxChunkSizeTextField.textProperty().addListener(( observable, oldValue, newValue ) -> {
			if ( !newValue.isEmpty() )
				UIStyle.applyError(VerifyUtil.simpleIntegerCheck(newValue), maxChunkSizeTextField);
		});

		maximumChunkSizeSelectionModel = maxChunkSizeTextField.textProperty();

		hBox.getChildren().addAll(maxChunkSizeLabel, maxChunkSizeTextField);

		return hBox;
	}

	private Node timeoutSelection( ProgrammerSettingDetails settingDetails )
	{
		HBox hBox = new HBox();

		Text receiveTimeoutLabel = new Text("Receive timout (ms): ");
		receiveTimeoutLabel.setId(UIConstants.TEXT_COLOR);

		TextField receiveTimeoutTextField = new TextField();
		receiveTimeoutTextField.setText(settingDetails.getReceiveTimeoutMilliseconds());

		receiveTimeoutTextField.textProperty().addListener(( observable, oldValue, newValue ) -> {
			if ( !newValue.isEmpty() )
				UIStyle.applyError(VerifyUtil.simpleIntegerCheck(newValue), receiveTimeoutTextField);
		});

		receiveTimeoutSelectionModel = receiveTimeoutTextField.textProperty();

		hBox.getChildren().addAll(receiveTimeoutLabel, receiveTimeoutTextField);

		return hBox;
	}

	private Node autodetectSerialPortSelection( ProgrammerSettingDetails settingDetails )
	{
		HBox hBox = new HBox();

		CheckBox autodetectCheckBox = new CheckBox("Autodetect serial ports");
		autodetectCheckBox.setAllowIndeterminate(false);
		autodetectCheckBox.setSelected(Boolean.valueOf(settingDetails.getAutoDetectSerialPorts()));

		autodetectSerialPortsSelectionModel = autodetectCheckBox.selectedProperty();

		hBox.getChildren().add(autodetectCheckBox);

		return hBox;
	}

	public ProgrammerSettingDetails getResults()
	{
		String programInChunks = String.valueOf(programInChunksSelectionModel.getValue());
		String maximumChunkSize = maximumChunkSizeSelectionModel.getValue();
		String receiveTimeout = receiveTimeoutSelectionModel.getValue();
		String autodetectSerialPorts = String.valueOf(autodetectSerialPortsSelectionModel.getValue());

		return new ProgrammerSettingDetails(programInChunks, maximumChunkSize, receiveTimeout, autodetectSerialPorts);
	}

	@Override
	public boolean isValid()
	{
		String timeout = receiveTimeoutSelectionModel.getValue();
		String maxChunkSize = maximumChunkSizeSelectionModel.getValue();

		return VerifyUtil.simpleIntegerCheck(timeout, maxChunkSize);
	}

	@Override
	public void submit()
	{
		//TODO programmer events
	}
}
