package edu.asu.plp.tool.prototype.view;

import static edu.asu.plp.tool.prototype.util.IntegerUtils.smartParse;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.function.Function;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.beans.value.ChangeListener;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.RowConstraints;
import javafx.scene.layout.VBox;
import javafx.util.Pair;
import edu.asu.plp.tool.backend.plpisa.sim.MemoryModule32Bit;
import edu.asu.plp.tool.prototype.util.IntegerUtils;

public class WatcherWindow extends BorderPane
{
	private static final double CP_PADDING = 5;
	private static final double CP_SPACING = 5;
	
	private ObservableList<MemoryRow> memoryAddresses;
	private ObservableList<RegisterRow> registers;
	private Map<String, Function<Integer, String>> valueDisplayOptions;
	private ObjectProperty<Function<Integer, String>> registerDisplayFunction;
	private ObjectProperty<Function<Integer, String>> memoryDisplayFunction;
	private MemoryModule32Bit memory;
	
	public WatcherWindow(MemoryModule32Bit memory)
	{
		this.memory = memory;

		Function<Integer, String> defaultDisplay = (value) -> Integer.toString(value);
		registerDisplayFunction = new SimpleObjectProperty<>(defaultDisplay);
		memoryDisplayFunction = new SimpleObjectProperty<>(defaultDisplay);
		valueDisplayOptions = new LinkedHashMap<>();
		populateDisplayOptions();
		memoryAddresses = FXCollections.observableArrayList();
		registers = FXCollections.observableArrayList();
		
		// TODO: remove placeholder
		memoryAddresses.add(new MemoryRow(10025, 100000));
		registers.add(new RegisterRow("$t0", "$8", 100000));
		
		TableView<RegisterRow> watchedRegisters = createRegisterTable();
		TableView<MemoryRow> watchedAddresses = createMemoryTable();
		Node registerControlPanel = createRegisterControlPanel();
		Node memoryControlPanel = createMemoryControlPanel();

		registerDisplayFunction.addListener(updateOnChangeEvent(registers));
		memoryDisplayFunction.addListener(updateOnChangeEvent(memoryAddresses));
		
		GridPane center = new GridPane();
		center.add(watchedRegisters, 0, 0);
		center.add(registerControlPanel, 0, 1);
		center.add(watchedAddresses, 1, 0);
		center.add(memoryControlPanel, 1, 1);
		
		ColumnConstraints constraint = new ColumnConstraints();
		constraint.setPercentWidth(50);
		center.getColumnConstraints().add(constraint);
		constraint = new ColumnConstraints();
		constraint.setPercentWidth(50);
		center.getColumnConstraints().add(constraint);
		
		RowConstraints rowConstraint = new RowConstraints();
		rowConstraint.setPercentHeight(80);
		center.getRowConstraints().add(rowConstraint);
		rowConstraint = new RowConstraints();
		rowConstraint.setPercentHeight(20);
		center.getRowConstraints().add(rowConstraint);
		
		this.setCenter(center);
	}
	
	private <T, G> ChangeListener<G> updateOnChangeEvent(ObservableList<T> model)
	{
		return (value, current, old) -> {
			ObservableList<T> temp = FXCollections.observableArrayList();
			temp.addAll(model);
			model.removeAll(model);
			model.addAll(temp);
		};
	}
	
	private void populateDisplayOptions()
	{
		valueDisplayOptions.put("Decimal", (value) -> Integer.toString(value));
		valueDisplayOptions.put("Hex", (value) -> "0x" + Integer.toString(value, 16));
		valueDisplayOptions.put("Binary", (value) -> "0b" + Integer.toString(value, 2));
		valueDisplayOptions.put("Packed ASCII", IntegerUtils::toAsciiString);
	}
	
