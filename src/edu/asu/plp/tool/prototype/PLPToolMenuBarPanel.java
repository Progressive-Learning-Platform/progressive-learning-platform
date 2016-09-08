package edu.asu.plp.tool.prototype;

import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.control.SeparatorMenuItem;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.scene.layout.BorderPane;

public class PLPToolMenuBarPanel extends BorderPane
{
	private Controller controller;
	
	public PLPToolMenuBarPanel(Controller controller)
	{
		this.controller = controller;
		MenuBar menuBar = new MenuBar();
		
		Menu file = createFileMenu();
		Menu edit = createEditMenu();
		Menu view = createViewMenu();
		Menu project = createProjectMenu();
		Menu tools = createToolsMenu();
		Menu simulation = createSimulationMenu();
		Menu help = createHelpMenu();
		
		menuBar.getMenus().addAll(file, edit, view, project, tools, simulation, help);
		this.setCenter(menuBar);
	}
	
	private Menu createToolsMenu()
	{
		Menu toolsMenu = new Menu("Tools");
		MenuItem itemOptions = new MenuItem("Options");
		itemOptions.setOnAction((e) -> controller.showOptionsMenu());
		
		Menu modules = new Menu("Modules");
		MenuItem itemModuleManager = new MenuItem("Module Manager...");
		itemModuleManager.setOnAction((e) -> controller.showModuleManager());
		
		MenuItem itemLoadJar = new MenuItem("Load Module JAR File...");
		itemLoadJar.setOnAction((e) -> controller.loadModule());
		
		MenuItem itemClearCache = new MenuItem("Clear Module Auto-Load Cache");
		itemClearCache.setOnAction((e) -> controller.clearModuleCache());
		
		MenuItem itemSerialTerminal = new MenuItem("Serial Terminal");
		itemSerialTerminal.setAccelerator(new KeyCodeCombination(KeyCode.T,
				KeyCombination.CONTROL_DOWN));
		// TODO: set action
		
		MenuItem itemNumConverter = new MenuItem("Number Converter");
		itemNumConverter.setAccelerator(new KeyCodeCombination(KeyCode.F12));
		itemNumConverter.setOnAction((e) -> controller.showNumberConverter());
		
		modules.getItems().addAll(itemModuleManager, itemLoadJar, itemClearCache);
		toolsMenu.getItems().addAll(itemOptions, modules, new SeparatorMenuItem(),
				itemSerialTerminal, itemNumConverter);
		
		return toolsMenu;
	}
	
