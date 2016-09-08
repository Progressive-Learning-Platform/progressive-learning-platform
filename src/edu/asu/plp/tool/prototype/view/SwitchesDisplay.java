package edu.asu.plp.tool.prototype.view;

import javafx.beans.value.ObservableValue;
import javafx.scene.control.Button;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.text.Font;
import javafx.scene.text.TextAlignment;

public class SwitchesDisplay extends BorderPane
{
	private static final int DEFAULT_SIZE = 100;
	private static final int NUMBER_OF_SWITCHES = 8;
	private static final int FONT_SIZE = 30;
	private static final String FONT_NAME = "Arial";
	private static final Paint FONT_COLOR = Color.WHITE;
	private static final String ACTIVE_COLOR = "green";
	private static final String INACTIVE_COLOR = "black";
	
	private static class Switch extends Button
	{
		private boolean isActive;
	}
	
	/**
	 * The display (fx Button) and state (active or inactive) of each switch.
	 * <p>
	 * Each switch corresponds to its index in this array. For instance, switches[0] will return
	 * the display and state for switch0
	 * <p>
	 * Switches are displayed in order, from left to right
	 */
	private Switch[] switches;
	
	public SwitchesDisplay()
	{
		GridPane grid = new GridPane();
		switches = new Switch[NUMBER_OF_SWITCHES];
		for (int index = 0; index < NUMBER_OF_SWITCHES; index++)
		{
			Switch button = createSwitch(index);
			
			switches[index] = button;
			int position = index;
			grid.add(button, position, 0);
		}
		
		setCenter(grid);
		
		this.widthProperty().addListener(this::onSizeChange);
	}
	
	private void onSizeChange(ObservableValue<? extends Number> value, Number old,
			Number current)
	{
		int size = current.intValue() / NUMBER_OF_SWITCHES;
		resizeSwitches(size);
	}
	
	private void resizeSwitches(int size)
	{
		for (Switch button : switches)
		{
			button.setPrefHeight(size);
			button.setPrefWidth(size);
		}
	}
	
	private Switch createSwitch(int index)
	{
		Switch button = new Switch();
		button.setFont(new Font(FONT_NAME, FONT_SIZE));
		button.setTextAlignment(TextAlignment.CENTER);
		button.setTextFill(FONT_COLOR);
		button.setPrefHeight(DEFAULT_SIZE);
		button.setPrefWidth(DEFAULT_SIZE);
		
		String labelText = Integer.toString(index);
		button.setText(labelText);
		updateSwitchStyle(button);
		
		button.setOnAction((event) -> toggle(button));
		
		return button;
	}
	
	public void setSwitchState(int index, boolean isActive)
	{
		Switch button = switches[index];
		button.isActive = isActive;
		updateSwitchStyle(button);
	}
	
	public void toggleSwitchState(int index)
	{
		Switch button = switches[index];
		toggle(button);
	}
	
	private void toggle(Switch button)
	{
		button.isActive = !button.isActive;
		updateSwitchStyle(button);
	}
	
	private void updateSwitchStyle(Switch button)
	{
		String style = "-fx-border-color: white; -fx-text-align: center; -fx-background-color:";
		style += (button.isActive) ? ACTIVE_COLOR : INACTIVE_COLOR;
		button.setStyle(style);
	}
	
	/**
	 * Alias for {@link #isSwitchActive(int)}
	 * 
	 * @param switchIndex
	 *            Index of the Switch to retrieve the state of
	 * @return True if the Switch is active, false otherwise
	 */
	public boolean getSwitchState(int switchIndex)
	{
		return isSwitchActive(switchIndex);
	}
	
	public boolean isSwitchActive(int index)
	{
		Switch button = switches[index];
		return button.isActive;
	}
}
