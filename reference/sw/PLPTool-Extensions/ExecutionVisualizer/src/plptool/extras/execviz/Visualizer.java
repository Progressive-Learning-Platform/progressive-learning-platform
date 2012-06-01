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

package plptool.extras.execviz;

import edu.uci.ics.jung.graph.*;
import edu.uci.ics.jung.visualization.*;

import java.awt.Graphics;
import javax.swing.JPanel;

/**
 *
 * @author Wira
 */
public class Visualizer extends JPanel {
    private RuntimeProfile p;

    public Visualizer(RuntimeProfile p) {
        this.p = p;
    }

    @Override
    public void paint(Graphics g) {

    }
}
