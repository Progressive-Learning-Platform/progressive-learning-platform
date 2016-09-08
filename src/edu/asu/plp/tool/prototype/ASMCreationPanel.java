package edu.asu.plp.tool.prototype;

import java.util.function.Consumer;

import javafx.event.ActionEvent;
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
import javafx.stage.Stage;
import moore.util.Subroutine;
import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.prototype.util.Dialogues;

/**
 * An FX Panel to allow users to enter details for a new {@link ASMFile}.
 * <p>
 * With this pane, users enter the name and associated project for the new ASMFile, and
 * finish by pressing a "Create" button.
 * <p>
 * When this button is pressed, this class will call {@link #onCreateASM} followed by
 * {@link #finallyOperation}, which can both be dynamically set using
 * {@link #setOnCreateASM(Consumer)} and {@link #setFinallyOperation(Subroutine)}
 * respectively.
 * <p>
 * In order to enforce correctness, {@link #onCreateASM} must be declared on instantiation
 * of this class (using the {@link #ASMCreationPanel(Consumer)} constructor) and cannot be
 * null. Attempting to set {@link #onCreateASM} to null will result in an
 * {@link IllegalArgumentException}.
 * <p>
 * The {@link #finallyOperation}, on the other hand, does not need to be specified, and
 * will not be executed if it is null. However, it is typical for this panel to be the
 * sole element of a {@link Stage} or other popup window, and in this case, the
 * {@link #finallyOperation} can (and should) be used to close the window on completion.
 * 
 * @author Moore, Zachary
 *
 */
public class ASMCreationPanel extends BorderPane
{
	/** Field to enter the name of the new ASMFile */
	private TextField nameText;
	
	/** Field to enter the name of the project to which the new file will belong */
	private ComboBox<String> projectListDropdown;
	
	/** Routine to be performed after {@link #onCreateASM} (usually to close the panel) */
	private Subroutine finallyOperation;
	
	/** Method to call when "create" is clicked, given the fields of this form */
	private Consumer<ASMCreationDetails> onCreateASM;
	
	/**
	 * @param onCreateASM
	 *            Method to call when "create" is clicked. This parameter must be
	 *            non-null.
	 * 
	 * @throws IllegalArgumentException
	 *             if onCreateASM is null
	 */
	public ASMCreationPanel(Consumer<ASMCreationDetails> onCreateASM)
	{
		if (onCreateASM == null)
			throw new IllegalArgumentException("onCreateASM must be non-null");
		
		this.onCreateASM = onCreateASM;
		this.setPadding(new Insets(20));
		GridPane grid = new GridPane();
		HBox buttons = new HBox(10);
		grid.setHgap(10);
		grid.setVgap(30);
		grid.setPadding(new Insets(10, 10, 10, 10));
		
		Label ASMFileName = new Label();
		ASMFileName.setText("File Name: ");
		ASMFileName.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		nameText = new TextField();
		nameText.setText("");
		nameText.requestFocus();
		nameText.setPrefWidth(200);
		
		Label projectName = new Label();
		projectName.setText("Add to Project: ");
		projectName.setFont(Font.font("Arial", FontWeight.NORMAL, 16));
		
		projectListDropdown = new ComboBox<>();
		
		Button create = new Button();
		create.setText("Create");
		create.setOnAction(this::onCreateASMClicked);
		
		grid.add(ASMFileName, 0, 0);
		grid.add(nameText, 1, 0);
		grid.add(projectName, 0, 1);
		grid.add(projectListDropdown, 1, 1);
		
		this.setCenter(grid);
		buttons.getChildren().add(create);
		buttons.setAlignment(Pos.BASELINE_RIGHT);
		this.setBottom(buttons);
	}
	
	public void setProjectName(String projectName)
	{
		projectListDropdown.setValue(projectName);
	}
	
	public void addProjectName(String name)
	{
		projectListDropdown.getItems().add(name);
	}
	
	public void setFinallyOperation(Subroutine finallyOperation)
	{
		this.finallyOperation = finallyOperation;
	}
	
	public void setOnCreateASM(Consumer<ASMCreationDetails> onCreateASM)
	{
		this.onCreateASM = onCreateASM;
	}
	
	private void onCreateASMClicked(ActionEvent event)
	{
		ASMCreationDetails details = extractDetailsFromGUI();
		boolean isValid = validateDefaultFileDetails(details);
		if (isValid)
		{
			if (onCreateASM == null)
				throw new IllegalStateException("onCreateASM handler not defined");
			else
				onCreateASM.accept(details);
			
			if (finallyOperation != null)
				finallyOperation.perform();
		}
	}
	
	private ASMCreationDetails extractDetailsFromGUI()
	{
		String projectName = projectListDropdown.getValue();
		String fileName = nameText.getText();
		
		return new ASMCreationDetails(projectName, fileName);
	}
	
	private boolean validateDefaultFileDetails(ASMCreationDetails details)
	{
		String projectName = details.getProjectName();
		String fileName = details.getFileName();
		
		if (projectName == null || projectName.trim().isEmpty())
		{
			Dialogues.showInfoDialogue("You did not select a current Project");
		}
		else if (fileName == null || fileName.trim().isEmpty())
		{
			Dialogues.showInfoDialogue("You entered an invalid File Name");
		}
		else
		{
			return true;
		}
		
		return false;
	}
}
