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
    private final int MARGIN = 2;
    
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
        String str;
        int i, j, k;
        g.setColor(Color.BLACK);
        g.fillRect(0, 0, getWidth(), getHeight());        
        
        if(f.e == null) {
            g.setColor(Color.YELLOW);
            g.setFont(NOTICE);
            str = "Cache is not initialized";
            g.drawString(str, 2, 2 + getTotalHeight(g2, str, NOTICE));
            return;
        } else {
            int sets = f.e.blocks / f.e.associativity;
            int setPixels = (getHeight()-2*MARGIN) / sets;
            g.setFont(SET_HEADER);
            
            for(i = 0; i < sets; i++) {
                if(highlightSet == i) {
                    if(f.e.lastAccessType == 1)
                        g.setColor(new Color(0, 0, f.e.lastHit ? 60 : 255));
                    else
                        g.setColor(new Color(f.e.lastHit ? 60 : 255, 0, 0));
                    g.fillRect(MARGIN, MARGIN+i*setPixels, getWidth()-2*MARGIN, setPixels);
                }
                str = "" + i;
                g.setColor(Color.YELLOW);
                g.drawString(str, MARGIN, MARGIN+i*setPixels + setPixels/2 + getTotalHeight(g2, str, SET_HEADER)/2);
            }
        }
    }
    
    private LineMetrics getMetrics(Graphics2D g, String str, Font f) {
        FontRenderContext frc = g.getFontRenderContext();
        return f.getLineMetrics(str, frc);
    }
    
    private int getTotalHeight(Graphics2D g, String str, Font f) {
        return (int) (getMetrics(g, str, f).getHeight() + getMetrics(g, str, f).getDescent());
    }
}

