/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui.frames;

import java.awt.*;
import javax.swing.*;
import javax.swing.text.*;

/**
 *
 * @author wira
 */
public class TextLineHighlighter implements Highlighter.HighlightPainter {

    private JTextComponent txtEditor;
    private int yPos;
    private int old_yPos;

    public TextLineHighlighter(JTextComponent c) {
        txtEditor = c;
        old_yPos = -1;
        yPos = -1;
        try {
            c.getHighlighter().addHighlight(0, 0, this);
        } catch(Exception e) {}
    }

    public void setY(int y) {
        old_yPos = yPos;
        yPos = y;
    }

    public void repaint() {
        txtEditor.repaint(0, yPos, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());

        if(old_yPos > -1) {
            txtEditor.repaint(0, old_yPos, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
        }
    }

    public void paint(java.awt.Graphics g, int p0, int p1, java.awt.Shape bounds, JTextComponent txtEditor) {
        if(yPos > -1) {
            if(old_yPos > -1) {
                g.setColor(Color.WHITE);
                g.fillRect(0, old_yPos, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
            }

            g.setColor(new Color(225,225,255));
            g.fillRect(0, yPos, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
        } else if(yPos == -1 && old_yPos > -1) {
            g.setColor(Color.WHITE);
            g.fillRect(0, old_yPos, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
        }
    }
}
