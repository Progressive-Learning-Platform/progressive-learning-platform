package edu.asu.plp.tool.prototype.util;

import javafx.scene.Node;

/**
 * Utility methods for styling/unstyling the UI.
 *
 * @author by Morgan on 3/8/2016.
 */
public class UIStyle
{
	/**
	 * CSS Class identifier that can be appended to any node that supports it, to activate an error mode.
	 * <p>
	 * Currently only the text-input node supports it (Which is contained in TextFields and the ComboBox if editable).
	 */
	private static final String ERROR_IDENTIFIER = "error";

	/**
	 * Adds or removes the error class identifier to a node.
	 * <p>
	 * e.g. The normal {@link javafx.scene.control.TextField} contains a .text-input that you can style. A theme can
	 * style .text-input.error.
	 * <p>
	 * If isValid is true it will remove any error styling. If false it will apply said styling.
	 * <p>
	 * NOTE: This will only apply error styling to the node, if and only if the current applied theme supports error
	 * styling for the node.
	 *
	 * @param isValid
	 * 		True if the node has no errors, false otherwise.
	 * @param node
	 * 		Node to style
	 */
	public static void applyError( boolean isValid, Node node )
	{
		if ( isValid )
		{
			if ( node.getStyleClass().contains(ERROR_IDENTIFIER) )
				node.getStyleClass().remove(ERROR_IDENTIFIER);
		} else
		{
			if ( !node.getStyleClass().contains(ERROR_IDENTIFIER) )
				node.getStyleClass().add(ERROR_IDENTIFIER);
		}
	}
}
