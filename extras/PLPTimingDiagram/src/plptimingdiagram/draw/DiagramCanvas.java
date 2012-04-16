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
import java.awt.event.MouseMotionListener;
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class DiagramCanvas extends JPanel implements MouseListener, MouseMotionListener {
    private int DEFAULT_X_AXIS_HEIGHT_PIXELS = 60;
    private int DEFAULT_SIGNAL_SLANT_PIXELS = 2;
    private int DEFAULT_BUS_SLANT_PIXELS = 4;

    private TimingDiagram tD;
    private XAxis axis;
    private int xAxisHeight = DEFAULT_X_AXIS_HEIGHT_PIXELS;

    private boolean cursorEnable = true;
    private boolean hoverCursorEnable = true;
    private boolean removeOnDoubleClick = false;

    // Colors
    private Color background = Color.white;
    private Color defaultSignalColor = new Color(135, 0, 0);
    private Color majorGrid = new Color(200, 200, 200);
    private Color minorGrid = new Color(220, 220, 220);
    private Color textColor = Color.black;
    private Color leftMarginBackgroundColor = new Color(200, 200, 235);
    private Color activeCursor = Color.red;
    private Color activeCursorText = Color.red;
    private Color activeCursorSignalText = Color.red;
    private Color hoverCursor = Color.gray;
    private Color hoverCursorFill = Color.gray;
    private Color hoverCursorDelta = Color.darkGray;

    // Cursor
    private int yCursor = -1;
    private int yHoverCursor = -1;

    public DiagramCanvas(TimingDiagram tD, XAxis axis) {
        super();
        this.tD = tD;
        this.axis = axis;
        this.addMouseListener(this);
        this.addMouseMotionListener(this);
    }

    public void setCursorEnable(boolean b) {
        this.cursorEnable = b;
    }

    public void setHoverCursorEnable(boolean b) {
        this.hoverCursorEnable = b;
    }

    public void setRemoveSignalOnDoubleClick(boolean b) {
        this.removeOnDoubleClick = b;
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

    public void setTransitionSlantPixels(int signal, int bus) {
        DEFAULT_SIGNAL_SLANT_PIXELS = signal;
        DEFAULT_BUS_SLANT_PIXELS = bus;
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

    public int getYCursor() {
        return yCursor;
    }

    public void setRemoveOnDoubleClick(boolean b) {
        removeOnDoubleClick = b;
    }

    public void doZoomInOnCursor(double ratio) {
        double start = axis.getDomainStart();
        double end = axis.getDomainEnd();
        double center = this.getYCursorTime()+start;
        double leftRatio = (center-start)/(end-start);
        double rightRatio = (end-center)/(end-start);
        double newDomainWidth = (1/ratio)*(end-start);
        double leftMargin = newDomainWidth*leftRatio;
        double rightMargin = newDomainWidth*rightRatio;

        if(center >= 0) {
            axis.setDomain(start+leftMargin, end-rightMargin);
            repaint();
        }
    }

    public void doZoomOutOnCursor(double ratio) {
        double start = axis.getDomainStart();
        double end = axis.getDomainEnd();
        double center = this.getYCursorTime()+start;
        double leftRatio = (center-start)/(end-start);
        double rightRatio = (end-center)/(end-start);
        double newDomainWidth = ratio*(end-start);
        double leftMargin = newDomainWidth*leftRatio;
        double rightMargin = newDomainWidth*rightRatio;

        if(start-leftMargin < 0)
            leftMargin = start;

        if(center >= 0) {
            axis.setDomain(start-leftMargin, end+rightMargin);
            repaint();
        }
    }

    public void doLeftShift(double ratio) {
        if(ratio > 0 && ratio <= 1) {
            double start = axis.getDomainStart();
            double end = axis.getDomainEnd();
            double shift = ratio * (end-start);

            if(start-shift >= 0) {
                axis.setDomain(start-shift, end-shift);
                repaint();
            }
        }
    }

    public void doRightShift(double ratio) {
        if(ratio > 0 && ratio <= 1) {
            double start = axis.getDomainStart();
            double end = axis.getDomainEnd();
            double shift = ratio * (end-start);
            axis.setDomain(start+shift, end+shift);
            repaint();
        }
    }


    public double getYCursorTime() {
        return ((yCursor-xOffset) / (double)(W-xOffset)) * (axis.getDomainEnd()-axis.getDomainStart());
    }

    private int H;
    private int W;
    private int sigHeight;
    private int sigCaptionOffset;
    private int yMargin;
    private int xOffset;
    private boolean onlyDrawHoverOverlay = false;

    @Override
    public void paint(Graphics g1) {
        if(axis.getIntervals() == 0 || axis.nonPositiveDomain())
            return;

        //if(onlyDrawHoverOverlay) {
        //    this.drawHoverCursorOverlay(g1);
        //    return;
        //}

        if(!this.getSize().equals(this.getParent().getSize()))
            this.setSize(this.getParent().getWidth(), this.getParent().getHeight());
        H = this.getHeight();
        W = this.getWidth();

        Graphics2D g = (Graphics2D) g1;
        BufferedImage image = new BufferedImage(W, H, BufferedImage.TYPE_INT_RGB);
        Graphics2D ig = image.createGraphics();

        int numberOfSignals = tD.getNumberOfSignals();
        if(numberOfSignals > 0)
            sigHeight = 45; //(H - xAxisHeight) / numberOfSignals;
        
        yMargin = 10;
        xOffset = 100;

        ig.setFont(new Font("sansserif", Font.BOLD, 12));
        sigCaptionOffset = ig.getFontMetrics().getHeight() + 5;
        double domainIntervalWidth = (W-xOffset) / (double) axis.getIntervals();
        ig.setColor(background);
        ig.fillRect(0, 0, W, H);

        ig.setStroke(new BasicStroke(1.0f));
        ig.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);

        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();

        for(int i = 0; i < numberOfSignals; i++) {
            if(i % 2 == 0)
                ig.setColor(new Color(250, 250, 250));
            else
                ig.setColor(new Color(240, 240, 240));
            ig.fillRect(0, (int) (i * sigHeight), W, (int) (sigHeight));
            ig.setColor(new Color(240, 240, 240));
            //ig.fillRect(0, (int) (i * sigHeight + 0.20 * sigHeight), W, (int) (0.60 * sigHeight));
        }

        ig.setColor(leftMarginBackgroundColor);
        ig.fillRect(0, 0, xOffset, H);
        // Draw our grid
        double interval = (domainEnd - domainStart) / axis.getIntervals();
        for(int i = 0; i < axis.getIntervals(); i++) {
            ig.setColor(minorGrid);
            ig.setStroke(new BasicStroke(1.0f, BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 3.0f, new float[] {5.0f,7.0f}, 0.0f));
            ig.drawLine(xOffset+(int) (i*domainIntervalWidth), 0, xOffset+(int) (i*domainIntervalWidth), H);
            ig.setColor(textColor);
            ig.setStroke(new BasicStroke(1.2f));
            ig.drawString(String.format("%d", (int) (i*interval + domainStart)), xOffset+(int) (i*domainIntervalWidth+5), (H-xAxisHeight) + g.getFontMetrics().getHeight()+20);
        }
        ig.drawLine(xOffset, H - xAxisHeight + 10, W, H - xAxisHeight + 10);

        // Draw our timing diagram
        ig.setStroke(new BasicStroke(1.2f));
        for(int i = 0; i < numberOfSignals; i++) {
            Signal signal = tD.getSignal(i);
            ArrayList edgesWithinRange = null;
            edgesWithinRange = signal.getEdgesWithinRange(domainStart, domainEnd);
                
            ig.setColor(defaultSignalColor);
            for(int j = 0; signal != null && j < edgesWithinRange.size() - 1; j++) {
                ArrayList edgesBeforeDomainStart = signal.getEdgesWithinRange(0, domainStart);
                if(signal instanceof Line) {
                    LineEdge edge = (LineEdge) edgesWithinRange.get(j);
                    LineEdge nextEdge = (LineEdge) edgesWithinRange.get(j+1);
                    boolean change = edge.getSignal() != nextEdge.getSignal();
                    boolean risingEdge = edge.getSignal() == 0 && nextEdge.getSignal() == 1;
                    if(j == 0 && edge.getTime() != domainStart) {
                        
                        if(edgesBeforeDomainStart.isEmpty()) {
                            ig.setColor(new Color(200, 200, 200));
                            this.drawUnknownSignal(ig, i, domainStart, edge.getTime(), true);
                        } else {
                            LineEdge lastEdge = (LineEdge) edgesBeforeDomainStart.get(edgesBeforeDomainStart.size()-1);
                            boolean firstChange = lastEdge.getSignal() != edge.getSignal();
                            this.drawSignalLine(ig, i, domainStart, edge.getTime(), lastEdge.getSignal(), firstChange);
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
                        if(edgesBeforeDomainStart.isEmpty()) {
                            ig.setColor(new Color(200, 200, 200));
                            this.drawUnknownSignal(ig, i, domainStart, edge.getTime(), true);
                        } else {
                            BusEdge lastEdge = (BusEdge) edgesBeforeDomainStart.get(edgesBeforeDomainStart.size()-1);
                            boolean firstChange = lastEdge.getSignal() != edge.getSignal();
                            this.drawBusSignal(ig, i, domainStart, edge.getTime(), lastEdge.getSignal(), firstChange);
                            this.drawBusTransition(ig, i, edge.getTime());
                        }
                    }
                    ig.setColor(defaultSignalColor);
                    
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
                    this.drawSignalLine(ig, i, domainStart, lastEdge.getTime(), secondFromLastEdge.getSignal(), change);
                } else {
                    ig.setColor(new Color(200, 200, 200));
                    this.drawUnknownSignal(ig, i, domainStart, lastEdge.getTime(), true);
                }
                ig.setColor(defaultSignalColor);
                this.drawSignalLine(ig, i, lastEdge.getTime(), domainEnd,lastEdge.getSignal(), false);
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
                    this.drawBusSignal(ig, i, domainStart, lastEdge.getTime(), secondFromLastEdge.getSignal(), change);
                } else {
                    ig.setColor(new Color(200, 200, 200));
                    this.drawUnknownSignal(ig, i, domainStart, lastEdge.getTime(), true);
                }
                ig.setColor(defaultSignalColor);
                this.drawBusSignal(ig, i, lastEdge.getTime(), domainEnd, lastEdge.getSignal(), false);
                if(secondFromLastEdge != null && lastEdge.getSignal() != secondFromLastEdge.getSignal()) {
                    this.drawBusTransition(ig, i, lastEdge.getTime());
                }
            }

            // No edge in the domain, but there's one right before
            if(signal instanceof Line && edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                ArrayList<LineEdge> edges = signal.getEdgesWithinRange(0, domainStart);
                this.drawSignalLine(ig, i, domainStart, domainEnd, (edges.get(edges.size() - 1)).getSignal(), false);
            } else if(signal instanceof Bus && edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).size() > 0) {
                ArrayList<BusEdge> edges = signal.getEdgesWithinRange(0, domainStart);
                this.drawBusSignal(ig, i, domainStart, domainEnd, (edges.get(edges.size() - 1)).getSignal(), false);
            }

            // Empty
            if(edgesWithinRange.isEmpty() && signal.getEdgesWithinRange(0, domainStart).isEmpty()) {
                ig.setColor(new Color(200, 200, 200));
                this.drawUnknownSignal(ig, i, domainStart, domainEnd, true);
            }

            if(signal.getName() != null) {
                int stringH = ig.getFontMetrics().getHeight();
                int stringD = ig.getFontMetrics().getDescent();
                ig.setColor(Color.white);
                ig.fillRect(0, stringD+ (int) (i*sigHeight+(0.5*sigHeight-0.5*ig.getFontMetrics().getHeight())), ig.getFontMetrics().stringWidth(signal.getName())+10, stringH);
                ig.setColor(textColor);
                ig.drawString(signal.getName(), 5, (int) (i*sigHeight+(0.5*sigHeight+0.5*ig.getFontMetrics().getHeight())));
            }
        }

        drawHoverCursorOverlay(ig);
        
        if(cursorEnable && yCursor > -1 && yCursor >= xOffset) {
            int stringH = ig.getFontMetrics().getHeight();
            int StringD = ig.getFontMetrics().getDescent();

            ig.setColor(this.activeCursor);
            ig.drawLine(yCursor, 0, yCursor, H);
            double xVal = ((yCursor-xOffset) / (double)(W-xOffset)) * (domainEnd-domainStart);
            String time = String.format("%.2f", xVal+domainStart);
            ig.setColor(Color.white);
            ig.fillRect(yCursor+1, H-stringH+StringD-10, ig.getFontMetrics().stringWidth(time)+10, stringH+10);
            ig.setColor(this.activeCursorText);
            ig.drawString(time, yCursor+6, H-5);

            for(int i = 0; i < numberOfSignals; i++) {
                int yPos = (i+1)*sigHeight + yMargin - 5;
                Signal signal = tD.getSignal(i);
                if(signal instanceof Bus) {
                    ArrayList<BusEdge> edges = signal.getEdgesWithinRange(0, xVal+domainStart);
                    if(edges.size() > 0) {
                        String value = String.format("%08x", edges.get(edges.size() - 1).getSignal());
                        ig.setColor(Color.white);
                        ig.fillRect(yCursor+1, yPos-stringH+StringD, ig.getFontMetrics().stringWidth(value)+10, stringH);
                        ig.setColor(this.activeCursorSignalText);
                        ig.drawString(value, yCursor+6, yPos);
                    }
                }
            }
        }

        g.drawImage(image, 0, 0, this);
    }

    public void drawHoverCursorOverlay(Graphics g) {
        if(hoverCursorEnable && yHoverCursor > -1 && yHoverCursor >= xOffset) {
            g.setColor(this.hoverCursor);
            g.drawLine(yHoverCursor, 0, yHoverCursor, H);
            if(yCursor != -1) {
                double delta = convertXPixelsToTime(yCursor) - convertXPixelsToTime(yHoverCursor);
                int center = H-(int)(g.getFontMetrics().getHeight()/2.0)-5+g.getFontMetrics().getDescent();
                int[] x = {3, 6, 9};
                int[] y = {center+3, center-3, center+3};
                g.setColor(this.hoverCursorDelta);
                g.drawPolygon(x, y, 3);
                g.drawString(String.format("%.2f", delta), 13, H-5);
                g.setColor(this.hoverCursorFill);
                if(yHoverCursor<=yCursor)
                    g.fillRect(yHoverCursor, H-10, yCursor-yHoverCursor, 10);
                else
                    g.fillRect(yCursor, H-10, yHoverCursor-yCursor, 10);
            }
        }
    }

    public double convertXPixelsToTime(int x) {
        return ((x-xOffset) / (double)(W-xOffset)) * (axis.getDomainEnd()-axis.getDomainStart());
    }

    public void drawTransition(Graphics2D ig, int i, double time, boolean rising) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.drawLine(xOffset+(int) ((time - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + (rising ? 1 : -1)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                    (int) (i*sigHeight+yMargin),
                    xOffset+(int) ((time - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + (rising ? -1 : 1)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                    (int) ((i+1)*sigHeight-yMargin));
    }

    public void drawSignalLine(Graphics2D ig, int i, double start, double end, int signal, boolean xTransitionOffset) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.drawLine(xOffset+(int) ((start - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + this.DEFAULT_SIGNAL_SLANT_PIXELS,
                    (int) (signal == 1 ? i*sigHeight+yMargin : i*sigHeight+sigHeight-yMargin),
                    xOffset+(int) ((end - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + (xTransitionOffset ? -1 : 0)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                    (int) (signal == 1 ? i*sigHeight+yMargin : i*sigHeight+sigHeight-yMargin));
    }

    public void drawUnknownSignal(Graphics2D ig, int i, double start, double end, boolean xTransitionOffset) {
        ig.setColor(new Color(200, 200, 200));
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.fillRect(xOffset+(int) ((start - domainStart) / (domainEnd - domainStart) * (W-xOffset)),
                     i*sigHeight+yMargin,
                     (int) ((end - start) / (domainEnd - domainStart) * (W-xOffset)) + (xTransitionOffset ? 1 : 0)*this.DEFAULT_SIGNAL_SLANT_PIXELS,
                     sigHeight-2*yMargin);
    }
    
    public void drawBusTransition(Graphics2D ig, int i, double time) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        ig.drawLine(xOffset+(int) ((time - domainStart) / (domainEnd - domainStart) * (W-xOffset)) - this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) (i*sigHeight+yMargin),
                    xOffset+(int) ((time - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) ((i+1)*sigHeight-yMargin));
        ig.drawLine(xOffset+(int) ((time - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) (i*sigHeight+yMargin),
                    xOffset+(int) ((time - domainStart) / (domainEnd - domainStart) * (W-xOffset)) - this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) ((i+1)*sigHeight-yMargin));
    }

    public void drawBusSignal(Graphics2D ig, int i, double start, double end, long signal, boolean xTransitionOffset) {
        double domainStart = axis.getDomainStart();
        double domainEnd = axis.getDomainEnd();
        if(ig.getFontMetrics().stringWidth(String.format("%08x", signal))+20 < (end-start) / (domainEnd-domainStart) * (W-xOffset))
            ig.drawString(String.format("%08x", signal), xOffset+(int) ((start - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + 10, (int)((i+.5)*sigHeight+(0.5*ig.getFontMetrics().getHeight())-ig.getFontMetrics().getDescent()));
        ig.drawLine(xOffset+(int) ((start - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) (i*sigHeight+yMargin),
                    xOffset+(int) ((end - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + (xTransitionOffset ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) (i*sigHeight+yMargin));
        ig.drawLine(xOffset+(int) ((start - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) ((i+1)*sigHeight-yMargin),
                    xOffset+(int) ((end - domainStart) / (domainEnd - domainStart) * (W-xOffset)) + (xTransitionOffset ? -1 : 1)*this.DEFAULT_BUS_SLANT_PIXELS,
                    (int) ((i+1)*sigHeight-yMargin));
    }

    public void drawLine(Graphics2D ig, int i, double start, double end, boolean sigChange) {

    }

    public void clearHoverCursor() {
        yHoverCursor = -1;
        repaint();
    }

    @Override
    public void mouseExited(MouseEvent me) {
        yHoverCursor = -1;
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
        if(removeOnDoubleClick && me.getClickCount()==2 && me.getX() < xOffset && sigHeight > 0) {
            int index = me.getY() / sigHeight;
            if(index < tD.getNumberOfSignals()) {
                tD.removeSignal(index);
                repaint();
            }
        }

        if(me.getButton() == MouseEvent.BUTTON1) {
            setYCursor(me.getX());
            repaint();
        }
    }

    @Override
    public void mouseMoved(MouseEvent me) {
        if(!this.hoverCursorEnable)
            return;

        yHoverCursor = me.getX();
        repaint();
    }

    @Override
    public void mouseDragged(MouseEvent me) {

    }
}

