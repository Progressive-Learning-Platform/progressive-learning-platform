package edu.asu.plp.tool.prototype;

import java.util.HashSet;
import java.util.Set;

import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.SplitPane;
import javafx.scene.effect.DropShadow;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import edu.asu.plp.tool.backend.plpisa.sim.PLPMemoryModule;
import edu.asu.plp.tool.prototype.view.LEDDisplay;
import edu.asu.plp.tool.prototype.view.SevenSegmentPanel;
import edu.asu.plp.tool.prototype.view.SwitchesDisplay;
import edu.asu.plp.tool.prototype.view.UARTPanel;
import edu.asu.plp.tool.prototype.view.WatcherWindow;

public class EmulationWindow extends BorderPane
{
	public EmulationWindow()
	{
		GridPane demoGrid = createDemo();
		HBox topBar = createTopBar();
		
		this.setTop(topBar);
		this.setCenter(demoGrid);
	}
	
	private GridPane createDemo()
	{
		GridPane grid = new GridPane();
		grid.setHgap(20);
		grid.setVgap(10);
		ColumnConstraints column1 = new ColumnConstraints();
		column1.setMinWidth(150);
		grid.getColumnConstraints().add(column1);
		
		VBox leftSide = new VBox(25);
		leftSide.setSpacing(10);
		VBox rightSide = new VBox(25);
		rightSide.setSpacing(10);
		VBox checkOptions = new VBox();
		checkOptions.setPadding(new Insets(10));
		checkOptions.setSpacing(8);
		
		DropShadow backgroundColor = new DropShadow();
		backgroundColor.setColor(Color.BLACK);
		
		LEDDisplay ledDisplay = new LEDDisplay();
		ledDisplay.setPadding(new Insets(10));
		ledDisplay.setStyle("-fx-background-color: grey;");
		
		SwitchesDisplay switchesDisplay = new SwitchesDisplay();
		switchesDisplay.setPadding(new Insets(10));
		switchesDisplay.setStyle("-fx-background-color: grey;");
		
		UARTPanel uartDisplay = new UARTPanel();
		uartDisplay.setPadding(new Insets(10));
		uartDisplay.setStyle("-fx-background-color: grey;");

		// TODO: pass active memory module to WatcherWindow
		WatcherWindow watcherWindowDisplay = new WatcherWindow(new PLPMemoryModule());
		watcherWindowDisplay.setPadding(new Insets(10));
		watcherWindowDisplay.setStyle("-fx-background-color: grey;");
		
		SevenSegmentPanel sevenSegDisplay = new SevenSegmentPanel();
		sevenSegDisplay.setStyle("-fx-background-color: grey;");
		
		Label ledLabel = label("LEDs");
		Label switchesLabel = label("Switches");
		Label uartLabel = label("UART");
		Label sevenSegLabel = label("Seven Segment Display");
		Label watcherWindowLabel = label("Watcher Window");
		
		leftSide.getChildren().addAll(watcherWindowLabel, watcherWindowDisplay);
		rightSide.getChildren()
				.addAll(sevenSegLabel, sevenSegDisplay, ledLabel,
						ledDisplay, switchesLabel, switchesDisplay, uartLabel, uartDisplay);
		
		Text title = new Text("Windows");
		title.setFont(Font.font("Arial", FontWeight.BOLD, 14));
		checkOptions.getChildren().add(title);
		
		CheckBox sevenSegCheckBox = new CheckBox("7 Segment Display");
		sevenSegCheckBox.setSelected(true);
		bindDisplaysToCheckBox(sevenSegCheckBox, sevenSegLabel, sevenSegDisplay);
		
		CheckBox ledCheckBox = new CheckBox("LED's");
		ledCheckBox.setSelected(true);
		bindDisplaysToCheckBox(ledCheckBox, ledLabel, ledDisplay);
		
		CheckBox uartCheckBox = new CheckBox("UART");
		uartCheckBox.setSelected(true);
		bindDisplaysToCheckBox(uartCheckBox, uartLabel, uartDisplay);
		
		CheckBox switchesCheckBox = new CheckBox("Switches");
		switchesCheckBox.setSelected(true);
		bindDisplaysToCheckBox(switchesCheckBox, switchesLabel, switchesDisplay);
		
		CheckBox watcherWindowCheckBox = new CheckBox("Watcher Window");
		watcherWindowCheckBox.setSelected(true);
		bindDisplaysToCheckBox(watcherWindowCheckBox, watcherWindowLabel, watcherWindowDisplay);
		
		checkOptions.getChildren().addAll(sevenSegCheckBox, ledCheckBox, uartCheckBox,
				switchesCheckBox, watcherWindowCheckBox);
		
		SplitPane splitPane = new SplitPane();
		splitPane.setStyle("-fx-box-border: transparent;");
		splitPane.setStyle("-fx-padding: 4 10 10 10;");
		Node divider = splitPane.lookup(".split-pane-divider");
		if (divider != null)
		{
			divider.setStyle("-fx-background-color: transparent;");
		}
		splitPane.getItems().addAll(leftSide, rightSide);
		
		grid.add(checkOptions, 0, 0);
		grid.add(splitPane, 1, 0);
		// grid.add(rightSide, 2, 0);
		
		return grid;
	}
	
