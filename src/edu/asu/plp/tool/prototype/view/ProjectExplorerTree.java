package edu.asu.plp.tool.prototype.view;

import edu.asu.plp.tool.backend.isa.ASMFile;
import edu.asu.plp.tool.prototype.model.Project;
import javafx.collections.ListChangeListener.Change;
import javafx.collections.ObservableList;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Background;
import javafx.scene.layout.BorderPane;
import javafx.util.Pair;

import java.util.function.Consumer;

/**
 * An FX view of a list of known projects and their files displayed as a tree. This class
 * only displays projects specified by a given project model (see
 * {@link #setProjectsModel(ObservableList)}) and is not responsible for adding or
 * removing projects from the model.
 * <p>
 * However, {@link ProjectExplorerTree} uses data binding such that any changes made to
 * the project model will be reflected in this view automatically.
 * 
 * @author Moore, Zachary
 *
 */
public class ProjectExplorerTree extends BorderPane
{
	public static final String DEFAULT_EMPTY_MESSAGE = "No projects are open!";
	
	/**
	 * The project model displayed by this tree. All projects in this list should be
	 * displayed by the tree in the same order, with the same file order within projects.
	 * <p>
	 * This list is backed externally, and will thus be modified externally.
	 */
	private ObservableList<? extends Project> projects;
	
	/** The view of this explorer */
	private TreeView<String> projectTreeDisplay;
	
	/**
	 * This Node is displayed when the tree is empty, instead of
	 * {@link #projectTreeDisplay}
	 */
	private Node emptyTreeDisplay;
	
	/**
	 * If a file item (not a project item) is double clicked in
	 * {@link #projectTreeDisplay}, this explorer should fire {@link #onFileDoubleClicked}
	 * <p>
	 * This is intended to be specified externally (not inside this class) using
	 * {@link #setOnFileDoubleClicked(Consumer)}
	 */
	private Consumer<ASMFile> onFileDoubleClicked;
	
	public ProjectExplorerTree(ObservableList<? extends Project> projectsModel)
	{
		assert projectsModel != null;
		emptyTreeDisplay = new Label(DEFAULT_EMPTY_MESSAGE);
		projectTreeDisplay = createEmptyRootedProjectTree();
		projectTreeDisplay.setOnMouseClicked(this::onTreeClick);
		setCenter(projectTreeDisplay);
		
		setProjectsModel(projectsModel);
	}
	
	/**
	 * Set the listener for when a file is double clicked in this view. If an item
	 * representing a file is double clicked, the specified {@link Consumer} will be
	 * invoked, passing the selected {@link ASMFile} as a parameter.
	 * <p>
	 * The specified listener will not be invoked if a file is single clicked, or if a
	 * <em>project</em> is double clicked.
	 * 
	 * @param onFileDoubleClicked
	 *            Listener to be called when a file item is double clicked
	 */
	public void setOnFileDoubleClicked(Consumer<ASMFile> onFileDoubleClicked)
	{
		this.onFileDoubleClicked = onFileDoubleClicked;
	}
	
	/**
	 * Set the project model and update this view to display the specified projects and
	 * their files.
	 * <p>
	 * This view will no longer monitor or update in response to the old model.
	 * 
	 * @param projectsModel
	 */
	public void setProjectsModel(ObservableList<? extends Project> projectsModel)
	{
		if (projects != null)
			this.projects.removeListener(this::projectListChanged);
		
		assert projectsModel != null;
		this.projects = projectsModel;
		this.projects.addListener(this::projectListChanged);
		
		this.projectTreeDisplay.getRoot().getChildren().clear();
		for (Project project : projectsModel)
			addProjectToTree(project);
		
		validateDisplay();
	}
	
	/**
	 * Specifies an active file to be focused (and highlighted) by this
	 * {@link ProjectExplorerTree}. This could be used, for instance, to emphasize a
	 * specific file during a tutorial, or if the file is selected in a different view -
	 * to synchronize views.
	 * <p>
	 * This method is typically used by a controller or driver.
	 * 
	 * @param file
	 *            The target to focus
	 */
	public void setActiveFile(ASMFile file)
	{
		TreeItem<String> fileNode = getFileNode(file);
		int focusIndex = getGlobalIndexOf(fileNode);
		projectTreeDisplay.getFocusModel().focus(focusIndex);
		projectTreeDisplay.getSelectionModel().select(focusIndex);
	}
	
	public Pair<Project, ASMFile> getActiveSelection()
	{
		TreeItem<String> selection = projectTreeDisplay.getSelectionModel()
				.getSelectedItem();
		
		if (selection != null)
		{
			TreeItem<String> parent = selection.getParent();
			
			if (parent != null && parent.getValue().length() > 0)
			{
				// Selection is a file
				Project project = lookupProjectByName(parent.getValue());
				for (ASMFile file : project)
				{
					if (file.getName().equals(selection.getValue()))
					{
						return new Pair<>(project, file);
					}
				}
				
				// If loop exits without return, then the tree state is invalid
				throw new IllegalStateException("Expected file {" + selection.getValue()
						+ "} to be a child of the project {" + project.getName()
						+ "} but no such file was found.");
			}
			else
			{
				// Selection is a project
				Project project = lookupProjectByName(selection.getValue());
				return new Pair<>(project, null);
			}
		}
		
		return null;
	}
	
