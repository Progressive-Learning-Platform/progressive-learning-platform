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

import plptool.mips.*;
import plptool.Constants;

/**
 *
 * @author wira
 */
public class CPULevel1Canvas extends JPanel implements MouseListener, MouseMotionListener {
    private SimCore sim;
    private int instructionsRetired;

    private int H, W;
    private double leftMargin = 0.1;
    private double rightMargin = 0.1;
    private int stageSpacing = 40;
    private int totalStageSpacing = 4 * 40;

    private boolean fwd_ex_ex = false;
    private boolean fwd_mem_ex = false;
    private boolean fwd_mem_mem = false;

    private Color clrCanvasBG = Color.black;
    private Color clrCanvasText = Color.white;
    private Color clrIdleStage = new Color(15, 15, 15);
    private Color clrIdleLabel = Color.darkGray;
    private Color clrActiveLabel = Color.white;
    private Color[] clrInstr = {new Color(0, 0, 150), new Color(0, 150, 150), new Color(0, 150, 0), new Color(150, 150, 0), new Color(150, 0, 0)};
    private Color clrArrow = new Color(200, 185, 0);
    private Color clrMemOutArrow = Color.blue;
    private Color clrMemInArrow = Color.red;

    private int[] clrSelector = {0, 0, 0, 0, 0};

    private int xLeftOffset = 10;
    private int yTopOffset = 200;
    private int stageBlockW = 150;
    private int stageBlockH = 150;
    private int arrowSpacing = (int)(stageBlockH/6.0);
    private int arrowHeight = (int)((stageBlockH - (3*stageBlockH/6.0))/2.0);

    private int[] id_out_arrow_xcoords = {xLeftOffset+2*stageBlockW+stageSpacing-(int)(stageBlockW/4.0),
                                          xLeftOffset+2*stageBlockW+2*stageSpacing+(int)(stageBlockW/5.0),
                                          xLeftOffset+2*stageBlockW+2*stageSpacing+(int)(stageBlockW/4.0),
                                          xLeftOffset+2*stageBlockW+2*stageSpacing+(int)(stageBlockW/5.0),
                                          xLeftOffset+2*stageBlockW+stageSpacing-(int)(stageBlockW/4.0)};

    private int[] id_out_1_arrow_ycoords = {yTopOffset+(int)(stageBlockH/6.0),
                                            yTopOffset+(int)(stageBlockH/6.0),
                                            yTopOffset+(int)(stageBlockH/6.0+arrowHeight/2.0),
                                            yTopOffset+(int)(stageBlockH/6.0)+arrowHeight,
                                            yTopOffset+(int)(stageBlockH/6.0)+arrowHeight};
    private int[] id_out_2_arrow_ycoords = {yTopOffset+(int)(2*stageBlockH/6.0+arrowHeight),
                                            yTopOffset+(int)(2*stageBlockH/6.0)+arrowHeight,
                                            yTopOffset+(int)(2*stageBlockH/6.0+1.5*arrowHeight),
                                            yTopOffset+(int)(2*stageBlockH/6.0)+2*arrowHeight,
                                            yTopOffset+(int)(2*stageBlockH/6.0)+2*arrowHeight};

    private int memXStart = xLeftOffset + 3*stageBlockW + 3*stageSpacing;

    private int[] mem_out_xcoords = {memXStart+arrowSpacing, memXStart+arrowSpacing+(int)(arrowHeight/2.0), memXStart+arrowSpacing+arrowHeight};
    private int[] mem_out_ycoords = {yTopOffset-arrowSpacing, yTopOffset-arrowSpacing-arrowHeight, yTopOffset-arrowSpacing};

    private int[] mem_in_xcoords = {memXStart+2*arrowSpacing+arrowHeight, memXStart+2*arrowSpacing+(int)(1.5*arrowHeight), memXStart+2*arrowSpacing+2*arrowHeight};
    private int[] mem_in_ycoords = {yTopOffset-arrowSpacing-arrowHeight, yTopOffset-arrowSpacing, yTopOffset-arrowSpacing-arrowHeight};

    private Font fontCaption = new Font("Monospaced", Font.BOLD, 12);
    private Font fontStageLabel = new Font("Monospaced", Font.BOLD, 42);

    public CPULevel1Canvas(SimCore sim) {
        this.sim = sim;
        addMouseListener(this);
        addMouseMotionListener(this);
        instructionsRetired = 0;
    }

    public void setInstructionsRetired(int in) {
        this.instructionsRetired = in;
    }

    public void setColorSelector(int[] clr) {
        clrSelector = clr;
    }

    public void clearFwdFlags() {
        fwd_ex_ex = false;
        fwd_mem_ex = false;
        fwd_mem_mem = false;
    }

