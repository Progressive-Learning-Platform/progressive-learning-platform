package edu.asu.plp.tool.prototype;

import java.io.File;
import java.nio.file.Path;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.stage.DirectoryChooser;
import javafx.stage.Stage;
import moore.util.Subroutine;
import edu.asu.plp.tool.prototype.model.Project;
import edu.asu.plp.tool.prototype.util.Dialogues;

/**
 * An FX Panel to allow users to enter details for a new {@link Project}.
 * <p>
 * With this pane, users enter the details for the new ASMFile, specified by
 * {@link ProjectCreationDetails}, and finish by pressing a "Create" button.
 * <p>
 * When this button is pressed, this class will call a method to create a project,
 * depending on the type of project selected by the user. Project types and their
 * corresponding create methods can be dynamically added using
 * {@link #addProjectType(String, Consumer)}
 * <p>
 * Similar to {@link ASMCreationPanel}, this panel also specifies a
 * {@link #finallyOperation} to be called after the project creation method has completed.
 * Note that this method does not need to be specified, and will not be executed if it is
 * null. However, it is typical for this panel to be the sole element of a {@link Stage}
 * or other popup window, and in this case, the {@link #finallyOperation} can (and should)
 * be used to close the window on completion.
 * 
 * @author Moore, Zachary
 *
 */
public class ProjectCreationPanel extends BorderPane
{
	/** Input field for the name of the new project */
	private TextField projectNameField;
	
	/**
	 * Input field for the name of a new asm file to serve as the main source file for the
	 * new project
	 */
	private TextField mainSourceFileNameField;
	
	/**
	 * Input field to specify the location on disk where the project will be stored. This
	 * will typically be a directory, but may be a file, depending on the user-specified
	 * handler
	 */
	private TextField projectLocationField;
	
	/**
	 * Dropdown of all valid project types, specified by
	 * {@link #addProjectType(String, Consumer)}
	 */
	private ComboBox<String> projectTypeDropdown;
	
	/** Routine to be performed after a project is created (usually to close the panel) */
	private Subroutine finallyOperation;
	
	/** Mapping of project types to the project creation handler for that type */
	private Map<String, Consumer<ProjectCreationDetails>> projectCreationHandlers;
	