	public int size()
	{
		return projects.size();
	}
	
	public boolean isEmpty()
	{
		return projects.size() == 0;
	}
	
	public boolean setExpanded(Project project, boolean isExpanded)
	{
		TreeItem<String> node = getProjectNode(project);
		if (node != null)
			node.expandedProperty().set(isExpanded);
		
		return (node != null);
	}
	
	public boolean collapseProject(Project project)
	{
		return setExpanded(project, false);
	}
	
	public boolean expandProject(Project project)
	{
		return setExpanded(project, true);
	}
	
	private int getGlobalIndexOf(TreeItem<String> fileNode)
	{
		TreeItem<String> root = projectTreeDisplay.getRoot();
		ObservableList<TreeItem<String>> projectNodes = root.getChildren();
		
		int index = 0;
		for (TreeItem<String> node : projectNodes)
		{
			if (node.equals(fileNode))
				return index;
			
			index++;
			for (TreeItem<String> child : node.getChildren())
			{
				if (child.equals(fileNode))
					return index;
				index++;
			}
		}
		
		throw new IllegalArgumentException(fileNode.getValue() + " not found in tree");
	}
	
	private void onTreeClick(MouseEvent event)
	{
		if (event.getClickCount() == 2)
		{
			if (onFileDoubleClicked != null)
			{
				Pair<Project, ASMFile> selection = getActiveSelection();
				if (selection == null)
					return;
				
				ASMFile selectedFile = selection.getValue();
				if (selectedFile != null)
					onFileDoubleClicked.accept(selectedFile);
			}
		}
	}
	
	private Project lookupProjectByName(String value)
	{
		for (Project project : projects)
			if (project.getName().equals(value))
				return project;
		
		throw new IllegalStateException("Selected project not found: " + value);
	}
	
	private TreeView<String> createEmptyRootedProjectTree()
	{
		TreeItem<String> root = new TreeItem<String>("");
		root.setExpanded(true);
		
		TreeView<String> treeView = new TreeView<String>(root);
		treeView.showRootProperty().set(false);
		treeView.setBackground(Background.EMPTY);
		
		return treeView;
	}
	
	private void projectListChanged(Change<? extends Project> change)
	{
		while (change.next())
		{
			for (Project project : change.getAddedSubList())
				addProjectToTree(project);
			
			for (Project project : change.getRemoved())
				removeProjectFromTree(project);
		}
		
		validateDisplay();
	}
	
	private void projectFilesChanged(Change<? extends ASMFile> change)
	{
		while (change.next())
		{
			for (ASMFile file : change.getAddedSubList())
				addFileToTree(file);
			
			for (ASMFile file : change.getRemoved())
				removeFileFromTree(file);
		}
		
		validateDisplay();
	}
	
	private void addProjectToTree(Project project)
	{
		project.addListener(this::projectFilesChanged);
		
		/*
		String projectTypeString = project.getType();
		projectTypeString = " (" + projectTypeString.toUpperCase() + ")";
		TreeItem<String> projectItem = new TreeItem<>(project.getName() + projectTypeString);
		*/
		TreeItem<String> projectItem = new TreeItem<>(project.getName());
		
		for (ASMFile file : project)
		{
			TreeItem<String> item = new TreeItem<>(file.getName());
			projectItem.getChildren().add(item);
		}
		
		projectTreeDisplay.getRoot().getChildren().add(projectItem);
	}
	
	private void addFileToTree(ASMFile file)
	{
		TreeItem<String> fileNode = new TreeItem<>(file.getName());
		TreeItem<String> projectNode = getProjectNode(file.getProject());
		
		projectNode.getChildren().add(fileNode);
	}
	
	private void removeProjectFromTree(Project project)
	{
		TreeItem<String> projectNode = getProjectNode(project);
		projectTreeDisplay.getRoot().getChildren().remove(projectNode);
	}
	
	private void validateDisplay()
	{
		if (this.isEmpty())
			this.setCenter(emptyTreeDisplay);
		else
			this.setCenter(projectTreeDisplay);
	}
	
	private void removeFileFromTree(ASMFile file)
	{
		Project project = file.getProject();
		TreeItem<String> projectNode = getProjectNode(project);
		TreeItem<String> fileNode = getFileNode(file, projectNode);
		projectNode.getChildren().remove(fileNode);
	}
	
	private TreeItem<String> getProjectNode(Project project)
	{
		String name = project.getName();
		
		for (TreeItem<String> node : projectTreeDisplay.getRoot().getChildren())
		{
			if (node.getValue().equals(name))
				return node;
		}
		
		return null;
	}
	
	private TreeItem<String> getFileNode(ASMFile file)
	{
		Project project = file.getProject();
		TreeItem<String> projectNode = getProjectNode(project);
		
		return getFileNode(file, projectNode);
	}
	
	private TreeItem<String> getFileNode(ASMFile file, TreeItem<String> projectNode)
	{
		String name = file.getName();
		
		for (TreeItem<String> node : projectNode.getChildren())
		{
			if (node.getValue().equals(name))
				return node;
		}
		
		return null;
	}

}