	private Node createRegisterControlPanel()
	{
		BorderPane registerPanel = new BorderPane();
		
		Label watchRegisterLabel = new Label("Watch Register: ");
		registerPanel.setLeft(watchRegisterLabel);
		setAlignment(watchRegisterLabel, Pos.CENTER);
		
		TextField registerNameField = new TextField();
		registerPanel.setCenter(registerNameField);
		setAlignment(registerNameField, Pos.CENTER);
		
		Button watchRegisterButton = new Button("Add");
		watchRegisterButton.setOnAction((event) -> watchRegister(registerNameField
				.getText()));
		registerPanel.setRight(watchRegisterButton);
		setAlignment(watchRegisterButton, Pos.CENTER);
		
		Pair<Node, ComboBox<String>> optionsRowPair = createDisplayOptionsRow();
		Node displayOptions = optionsRowPair.getKey();
		ComboBox<String> displayDropdown = optionsRowPair.getValue();
		displayDropdown.setOnAction((event) -> {
			String selection = displayDropdown.getSelectionModel().getSelectedItem();
			Function<Integer, String> function = valueDisplayOptions.get(selection);
			registerDisplayFunction.set(function);
		});
		
		VBox controlPanel = new VBox();
		controlPanel.getChildren().add(registerPanel);
		controlPanel.getChildren().add(displayOptions);
		controlPanel.setAlignment(Pos.CENTER);
		setAlignment(controlPanel, Pos.CENTER);
		controlPanel.setPadding(new Insets(CP_PADDING));
		controlPanel.setSpacing(CP_SPACING);
		
		return controlPanel;
	}
	
	private Node createMemoryControlPanel()
	{
		BorderPane addressPanel = new BorderPane();
		
		Label watchAddressLabel = new Label("Watch Address: ");
		addressPanel.setLeft(watchAddressLabel);
		setAlignment(watchAddressLabel, Pos.CENTER);
		
		TextField addressField = new TextField();
		addressPanel.setCenter(addressField);
		setAlignment(addressField, Pos.CENTER);
		
		Button watchAddressButton = new Button("Add");
		watchAddressButton.setOnAction((event) -> watchMemoryAddress(addressField
				.getText()));
		addressPanel.setRight(watchAddressButton);
		setAlignment(watchAddressButton, Pos.CENTER);
		
		BorderPane rangePanel = new BorderPane();
		
		Label watchRangeFromLabel = new Label("Watch Range From ");
		rangePanel.setLeft(watchRangeFromLabel);
		setAlignment(watchRangeFromLabel, Pos.CENTER);
		
		HBox inputBox = new HBox();
		
		TextField fromField = new TextField();
		inputBox.getChildren().add(fromField);
		fromField.setPrefWidth(Integer.MAX_VALUE);
		
		Label toLabel = new Label(" To ");
		toLabel.setMinSize(Label.USE_PREF_SIZE, Label.USE_PREF_SIZE);
		inputBox.getChildren().add(toLabel);
		inputBox.setAlignment(Pos.CENTER);
		
		TextField toField = new TextField();
		toField.setPrefWidth(Integer.MAX_VALUE);
		inputBox.getChildren().add(toField);
		
		rangePanel.setCenter(inputBox);
		setAlignment(inputBox, Pos.CENTER);
		
		Button watchRangeButton = new Button("Add");
		watchRangeButton.setOnAction((event) -> watchMemoryRange(fromField.getText(),
				toField.getText()));
		rangePanel.setRight(watchRangeButton);
		setAlignment(watchRangeButton, Pos.CENTER);
		
		Pair<Node, ComboBox<String>> optionsRowPair = createDisplayOptionsRow();
		Node displayOptions = optionsRowPair.getKey();
		ComboBox<String> displayDropdown = optionsRowPair.getValue();
		displayDropdown.setOnAction((event) -> {
			String selection = displayDropdown.getSelectionModel().getSelectedItem();
			Function<Integer, String> function = valueDisplayOptions.get(selection);
			memoryDisplayFunction.set(function);
		});
		
		VBox controlPanel = new VBox();
		controlPanel.getChildren().add(addressPanel);
		controlPanel.getChildren().add(rangePanel);
		controlPanel.getChildren().add(displayOptions);
		controlPanel.setAlignment(Pos.CENTER);
		setAlignment(controlPanel, Pos.CENTER);
		controlPanel.setPadding(new Insets(CP_PADDING));
		controlPanel.setSpacing(CP_SPACING);
		
		return controlPanel;
	}
	
