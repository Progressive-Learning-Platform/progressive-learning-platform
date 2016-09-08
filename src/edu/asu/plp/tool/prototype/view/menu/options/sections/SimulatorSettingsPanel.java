package edu.asu.plp.tool.prototype.view.menu.options.sections;

import edu.asu.plp.tool.prototype.model.Submittable;
import edu.asu.plp.tool.prototype.util.UIConstants;
import edu.asu.plp.tool.prototype.view.menu.options.details.SimulatorSettingDetails;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.DoubleProperty;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Slider;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Text;

/**
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class SimulatorSettingsPanel extends BorderPane implements Submittable
{
	private DoubleProperty simulationSpeedModel;
	private BooleanProperty allowExecutionOfNonInstructionMemoryModel;
	private BooleanProperty assumeZeroOnReadsFromUninitializedMemoryModel;

	public SimulatorSettingsPanel()
	{
		this(SimulatorSettingDetails.DEFAULT);
	}

	public SimulatorSettingsPanel( SimulatorSettingDetails details )
	{
		SimulatorSettingDetails settingDetails = ( details != null ) ? details : SimulatorSettingDetails.DEFAULT;
		VBox settingsColumn = new VBox();

		settingsColumn.getChildren().add(simulationSpeedSelection(settingDetails));
		settingsColumn.getChildren()
				.add(executionOnNonInstructionMemorySelection(settingDetails));
		settingsColumn.getChildren().add(zeroOnReadsFromUninitializedMemorySelection(settingDetails));

		settingsColumn.setPadding(new Insets(10));
		settingsColumn.setSpacing(8);

		setCenter(settingsColumn);
	}

	private Node simulationSpeedSelection( SimulatorSettingDetails settingDetails )
	{
		VBox vBox = new VBox();

		Text simulationSpeedLabel = new Text("Simulation Speed (milliseconds / cycle)");
		simulationSpeedLabel.setId(UIConstants.TEXT_COLOR);

		//TODO take in settings for slider
		Slider simulationSpeedSlider = new Slider(0, 1000, 100);
		simulationSpeedSlider.setShowTickMarks(true);
		simulationSpeedSlider.setShowTickLabels(true);
		simulationSpeedSlider.setMajorTickUnit(100);
		simulationSpeedSlider.setBlockIncrement(100);

		simulationSpeedModel = simulationSpeedSlider.valueProperty();

		vBox.getChildren().addAll(simulationSpeedLabel, simulationSpeedSlider);

		return vBox;
	}

	private Node executionOnNonInstructionMemorySelection( SimulatorSettingDetails settingDetails )
	{
		HBox hBox = new HBox();

		//TODO ensure numerical values only
		CheckBox executionOfNonInstructionMemoryCheckBox = new CheckBox("Allow execution of non-instruction memory");
		executionOfNonInstructionMemoryCheckBox.setAllowIndeterminate(false);
		executionOfNonInstructionMemoryCheckBox
				.setSelected(Boolean.valueOf(settingDetails.getAllowExecutionOfNonInstructionMemory()));

		allowExecutionOfNonInstructionMemoryModel = executionOfNonInstructionMemoryCheckBox.selectedProperty();

		hBox.getChildren().add(executionOfNonInstructionMemoryCheckBox);

		return hBox;
	}

	private Node zeroOnReadsFromUninitializedMemorySelection( SimulatorSettingDetails settingDetails )
	{
		HBox hBox = new HBox();

		//TODO ensure numerical values only
		CheckBox zeroOnReadsFromUninitializedMemoryCheckBox =
				new CheckBox("Assume zero on reads from uninitialized memory");
		zeroOnReadsFromUninitializedMemoryCheckBox.setAllowIndeterminate(false);
		zeroOnReadsFromUninitializedMemoryCheckBox
				.setSelected(Boolean.valueOf(settingDetails.getAssumeZeroOnReadsFromUninitializedMemory()));

		assumeZeroOnReadsFromUninitializedMemoryModel = zeroOnReadsFromUninitializedMemoryCheckBox.selectedProperty();

		hBox.getChildren().add(zeroOnReadsFromUninitializedMemoryCheckBox);

		return hBox;
	}


	public SimulatorSettingDetails getResults()
	{
		String simulationSpeedMillisecondsCycle = Double.toString(simulationSpeedModel.getValue());
		String allowExecutionOfNonInstructionMemory =
				String.valueOf(allowExecutionOfNonInstructionMemoryModel.getValue());
		String assumeZeroOnReadsFromUninitializedMemory =
				String.valueOf(assumeZeroOnReadsFromUninitializedMemoryModel.getValue());


		return new SimulatorSettingDetails(simulationSpeedMillisecondsCycle, allowExecutionOfNonInstructionMemory,
										   assumeZeroOnReadsFromUninitializedMemory);
	}

	@Override
	public boolean isValid()
	{
		return true;
	}

	@Override
	public void submit()
	{
		//TODO simulator events
	}
}
