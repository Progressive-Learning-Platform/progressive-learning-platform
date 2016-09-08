package edu.asu.plp.tool.prototype.view.menu.options;


import edu.asu.plp.tool.prototype.model.OptionSection;
import edu.asu.plp.tool.prototype.view.SwapPane;
import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.SplitPane;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Pane;
import moore.util.Subroutine;

import java.util.Map;

/**
 * Options Menu that allows changing settings of the application.
 * <p>
 * NOTE: Typical usage is to wrap it in a stage and display it as another window, that maintains the focus of the
 * program
 * until exited.
 *
 * @author Nesbitt, Morgan on 2/27/2016.
 */
public class OptionsPane extends BorderPane
{
	/**
	 * Tree for displaying all the possible option panel names.
	 * <p>
	 * E.g. Application, Editor, etc.
	 */
	private OptionsSettingsTree sections;

	/**
	 * Panel on the right hand side of the split pane. All option panels will be swapped in and out of this SwapPane.
	 */
	private SwapPane sectionView;

	/**
	 * Mapping from {@link OptionsSettingsTree} sections to the Panes they link to.
	 */
	private Map<OptionSection, Pane> optionScreenMap;

	/**
	 * Action that can be exited upon hitting the okay button.
	 */
	private Subroutine okAction;

	/**
	 * Action that can be exited upon hitting the cancel button.
	 */
	private Subroutine cancelAction;

	/**
	 * Creation of the OptionsPane. Pass in a map that fully maps all titles to their respective panes.
	 * <p>
	 * CAUTION: Map should can't be null. However, the map can be empty.
	 *
	 * @param optionsMenuModel
	 * 		Map from tree names to panes
	 */
	public OptionsPane( Map<OptionSection, Pane> optionsMenuModel )
	{
		//TODO use some kind of selection model to default to select first item or last item selected
		optionScreenMap = optionsMenuModel;
		sections = new OptionsSettingsTree(optionsMenuModel.keySet());
				
		sections.setTreeDoubleClick(this::onTreeDoubleClick);

		sectionView = new SwapPane();

		SplitPane sectionContentSplitPane = new SplitPane();
		sectionContentSplitPane.setOrientation(Orientation.HORIZONTAL);
		sectionContentSplitPane.getItems().addAll(sections, sectionView);

		sectionContentSplitPane.setDividerPositions(0.2, 1);
		setCenter(sectionContentSplitPane);

		okAction = () -> {
		};
		cancelAction = () -> {
		};

		Button okButton = new Button();
		okButton.setText("OK");
		okButton.setOnMouseClicked(this::okButtonClicked);

		Button cancelButton = new Button();
		cancelButton.setText("Cancel");
		cancelButton.setOnMouseClicked(this::cancelButtonClicked);

		HBox buttonBar = new HBox();
		buttonBar.getChildren().addAll(okButton, cancelButton);
		buttonBar.setAlignment(Pos.BASELINE_RIGHT);

		setBottom(buttonBar);
	}

	/**
	 * Called to switch the swappane to whatever the was clicked from the sections tree.
	 * <p>
	 * Utilizes both in case the child node is a separate pane or for future usage, that a specific element in the pane
	 * could be focused/highlighted.
	 * <p>
	 * CAUTION: Both parameters should be non-null.
	 *
	 * @param selection
	 * 		The item in the tree that was actually click.
	 * @param selectionRoot
	 * 		Most root node of the selection that was clicked (Can be same node).
	 */
	private void onTreeDoubleClick( OptionSection selection, OptionSection selectionRoot )
	{
		//Note these are separate in the instance that you want to break them up
		//TODO focus to specifc area (if desired) when not equal
		if ( optionScreenMap.containsKey(selection) )
			sectionView.setActivePane(optionScreenMap.get(selection));
		else
			sectionView.setActivePane(optionScreenMap.get(selectionRoot));
	}

	/**
	 * Sets okAction field to parameter if non-null, or empty subroutine otherwise.
	 * <p>
	 * CAUTION: This will overwrite the current okAction regardless of what is passed in.
	 *
	 * @param okAction
	 * 		okAction to substitute in
	 */
	public void setOkAction( Subroutine okAction )
	{
		this.okAction = ( okAction != null ) ? okAction : () -> {
		};
	}

	/**
	 * Sets cancelAction field to parameter if non-null, or empty subroutine otherwise.
	 * <p>
	 * CAUTION: This will overwrite the current cancelAction regardless of what is passed in.
	 *
	 * @param cancelAction
	 * 		cancelAction to substitute in
	 */
	public void setCancelAction( Subroutine cancelAction )
	{
		this.cancelAction = ( cancelAction != null ) ? cancelAction : () -> {
		};
	}

	/**
	 * Performs the subroutine of the okAction field when the button is clicked.
	 *
	 * @param mouseEvent
	 */
	private void okButtonClicked( MouseEvent mouseEvent )
	{
		okAction.perform();
	}

	/**
	 * Performs the subroutine of the cancelAction field when the button is clicked.
	 *
	 * @param mouseEvent
	 */
	private void cancelButtonClicked( MouseEvent mouseEvent )
	{
		cancelAction.perform();
	}
}
