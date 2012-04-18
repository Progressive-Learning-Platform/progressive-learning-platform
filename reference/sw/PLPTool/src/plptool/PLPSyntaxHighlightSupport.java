/*
    Copyright 2012 PLP Contributors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

package plptool;

import plptool.gui.ProjectDriver;

/**
 * This class is the interface to the PLPTool syntax highlighting system.
 * ISA implementations wishing to have syntax highlighting support will have 
 * to implement this class and override the getSyntaxHighlightingSupport()
 * method in its corresponding ISA meta class.
 *
 * @author wira
 */
public interface PLPSyntaxHighlightSupport {
    /**
     * Main syntax highlight method. This is called by the IDE whenever a
     * syntax highlight event is needed
     *
     * @param plp Reference to the ProjectDriver
     * @param text Text to be syntax highlighted
     * @param position Starting position of the text in the document
     */
    public abstract void syntaxHighlightTextAction(
            ProjectDriver plp, String text, int position);

    /**
     * Called by the IDE whenever new style needs to be re-applied
     */
    public abstract void newStyle();
}
