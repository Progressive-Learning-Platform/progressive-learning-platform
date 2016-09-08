package edu.asu.plp.tool.prototype;

public interface Controller
{
	// Project Management
	void createNewProject();
	
	void openProject();
	
	void saveActiveProject();
	
	void saveActiveProjectAs();
	
	void printActiveFile();
	
	void createNewASM();
	
	void importASM();
	
	void exportASM();
	
	void removeASM();
	
	void setMainASMFile();
	
	// Application Controls
	void exit();
	
	void reportIssue();
	
	void showIssuesPage();
	
	void showOptionsMenu();
	
	void showAboutPLPTool();
	
	void showThirdPartyLicenses();
	
	// Application Controls :: View Manipulation
	void clearConsole();
	
	void clearAllBreakpoints();
	
	// Utilities
	void showNumberConverter();
	
	// Backend Interaction
	void assembleActiveProject();
	
	void simulateActiveProject();
	
	void downloadActiveProjectToBoard();
	
	void stepSimulation();
	
	void triggerSimulationInterrupt();
	
	void resetSimulation();
	
	void runSimulation();
	
	void changeSimulationSpeed(int requestedSpeed);
	
	void stopSimulation();
	
	// General ISA Emulation Views
	void showWatcherWindow();
	
	// Module Management
	void loadModule();
	
	void clearModuleCache();
	
	void showModuleManager();
	
	// Language References
	void showQuickReference();
	
	void showOnlineManual();
	
	void saveAll();
	
	void showEmulationWindow();
	
	void openCpuViewWindow();
	
	void editCopy();
	
	void editCut();
	
	void editPaste();
	
	void editFandR();	
  
	void editUndo();
	
	void editRedo();	
	
}
