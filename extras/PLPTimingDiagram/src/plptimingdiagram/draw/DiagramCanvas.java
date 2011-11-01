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
    private final int DEFAULT_SIGNAL_SLANT_PIXELS = 0;
    private final int DEFAULT_BUS_SLANT_PIXELS = 4;

    private TimingDiagram tD;
    private XAxis axis;
    private int xAxisHeight = DEFAULT_X_AXIS_HEIGHT_PIXELS;

    // Colors
    private Color background = Color.white;
    private Color defaultSignalColor = new Color(35, 35, 35);
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

    @Override
    public void paint(Graphics g1) {
        if(axis.getIntervals() == 0 || axis.nonPositiveDomain())
            return;

        this.setSize(this.getParent().getWidth(), this.getParent().getHeight());
        int H = this.getHeight();
        int W = this.getWidth();
        int numberOfSignals = tD.getNumberOfSignals();
        int sigHeight = (H - xAxisHeight) / numberOfSignals;
        Graphics2D g = (Graphics2D) g1;
        BufferedImage image = new BufferedImage(W, H, BufferedImage.TYPE_INT_RGB);
        Graphics2D ig = image.createGraphics();

        ig.setFont(new Font("Monospaced", Font.BOLD, 12));
        double domainIntervalWidth = W / (double) axis.getIntervals();
        ig.setColor(background);
        ig.fillRect(0, 0, W, H);

        ig.setStroke(new BasicStroke(1.0f));
        ig.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();

        // Draw our grid
        double interval = (domainEnd - domainStart) / axis.getIntervals();

        for(int i = 0; i < numberOfSignals; i++) {
            ig.setColor(new Color(240, 240, 240));
            ig.fillRect(0, (int) (i * sigHeight + 0.20 * sigHeight), W, (int) (0.60 * sigHeight));
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
            if(signal.getName() != null) {
                ig.setColor(textColor);
                ig.drawString(signal.getName(), 5, (i+1)*sigHeight);
            }
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
                            ig.fillRect(0, (int)((i+0.2)*sigHeight), (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W), (int)(i+0.6*sigHeight));
                        } else {
                            LineEdge lastEdge = edgesBeforeDomainStart.get(edgesBeforeDomainStart.size()-1);
                            boolean firstChange = lastEdge.getSignal() != edge.getSignal();
                            ig.drawLine(0,
                                        (int) ((i+(lastEdge.getSignal() == 1 ? .20 : .80))*sigHeight),
                                        (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (firstChange ? -1 : 1)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                        (int) ((i+(lastEdge.getSignal() == 1 ? .20 : .80))*sigHeight));
                            if(lastEdge.getSignal() == 1 && edge.getSignal() == 0)
                                ig.drawLine((int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                           (int) ((i+.20)*sigHeight),
                                           (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                           (int) ((i+.80)*sigHeight));
                            else if(lastEdge.getSignal() == 0 && edge.getSignal() == 1)
                                ig.drawLine((int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                           (int) ((i+.20)*sigHeight),
                                           (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                           (int) ((i+.80)*sigHeight));
                        }
                    }
                    ig.setColor(defaultSignalColor);

                    ig.drawLine((int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                               (int) ((i+(edge.getSignal() == 1 ? .20 : .80))*sigHeight),
                               (int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (change ? -1 : 1)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                               (int) ((i+(edge.getSignal() == 1 ? .20 : .80))*sigHeight));
                    if(change && !risingEdge)
                        ig.drawLine((int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.20)*sigHeight),
                                   (int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.80)*sigHeight));
                    else if(change && risingEdge)
                        ig.drawLine((int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.20)*sigHeight),
                                   (int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.80)*sigHeight));
                } else if(signal instanceof Bus) {
                    BusEdge edge = (BusEdge) edgesWithinRange.get(j);
                    BusEdge nextEdge = (BusEdge) edgesWithinRange.get(j+1);
                    boolean change = edge.getSignal() != nextEdge.getSignal();

                    if(j == 0 && edge.getTime() != domainStart) {
                        ArrayList<BusEdge> edgesBeforeDomainStart = tD.getSignal(i).getEdgesWithinRange(0, domainStart);
                        if(edgesBeforeDomainStart.isEmpty()) {
                            ig.setColor(new Color(200, 200, 200));
                            ig.fillRect(0, (int)((i+0.2)*sigHeight), (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W), (int)(i+0.6*sigHeight));
                        } else {
                            BusEdge lastEdge = edgesBeforeDomainStart.get(edgesBeforeDomainStart.size()-1);
                            boolean firstChange = lastEdge.getSignal() != edge.getSignal();
                            ig.drawLine(0,
                                        (int) ((i+.20)*sigHeight),
                                        (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (firstChange ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                                        (int) ((i+.20)*sigHeight));
                            ig.drawLine(0,
                                        (int) ((i+.80)*sigHeight),
                                        (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (firstChange ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                                        (int) ((i+.80)*sigHeight));
                            ig.drawString(String.format("%08x", lastEdge.getSignal()), 10, (int)((i+.5)*sigHeight));
                            ig.drawLine((int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                                       (int) ((i+.20)*sigHeight),
                                       (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                                       (int) ((i+.80)*sigHeight));
                            ig.drawLine((int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                                       (int) ((i+.20)*sigHeight),
                                       (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                                       (int) ((i+.80)*sigHeight));
                        }
                    }
                    ig.setColor(defaultSignalColor);

                    ig.drawString(String.format("%08x", edge.getSignal()), (int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + 10, (int)((i+.5)*sigHeight));
                    ig.drawLine((int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.20)*sigHeight),
                               (int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (change ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.20)*sigHeight));
                    ig.drawLine((int) ((edge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.80)*sigHeight),
                               (int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (change ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.80)*sigHeight));
                    ig.drawLine((int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.20)*sigHeight),
                               (int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.80)*sigHeight));
                    ig.drawLine((int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.20)*sigHeight),
                               (int) ((nextEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.80)*sigHeight));
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
                    ig.drawLine(0,
                            (int) ((i+(secondFromLastEdge.getSignal() == 1 ? .20 : .80))*sigHeight),
                            (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (change ? -1 : 1)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                            (int) ((i+(secondFromLastEdge.getSignal() == 1 ? .20 : .80))*sigHeight));
                } else {
                    ig.setColor(new Color(200, 200, 200));
                    ig.fillRect(0, (int)((i+0.2)*sigHeight), (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W), (int)(i+0.6*sigHeight));
                }
                ig.setColor(defaultSignalColor);
                ig.drawLine((int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                               (int) ((i+(lastEdge.getSignal() == 1 ? .20 : .80))*sigHeight),
                               W,
                               (int) ((i+(lastEdge.getSignal() == 1 ? .20 : .80))*sigHeight));
                if(secondFromLastEdge != null && lastEdge.getSignal() != secondFromLastEdge.getSignal()) {
                    if(secondFromLastEdge.getSignal() == 1 && lastEdge.getSignal() == 0) {
                        ig.drawLine((int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.20)*sigHeight),
                                   (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.80)*sigHeight));
                    } else {
                        ig.drawLine((int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.20)*sigHeight),
                                   (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_SIGNAL_SLANT_PIXELS,
                                   (int) ((i+.80)*sigHeight));
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
                    ig.drawString(String.format("%08x", secondFromLastEdge.getSignal()), 10, (int)((i+.5)*sigHeight));
                    ig.drawLine(0,
                            (int) ((i+.20)*sigHeight),
                            (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (change ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                            (int) ((i+.20)*sigHeight));
                    ig.drawLine(0,
                            (int) ((i+.80)*sigHeight),
                            (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + (change ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                            (int) ((i+.80)*sigHeight));
                } else {
                    ig.setColor(new Color(200, 200, 200));
                    ig.fillRect(0, (int)((i+0.2)*sigHeight), (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W), (int)(i+0.6*sigHeight));
                }
                ig.setColor(defaultSignalColor);
                ig.drawString(String.format("%08x", lastEdge.getSignal()), (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + 10, (int)((i+.5)*sigHeight));
                ig.drawLine((int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                            (int) ((i+.20)*sigHeight),
                            W,
                            (int) ((i+.20)*sigHeight));
                ig.drawLine((int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                            (int) ((i+.80)*sigHeight),
                            W,
                            (int) ((i+.80)*sigHeight));
                if(secondFromLastEdge != null && lastEdge.getSignal() != secondFromLastEdge.getSignal()) {
                    ig.drawLine((int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.20)*sigHeight),
                               (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.80)*sigHeight));
                    ig.drawLine((int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) + this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.20)*sigHeight),
                               (int) ((lastEdge.getTime() - domainStart) / (domainEnd - domainStart) * W) - this.DEFAULT_BUS_SLANT_PIXELS,
                               (int) ((i+.80)*sigHeight));
                }
            }

            // No edge in the domain, but there's one right before
            if(signal instanceof Line && edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                ArrayList<LineEdge> edges = signal.getEdgesWithinRange(0, domainStart);
                ig.drawLine(0,
                            (int) ((i+((edges.get(edges.size() - 1)).getSignal() == 1 ? 0.20 : 0.80)) * sigHeight),
                            W,
                            (int) ((i+((edges.get(edges.size() - 1)).getSignal() == 1 ? 0.20 : 0.80)) * sigHeight));
            } else if(signal instanceof Bus && edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                ArrayList<BusEdge> edges = signal.getEdgesWithinRange(0, domainStart);
                ig.drawString(String.format("%08x", (edges.get(edges.size() - 1)).getSignal()), 10, (int)((i+.5)*sigHeight));
                ig.drawLine(0,
                            (int) ((i+.20) * sigHeight),
                            W,
                            (int) ((i+.20) * sigHeight));
                ig.drawLine(0,
                            (int) ((i+.80) * sigHeight),
                            W,
                            (int) ((i+.80) * sigHeight));
            }

            // Empty
            if(edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).isEmpty()) {
                ig.setColor(new Color(200, 200, 200));
                            ig.fillRect(0, (int)((i+0.2)*sigHeight),W, (int)(i+0.6*sigHeight));
            }
        }

        ig.setColor(Color.red);
        if(yCursor > -1) {
            ig.drawLine(yCursor, 0, yCursor, H);
            double xVal = (yCursor / (double)W) * (domainEnd - domainStart);
            ig.drawString(String.format("%.2f", xVal+domainStart), yCursor+5, H-5);

            for(int i = 0; i < numberOfSignals; i++) {
                int yPos = (i+1)*sigHeight;
                Signal signal = tD.getSignal(i);
                if(signal instanceof Bus) {
                    ArrayList<BusEdge> edges = signal.getEdgesWithinRange(0, xVal+domainStart);
                    if(edges.size() > 0) {
                        String value = String.format("%08x", edges.get(edges.size() - 1).getSignal());
                        ig.drawString(value, yCursor+5, yPos);
                    }
                }
            }
        }

        g.drawImage(image, 0, 0, this);
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