	private Menu createSimulationMenu()
	{
		Menu simulationMenu = new Menu("Simulation");
		MenuItem itemStep = new MenuItem("Step");
		itemStep.setGraphic(new ImageView(new Image("toolbar_step.png")));
		itemStep.setAccelerator(new KeyCodeCombination(KeyCode.F5));
		itemStep.setOnAction((e) -> controller.stepSimulation());
		
		MenuItem itemEmulationWindow = new MenuItem("Emulation Window");
		itemEmulationWindow.setOnAction((e) -> controller.showEmulationWindow());
		
		MenuItem cpuViewWindow = new MenuItem("CPU View");
		cpuViewWindow.setOnAction((e) -> controller.openCpuViewWindow());
		
		MenuItem itemReset = new MenuItem("Reset");
		itemReset.setGraphic(new ImageView(new Image("toolbar_reset.png")));
		itemReset.setAccelerator(new KeyCodeCombination(KeyCode.F9));
		itemReset.setOnAction((e) -> controller.resetSimulation());
		
		MenuItem itemRun = new MenuItem("Run");
		itemRun.setAccelerator(new KeyCodeCombination(KeyCode.F7));
		itemRun.setOnAction((e) -> controller.runSimulation());
		
		Menu cyclesSteps = new Menu("Cycles/Steps");
		MenuItem itemOne = new MenuItem("1");
		itemOne.setAccelerator(new KeyCodeCombination(KeyCode.NUMPAD1,
				KeyCombination.ALT_DOWN));
		itemOne.setOnAction((event) -> controller.changeSimulationSpeed(1));
		
		MenuItem itemFive = new MenuItem("5");
		itemFive.setAccelerator(new KeyCodeCombination(KeyCode.NUMPAD2,
				KeyCombination.ALT_DOWN));
		itemFive.setOnAction((event) -> controller.changeSimulationSpeed(5));
		
		MenuItem itemTwenty = new MenuItem("20");
		itemTwenty.setAccelerator(new KeyCodeCombination(KeyCode.NUMPAD3,
				KeyCombination.ALT_DOWN));
		itemTwenty.setOnAction((event) -> controller.changeSimulationSpeed(20));
		
		MenuItem itemHundred = new MenuItem("100");
		itemHundred.setAccelerator(new KeyCodeCombination(KeyCode.NUMPAD4,
				KeyCombination.ALT_DOWN));
		itemHundred.setOnAction((event) -> controller.changeSimulationSpeed(100));
		
		MenuItem itemFiveThousand = new MenuItem("5000");
		itemFiveThousand.setAccelerator(new KeyCodeCombination(KeyCode.NUMPAD5,
				KeyCombination.ALT_DOWN));
		itemFiveThousand.setOnAction((event) -> controller.changeSimulationSpeed(5000));
		
		MenuItem itemClearBreakpoints = new MenuItem("Clear Breakpoints");
		itemClearBreakpoints.setAccelerator(new KeyCodeCombination(KeyCode.B,
				KeyCombination.CONTROL_DOWN));
		itemClearBreakpoints.setOnAction((e) -> controller.clearAllBreakpoints());
		
		Menu views = new Menu("Views");
		MenuItem itemCpuView = new MenuItem("CPU View");
		itemCpuView.setAccelerator(new KeyCodeCombination(KeyCode.C,
				KeyCombination.CONTROL_DOWN, KeyCombination.SHIFT_DOWN));
		// TODO: set action
		
		MenuItem itemCpuWindow = new MenuItem("Watcher Window");
		itemCpuWindow.setAccelerator(new KeyCodeCombination(KeyCode.W,
				KeyCombination.CONTROL_DOWN, KeyCombination.SHIFT_DOWN));
		itemCpuWindow.setOnAction((e) -> controller.showWatcherWindow());
		
		MenuItem itemSimControlWindow = new MenuItem("Simulation Control Window");
		itemSimControlWindow.setAccelerator(new KeyCodeCombination(KeyCode.R,
				KeyCombination.CONTROL_DOWN));
		// TODO: itemSimControlWindow.setOnAction(); {{what does this button do?}}
		
		Menu toolsSubMenu = new Menu("Tools");
		MenuItem itemioRegistry = new MenuItem("I/O Registry");
		itemioRegistry.setAccelerator(new KeyCodeCombination(KeyCode.R,
				KeyCombination.CONTROL_DOWN, KeyCombination.SHIFT_DOWN));
		// TODO: itemioRegistry.setOnAction(); {{what does this button do?}}
		
		MenuItem itemASMView = new MenuItem("ASM View");
		// TODO: itemASMView.setOnAction(); {{what does this button do?}}
		
		MenuItem itemCreateMemVis = new MenuItem("Create a PLP CPU Memory Visualizer");
		// TODO: itemCreateMemVis.setOnAction(); {{what does this button do?}}
		
		MenuItem itemRemoveMemVis = new MenuItem("Remove Memory Visualizers from Project");
		// TODO: itemRemoveMemVis.setOnAction(); {{what does this button do?}}
		
		MenuItem itemDisplayBus = new MenuItem("Display Bus Monitor Timing Diagram");
		// TODO: itemDisplayBus.setOnAction(); {{what does this button do?}}
		
		MenuItem itemExitSim = new MenuItem("ExitSimulation");
		itemExitSim.setAccelerator(new KeyCodeCombination(KeyCode.F11));
		itemExitSim.setOnAction((e) -> controller.stopSimulation());
		
		cyclesSteps.getItems().addAll(itemOne, itemFive, itemTwenty, itemHundred,
				itemFiveThousand);
		views.getItems().addAll(itemCpuView, itemCpuWindow, itemSimControlWindow);
		toolsSubMenu.getItems().addAll(itemioRegistry, itemASMView,
				new SeparatorMenuItem(), itemCreateMemVis, itemRemoveMemVis,
				itemDisplayBus);
		simulationMenu.getItems().addAll(itemEmulationWindow, cpuViewWindow, itemStep, itemReset, new SeparatorMenuItem(),
				itemRun, cyclesSteps, itemClearBreakpoints, new SeparatorMenuItem(),
				views, toolsSubMenu, new SeparatorMenuItem(), itemExitSim);
		
		return simulationMenu;
	}
	
