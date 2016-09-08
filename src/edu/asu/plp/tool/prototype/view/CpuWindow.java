package edu.asu.plp.tool.prototype.view;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.RadioButton;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;

public class CpuWindow extends BorderPane
{
	public class RegisterRow
	{
		private StringProperty registerName;
		private IntegerProperty content;
		private TextField editContent;
		
		public RegisterRow(String register, int content, TextField editContent)
		{
			this.registerName = new SimpleStringProperty(register);
			this.content = new SimpleIntegerProperty(content);
			this.editContent = new TextField();
			
		}
		
		public RegisterRow(String register, IntegerProperty content,
				TextField editContent)
		{
			this.registerName = new SimpleStringProperty(register);
			this.content = content;
			this.editContent = new TextField();
		}
		
		public String getRegister()
		{
			return registerName.get();
		}
		
		public void setRegister(String register)
		{
			registerName.set(register);
		}
		
		public String getContent()
		{
			return "0x" + Integer.toString(content.get(), 16).toUpperCase();
		}
		
		public void setContent(int content)
		{
			this.content.set(content);
		}
		
		public void setContent(String content)
		{
			int oldValue = this.content.get();
			try
			{
				setContent(Integer.parseInt(content));
			}
			catch (Exception e)
			{
				setContent(oldValue);
			}
		}
		
		public TextField getEditContent()
		{
			return editContent;
		}
		
		public void setEditContent(TextField editContent)
		{
			this.editContent = editContent;
		}
	}
	
	public class DisassemblyRow
	{
		private StringProperty pc;
		private CheckBox checkBox;
		private IntegerProperty address;
		private StringProperty instructionHex;
		private StringProperty instructionStatement;
		
		public DisassemblyRow(String pc, CheckBox checkBox, int address,
				String instructionHex, String instructionStatement)
		{
			this.pc = new SimpleStringProperty(pc);
			this.checkBox = new CheckBox();
			this.address = new SimpleIntegerProperty(address);
			this.instructionHex = new SimpleStringProperty(instructionHex);
			this.instructionStatement = new SimpleStringProperty(instructionStatement);
		}
		
		public DisassemblyRow(String pc, CheckBox checkBox, IntegerProperty address,
				String instructionHex, String instructionStatement)
		{
			this.pc = new SimpleStringProperty(pc);
			this.checkBox = new CheckBox();
			this.address = address;
			this.instructionHex = new SimpleStringProperty(instructionHex);
			this.instructionStatement = new SimpleStringProperty(instructionStatement);
		}
		
		public String getPc()
		{
			return pc.get();
		}
		
		public void setPc(String pc)
		{
			this.pc.set(pc);
		}
		
		public CheckBox getCheckBox()
		{
			return checkBox;
		}
		
		public void setCheckBox(CheckBox checkBox)
		{
			this.checkBox = checkBox;
		}
		
		public String getAddress()
		{
			return Integer.toString(address.get());
		}
		
		public void setAddress(int address)
		{
			this.address.set(address);
		}
		
		public String getInstructionHex()
		{
			return instructionHex.get();
		}
		
		public void setInstructionHex(String instructionHex)
		{
			this.instructionHex.set(instructionHex);
		}
		
		public String getInstructionStatement()
		{
			return instructionStatement.get();
		}
		
		public void setInstructionStatement(String instructionStatement)
		{
			this.instructionStatement.set(instructionStatement);
		}
		
	}
	
	public class MemoryMapRow
	{
		private int index;
		private String moduleName;
		private IntegerProperty startAddress;
		private IntegerProperty endAddress;
		private CheckBox enable;
		
		public MemoryMapRow(int index, String moduleName, int startAddress,
				int endAddress, CheckBox enable)
		{
			this.index = index;
			this.moduleName = moduleName;
			this.startAddress = new SimpleIntegerProperty(startAddress);
			this.endAddress = new SimpleIntegerProperty(endAddress);
			this.enable = new CheckBox();
		}
		
		public MemoryMapRow(int index, String moduleName, IntegerProperty startAddress,
				IntegerProperty endAddress, CheckBox enable)
		{
			this.index = index;
			this.moduleName = moduleName;
			this.startAddress = startAddress;
			this.endAddress = endAddress;
			this.enable = new CheckBox();
		}
		
