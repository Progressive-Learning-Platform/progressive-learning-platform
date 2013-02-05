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
    private final int STATS_MARGIN = 100;
    private final int LEFT_MARGIN = 5;
    private final int RIGHT_MARGIN = 5;
    private final int TOP_MARGIN = 5+STATS_MARGIN;
    private final int BOTTOM_MARGIN = 5;

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
        
        if(f.e == null) {
            g2.setColor(Color.YELLOW);
            g2.setFont(NOTICE);
            str = "Cache is not initialized";
            g2.drawString(str, 2, 2 + getTotalHeight(g2, str, NOTICE));
            return;
        } else {
            int ratio;
            int sets = f.e.blocks / f.e.associativity;
            int setPixels = (getHeight()-TOP_MARGIN-BOTTOM_MARGIN) / sets;
            int traceEntryPixels = (getWidth()-LEFT_MARGIN-RIGHT_MARGIN) / f.e.TRACE_SIZE;

            for(i = f.e.trace.size()-1; i >= 0; i--) {
                j = f.e.trace.size()-i-1;
                g2.setFont(SET_HEADER);
                if(j == 0) {
                    c = Color.WHITE;
                } else {
                    ratio = (int) (((float)(f.e.TRACE_SIZE-j)/(float)f.e.TRACE_SIZE) * 255);
                    c = new Color(ratio, ratio, 0);
                }
                g2.setColor(c);
                str = PLPToolbox.format32Hex(f.e.trace.get(i));
                g2.drawString(str, LEFT_MARGIN+(j)*traceEntryPixels, 15+getTotalHeight(g2, str, NOTICE));
            }

            if(f.e.lastAccess != -1) {
                g2.setColor(Color.YELLOW);
                str = "Tag : " + PLPToolbox.format32Hex(f.e.lastAccess >> (f.e.blockOffset+f.e.indexBits));
                str += " - Index : " + ((f.e.lastAccess >> f.e.blockOffset) % (f.e.blocks / f.e.associativity));
                g2.drawString(str, LEFT_MARGIN, 15+15+2*getTotalHeight(g2, str, NOTICE));
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
            }
        }
    }
    
    private LineMetrics getMetrics(Graphics2D g, String str, Font f) {
        FontRenderContext frc = g.getFontRenderContext();
        return f.getLineMetrics(str, frc);
    }
    
    private int getTotalHeight(Graphics2D g, String str, Font f) {
        return (int) (getMetrics(g, str, f).getDescent());
    }
}

