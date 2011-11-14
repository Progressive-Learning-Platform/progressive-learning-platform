/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mods;

import java.awt.image.BufferedImage;
import java.awt.Graphics;
import java.awt.Color;

/**
 *
 * @author CAESAR
 */
public class VGACanvas extends javax.swing.JPanel {

    private BufferedImage I;

    public VGACanvas(BufferedImage I) {
        this.I = I;
    }

    public void setImage(BufferedImage I) {
        this.I = I;
    }

    @Override
    public void paint(Graphics g) {
        g.drawImage(I, 0, 0, Color.BLACK, null);
    }
}
