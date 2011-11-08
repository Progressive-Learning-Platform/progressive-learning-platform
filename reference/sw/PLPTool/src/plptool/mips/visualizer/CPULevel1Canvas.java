/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mips.visualizer;

import javax.swing.JPanel;
import java.awt.image.BufferedImage;
import java.awt.BasicStroke;
import java.awt.RenderingHints;
import java.awt.Font;
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.MouseListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;

import plptool.gui.ProjectDriver;
import plptool.mips.SimCore;
import plptool.mips.Asm;

/**
 *
 * @author wira
 */
public class CPULevel1Canvas extends JPanel implements MouseListener, MouseMotionListener {
    private SimCore sim;

    public CPULevel1Canvas(SimCore sim) {
        this.sim = sim;
        addMouseListener(this);
        addMouseMotionListener(this);
    }

    private int H, W;
    private double leftMargin = 0.1;
    private double rightMargin = 0.1;
    private int stageSpacing = 40;
    private int totalStageSpacing = 4 * 40;

    private Color clrCanvasBG = Color.black;
    private Color clrCanvasText = Color.white;
    private Color clrIdleStage = Color.gray;
    private Color clrActiveStage = Color.red;
    private Color clrIdleLabel = Color.black;
    private Color clrActiveLabel = Color.white;

    private int xLeftOffset = 10;
    private int yTopOffset = 10;
    private int stageBlockW = 60;

    @Override
    public void paint(Graphics g1) {
        this.setSize(this.getParent().getSize());
        H = this.getHeight();
        W = this.getWidth();
        int stageBlockH = stageBlockW;

        Graphics2D g = (Graphics2D) g1;
        BufferedImage image = new BufferedImage(W, H, BufferedImage.TYPE_INT_RGB);
        Graphics2D ig = image.createGraphics();

        ig.setColor(clrCanvasBG);
        ig.fillRect(0, 0, W, H);

        ig.setColor(sim.id_stage.i_instrAddr == -1 || sim.id_stage.i_bubble ? clrIdleStage : clrActiveStage);
        ig.fillRect(xLeftOffset, yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.id_stage.instrAddr == -1 || sim.id_stage.bubble ? clrIdleStage : clrActiveStage);
        ig.fillRect(xLeftOffset+1*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.ex_stage.instrAddr == -1 || sim.ex_stage.bubble ? clrIdleStage : clrActiveStage);
        ig.fillRect(xLeftOffset+2*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.mem_stage.instrAddr == -1 || sim.mem_stage.bubble ? clrIdleStage : clrActiveStage);
        ig.fillRect(xLeftOffset+3*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.wb_stage.instrAddr == -1 || sim.wb_stage.bubble ? clrIdleStage : clrActiveStage);
        ig.fillRect(xLeftOffset+4*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);

        g.drawImage(image, 0, 0, this);
    }

    public void mouseExited(MouseEvent me) {

    }

    public void mouseEntered(MouseEvent me) {

    }

    public void mouseReleased(MouseEvent me) {

    }

    public void mousePressed(MouseEvent me) {

    }

    public void mouseClicked(MouseEvent me) {

    }

    public void mouseMoved(MouseEvent me) {

    }

    public void mouseDragged(MouseEvent me) {

    }
}

