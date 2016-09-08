package edu.asu.plp.tool.prototype.view;

import javafx.beans.value.ObservableValue;
import javafx.scene.control.Label;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.text.Font;
import javafx.scene.text.TextAlignment;

public class LEDDisplay extends BorderPane
{
	private static final int DEFAULT_SIZE = 100;
	private static final int NUMBER_OF_LEDS = 8;
	private static final int FONT_SIZE = 30;
	private static final String FONT_NAME = "Arial";
	private static final Paint FONT_COLOR = Color.WHITE;
	private static final String LIT_COLOR = "green";
	private static final String UNLIT_COLOR = "black";
	
	private static class LED extends BorderPane
	{
		private boolean isLit;
	}
	
	/**
	 * The display (border pane) and state (on or off) of each LED.
	 * <p>
	 * Each led corresponds to index of this array. For instance, ledNodes[0] will return
	 * the display and state for LED0
	 * <p>
	 * Note that LEDs are displayed in reverse order (i.e. LED0 is on the far right of the
	 * display)
	 */
	private LED[] ledNodes;
	
	public LEDDisplay()
	{
		GridPane grid = new GridPane();
		ledNodes = new LED[NUMBER_OF_LEDS];
		for (int index = 0; index < NUMBER_OF_LEDS; index++)
		{
			LED led = createLED(index);
			
			ledNodes[index] = led;
			//int position = NUMBER_OF_LEDS - index - 1;
			int position = index;
			grid.add(led, position, 0);
		}
		
		setCenter(grid);
		
		this.widthProperty().addListener(this::onSizeChange);
	}
	
	private void onSizeChange(ObservableValue<? extends Number> value, Number old,
			Number current)
	{
		int size = current.intValue() / NUMBER_OF_LEDS;
		resizeLEDs(size);
	}
	
	private void resizeLEDs(int size)
	{
		for (BorderPane led : ledNodes)
		{
			led.setMinHeight(size);
			led.setPrefWidth(size);
		}
	}
	
	private LED createLED(int number)
	{
		String labelText = Integer.toString(number);
		Label ledLabel = new Label(labelText);
		ledLabel.setFont(new Font(FONT_NAME, FONT_SIZE));
		ledLabel.setTextAlignment(TextAlignment.CENTER);
		ledLabel.setTextFill(FONT_COLOR);
		
		LED led = new LED();
		led.setMinHeight(DEFAULT_SIZE);
		led.setPrefWidth(DEFAULT_SIZE);
		led.setCenter(ledLabel);
		updateLEDStyle(led);
		
		return led;
	}
	
	public void setLEDState(int index, boolean isLit)
	{
		LED led = ledNodes[index];
		led.isLit = isLit;
		updateLEDStyle(led);
	}
	
	private void updateLEDStyle(LED led)
	{
		String style = "-fx-border-color: white; -fx-text-align: center; -fx-background-color:";
		style += (led.isLit) ? LIT_COLOR : UNLIT_COLOR;
		led.setStyle(style);
	}
	
	/**
	 * Alias for {@link #isLEDLit(int)}
	 * 
	 * @param ledIndex
	 *            Index of the LED to retrieve the state of
	 * @return True if the LED is lit, false otherwise
	 */
	public boolean getLEDState(int ledIndex)
	{
		return isLEDLit(ledIndex);
	}
	
	public boolean isLEDLit(int index)
	{
		LED led = ledNodes[index];
		return led.isLit;
	}
	
	public void toggleLEDState(int ledIndex)
	{
		boolean newState = !getLEDState(ledIndex);
		setLEDState(ledIndex, newState);
	}
}
