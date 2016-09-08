package edu.asu.plp.tool.prototype.model;

/**
 * Any Class or Enum can extend this interface to be used as a key for retrieving a setting from {@link
 * edu.asu.plp.tool.prototype.ApplicationSettings}
 * <p>
 * The value passed into {@link edu.asu.plp.tool.prototype.ApplicationSettings#getSetting(Setting)} will have its
 * respective toString() method called.
 * <p>
 * NOTE: It is standardized that the class or enumeration extending this, do not include any save variants (version) and
 * only provide the standard name. e.g editor font in an enumation would be EDITOR_FONT and only that. With no
 * SAVED_EDITOR_FONT.
 * <p>
 * Please see {@link SettingUtil#prependSaveLabel(Setting)} for creating a saved variant of your standard name. It will
 * create a label, in accordance with how {@link edu.asu.plp.tool.prototype.ApplicationSettings} loads/saves settings.
 *
 * You may also see {@link SettingUtil#loadSavedSettingDefaultIfNotPresent(Setting)}
 *
 * @author Nesbitt, Morgan Created on 2/23/2016.
 */
public interface Setting
{
}