    public void setFwdExEx() {
        fwd_ex_ex = true;
    }

    public void setFwdMemEx() {
        fwd_mem_ex = true;
    }

    public void setFwdMemMem() {
        fwd_mem_mem = true;
    }

    @Override
    public void paint(Graphics g1) {
        this.setSize(this.getParent().getSize());
        H = this.getHeight();
        W = this.getWidth();

        Graphics2D g = (Graphics2D) g1;
        BufferedImage image = new BufferedImage(W, H, BufferedImage.TYPE_INT_RGB);
        Graphics2D ig = image.createGraphics();

        int fCH = ig.getFontMetrics(fontCaption).getHeight();
        int fDH = ig.getFontMetrics(fontCaption).getDescent();

        int fSH = ig.getFontMetrics(fontStageLabel).getHeight();
        int fSD = ig.getFontMetrics(fontStageLabel).getDescent();

        int labelBaseXPosition = xLeftOffset + (int) (stageBlockH/2.0);

        ig.setColor(clrCanvasBG);
        ig.fillRect(0, 0, W, H);

        ig.setStroke(new BasicStroke(1.0f));
        ig.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        ig.setColor(sim.id_stage.i_instrAddr == -1 || sim.id_stage.i_bubble ? clrIdleStage : clrInstr[clrSelector[0]]);
        ig.fillRect(xLeftOffset, yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.id_stage.i_instrAddr == -1 || sim.id_stage.i_bubble ? clrIdleLabel : clrActiveLabel);
        ig.setFont(fontStageLabel);
        ig.drawString("IF", labelBaseXPosition-(int)(ig.getFontMetrics().stringWidth("IF")/2.0), yTopOffset+(int)(stageBlockH/2.0+fSH/2.0-fSD));
        ig.setColor(clrCanvasText);
        ig.setFont(fontCaption);
        if(!sim.id_stage.i_bubble && sim.id_stage.i_instrAddr != -1) {
            ig.drawString(String.format("0x%08x", sim.id_stage.i_instrAddr), xLeftOffset, yTopOffset+stageBlockH+5+fCH);
            ig.drawString(MIPSInstr.mnemonic(sim.id_stage.i_instruction), xLeftOffset, yTopOffset+stageBlockH+2*(5+fCH));
        }

        ig.setColor(sim.id_stage.instrAddr == -1 || sim.id_stage.bubble ? clrIdleStage : clrInstr[clrSelector[1]]);
        ig.fillRect(xLeftOffset+1*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.id_stage.instrAddr == -1 || sim.id_stage.bubble ? clrIdleLabel : clrActiveLabel);
        ig.setFont(fontStageLabel);
        ig.drawString("ID", labelBaseXPosition+1*(stageBlockW+stageSpacing)-(int)(ig.getFontMetrics().stringWidth("ID")/2.0), yTopOffset+(int)(stageBlockH/2.0+fSH/2.0-fSD));
        ig.setColor(clrCanvasText);
        ig.setFont(fontCaption);
        if(!sim.id_stage.bubble && sim.id_stage.instrAddr != -1) {
            ig.drawString(String.format("0x%08x", sim.id_stage.instrAddr), xLeftOffset+1*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+5+fCH);
            ig.drawString(MIPSInstr.mnemonic(sim.id_stage.instruction), xLeftOffset+1*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+2*(5+fCH));
        }

        ig.setColor(sim.ex_stage.instrAddr == -1 || sim.ex_stage.bubble ? clrIdleStage : clrInstr[clrSelector[2]]);
        ig.fillRect(xLeftOffset+2*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.ex_stage.instrAddr == -1 || sim.ex_stage.bubble ? clrIdleLabel : clrActiveLabel);
        ig.setFont(fontStageLabel);
        ig.drawString("EX", labelBaseXPosition+2*(stageBlockW+stageSpacing)-(int)(ig.getFontMetrics().stringWidth("EX")/2.0), yTopOffset+(int)(stageBlockH/2.0+fSH/2.0-fSD));
        ig.setColor(clrCanvasText);
        ig.setFont(fontCaption);
        if(!sim.ex_stage.bubble && sim.ex_stage.instrAddr != -1) {
            ig.drawString(String.format("0x%08x", sim.ex_stage.instrAddr), xLeftOffset+2*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+5+fCH);
            ig.drawString(MIPSInstr.mnemonic(sim.ex_stage.instruction), xLeftOffset+2*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+2*(5+fCH));
        }

        ig.setColor(sim.mem_stage.instrAddr == -1 || sim.mem_stage.bubble ? clrIdleStage : clrInstr[clrSelector[3]]);
        ig.fillRect(xLeftOffset+3*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.mem_stage.instrAddr == -1 || sim.mem_stage.bubble ? clrIdleLabel : clrActiveLabel);
        ig.setFont(fontStageLabel);
        ig.drawString("MEM", labelBaseXPosition+3*(stageBlockW+stageSpacing)-(int)(ig.getFontMetrics().stringWidth("MEM")/2.0), yTopOffset+(int)(stageBlockH/2.0+fSH/2.0-fSD));
        ig.setColor(clrCanvasText);
        ig.setFont(fontCaption);
        if(!sim.mem_stage.bubble && sim.mem_stage.instrAddr != -1) {
            ig.drawString(String.format("0x%08x", sim.mem_stage.instrAddr), xLeftOffset+3*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+5+fCH);
            ig.drawString(MIPSInstr.mnemonic(sim.mem_stage.instruction), xLeftOffset+3*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+2*(5+fCH));
        }
        ig.setColor(clrIdleStage);
        ig.fillPolygon(mem_out_xcoords, mem_out_ycoords, 3);
        ig.fillPolygon(mem_in_xcoords, mem_in_ycoords, 3);
        if(!sim.mem_stage.bubble && sim.mem_stage.ctl_memwrite == 1) {
            ig.setColor(clrMemOutArrow);
            ig.fillPolygon(mem_out_xcoords, mem_out_ycoords, 3);

        } else if(!sim.mem_stage.bubble && sim.mem_stage.ctl_memread == 1) {
            ig.setColor(clrMemInArrow);
            ig.fillPolygon(mem_in_xcoords, mem_in_ycoords, 3);
        }


        ig.setColor(sim.wb_stage.instrAddr == -1 || sim.wb_stage.bubble ? clrIdleStage : clrInstr[clrSelector[4]]);
        ig.fillRect(xLeftOffset+4*(stageBlockW+stageSpacing), yTopOffset, stageBlockW, stageBlockH);
        ig.setColor(sim.wb_stage.instrAddr == -1 || sim.wb_stage.bubble ? clrIdleLabel : clrActiveLabel);
        ig.setFont(fontStageLabel);
        ig.drawString("WB", labelBaseXPosition+4*(stageBlockW+stageSpacing)-(int)(ig.getFontMetrics().stringWidth("WB")/2.0), yTopOffset+(int)(stageBlockH/2.0+fSH/2.0-fSD));
        ig.setColor(clrCanvasText);
        ig.setFont(fontCaption);
        if(!sim.wb_stage.bubble && sim.wb_stage.instrAddr != -1) {
            ig.drawString(String.format("0x%08x", sim.wb_stage.instrAddr), xLeftOffset+4*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+5+fCH);
            ig.drawString(MIPSInstr.mnemonic(sim.wb_stage.instruction), xLeftOffset+4*(stageBlockW+stageSpacing), yTopOffset+stageBlockH+2*(5+fCH));
        }

        long f = sim.getFlags();
        if((f & SimCore.PLP_SIM_FWD_EX_EX_RT) == SimCore.PLP_SIM_FWD_EX_EX_RT)
            ;
        if((f & SimCore.PLP_SIM_FWD_EX_EX_RS) == SimCore.PLP_SIM_FWD_EX_EX_RS)
            ;
            ;
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_RT) == SimCore.PLP_SIM_FWD_MEM_EX_RT)
            ;
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_RS) == SimCore.PLP_SIM_FWD_MEM_EX_RS)
            ;
            ;
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_LW_RT) == SimCore.PLP_SIM_FWD_MEM_EX_LW_RT)
            ;
        if((f & SimCore.PLP_SIM_FWD_MEM_EX_LW_RS) == SimCore.PLP_SIM_FWD_MEM_EX_LW_RS)
            ;
        if((f & SimCore.PLP_SIM_FWD_MEM_MEM) == SimCore.PLP_SIM_FWD_MEM_MEM)
            ;
        if((f & SimCore.PLP_SIM_IF_STALL_SET) == SimCore.PLP_SIM_IF_STALL_SET)
            ;
        if((f & SimCore.PLP_SIM_ID_STALL_SET) == SimCore.PLP_SIM_ID_STALL_SET)
            ;
        if((f & SimCore.PLP_SIM_EX_STALL_SET) == SimCore.PLP_SIM_EX_STALL_SET)
            ;
        if((f & SimCore.PLP_SIM_MEM_STALL_SET) == SimCore.PLP_SIM_MEM_STALL_SET)
            ;

        ig.setColor(clrCanvasText);
        ig.setFont(fontCaption);
        ig.drawString("Instructions retired: ", xLeftOffset+5*(stageBlockW+stageSpacing), yTopOffset+5+fCH);
        ig.drawString(instructionsRetired+"", xLeftOffset+5*(stageBlockW+stageSpacing), yTopOffset+10+2*fCH);

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