	private Menu createHelpMenu()
	{
		Menu helpMenu = new Menu("Help");
		MenuItem itemQuickRef = new MenuItem("Quick Reference");
		itemQuickRef.setAccelerator(new KeyCodeCombination(KeyCode.F1));
		itemQuickRef.setOnAction((e) -> controller.showQuickReference());
		
		MenuItem itemOnlineManual = new MenuItem("Online Manual");
		itemOnlineManual.setOnAction((e) -> controller.showOnlineManual());
		
		MenuItem itemReportIssue = new MenuItem("Report Issue (Requires Google Account");
		itemReportIssue.setOnAction((e) -> controller.reportIssue());
		
		MenuItem itemIssuesPage = new MenuItem("Open Issues Page");
		itemIssuesPage.setOnAction((e) -> controller.showIssuesPage());
		
		MenuItem itemAboutPLP = new MenuItem("About PLP Tool...");		
		itemAboutPLP.setGraphic(new ImageView(new Image("toolbar_new.png")));
		itemAboutPLP.setOnAction((e) -> controller.showAboutPLPTool());
		
		MenuItem itemSWLicense = new MenuItem("Third Party Software License");
		itemSWLicense.setOnAction((e) -> controller.showThirdPartyLicenses());
		
		helpMenu.getItems().addAll(itemQuickRef, itemOnlineManual,
				new SeparatorMenuItem(), itemReportIssue, itemIssuesPage,
				new SeparatorMenuItem(), itemAboutPLP, itemSWLicense);
		
		return helpMenu;
	}
	
	private Menu createProjectMenu()
	{
		Menu projectMenu = new Menu("Project");
		MenuItem itemAssemble = new MenuItem("Assemble");
		itemAssemble.setGraphic(new ImageView(new Image("toolbar_assemble.png")));
		itemAssemble.setAccelerator(new KeyCodeCombination(KeyCode.F2));
		itemAssemble.setOnAction((e) -> controller.assembleActiveProject());
		
		MenuItem itemSimulate = new MenuItem("Simulate");
		itemSimulate.setGraphic(new ImageView(new Image("toolbar_simulate.png")));
		itemSimulate.setAccelerator(new KeyCodeCombination(KeyCode.F3));
		itemSimulate.setOnAction((e) -> controller.simulateActiveProject());
		
		MenuItem itemPLPBoard = new MenuItem("Program PLP Board...");
		itemPLPBoard.setGraphic(new ImageView(new Image("toolbar_program.png")));
		itemPLPBoard.setAccelerator(new KeyCodeCombination(KeyCode.F4,
				KeyCombination.SHIFT_DOWN));
		itemPLPBoard.setOnAction((e) -> controller.downloadActiveProjectToBoard());
		
		MenuItem itemQuickProgram = new MenuItem("Quick Program");
		itemQuickProgram.setAccelerator(new KeyCodeCombination(KeyCode.F4));
		// TODO: itemQuickProgram.setOnAction(); {{what does this button do?}}
		
		MenuItem itemNewASM = new MenuItem("New ASM File...");
		itemNewASM.setOnAction((e) -> controller.createNewASM());
		
		MenuItem itemImportASM = new MenuItem("Import ASM File...");
		itemImportASM.setOnAction((e) -> controller.importASM());
		
		MenuItem itemExportASM = new MenuItem("Export Selected ASM File...");
		itemExportASM.setOnAction((e) -> controller.exportASM());
		
		MenuItem itemRemoveASM = new MenuItem("Remove Selected ASM File from Project");
		itemRemoveASM.setAccelerator(new KeyCodeCombination(KeyCode.E,
				KeyCombination.CONTROL_DOWN));
		itemRemoveASM.setOnAction((e) -> controller.removeASM());
		
		MenuItem itemCurrentAsMain = new MenuItem("Set Current Open File as Main Program");
		itemCurrentAsMain.setOnAction((e) -> controller.setMainASMFile());
		
		projectMenu.getItems().addAll(itemAssemble, itemSimulate, itemPLPBoard,
				itemQuickProgram, new SeparatorMenuItem(), itemNewASM, itemImportASM,
				itemExportASM, itemRemoveASM, new SeparatorMenuItem(), itemCurrentAsMain);
		
		return projectMenu;
	}
	
	private Menu createViewMenu()
	{
		Menu viewMenu = new Menu("View");
		
		MenuItem itemClearOutput = new MenuItem("Clear Output Pane");
		itemClearOutput.setAccelerator(new KeyCodeCombination(KeyCode.D,
				KeyCombination.CONTROL_DOWN));
		itemClearOutput.setOnAction((e) -> controller.clearConsole());
		
		viewMenu.getItems().addAll(itemClearOutput);
		return viewMenu;
	}
	