		public void setIndex(int index)
		{
			this.index = index;
		}
		
		public String getIndex()
		{
			return Integer.toString(index);
		}
		
		public void setModuleName(String moduleName)
		{
			this.moduleName = moduleName;
		}
		
		public String getModuleName()
		{
			return moduleName;
		}
		
		public void setStartAddress(int startAddress)
		{
			this.startAddress.set(startAddress);
		}
		
		public String getStartAddress()
		{
			return "0x" + Integer.toString(startAddress.get(), 16).toUpperCase();
		}
		
		public void setEndAddress(int endAddress)
		{
			this.endAddress.set(endAddress);
		}
		
		public String getEndAddress()
		{
			return "0x" + Integer.toString(endAddress.get(), 16).toUpperCase();
		}
		
		public CheckBox getEnable()
		{
			return enable;
		}
		
		public void setEnable(CheckBox enable)
		{
			this.enable = enable;
		}
	}
	
	private ObservableList<RegisterRow> registers;
	private ObservableList<DisassemblyRow> disassembly;
	private ObservableList<MemoryMapRow> memoryMap;
	private String[] registerNames = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2",
			"$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
			"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t9", "$i0", "$i1",
			"$iv", "$sp", "$sp", "$ir", "$ra" };
	private String[] moduleNames = { "Interrup Controller", "Interrupt Button",
			"MemModule", "MemModule", "LEDArray", "Switches", "PLPID", "Timer",
			"Seven Segments", "UART", "VGA", "GPIO" };
			
	public CpuWindow()
	{
		TabPane cpuWindowTabs = new TabPane();
		registers = FXCollections.observableArrayList();
		disassembly = FXCollections.observableArrayList();
		memoryMap = FXCollections.observableArrayList();
		
		Tab registerFileTab = new Tab();
		registerFileTab.setText("Register");
		registerFileTab.setClosable(false);
		
		Tab disassemblyTab = new Tab();
		disassemblyTab.setText("Disassembly");
		disassemblyTab.setClosable(false);
		
		Tab memoryMapTab = new Tab();
		memoryMapTab.setText("About");
		memoryMapTab.setClosable(false);
		
		Tab simOptionsTab = new Tab();
		simOptionsTab.setText("Sim Options");
		simOptionsTab.setClosable(false);
		
		Tab consoleTab = new Tab();
		consoleTab.setText("Console");
		consoleTab.setClosable(false);
		
		cpuWindowTabs.getTabs().addAll(registerFileTab, disassemblyTab, memoryMapTab,
				simOptionsTab, consoleTab);
				
		TableView<RegisterRow> registerTabContent = createRegisterTabContent();
		//TODO: Replace PlaceHolders with actual content from backend
		for (int i = 0; i < 32; i++)
		{
			registers.add(new RegisterRow(i + ": " + registerNames[i], i * 250,
					new TextField()));
		}
		registerFileTab.setContent(registerTabContent);
		
		TableView<DisassemblyRow> disassemblyTabContent = createDisassemblyTabContent();
		//TODO: Replace PlaceHolders with actual content from backend
		disassembly.add(new DisassemblyRow("", new CheckBox(), 1000,
				"0x1234ABCD", "li $t1, $s1"));
		disassembly.add(new DisassemblyRow("", new CheckBox(), 2000,
				"0x00AD246C", "sw $s3, $t0"));
		disassemblyTab.setContent(disassemblyTabContent);
		
		TableView<MemoryMapRow> memoryMapTabContent = createMemoryMapTabContent();
		//TODO: Replace PlaceHolders with actual content from backend
		for(int i = 0; i < 12; i++)
		{
			memoryMap.add(new MemoryMapRow(i, moduleNames[i], i*13, i*20, new CheckBox()));
		}
		memoryMapTab.setContent(memoryMapTabContent);
		
		VBox simOptionsTabContent = createSimOptionsTabContent();
		simOptionsTab.setContent(simOptionsTabContent);
		
		VBox consoleTabContent = createConsoleTabContent();
		consoleTab.setContent(consoleTabContent);
		
		HBox topHBox = createTopBar();
		this.setTop(topHBox);
		this.setCenter(cpuWindowTabs);
		
	}
	
	private HBox createTopBar()
	{
		HBox counterHBox = new HBox();
		counterHBox.setPadding(new Insets(15, 15, 15, 15));
		counterHBox.setSpacing(10);
		
		Label programCounterLabel = new Label("Program Count");
		programCounterLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		TextField programCounterText = new TextField();
		programCounterText.setPrefWidth(50);
		
		Label nextInstructionLabel = new Label("Next Instruction");
		nextInstructionLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		TextField nextInstructionText = new TextField();
		nextInstructionText.setPrefWidth(200);
		counterHBox.getChildren().addAll(programCounterLabel, programCounterText,
				nextInstructionLabel, nextInstructionText);
				
		return counterHBox;
	}
	
	private TableView<RegisterRow> createRegisterTabContent()
	{
		TableView<RegisterRow> table = new TableView<>();
		table.setEditable(true);
		
		TableColumn<RegisterRow, String> nameColumn = new TableColumn<>("Register");
		nameColumn.setCellValueFactory(registerFactory("register"));
		setPercentSize(table, nameColumn, 1.0 / 3.0);
		table.getColumns().add(nameColumn);
		
		TableColumn<RegisterRow, String> contentsColumn = new TableColumn<>("Contents");
		contentsColumn.setCellValueFactory(registerFactory("content"));
		setPercentSize(table, contentsColumn, 1.0 / 3.0);
		table.getColumns().add(contentsColumn);
		
		TableColumn<RegisterRow, String> valueColumn = new TableColumn<>("Edit Contents");
		valueColumn.setCellValueFactory(registerFactory("editContent"));
		setPercentSize(table, valueColumn, 1.0 / 3.0);
		table.getColumns().add(valueColumn);
		
		table.setItems(registers);
		return table;
	}
	
	private TableView<DisassemblyRow> createDisassemblyTabContent()
	{
		TableView<DisassemblyRow> table = new TableView<>();
		table.setEditable(true);
		
		TableColumn<DisassemblyRow, String> pcColumn = new TableColumn<>("PC");
		pcColumn.setCellValueFactory(disassemblyFactory("pc"));
		setPercentSize(table, pcColumn, 1.0 / 5.0);
		table.getColumns().add(pcColumn);
		
		TableColumn<DisassemblyRow, CheckBox> breakpointColumn = new TableColumn<>(
				"Breakpoint");
		breakpointColumn.setCellValueFactory(disassemblyFactoryCheckBox("checkBox"));
		setPercentSize(table, breakpointColumn, 1.0 / 5.0);
		table.getColumns().add(breakpointColumn);
		
		TableColumn<DisassemblyRow, String> addressColumn = new TableColumn<>("Address");
		addressColumn.setCellValueFactory(disassemblyFactory("address"));
		setPercentSize(table, addressColumn, 1.0 / 5.0);
		table.getColumns().add(addressColumn);
		
		TableColumn<DisassemblyRow, String> instructionHexColumn = new TableColumn<>(
				"Instruction (Hex)");
		instructionHexColumn.setCellValueFactory(disassemblyFactory("instructionHex"));
		setPercentSize(table, instructionHexColumn, 1.0 / 5.0);
		table.getColumns().add(instructionHexColumn);
		
		TableColumn<DisassemblyRow, String> instructionStatementColumn = new TableColumn<>(
				"Instruction");
		instructionStatementColumn
				.setCellValueFactory(disassemblyFactory("instructionStatement"));
		setPercentSize(table, instructionStatementColumn, 1.0 / 5.0);
		table.getColumns().add(instructionStatementColumn);
		
		table.setItems(disassembly);
		return table;
	}
	
	private TableView<MemoryMapRow> createMemoryMapTabContent()
	{
		TableView<MemoryMapRow> table = new TableView<>();
		table.setEditable(true);
		
		TableColumn<MemoryMapRow, String> indexColumn = new TableColumn<>("Index");
		indexColumn.setCellValueFactory(memoryMapFactory("index"));
		setPercentSize(table, indexColumn, 1.0 / 5.0);
		table.getColumns().add(indexColumn);
		
		TableColumn<MemoryMapRow, String> moduleNameColumn = new TableColumn<>(
				"Module Name");
		moduleNameColumn.setCellValueFactory(memoryMapFactory("moduleName"));
		setPercentSize(table, moduleNameColumn, 1.0 / 5.0);
		table.getColumns().add(moduleNameColumn);
		
		TableColumn<MemoryMapRow, String> startAddressColumn = new TableColumn<>(
				"Start Address");
		startAddressColumn.setCellValueFactory(memoryMapFactory("startAddress"));
		setPercentSize(table, startAddressColumn, 1.0 / 5.0);
		table.getColumns().add(startAddressColumn);
		
		TableColumn<MemoryMapRow, String> endAddressColumn = new TableColumn<>(
				"Instruction");
		endAddressColumn.setCellValueFactory(memoryMapFactory("endAddress"));
		setPercentSize(table, endAddressColumn, 1.0 / 5.0);
		table.getColumns().add(endAddressColumn);
		
		TableColumn<MemoryMapRow, CheckBox> enableColumn = new TableColumn<>("Enabled");
		enableColumn.setCellValueFactory(memoryMapFactoryCheckBox("enable"));
		setPercentSize(table, enableColumn, 1.0 / 5.0);
		table.getColumns().add(enableColumn);
		
		table.setItems(memoryMap);
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
	
	private static PropertyValueFactory<DisassemblyRow, String> disassemblyFactory(
			String attribute)
	{
		return new PropertyValueFactory<DisassemblyRow, String>(attribute);
	}
	
	private static PropertyValueFactory<DisassemblyRow, CheckBox> disassemblyFactoryCheckBox(
			String attribute)
	{
		return new PropertyValueFactory<DisassemblyRow, CheckBox>(attribute);
	}
	
	private static PropertyValueFactory<MemoryMapRow, String> memoryMapFactory(
			String attribute)
	{
		return new PropertyValueFactory<MemoryMapRow, String>(attribute);
	}
	
	private static PropertyValueFactory<MemoryMapRow, CheckBox> memoryMapFactoryCheckBox(
			String attribute)
	{
		return new PropertyValueFactory<MemoryMapRow, CheckBox>(attribute);
	}
	
	private VBox createSimOptionsTabContent()
	{
		//TODO: Connect to backend
		VBox vbox = new VBox(20);
		vbox.setPadding(new Insets(25));
		
		Label archOptions = new Label();
		archOptions.setText("Architecture Options: ");
		archOptions.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		CheckBox exToEx = new CheckBox();
		exToEx.setText("EX -> EX Forwarding");
		exToEx.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		CheckBox memToEx = new CheckBox();
		memToEx.setText("MEM -> EX Forwarding");
		memToEx.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		CheckBox memToExForLoadWord = new CheckBox();
		memToExForLoadWord.setText("MEM -> EX Forwarding for Load Word - Use Hazard");
		memToExForLoadWord.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		Label branchPrediction = new Label();
		branchPrediction.setText("Branch Prediction: ");
		branchPrediction.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		RadioButton branchAlways = new RadioButton();
		branchAlways.setText("Branch Always");
		branchAlways.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		RadioButton branchNever = new RadioButton();
		branchNever.setText("Branch Never");
		branchNever.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		RadioButton last = new RadioButton();
		last.setText("Last");
		last.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		RadioButton random = new RadioButton();
		random.setText("Random");
		random.setFont(Font.font("Arial", FontWeight.NORMAL, 14));
		
		vbox.getChildren().addAll(archOptions, exToEx, memToEx, memToExForLoadWord, branchPrediction, branchAlways, branchNever, last, random);
		
		return vbox;
	}
	
	private VBox createConsoleTabContent()
	{
		//TODO: Connect to backend
		VBox vbox = new VBox(10);
		
		Label title = new Label();
		title.setText("Debug Console (PLPSimCL)");
		title.setFont(Font.font("Arial", FontWeight.NORMAL, 12));
		
		TextArea consoleTextArea = new TextArea();
		consoleTextArea.setMinHeight(400);
		
		HBox interactionLine = new HBox(15);
		
		TextField executeStatement = new TextField();
		executeStatement.setPrefWidth(750);
		
		Button executeButton = new Button("Execute");
		Button clearButton = new Button("Clear");
		
		interactionLine.getChildren().addAll(executeStatement, executeButton, clearButton);
		
		vbox.getChildren().addAll(title, consoleTextArea, interactionLine);
		
		return vbox;
		
		
		
		
	}
	
}
