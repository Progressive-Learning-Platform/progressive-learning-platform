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
package plptool.extras.cachesim;

import plptool.*;
import javax.swing.*;
import java.awt.*;
import java.awt.font.*;

/**
 *
 * @author wira
 */
public class CacheVisualizerCanvas extends JPanel {
    
    private DefaultCacheFrame f;
    private int highlightSet = -1;
    
    private Font NOTICE = new Font("Monospaced", Font.BOLD, 36);
    private Font SET_HEADER = new Font("Monospaced", Font.BOLD, 24);
    private Font CONTENT = new Font("Monospaced", Font.PLAIN, 24);
    private int STATS_MARGIN = 120;
    private final int LEFT_MARGIN = 5;
    private final int RIGHT_MARGIN = 5;
    private final int TOP_MARGIN = STATS_MARGIN;
    private final int BOTTOM_MARGIN = 5;
    private final int SET_LEFT_MARGIN = 40;

    public CacheVisualizerCanvas(DefaultCacheFrame f) {
        this.f = f;
    }
    
    public void highlightSet(int index) {
        highlightSet = index;
    }
    
    @Override
    public void paint(Graphics g) {
        this.setSize(this.getParent().getWidth(), this.getParent().getHeight());
        Graphics2D g2 = (Graphics2D) g;
        Color c;
        String str;
        int i, j, k;
        g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2.setColor(Color.BLACK);
        g2.fillRect(0, 0, getWidth(), getHeight());
        STATS_MARGIN = 5+3*getTotalHeight(g2, "Ayr", CONTENT)+20;
        
        if(f.e == null) {
            g2.setColor(Color.YELLOW);
            g2.setFont(NOTICE);
            str = "Cache is not initialized";
            g2.drawString(str, 2, 2 + getTotalHeight(g2, str, NOTICE));
            return;
        } else {
            int ratio, offsetPixels;
            int sets = f.e.blocks / f.e.associativity;
            int setPixels = (getHeight()-TOP_MARGIN-BOTTOM_MARGIN) / sets;
            int traceEntryPixels = (getWidth()-LEFT_MARGIN-RIGHT_MARGIN) / f.e.TRACE_SIZE;
            int assocWidth = (getWidth()-LEFT_MARGIN-RIGHT_MARGIN-SET_LEFT_MARGIN) / f.e.associativity;

            for(i = f.e.getTraceSize()-1; i >= 0; i--) {
                j = f.e.getTraceSize()-i-1;
                g2.setFont(SET_HEADER);
                if(j == 0) {
                    c = Color.WHITE;
                } else {
                    ratio = (int) (((float)(f.e.TRACE_SIZE-j)/(float)f.e.TRACE_SIZE) * 255);
                    c = new Color(ratio, ratio, 0);
                }
                g2.setColor(c);
                str = PLPToolbox.format32Hex(f.e.getTraceItem(i));
                g2.drawString(str, LEFT_MARGIN+(j)*traceEntryPixels, 5+getTotalHeight(g2, str, SET_HEADER));
            }

            if(f.e.lastAccess != -1) {
                g2.setFont(SET_HEADER);
                g2.setColor(Color.YELLOW);
                str = "Tag : " + PLPToolbox.format32Hex(f.e.lastAccess >> (f.e.blockOffset+f.e.indexBits));
                str += " - Index : " + ((f.e.lastAccess >> f.e.blockOffset) % (f.e.blocks / f.e.associativity));
                g2.drawString(str, LEFT_MARGIN, 5+5+2*getTotalHeight(g2, str, SET_HEADER));
                String tmp = String.format("%32s",Long.toBinaryString(f.e.lastAccess)).replace(' ', '0');
                str = tmp.substring(0, 32-f.e.blockOffset-f.e.indexBits);
                offsetPixels = getTextWidth(g2, str);
                g2.setColor(Color.CYAN);
                g2.drawString(str, LEFT_MARGIN, 5+5+5+3*getTotalHeight(g2, str, SET_HEADER));
                g2.setColor(Color.RED);
                str = tmp.substring(32-f.e.blockOffset-f.e.indexBits, 32-f.e.blockOffset);
                g2.drawString(str, LEFT_MARGIN+offsetPixels, 5+5+5+3*getTotalHeight(g2, str, SET_HEADER));
                offsetPixels += getTextWidth(g2, str);
                str = tmp.substring(32-f.e.blockOffset);
                g2.setColor(Color.GRAY);
                g2.drawString(str, LEFT_MARGIN+offsetPixels, 5+5+5+3*getTotalHeight(g2, str, SET_HEADER));
            }
            
            for(i = 0; i < sets; i++) {
                g2.setFont(SET_HEADER);
                if(highlightSet == i) {
                    if(f.e.lastAccessType == 1)
                        g2.setColor(new Color(0, 0, f.e.lastHit ? 60 : 255));
                    else
                        g2.setColor(new Color(f.e.lastHit ? 60 : 255, 0, 0));
                    g2.fillRect(LEFT_MARGIN, TOP_MARGIN+i*setPixels, getWidth()-LEFT_MARGIN-RIGHT_MARGIN, setPixels);
                }
                str = "" + i;
                g2.setColor(Color.YELLOW);
                g2.drawString(str, LEFT_MARGIN, TOP_MARGIN+i*setPixels + setPixels/2 + getTotalHeight(g2, str, SET_HEADER)/2);
                for(j = 0; j < f.e.associativity; j++) {
                    if(!f.e.invalid[i][j]) {
                        g2.setColor(f.e.lastSlot == j && highlightSet == i ? Color.CYAN : f.e.dirty[i][j] ? Color.DARK_GRAY : Color.GRAY);
                        str = PLPToolbox.format32Hex(f.e.linesBase[i][j] >> (f.e.blockOffset+f.e.indexBits));
                        g2.drawString(str, SET_LEFT_MARGIN + j*assocWidth+8, TOP_MARGIN+i*setPixels + setPixels/2 + getTotalHeight(g2, str, SET_HEADER)/2);
                    }
                }
                g2.setColor(Color.DARK_GRAY);
                g2.drawLine(LEFT_MARGIN, TOP_MARGIN+i*setPixels, getWidth()-RIGHT_MARGIN, TOP_MARGIN+i*setPixels);
            }
            
            for(i = 0; i < f.e.associativity; i++) {
                g2.setColor(Color.DARK_GRAY);
                g2.drawLine(SET_LEFT_MARGIN + i*assocWidth, TOP_MARGIN, SET_LEFT_MARGIN + i*assocWidth, getHeight()-BOTTOM_MARGIN);
            }
            
            
        }
    }
    
    private LineMetrics getMetrics(Graphics2D g, String str, Font f) {
        FontRenderContext frc = g.getFontRenderContext();
        return f.getLineMetrics(str, frc);
    }
    
    private int getTotalHeight(Graphics2D g, String str, Font f) {
        return (int) (getMetrics(g, str, f).getAscent()-getMetrics(g, str, f).getLeading()-getMetrics(g, str, f).getDescent());
    }
    private int getTextWidth(Graphics2D g, String str) {
        return (int) (g.getFontMetrics().stringWidth(str));
    }
}