	private Label label(String name)
	{
		Label label = new Label(name + ": ");
		label.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		return label;
	}

	public HBox createTopBar()
	{
		HBox hbox = new HBox();
		hbox.setPadding(new Insets(15, 15, 15, 15));
		hbox.setSpacing(10);
		hbox.setStyle("-fx-background-color: lightsteelblue;");
		ObservableList<Node> buttons = hbox.getChildren();
		Set<Node> buttonEffectsSet = new HashSet<>();
		
		Node runButton = new ImageView("toolbar_run.png");
		runButton.setOnMouseClicked((event) -> {
			// TODO: Attach to Backend
			});
		buttons.add(runButton);
		buttonEffectsSet.add(runButton);
		
		Node stepButton = new ImageView("toolbar_step.png");
		runButton.setOnMouseClicked((event) -> {
			// TODO: Attach to Backend
			});
		buttons.add(stepButton);
		buttonEffectsSet.add(stepButton);
		
		Node resetButton = new ImageView("toolbar_reset.png");
		runButton.setOnMouseClicked((event) -> {
			// TODO: Attach to Backend
			});
		buttons.add(resetButton);
		buttonEffectsSet.add(resetButton);
		
		buttonEffectsSet.forEach(EmulationWindow::setButtonEffect);
		
		Label cycleLabel = new Label();
		cycleLabel.setText("Cycle: ");
		cycleLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 18));
		buttons.add(cycleLabel);
		
		Label cycleLabelCount = new Label();
		cycleLabelCount.setText("0");
		cycleLabelCount.setFont(Font.font("Arial", FontWeight.NORMAL, 18));
		buttons.add(cycleLabelCount);
		
		Label stepLabel = new Label();
		stepLabel.setText("Step: ");
		stepLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 18));
		buttons.add(stepLabel);
		
		Label stepLabelCount = new Label();
		stepLabelCount.setText("0");
		stepLabelCount.setFont(Font.font("Arial", FontWeight.NORMAL, 18));
		buttons.add(stepLabelCount);
		
		Label simModeState = new Label();
		simModeState.setText("Sim Mode");
		simModeState.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		buttons.add(simModeState);
		
		Node simModeImage = new ImageView("sim_mode_on.png");
		buttons.add(simModeImage);
		
		return hbox;
		
	}
	
	private void bindDisplaysToCheckBox(CheckBox checkBox, Node... nodes)
	{
		for (Node node : nodes)
		{
			node.visibleProperty().bind(checkBox.selectedProperty());
			node.managedProperty().bind(checkBox.selectedProperty());
		}
	}
	
	private static void setButtonEffect(Node node)
	{
		DropShadow rollOverColor = new DropShadow();
		rollOverColor.setColor(Color.ORANGERED);
		DropShadow clickColor = new DropShadow();
		clickColor.setColor(Color.DARKBLUE);
		
		node.addEventHandler(MouseEvent.MOUSE_ENTERED,
				(event) -> node.setEffect(rollOverColor));
		
		// Removing the shadow when the mouse cursor is off
		node.addEventHandler(MouseEvent.MOUSE_EXITED, (event) -> node.setEffect(null));
		
		// Darken shadow on click
		node.addEventHandler(MouseEvent.MOUSE_PRESSED,
				(event) -> node.setEffect(clickColor));
		
		// Restore hover style on click end
		node.addEventHandler(MouseEvent.MOUSE_RELEASED,
				(event) -> node.setEffect(rollOverColor));
	}
}