	private Pair<Node, ComboBox<String>> createDisplayOptionsRow()
	{
		BorderPane rowDisplay = new BorderPane();
		
		Label label = new Label("Display values as: ");
		rowDisplay.setLeft(label);
		
		ComboBox<String> dropdown = createDisplayOptionsDropdown();
		dropdown.setPrefWidth(Integer.MAX_VALUE);
		rowDisplay.setCenter(dropdown);
		
		return new Pair<>(rowDisplay, dropdown);
	}
	
	private ComboBox<String> createDisplayOptionsDropdown()
	{
		ObservableList<String> options = FXCollections.observableArrayList();
		options.addAll(valueDisplayOptions.keySet());
		
		ComboBox<String> dropdown = new ComboBox<>(options);
		dropdown.getSelectionModel().select(0);
		return dropdown;
	}
	
	private void watchRegister(String registerName)
	{
		if (registerName.length() == 0)
			return;
		// The memory module is responsible for equating the names "0" "$0" and "$zero"
		if (!memory.hasRegister(registerName))
			throw new IllegalArgumentException("There isn't a register with the name "
					+ registerName);
		
		String id = memory.getRegisterID(registerName);
		IntegerProperty register = memory.getRegisterValueProperty(registerName);
		RegisterRow row = new RegisterRow(registerName, id, register);
		registers.add(row);
	}
	
	private void watchMemoryAddress(String string)
	{
		if (string.length() == 0)
			return;
		int address = IntegerUtils.smartParse(string);
		watchMemoryAddress(address);
	}
	
	private void watchMemoryAddress(int address)
	{
		memory.validateAddress(address);
		
		IntegerProperty value = memory.getMemoryValueProperty(address);
		MemoryRow row = new MemoryRow(address, value);
		memoryAddresses.add(row);
	}
	
	private void watchMemoryRange(String from, String to)
	{
		int fromAddress = IntegerUtils.smartParse(from);
		int toAddress = IntegerUtils.smartParse(to);

		memory.validateAddress(fromAddress);
		memory.validateAddress(toAddress);
		
		if (toAddress < fromAddress)
		{
			int temp = toAddress;
			toAddress = fromAddress;
			fromAddress = temp;
		}
		
		for (int address = fromAddress; address <= toAddress; address += 4)
		{
			watchMemoryAddress(address);
		}
	}
	
	private TableView<RegisterRow> createRegisterTable()
	{
		TableView<RegisterRow> table = new TableView<>();
		table.setEditable(true);
		
		TableColumn<RegisterRow, String> nameColumn = new TableColumn<>("Name");
		nameColumn.setCellValueFactory(registerFactory("registerName"));
		setPercentSize(table, nameColumn, 1.0 / 3.0);
		table.getColumns().add(nameColumn);
		
		TableColumn<RegisterRow, String> idColumn = new TableColumn<>("Register");
		idColumn.setCellValueFactory(registerFactory("registerID"));
		setPercentSize(table, idColumn, 1.0 / 3.0);
		table.getColumns().add(idColumn);
		
		TableColumn<RegisterRow, String> valueColumn = new TableColumn<>("Value");
		valueColumn.setCellValueFactory(registerFactory("value"));
		setPercentSize(table, valueColumn, 1.0 / 3.0);
		table.getColumns().add(valueColumn);
		
		table.setItems(registers);
		table.setMinHeight(80);
		return table;
	}
	
