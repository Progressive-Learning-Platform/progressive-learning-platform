package edu.asu.plp.tool.prototype.util;

/**
 * Constants for use in styling the UI. Mainly for when styling using pure JavaFX references doesn't work by itself.
 * <p>
 * e.g. JavaFX doesn't allow you to style the color of the {@link javafx.scene.text.Text} node from plain CSS. Thus it
 * is necessary to attach an ID to the nodes that allow them to be styled ({@link UIConstants#TEXT_COLOR}).
 * <p>
 * NOTE: When using UIConstants for attaching ID's, it should be used in a way that all themes can access and style the
 * node. E.g. No UIConstants.DARK_BUTTON_COLOR should be created. Instead create a generic UIConstants.BUTTON_COLOR
 * that
 * all themes may add. So that there would not be "Dark" buttons in a light theme.
 *
 * @author Nesbitt, Morgan
 */
public class UIConstants
{
	/**
	 * Attach to any {@link javafx.scene.text.Text} and the respective theme should be able to style any text nodes
	 * with #text.
	 * <p>
	 * NOTE: This is purely for any menu items that may add a {@link javafx.scene.text.Text} node. The {@link
	 * edu.asu.plp.tool.prototype.view.menu.options.OptionsPane} uses this (for example). This is not intended for the
	 * editor or any non-menu usage.
	 */
	public static final String TEXT_COLOR = "text";
}
