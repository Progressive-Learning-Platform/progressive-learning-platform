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
public class CacheStatisticsCanvas extends JPanel {
    
    private DefaultCacheFrame f;
    private int highlightSet = -1;
    
    private Font NOTICE = new Font("Monospaced", Font.BOLD, 36);
    private Font SET_HEADER = new Font("Monospaced", Font.BOLD, 24);
    private Font CONTENT = new Font("Monospaced", Font.PLAIN, 24);
    private final int BAR_HEIGHT = 50;
    private final int BAR_MARGIN = 10;
    private final int LEFT_MARGIN = 5;
    private final int RIGHT_MARGIN = 5;
    private final int TOP_MARGIN = 40;
    private final int BOTTOM_MARGIN = 5;

    public CacheStatisticsCanvas(DefaultCacheFrame f) {
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
            int barWidth = getWidth() - LEFT_MARGIN - RIGHT_MARGIN - 2*BAR_MARGIN;
            int readHitsWidth = (int)( (float) f.e.stats.read_hits / (float) f.e.stats.read_accesses * barWidth);
            int writeHitsWidth = (int)( (float) f.e.stats.write_hits / (float) f.e.stats.write_accesses * barWidth);
            
            if(f.e.stats.read_accesses == 0) {
                g2.setColor(Color.DARK_GRAY);
                g2.fillRect(LEFT_MARGIN+BAR_MARGIN, TOP_MARGIN, barWidth, BAR_HEIGHT);
            } else {
                g2.setColor(Color.GREEN);
                g2.fillRect(LEFT_MARGIN+BAR_MARGIN, TOP_MARGIN, readHitsWidth, BAR_HEIGHT);
                g2.setColor(Color.RED);
                g2.fillRect(LEFT_MARGIN+BAR_MARGIN+readHitsWidth, TOP_MARGIN, barWidth-readHitsWidth, BAR_HEIGHT);
            }
            
            if(f.e.stats.write_accesses == 0) {
                g2.setColor(Color.DARK_GRAY);
                g2.fillRect(LEFT_MARGIN+BAR_MARGIN, 2*TOP_MARGIN+BAR_HEIGHT, barWidth, BAR_HEIGHT);
            } else {
                g2.setColor(Color.GREEN);
                g2.fillRect(LEFT_MARGIN+BAR_MARGIN, 2*TOP_MARGIN+BAR_HEIGHT, writeHitsWidth, BAR_HEIGHT);
                g2.setColor(Color.RED);
                g2.fillRect(LEFT_MARGIN+BAR_MARGIN+writeHitsWidth, 2*TOP_MARGIN+BAR_HEIGHT, barWidth-writeHitsWidth, BAR_HEIGHT);
            }
        }
    }
    
    private LineMetrics getMetrics(Graphics2D g, String str, Font f) {
        FontRenderContext frc = g.getFontRenderContext();
        return f.getLineMetrics(str, frc);
    }
    
    private int getTotalHeight(Graphics2D g, String str, Font f) {
        return (int) (getMetrics(g, str, f).getHeight());
    }
}

