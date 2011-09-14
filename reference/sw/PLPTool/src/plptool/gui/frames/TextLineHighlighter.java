/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.gui.frames;

import java.awt.*;
import javax.swing.*;
import javax.swing.text.*;
import plptool.Msg;
import plptool.Config;

/**
 *
 * @author wira
 */
public class TextLineHighlighter implements Highlighter.HighlightPainter {

    private JTextComponent txtEditor;
    private int yPos;
    private int old_yPos;
    private Color color;

    public TextLineHighlighter(JTextComponent c) {
        txtEditor = c;
        old_yPos = -1;
        yPos = -1;
        color = new Color(235,235,255);
        try {
            c.getHighlighter().addHighlight(0, 0, this);
        } catch(Exception e) {}
    }

    public void setY(int y) {
        if(y != yPos) {
            old_yPos = yPos;
            yPos = y;
        }
    }

    public void setLine(int line) {
        old_yPos = yPos;
        yPos = txtEditor.getFontMetrics(txtEditor.getFont()).getHeight() * line;
    }

    public void setColor(Color color) {
        this.color = color;
    }

    public void repaint() {
        txtEditor.repaint(0, yPos, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());

        if(old_yPos > -1) {
            txtEditor.repaint(0, old_yPos, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
        }
    }

    public void paint(java.awt.Graphics g, int p0, int p1, java.awt.Shape bounds, JTextComponent txtEditor) {
        int y;

        if(yPos > -1) {
            if(old_yPos > -1) {
                Msg.D("tlh clearing old_yPos: " + old_yPos, 10, null);
                y = old_yPos - txtEditor.getFontMetrics(txtEditor.getFont()).getHeight() + txtEditor.getFontMetrics(txtEditor.getFont()).getDescent();
                g.setColor(Color.WHITE);
                g.fillRect(0, y, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
            }

            if(!Config.simHighlightLine)
                return;

            Msg.D("tlh drawing yPos: " + yPos, 10, null);
            y = yPos - txtEditor.getFontMetrics(txtEditor.getFont()).getHeight() + txtEditor.getFontMetrics(txtEditor.getFont()).getDescent();
            g.setColor(color);
            g.fillRect(0, y, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
        } else if(yPos == -1 && old_yPos > -1) {
            Msg.D("tlh (yPos == -1) clearing old_yPos: " + old_yPos, 10, null);
            y = old_yPos - txtEditor.getFontMetrics(txtEditor.getFont()).getHeight() + txtEditor.getFontMetrics(txtEditor.getFont()).getDescent();
            g.setColor(Color.WHITE);
            g.fillRect(0, y, txtEditor.getWidth(), txtEditor.getFontMetrics(txtEditor.getFont()).getHeight());
        }
    }
}
