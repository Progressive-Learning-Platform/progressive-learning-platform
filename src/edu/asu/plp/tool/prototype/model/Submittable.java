package edu.asu.plp.tool.prototype.model;

/**
 * An interface for objects that take in some kind of input and can distribute said input by itself (Typically through
 * the {@link edu.asu.plp.tool.backend.EventRegistry}).
 * <p>
 * An example of this is the {@link edu.asu.plp.tool.prototype.view.menu.options.OptionsPane}. {@link
 * edu.asu.plp.tool.prototype.Main} hands it some panes which implement {@link Submittable}. In the Subroutine for
 * the okButton in the OptionsPane, it iterates through the submittables to see if they are all valid (via {@link
 * Submittable#isValid()}). If they are all valid it will call each of their {@link Submittable#submit()}.
 *
 * @author by Nesbitt, Morgan on 3/7/2016.
 */
public interface Submittable
{
	/**
	 * Determine whether the current state of the instance is valid.
	 *
	 * @return True if valid, false otherwise.
	 */
	boolean isValid();

	/**
	 * Submit instances input. Typically through {@link edu.asu.plp.tool.backend.EventRegistry} or functional methods.
	 */
	void submit();
}