	/**
	 * Instantiates a {@link ProjectCreationPanel} with no specified project types.
	 * <p>
	 * It is necessary to specify one or more project types using
	 * {@link #addProjectType(String, Consumer)}
	 */
	public ProjectCreationPanel()
	{
		// TODO: add a default project creation handler, and force it to be specified in
		// this constructor
		this.projectCreationHandlers = new HashMap<>();
		this.setPadding(new Insets(20));
		GridPane grid = new GridPane();
		HBox buttons = new HBox(10);
		grid.setHgap(10);
		grid.setVgap(30);
		grid.setPadding(new Insets(10, 10, 10, 10));
		
		Label projectNameLabel = new Label();
		projectNameLabel.setText("Project Name: ");
		projectNameLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		projectNameField = new TextField();
		projectNameField.setText("Project Name");
		projectNameField.requestFocus();
		projectNameField.setPrefWidth(200);
		
		Label mainSourceFileNameLabel = new Label();
		mainSourceFileNameLabel.setText("File Name: ");
		mainSourceFileNameLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		mainSourceFileNameField = new TextField();
		mainSourceFileNameField.setText("Main.asm");
		mainSourceFileNameField.setPrefWidth(200);
		
		Label projectLocationLabel = new Label();
		projectLocationLabel.setText("Location: ");
		projectLocationLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		projectLocationField = new TextField();
		projectLocationField.setPrefWidth(200);
		
		Button browseLocationButton = new Button();
		browseLocationButton.setText("Browse");
		browseLocationButton.setOnAction(this::onBrowseLocation);
		
		Label projectTypeLabel = new Label();
		projectTypeLabel.setText("Targetted ISA: ");
		projectTypeLabel.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		projectTypeDropdown = new ComboBox<>();
		
		Button createProject = new Button("Create Project");
		createProject.setOnAction(this::onCreateProjectClicked);
		
		createProject.setDefaultButton(true);
		Button cancelCreate = new Button("Cancel");
		cancelCreate.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent e)
			{
				Stage stage = (Stage) cancelCreate.getScene().getWindow();
				stage.close();
			}
		});
		
		grid.add(projectNameLabel, 0, 0);
		grid.add(projectNameField, 1, 0);
		grid.add(mainSourceFileNameLabel, 0, 1);
		grid.add(mainSourceFileNameField, 1, 1);
		grid.add(projectLocationLabel, 0, 2);
		grid.add(projectLocationField, 1, 2);
		grid.add(browseLocationButton, 2, 2);
		grid.add(projectTypeLabel, 0, 3);
		grid.add(projectTypeDropdown, 1, 3);
		
		this.setCenter(grid);
		
		buttons.getChildren().addAll(createProject, cancelCreate);
		buttons.setAlignment(Pos.BASELINE_RIGHT);
		this.setBottom(buttons);
	}
	
	/**
	 * Adds a project type and creation handler to this panel. The specified type will be
	 * displayed to the user in the {@link #projectTypeDropdown}, which is a
	 * {@link ComboBox}.
	 * 
	 * @param name
	 *            The name of the project type, to be displayed in the {@link ComboBox}
	 * @param handler
	 *            The method to be called when "create" is pressed for a project of this
	 *            type. This method should create a new project of type "name"
	 */
	public void addProjectType(String name, Consumer<ProjectCreationDetails> handler)
	{
		projectCreationHandlers.put(name, handler);
		projectTypeDropdown.getItems().add(name);
	}
	
	public void setSelectedType(String type)
	{
		projectTypeDropdown.setValue(type);
	}
	
	public void setFinallyOperation(Subroutine finallyOperation)
	{
		this.finallyOperation = finallyOperation;
	}
	
	private void onBrowseLocation(ActionEvent event)
	{
		DirectoryChooser directoryChooser = new DirectoryChooser();
		directoryChooser.setTitle("Choose Project Location");
		File file = directoryChooser.showDialog(null);
		if (file != null)
		{
			Path directoryPath = file.toPath();
			String newProjectName = projectNameField.getText();
			Path targetProjectPath = directoryPath.resolve(newProjectName);
			String targetLocation = targetProjectPath.toString();
			projectLocationField.setText(targetLocation);
		}
	}
	
	private void onCreateProjectClicked(ActionEvent event)
	{
		ProjectCreationDetails details = extractDetailsFromGUI();
		boolean isValid = validateDefaultProjectDetails(details);
		if (isValid)
		{
			Consumer<ProjectCreationDetails> createFunction;
			String selectedType = details.getProjectType();
			createFunction = projectCreationHandlers.get(selectedType);
			
			if (createFunction == null)
				throw new IllegalStateException("Type defined without handler");
			else
				createFunction.accept(details);
			
			if (finallyOperation != null)
				finallyOperation.perform();
		}
	}
	
	private ProjectCreationDetails extractDetailsFromGUI()
	{
		String name = projectNameField.getText();
		String mainSource = mainSourceFileNameField.getText();
		String location = projectLocationField.getText();
		String type = projectTypeDropdown.getValue();
		
		return new ProjectCreationDetails(name, mainSource, location, type);
	}
	
	private boolean validateDefaultProjectDetails(ProjectCreationDetails details)
	{
		String projectName = details.getProjectName();
		String fileName = details.getMainSourceFileName();
		String projectLocation = details.getProjectLocation();
		File projectDirectory = details.getProjectDirectory();
		
		if (projectName == null || projectName.trim().isEmpty())
		{
			Dialogues.showInfoDialogue("You entered an invalid Project Name");
		}
		else if (fileName == null || fileName.trim().isEmpty())
		{
			Dialogues.showInfoDialogue("You entered an invalid File Name");
		}
		else if (projectLocation == null || projectLocation.trim().isEmpty())
		{
			Dialogues.showInfoDialogue("You entered an invalid Project Location");
		}
		else if (projectDirectory.exists())
		{
			Dialogues.showInfoDialogue("This Project Already Exists");
		}
		else
		{
			return true;
		}
		
		return false;
	}
}
