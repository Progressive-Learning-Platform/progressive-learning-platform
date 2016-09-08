package edu.asu.plp.tool.prototype.view;

import javafx.scene.layout.BorderPane;
import javafx.scene.layout.Pane;

/**
 * A pane whose content can be swapped out dynamically.
 * <p>
 * NOTE: This class does not maintain a list of recent panes and only manages the current active pane.
 * <p>
 * E.g. Use in a split pane where one side is a collection of panes that need to be dynamically swapped.
 *
 * @author by Nesbitt, Morgan on 3/6/2016.
 */
public class SwapPane extends BorderPane
{
	private Pane activePane;

	/**
	 * Create empty {@link SwapPane}.
	 */
	public SwapPane()
	{
		this(new BorderPane());
	}

	/**
	 * Create {@link SwapPane} with activePane consuming all of {@link SwapPane}.
	 *
	 * @param activePane
	 */
	public SwapPane( Pane activePane )
	{
		setActivePane(activePane);
	}

	/**
	 * Swap current pane with the activePane parameter. If and only if the passed pane is not null.
	 *
	 * @param activePane
	 */
	public void setActivePane( Pane activePane )
	{
		if ( activePane != null )
		{
			this.activePane = activePane;
			this.setCenter(activePane);
		}
	}
}
