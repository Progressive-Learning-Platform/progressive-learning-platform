package edu.asu.plp.tool.prototype.model;

import java.util.List;

/**
 * For use in {@link edu.asu.plp.tool.prototype.view.menu.options.OptionsPane}. An instance of this class should
 * represent a {@link javafx.scene.control.TreeItem} in the tree view in the {@link
 * edu.asu.plp.tool.prototype.view.menu.options.OptionsPane}.
 * <p>
 * See {@link PLPOptions} for the default implementation of this class. Every instance of this interface represents a
 * parent even if it has no children. It must always be non-null and have a name.
 * <p>
 * Created by Nesbitt, Morgan on 2/28/2016.
 */
public interface OptionSection extends List<OptionSection>
{
	/**
	 * @return Name of item
	 */
	String getName();

	/**
	 * e.g. application.theme
	 * @return Path of item
	 */
	String getFullPath();

	/**
	 * @param parentPath Set parent item and all its children
	 */
	void setParent( String parentPath );
}
