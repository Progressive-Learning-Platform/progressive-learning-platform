package edu.asu.plp.tool.prototype.view;

import java.util.Collection;
import java.util.function.Consumer;

import javafx.collections.ListChangeListener.Change;
import javafx.collections.ObservableList;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Background;
import javafx.scene.layout.BorderPane;
import edu.asu.plp.tool.prototype.PLPLabel;
import edu.asu.plp.tool.prototype.model.ArrayListProperty;

public class OutlineView extends BorderPane
{
	public static final String DEFAULT_EMPTY_MESSAGE = "The active file has no labels to display";
	
	/**
	 * The model displayed by this view. All labels in this list should be displayed by
	 * the OutlineView in the same order.
	 * <p>
	 * This list is backed externally, and will thus be modified externally.
	 */
	private ObservableList<? extends PLPLabel> labels;
	
	/** The view to be displayed by this container */
	private TreeView<String> treeDisplay;
	
	/** This Node is displayed when the tree is empty, instead of {@link #treeDisplay} */
	private Node emptyTreeDisplay;
	
	/**
	 * This listener will be called whenever a label item is activated (clicked or double
	 * clicked) by the user
	 * <p>
	 * This is intended to be specified externally (not inside this class) using
	 * {@link #setOnAction(Consumer)}
	 */
	private Consumer<PLPLabel> onAction;
	
	public static OutlineView create(Collection<? extends PLPLabel> content)
	{
		ObservableList<PLPLabel> observableList = new ArrayListProperty<>();
		observableList.addAll(content);
		
		return new OutlineView(observableList);
	}
	
	public OutlineView(ObservableList<? extends PLPLabel> backingModel)
	{
		assert backingModel != null;
		emptyTreeDisplay = new Label(DEFAULT_EMPTY_MESSAGE);
		treeDisplay = createEmptyRootedProjectTree();
		treeDisplay.setOnMouseClicked(this::onTreeClick);
		
		setModel(backingModel);
	}
	
	/**
	 * Set the listener for when a label item is activated by a user through this view.
	 * The listener will be called every time a label is activated; the active label will
	 * be passed to the listener as its sole parameter.
	 * 
	 * @param onAction
	 *            Listener to be called when a label item is activated by the user
	 */
	public void setOnAction(Consumer<PLPLabel> onAction)
	{
		this.onAction = onAction;
	}
	
	/**
	 * Set the backing model and update this view to display the specified labels
	 * <p>
	 * This view will no longer monitor or update in response to the old model.
	 * 
	 * @param model
	 */
	public void setModel(ObservableList<? extends PLPLabel> model)
	{
		if (labels != null)
			this.labels.removeListener(this::projectListChanged);
		
		assert model != null;
		this.labels = model;
		this.labels.addListener(this::projectListChanged);
		
		this.treeDisplay.getRoot().getChildren().clear();
		for (PLPLabel label : model)
			addLabelToTree(label);
		
		validateDisplay();
	}
	
	public int size()
	{
		return labels.size();
	}
	
	public boolean isEmpty()
	{
		return this.size() == 0;
	}
	
	private void onTreeClick(MouseEvent event)
	{
		if (event.getClickCount() == 2)
		{
			if (onAction != null)
			{
				PLPLabel selection = getActiveSelection();
				if (selection == null)
					return;
				else
					onAction.accept(selection);
			}
		}
	}
	
	private PLPLabel lookupLabelByName(String name)
	{
		for (PLPLabel label : labels)
			if (label.getName().equals(name))
				return label;
		
		throw new IllegalStateException("Selected project not found: " + name);
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
	
	private void projectListChanged(Change<? extends PLPLabel> change)
	{
		while (change.next())
		{
			for (PLPLabel label : change.getAddedSubList())
				addLabelToTree(label);
			
			for (PLPLabel label : change.getRemoved())
				removeLabelFromTree(label);
		}
	}
	
	private void addLabelToTree(PLPLabel label)
	{
		TreeItem<String> projectItem = new TreeItem<>(label.getName());
		treeDisplay.getRoot().getChildren().add(projectItem);
	}
	
	private void removeLabelFromTree(PLPLabel label)
	{
		TreeItem<String> projectNode = getNode(label);
		treeDisplay.getRoot().getChildren().remove(projectNode);
		
		validateDisplay();
	}
	
	private void validateDisplay()
	{
		if (this.isEmpty())
			this.setCenter(emptyTreeDisplay);
		else
			this.setCenter(treeDisplay);
	}
	
	private TreeItem<String> getNode(PLPLabel label)
	{
		String name = label.getName();
		
		for (TreeItem<String> node : treeDisplay.getRoot().getChildren())
		{
			if (node.getValue().equals(name))
				return node;
		}
		
		return null;
	}
	
	public PLPLabel getActiveSelection()
	{
		TreeItem<String> selection = treeDisplay.getSelectionModel().getSelectedItem();
		
		if (selection != null)
			return lookupLabelByName(selection.getValue());
		else
			return null;
	}
}
