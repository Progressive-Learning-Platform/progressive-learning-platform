/*
    Copyright 2013 Wira Mulia

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

package plptool.gui.frames;

import javax.swing.*;
import java.awt.BorderLayout;
import plptool.*;
import plptool.gui.*;

/**
 *
 * @author wira
 */
public class IDE extends JFrame {
    private CodeEditorPane textPane;
    private ProjectDriver plp;

    public IDE(ProjectDriver plp) {
        this.plp = plp;
        textPane = new CodeEditorPane();

        getContentPane().add(textPane.getContainerWithLines(), BorderLayout.CENTER);
    }
}