	private TableView<MemoryRow> createMemoryTable()
	{
		TableView<MemoryRow> table = new TableView<>();
		table.setEditable(true);
		
		TableColumn<MemoryRow, String> idColumn = new TableColumn<>("Address");
		idColumn.setCellValueFactory(memoryFactory("address"));
		setPercentSize(table, idColumn, 0.5);
		table.getColumns().add(idColumn);
		
		TableColumn<MemoryRow, String> valueColumn = new TableColumn<>("Value");
		valueColumn.setCellValueFactory(memoryFactory("value"));
		setPercentSize(table, valueColumn, 0.5);
		table.getColumns().add(valueColumn);
		
		table.setItems(memoryAddresses);
		table.setMinHeight(80);
		return table;
	}
	
	private void setPercentSize(TableView<?> parent, TableColumn<?, ?> column,
			double percent)
	{
		parent.widthProperty().addListener(
				(item, old, current) -> column.setPrefWidth((double) current * percent));
	}
	
	private static PropertyValueFactory<RegisterRow, String> registerFactory(
			String attribute)
	{
		return new PropertyValueFactory<RegisterRow, String>(attribute);
	}
	
	private static PropertyValueFactory<MemoryRow, String> memoryFactory(String attribute)
	{
		return new PropertyValueFactory<MemoryRow, String>(attribute);
	}
	
	public class ValueRow
	{
		private IntegerProperty value;
		ObjectProperty<Function<Integer, String>> displayFunctionProperty;
		
		public ValueRow(int value, ObjectProperty<Function<Integer, String>> function)
		{
			this.value = new SimpleIntegerProperty(value);
			this.displayFunctionProperty = function;
		}
		
		public ValueRow(IntegerProperty value, ObjectProperty<Function<Integer, String>> function)
		{
			this.value = value;
			this.displayFunctionProperty = function;
		}
		
		public String getValue()
		{
			Function<Integer, String> displayFunction = displayFunctionProperty.get();
			int intValue = value.get();
			return displayFunction.apply(intValue);
		}
		
		public void setValue(int value)
		{
			this.value.set(value);
		}
		
		public void setValue(String value)
		{
			int oldValue = this.value.get();
			try
			{
				setValue(smartParse(value));
			}
			catch (Exception e)
			{
				setValue(oldValue);
			}
		}
	}
	
	public class RegisterRow extends ValueRow
	{
		private StringProperty registerName;
		private StringProperty registerID;
		
		public RegisterRow(String name, String id, int value)
		{
			super(value, WatcherWindow.this.registerDisplayFunction);
			registerName = new SimpleStringProperty(name);
			registerID = new SimpleStringProperty(id);
		}
		
		public RegisterRow(String name, String id, IntegerProperty value)
		{
			super(value, WatcherWindow.this.registerDisplayFunction);
			registerName = new SimpleStringProperty(name);
			registerID = new SimpleStringProperty(id);
		}
		
		public String getRegisterName()
		{
			return registerName.get();
		}
		
		public void setRegisterName(String name)
		{
			registerName.set(name);
		}
		
		public String getRegisterID()
		{
			return registerID.get();
		}
		
		public void setRegisterID(String id)
		{
			registerID.set(id);
		}
	}
	
	public class MemoryRow extends ValueRow
	{
		private IntegerProperty address;
		
		public MemoryRow(int address, IntegerProperty value)
		{
			super(value, WatcherWindow.this.memoryDisplayFunction);
			this.address = new SimpleIntegerProperty(address);
		}
		
		public MemoryRow(int address, int value)
		{
			super(value, WatcherWindow.this.memoryDisplayFunction);
			this.address = new SimpleIntegerProperty(address);
		}
		
		public String getAddress()
		{
			return Integer.toString(address.get());
		}
		
		public void setAddress(int value)
		{
			this.address.set(value);
		}
		
		public void setAddress(String address)
		{
			int oldAddress = this.address.get();
			try
			{
				setAddress(smartParse(address));
			}
			catch (Exception e)
			{
				setAddress(oldAddress);
			}
		}
	}
}