	private Menu createEditMenu()
	{
		Menu editMenu = new Menu("Edit");
		MenuItem itemCopy = new MenuItem("Copy");
		itemCopy.setAccelerator(new KeyCodeCombination(KeyCode.C,
				KeyCombination.CONTROL_DOWN));
		itemCopy.setOnAction((e) -> controller.editCopy());
		
		MenuItem itemCut = new MenuItem("Cut");		
		itemCut.setAccelerator(new KeyCodeCombination(KeyCode.X,
				KeyCombination.CONTROL_DOWN));
		itemCut.setOnAction((e) -> controller.editCut());
		
		MenuItem itemPaste = new MenuItem("Paste");
		itemPaste.setAccelerator(new KeyCodeCombination(KeyCode.V,
				KeyCombination.CONTROL_DOWN));
		itemPaste.setOnAction((e) -> controller.editPaste());
		
		MenuItem itemFandR = new MenuItem("Find and Replace");
		itemFandR.setAccelerator(new KeyCodeCombination(KeyCode.F,
				KeyCombination.CONTROL_DOWN));
		itemFandR.setOnAction((e) -> controller.editFandR());
		
		MenuItem itemUndo = new MenuItem("Undo");
		itemUndo.setAccelerator(new KeyCodeCombination(KeyCode.Z,
				KeyCombination.CONTROL_DOWN));
		itemUndo.setOnAction((e) -> controller.editUndo());
		
		MenuItem itemRedo = new MenuItem("Redo");
		itemRedo.setAccelerator(new KeyCodeCombination(KeyCode.Y,
				KeyCombination.CONTROL_DOWN));
		itemRedo.setOnAction((e) -> controller.editRedo());
		
		editMenu.getItems().addAll(itemCopy, itemCut, itemPaste, new SeparatorMenuItem(),
				itemFandR, new SeparatorMenuItem(), itemUndo, itemRedo);
		
		return editMenu;
	}
	
	private Menu createFileMenu()
	{
		Menu fileMenu = new Menu("File");
		MenuItem itemNew = new MenuItem("New PLP Project");
		itemNew.setGraphic(new ImageView(new Image("toolbar_new.png")));
		itemNew.setAccelerator(new KeyCodeCombination(KeyCode.N,
				KeyCombination.CONTROL_DOWN));
		itemNew.setOnAction((e) -> controller.createNewProject());
		
		MenuItem itemNewASMFile = new MenuItem("New ASM File");
		itemNewASMFile.setGraphic(new ImageView(new Image("menu_new.png")));
		itemNewASMFile.setOnAction((e) -> controller.createNewASM());
		
		MenuItem itemOpen = new MenuItem("Open Project");
		itemOpen.setGraphic(new ImageView(new Image("toolbar_open.png")));
		itemOpen.setAccelerator(new KeyCodeCombination(KeyCode.O,
				KeyCombination.CONTROL_DOWN));
		itemOpen.setOnAction((e) -> controller.openProject());
		
		MenuItem itemSave = new MenuItem("Save");
		itemSave.setGraphic(new ImageView(new Image("toolbar_save.png")));
		itemSave.setAccelerator(new KeyCodeCombination(KeyCode.S,
				KeyCombination.CONTROL_DOWN));
		itemSave.setOnAction((e) -> controller.saveActiveProject());
		
		MenuItem itemSaveAs = new MenuItem("Save As");
		itemSaveAs.setAccelerator(new KeyCodeCombination(KeyCode.A,
				KeyCombination.CONTROL_DOWN, KeyCombination.SHIFT_DOWN));
		itemSaveAs.setOnAction((e) -> controller.saveActiveProjectAs());
		
		MenuItem itemSaveAll = new MenuItem("Save All");
		itemSaveAll.setAccelerator(new KeyCodeCombination(KeyCode.A,
				KeyCombination.CONTROL_DOWN, KeyCombination.SHIFT_DOWN));
		itemSaveAll.setOnAction((e) -> controller.saveAll());
		
		MenuItem itemPrint = new MenuItem("Print");
		itemPrint.setAccelerator(new KeyCodeCombination(KeyCode.P,
				KeyCombination.CONTROL_DOWN));
		itemPrint.setOnAction((e) -> controller.printActiveFile());
		
		MenuItem itemExit = new MenuItem("Exit");
		itemExit.setAccelerator(new KeyCodeCombination(KeyCode.Q,
				KeyCombination.CONTROL_DOWN));
		itemExit.setOnAction((e) -> controller.exit());
		
		fileMenu.getItems().addAll(itemNew, itemNewASMFile, new SeparatorMenuItem(), itemOpen, itemSave,
				itemSaveAs, new SeparatorMenuItem(), itemPrint, new SeparatorMenuItem(),
				itemExit);
		
		return fileMenu;
	}
}
