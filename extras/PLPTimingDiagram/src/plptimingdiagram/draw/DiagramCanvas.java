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

package plptimingdiagram.draw;

import plptimingdiagram.TimingDiagram;
import plptimingdiagram.signals.*;
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
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class DiagramCanvas extends JPanel implements MouseListener {
    private final int DEFAULT_X_AXIS_HEIGHT_PIXELS = 60;
    private final int DEFAULT_SIGNAL_SLANT_PIXELS = 2;
    private final int DEFAULT_BUS_SLANT_PIXELS = 4;

    private TimingDiagram tD;
    private XAxis axis;
    private int xAxisHeight = DEFAULT_X_AXIS_HEIGHT_PIXELS;

    // Colors
    private Color background = Color.white;
    private Color defaultSignalColor = new Color(135, 0, 0);
    private Color majorGrid = new Color(200, 200, 200);
    private Color minorGrid = new Color(220, 220, 220);
    private Color textColor = Color.black;

    // Cursor
    private int yCursor = -1;

    public DiagramCanvas(TimingDiagram tD, XAxis axis) {
        super();
        System.out.println("New DiagramCanvas");
        this.tD = tD;
        this.axis = axis;
        this.addMouseListener(this);
    }

    public TimingDiagram getTimingDiagram() {
        return tD;
    }

    public XAxis getXAxis() {
        return axis;
    }

    public void setTimingDiagram(TimingDiagram tD) {
        this.tD = tD;
    }

    public void setXAxis(XAxis axis) {
        this.axis = axis;
    }

    public void setXAxisHeight(int height) {
        this.xAxisHeight = height;
    }

    public int getXAxisHeight() {
        return xAxisHeight;
    }

    public void setYCursor(int y) {
        yCursor = y;
    }

    private int H;
    private int W;
    private int sigHeight;
    private int sigCaptionOffset;
    private int yMargin;
    private int xMargin;

    @Override
    public void paint(Graphics g1) {
        if(axis.getIntervals() == 0 || axis.nonPositiveDomain())
            return;

        this.setSize(this.getParent().getWidth(), this.getParent().getHeight());
        H = this.getHeight();
        W = this.getWidth();
        int numberOfSignals = tD.getNumberOfSignals();
        sigHeight = (H - xAxisHeight) / numberOfSignals;
        Graphics2D g = (Graphics2D) g1;
        BufferedImage image = new BufferedImage(W, H, BufferedImage.TYPE_INT_RGB);
        Graphics2D ig = image.createGraphics();
        yMargin = 10;
        xMargin = 100;

        ig.setFont(new Font("sansserif", Font.BOLD, 12));
        sigCaptionOffset = ig.getFontMetrics().getHeight() + 5;
        double domainIntervalWidth = (W) / (double) axis.getIntervals();
        ig.setColor(background);
        ig.fillRect(0, 0, W, H);

        ig.setStroke(new BasicStroke(1.0f));
        ig.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();

        // Draw our grid
        double interval = (domainEnd - domainStart) / axis.getIntervals();

        for(int i = 0; i < numberOfSignals; i++) {
            if(i % 2 == 0)
                ig.setColor(new Color(250, 250, 250));
            else
                ig.setColor(new Color(240, 240, 240));
            ig.fillRect(0, (int) (i * sigHeight), W, (int) (sigHeight));
            ig.setColor(new Color(240, 240, 240));
            //ig.fillRect(0, (int) (i * sigHeight + 0.20 * sigHeight), W, (int) (0.60 * sigHeight));
        }

        for(int i = 0; i < axis.getIntervals(); i++) {
            ig.setColor(minorGrid);
            ig.setStroke(new BasicStroke(1.0f, BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 3.0f, new float[] {5.0f,7.0f}, 0.0f));
            ig.drawLine((int) (i*domainIntervalWidth), 0, (int) (i*domainIntervalWidth), H);
            ig.setColor(textColor);
            ig.setStroke(new BasicStroke(1.2f));
            ig.drawString(String.format("%d", (int) (i*interval + domainStart)), (int) (i*domainIntervalWidth+5), (H-xAxisHeight) + g.getFontMetrics().getHeight()+20);
        }
        ig.drawLine(0, H - xAxisHeight + 10, W, H - xAxisHeight + 10);

        // Draw our timing diagram
        ig.setStroke(new BasicStroke(1.2f));
        for(int i = 0; i < numberOfSignals; i++) {
            Signal signal = tD.getSignal(i);
            ArrayList edgesWithinRange = null;
            if(signal instanceof Line) {
                Line lineSignal = (Line) signal;
                edgesWithinRange = lineSignal.getEdgesWithinRange(domainStart, domainEnd);
            } else if(signal instanceof Bus) {
                Bus busSignal = (Bus) signal;
                edgesWithinRange = busSignal.getEdgesWithinRange(domainStart, domainEnd);
            }
                
            ig.setColor(defaultSignalColor);
            for(int j = 0; signal != null && j < edgesWithinRange.size() - 1; j++) {
                if(signal instanceof Line) {
                    LineEdge edge = (LineEdge) edgesWithinRange.get(j);
                    LineEdge nextEdge = (LineEdge) edgesWithinRange.get(j+1);
                    boolean change = edge.getSignal() != nextEdge.getSignal();
                    boolean risingEdge = edge.getSignal() == 0 && nextEdge.getSignal() == 1;

                    if(j == 0 && edge.getTime() != domainStart) {
                        ArrayList<LineEdge> edgesBeforeDomainStart = tD.getSignal(i).getEdgesWithinRange(0, domainStart);
                        if(edgesBeforeDomainStart.isEmpty()) {
                            ig.setColor(new Color(200, 200, 200));
                            this.drawUnknownSignal(ig, i, 0, edge.getTime(), true);
                        } else {
                            LineEdge lastEdge = edgesBeforeDomainStart.get(edgesBeforeDomainStart.size()-1);
                            boolean firstChange = lastEdge.getSignal() != edge.getSignal();
                            this.drawSignalLine(ig, i, 0, edge.getTime(), lastEdge.getSignal(), firstChange);
                            if(lastEdge.getSignal() == 1 && edge.getSignal() == 0)
                                this.drawTransition(ig, i, edge.getTime(), false);
                            else if(lastEdge.getSignal() == 0 && edge.getSignal() == 1)
                                this.drawTransition(ig, i, edge.getTime(), true);
                        }
                    }
                    ig.setColor(defaultSignalColor);
                    this.drawSignalLine(ig, i, edge.getTime(), nextEdge.getTime(), edge.getSignal(), change);
                    if(change && !risingEdge)
                        this.drawTransition(ig, i, nextEdge.getTime(), false);
                    else if(change && risingEdge)
                        this.drawTransition(ig, i, nextEdge.getTime(), true);

                } else if(signal instanceof Bus) {
                    BusEdge edge = (BusEdge) edgesWithinRange.get(j);
                    BusEdge nextEdge = (BusEdge) edgesWithinRange.get(j+1);
                    boolean change = edge.getSignal() != nextEdge.getSignal();

                    if(j == 0 && edge.getTime() != domainStart) {
                        ArrayList<BusEdge> edgesBeforeDomainStart = tD.getSignal(i).getEdgesWithinRange(0, domainStart);
                        if(edgesBeforeDomainStart.isEmpty()) {
                            ig.setColor(new Color(200, 200, 200));
                            this.drawUnknownSignal(ig, i, 0, edge.getTime(), true);
                        } else {
                            BusEdge lastEdge = edgesBeforeDomainStart.get(edgesBeforeDomainStart.size()-1);
                            boolean firstChange = lastEdge.getSignal() != edge.getSignal();
                            this.drawBusSignal(ig, i, 0, edge.getTime(), edge.getSignal(), firstChange);
                            this.drawBusTransition(ig, i, edge.getTime());
                        }
                    }
                    ig.setColor(defaultSignalColor);

                    ig.drawString(String.format("%08x", edge.getSignal()), (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + 10, (int)((i+.5)*sigHeight));
                    this.drawBusSignal(ig, i, edge.getTime(), nextEdge.getTime(), edge.getSignal(), change);
                    this.drawBusTransition(ig, i, nextEdge.getTime());
                }
            }

            // Handle last edge
            if(signal instanceof Line && edgesWithinRange.size() > 0) {
                LineEdge lastEdge = (LineEdge) edgesWithinRange.get(edgesWithinRange.size() - 1);
                LineEdge secondFromLastEdge = null;
                if(edgesWithinRange.size() > 1)
                    secondFromLastEdge = (LineEdge) edgesWithinRange.get(edgesWithinRange.size() - 2);
                else if(signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                    secondFromLastEdge = (LineEdge) signal.getEdgesWithinRange(0, domainStart).get(signal.getEdgesWithinRange(0, domainStart).size()-1);
                    boolean change = secondFromLastEdge.getSignal() != lastEdge.getSignal();
                    this.drawSignalLine(ig, i, 0, lastEdge.getTime(), secondFromLastEdge.getSignal(), change);
                } else {
                    ig.setColor(new Color(200, 200, 200));
                    this.drawUnknownSignal(ig, i, 0, lastEdge.getTime(), true);
                }
                ig.setColor(defaultSignalColor);
                this.drawSignalLine(ig, i, lastEdge.getTime(), W,lastEdge.getSignal(), false);
                if(secondFromLastEdge != null && lastEdge.getSignal() != secondFromLastEdge.getSignal()) {
                    if(secondFromLastEdge.getSignal() == 1 && lastEdge.getSignal() == 0) {
                        this.drawTransition(ig, i, lastEdge.getTime(), false);
                    } else {
                        this.drawTransition(ig, i, lastEdge.getTime(), true);
                    }
                }
            } else if(signal instanceof Bus && edgesWithinRange.size() > 0) {
                BusEdge lastEdge = (BusEdge) edgesWithinRange.get(edgesWithinRange.size() - 1);
                BusEdge secondFromLastEdge = null;
                if(edgesWithinRange.size() > 1)
                    secondFromLastEdge = (BusEdge) edgesWithinRange.get(edgesWithinRange.size() - 2);
                else if(signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                    secondFromLastEdge = (BusEdge) signal.getEdgesWithinRange(0, domainStart).get(signal.getEdgesWithinRange(0, domainStart).size()-1);
                    boolean change = secondFromLastEdge.getSignal() != lastEdge.getSignal();
                    this.drawBusSignal(ig, i, 0, lastEdge.getTime(), lastEdge.getSignal(), change);
                } else {
                    ig.setColor(new Color(200, 200, 200));
                    this.drawUnknownSignal(ig, i, 0, lastEdge.getTime(), true);
                }
                ig.setColor(defaultSignalColor);
                ig.drawString(String.format("%08x", lastEdge.getSignal()), (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + 10, (int)((i+.5)*sigHeight));
                this.drawBusSignal(ig, i, lastEdge.getTime(), W, lastEdge.getSignal(), false);
                if(secondFromLastEdge != null && lastEdge.getSignal() != secondFromLastEdge.getSignal()) {
                    this.drawBusTransition(ig, i, lastEdge.getTime());
                }
            }

            // No edge in the domain, but there's one right before
            if(signal instanceof Line && edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                ArrayList<LineEdge> edges = signal.getEdgesWithinRange(0, domainStart);
                this.drawSignalLine(ig, i, 0, W, (edges.get(edges.size() - 1)).getSignal(), false);
            } else if(signal instanceof Bus && edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                ArrayList<BusEdge> edges = signal.getEdgesWithinRange(0, domainStart);
                ig.drawString(String.format("%08x", (edges.get(edges.size() - 1)).getSignal()), 10, (int)((i+.5)*sigHeight));
                this.drawBusSignal(ig, i, 0, W, -1L, false);
            }

            // Empty
            if(edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).isEmpty()) {
                ig.setColor(new Color(200, 200, 200));
                this.drawUnknownSignal(ig, i, 0, W, true);
            }

            if(signal.getName() != null) {
                int stringH = ig.getFontMetrics().getHeight();
                int StringD = ig.getFontMetrics().getDescent();
                ig.setColor(Color.white);
                ig.fillRect(0, i*sigHeight+StringD, ig.getFontMetrics().stringWidth(signal.getName())+10, stringH);
                ig.setColor(textColor);
                ig.drawString(signal.getName(), 5, i*sigHeight+stringH);
            }
        }

        ig.setColor(Color.red);
        if(yCursor > -1) {
            ig.drawLine(yCursor, 0, yCursor, H);
            double xVal = (yCursor / (double)(W)) * (domainEnd-domainStart);
            ig.drawString(String.format("%.2f", xVal+domainStart), yCursor+5, H-5);

            for(int i = 0; i < numberOfSignals; i++) {
                int yPos = (i+1)*sigHeight - 5;
                Signal signal = tD.getSignal(i);
                if(signal instanceof Bus) {
                    ArrayList<BusEdge> edges = signal.getEdgesWithinRange(0, xVal+domainStart);
                    if(edges.size() > 0) {
                        String value = String.format("%08x", edges.get(edges.size() - 1).getSignal());
                        int stringH = ig.getFontMetrics().getHeight();
                        int StringD = ig.getFontMetrics().getDescent();
                        ig.setColor(Color.white);
                        ig.fillRect(yCursor+1, yPos-stringH+StringD, ig.getFontMetrics().stringWidth(value)+10, stringH);
                        ig.setColor(Color.red);
                        ig.drawString(value, yCursor+6, yPos);
                    }
                }
            }
        }

        g.drawImage(image, 0, 0, this);
    }

    public void drawTransition(Graphics2D ig, int i, double time, boolean rising) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.drawLine((int) ((time - domainStart) / (domainEnd - domainStart) * W) + (rising ? 1 : -1)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                   (int) (i*sigHeight+yMargin),
                   (int) ((time - domainStart) / (domainEnd - domainStart) * W) + (rising ? -1 : 1)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                   (int) ((i+1)*sigHeight-yMargin));
    }

    public void drawSignalLine(Graphics2D ig, int i, double start, double end, int signal, boolean xTransitionOffset) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.drawLine((int) ((start - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                   (int) (signal == 1 ? i*sigHeight+yMargin : i*sigHeight+sigHeight-yMargin),
                   (int) ((end - domainStart) / (domainEnd - domainStart) * W) + (xTransitionOffset ? -1 : 0)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                   (int) (signal == 1 ? i*sigHeight+yMargin : i*sigHeight+sigHeight-yMargin));
    }

    public void drawUnknownSignal(Graphics2D ig, int i, double start, double end, boolean xTransitionOffset) {
        ig.setColor(new Color(200, 200, 200));
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.fillRect((int) ((start - domainStart) / (domainEnd - domainStart) * W),
                    i*sigHeight+yMargin,
                    (int) ((end - start) / (domainEnd - domainStart) * W) + (xTransitionOffset ? 1 : 0)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                    sigHeight-2*yMargin);
    }
    
    public void drawBusTransition(Graphics2D ig, int i, double time) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.drawLine((int) ((time - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) (i*sigHeight+yMargin),
                   (int) ((time - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) ((i+1)*sigHeight-yMargin));
        ig.drawLine((int) ((time - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) (i*sigHeight+yMargin),
                   (int) ((time - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) ((i+1)*sigHeight-yMargin));
    }

    public void drawBusSignal(Graphics2D ig, int i, double start, double end, long signal, boolean xTransitionOffset) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.drawLine((int) ((start - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) (i*sigHeight+yMargin),
                   (int) ((end - domainStart) / (domainEnd - domainStart) * W) + (xTransitionOffset ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) (i*sigHeight+yMargin));
        ig.drawLine((int) ((start - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) ((i+1)*sigHeight-yMargin),
                   (int) ((end - domainStart) / (domainEnd - domainStart) * W) + (xTransitionOffset ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                   (int) ((i+1)*sigHeight-yMargin));
    }

    public void drawLine(Graphics2D ig, int i, double start, double end, boolean sigChange) {

    }

    @Override
    public void mouseExited(MouseEvent me) {

    }

    @Override
    public void mouseEntered(MouseEvent me) {
        
    }

    @Override
    public void mouseReleased(MouseEvent me) {

    }

    @Override
    public void mousePressed(MouseEvent me) {

    }

    @Override
    public void mouseClicked(MouseEvent me) {
        setYCursor(me.getX());
        repaint();
    }
}

