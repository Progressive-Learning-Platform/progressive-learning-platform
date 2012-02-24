/*
    Copyright 2012 PLP Contributors

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

package plptool.mips;

import plptool.Config;
import plptool.Msg;
import plptool.PLPSyntaxHighlightSupport;
import plptool.gui.ProjectDriver;
import plptool.gui.frames.Develop;

import java.awt.Color;
import javax.swing.text.*;
import java.util.regex.*;

/**
 * Syntax highlighter support for the plpmips ISA implementation
 *
 * @author wira
 */
public class SyntaxHighlightSupport implements PLPSyntaxHighlightSupport {
    private SimpleAttributeSet[] styles;

    private final static int RTYPE = 0;
    private final static int ITYPE = 1;
    private final static int BRANCH = 2;
    private final static int JUMP = 3;
    private final static int MEMTYPE = 4;
    private final static int NOP = 5;
    private final static int REG = 6;
    private final static int IMM = 7;
    private final static int LABEL = 8;
    private final static int COMMENT = 9;
    private final static int SYS = 10;

    //                                                        RTYPE              ITYPE              BRANCH             JUMP               MEMTYPE            NOP                    REG                IMM                  LABEL            COMMENT              SYS
    public static Color[]   syntaxColors                   = {new Color(0,0,255),new Color(0,0,255),new Color(0,0,255),new Color(0,0,255),new Color(0,0,255),new Color(127,127,127),new Color(255,0,0),new Color(255,153,0),new Color(0,0,0),new Color(0, 153, 0),new Color(204, 0, 102)};
    public static boolean[] syntaxBold                     = {false,             false,             false,             false,             false,             false,                 false,             false,               true,            false,               false};
    public static boolean[] syntaxItalic                   = {false,             false,             false,             false,             false,             false,                 false,             false,               false,           false,               false};

    public void syntaxHighlightTextAction(ProjectDriver plp, String text,
            int position) {
        if(styles == null)
            newStyle();
        StyledDocument doc = plp.g_dev.getEditor().getStyledDocument();
        int currentposition = 0;
        int startposition = 0;
        int texttype = -1;
        String currtext = "";
        while (currentposition < text.length()) {
            StringBuilder currtextbuffer = new StringBuilder();
            while (currentposition < text.length() && (text.charAt(currentposition) == '\t' || text.charAt(currentposition) == ' ' || text.charAt(currentposition) == ',')) {
                    currentposition++;
            }
            startposition = currentposition;
            while (currentposition < text.length() && (text.charAt(currentposition) != '\t' && text.charAt(currentposition) != ' ')) {
                    currtextbuffer.append(text.charAt(currentposition));
                    currentposition++;
            }
            currtext = currtextbuffer.toString();

            if(texttype == COMMENT || currtext.contains("#")) {
                    texttype = COMMENT;
            } else if (texttype == SYS || currtext.contains(".")) {
                    texttype = SYS;
            } else if(currtext.contains(":")) {
                    texttype = LABEL;
            } else if(currtext.contains("$")) {
                    texttype = REG;
            } else if(isimmediate(currtext)) {
                    texttype = IMM;
            } else if(currtext.equals("nop")) {
                    texttype = NOP;
            } else if (texttype == -1) {
                if(currtext.contains("w")) {
                        texttype = MEMTYPE;
                } else if(currtext.contains("j")) {
                        texttype = JUMP;
                } else if(currtext.contains("b")) {
                        texttype = BRANCH;
                } else if(currtext.contains("i")) {
                        texttype = ITYPE;
                } else {
                        texttype = RTYPE;
                }
            } else {
                texttype = LABEL;
            }

            doc.setCharacterAttributes(startposition+position, currentposition-startposition, styles[texttype], false);
            currentposition++;
        }
    }

    private boolean isimmediate(String num) {
        Pattern pattern0 = Pattern.compile("-?[0-9]*");
        Matcher matcher0 = pattern0.matcher(num);
        Pattern pattern1 = Pattern.compile("0x[0-9a-fA-F]*");
        Matcher matcher1 = pattern1.matcher(num);
        Pattern pattern2 = Pattern.compile("0b[0-1]*");
        Matcher matcher2 = pattern2.matcher(num);
        Pattern pattern3 = Pattern.compile("'\\S'");
        Matcher matcher3 = pattern3.matcher(num);
        return (matcher0.matches() || matcher1.matches() ||
                matcher2.matches() || matcher3.matches());
    }

    // Called whenever syntax styles change
    public void newStyle() {
        Style def = StyleContext.getDefaultStyleContext().getStyle(StyleContext.DEFAULT_STYLE);
        StyleConstants.setFontFamily(def,Config.devFont);
        StyleConstants.setFontSize(def,Config.devFontSize);
        styles = new SimpleAttributeSet[11];
        for(int i=0;i<11;i++) {
            styles[i] = new SimpleAttributeSet(def);
            StyleConstants.setForeground(styles[i],syntaxColors[i]);
            StyleConstants.setBold(styles[i], syntaxBold[i]);
            StyleConstants.setItalic(styles[i], syntaxItalic[i]);
        }
    }
}
