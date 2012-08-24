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

package plptool.testsuite;

import plptool.*;
import plptool.gui.*;
import plptool.gui.frames.*;
import plptool.mips.*;
import plptool.mods.*;
import plptool.dmf.*;

import java.awt.*;
import java.awt.event.*;

/**
 *
 * @author Wira
 */
public class AutoTest {
    private static ProjectDriver plp;
    private static DriverThread driver;
    private static java.awt.Robot r;

    public static String[] setup(String[] args) {
        String[] ret = args;

        for(int i = 0; i < args.length; i++)
            if(args[i].equals("--debug-autotest")) {
                ret = PLPToolbox.gobble(args, i);
                Msg.P("Commencing PLPTool Auto Test");
                configure();
            }

        return ret;
    }

    public static void configure() {
        AutoTestCallback cb = new AutoTestCallback();
        int[] nums = {
                    plptool.dmf.CallbackRegistry.START,
                    plptool.dmf.CallbackRegistry.LOAD_CONFIG_LINE,
                    plptool.dmf.CallbackRegistry.SAVE_CONFIG,
                    plptool.dmf.CallbackRegistry.COMMAND,
                    plptool.dmf.CallbackRegistry.EVENT,
                    plptool.dmf.CallbackRegistry.EXIT
                };
        Msg.S("Installing autotest hooks");
        plptool.dmf.CallbackRegistry.register(cb, nums);
    }


    static class AutoTestCallback implements plptool.dmf.Callback {
        public AutoTestCallback() {
            
        }

        public boolean callback(int callbackNum, Object param) {
            switch(callbackNum) {
                case CallbackRegistry.START:
                    plp = (ProjectDriver) param;
                    driver = new DriverThread();
                    driver.start();
            }
            return false;
        }
    }

    static class DriverThread extends Thread {
        @Override
        public void run() {
            Msg.S("Waiting for ProjectDriver to settle...");
            while(plp.plpfile == null)
                sl(100);
            Msg.S("Commencing tests");
            sl(1000);
            Msg.S("Testing undo/redo");
            plp.g_dev.toFront();
            plp.g_dev.getEditor().requestFocus();
            plp.g_dev.setEditorText("");
            sl(100);
            try {
                r = new java.awt.Robot();
                r.keyRelease(KeyEvent.VK_SHIFT);
                r.keyRelease(KeyEvent.VK_CONTROL);
                r.keyRelease(KeyEvent.VK_ALT);
                type("PLPTool autotest suite. Undo/redo test.");
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);

                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
                typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);

            } catch(Exception e) {

            }
            Msg.S("Autotest thread exiting");
        }

        private void typeChar(int...code) {
            for(int i = 0; i < code.length; i++)
                r.keyPress(code[i]);
            sl(50);
            for(int i = 0; i < code.length; i++)
                r.keyRelease(code[i]);
            sl(50);
           
        }

        private void type(String str) {
            for(int i = 0; i < str.length(); i++) {
                switch(str.charAt(i)) {
                    case 'a': typeChar(KeyEvent.VK_A); break;
                    case 'b': typeChar(KeyEvent.VK_B); break;
                    case 'c': typeChar(KeyEvent.VK_C); break;
                    case 'd': typeChar(KeyEvent.VK_D); break;
                    case 'e': typeChar(KeyEvent.VK_E); break;
                    case 'f': typeChar(KeyEvent.VK_F); break;
                    case 'g': typeChar(KeyEvent.VK_G); break;
                    case 'h': typeChar(KeyEvent.VK_H); break;
                    case 'i': typeChar(KeyEvent.VK_I); break;
                    case 'j': typeChar(KeyEvent.VK_J); break;
                    case 'k': typeChar(KeyEvent.VK_K); break;
                    case 'l': typeChar(KeyEvent.VK_L); break;
                    case 'm': typeChar(KeyEvent.VK_M); break;
                    case 'n': typeChar(KeyEvent.VK_N); break;
                    case 'o': typeChar(KeyEvent.VK_O); break;
                    case 'p': typeChar(KeyEvent.VK_P); break;
                    case 'q': typeChar(KeyEvent.VK_Q); break;
                    case 'r': typeChar(KeyEvent.VK_R); break;
                    case 's': typeChar(KeyEvent.VK_S); break;
                    case 't': typeChar(KeyEvent.VK_T); break;
                    case 'u': typeChar(KeyEvent.VK_U); break;
                    case 'v': typeChar(KeyEvent.VK_V); break;
                    case 'w': typeChar(KeyEvent.VK_W); break;
                    case 'x': typeChar(KeyEvent.VK_X); break;
                    case 'y': typeChar(KeyEvent.VK_Y); break;
                    case 'z': typeChar(KeyEvent.VK_Z); break;
                    case 'A': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_A); break;
                    case 'B': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_B); break;
                    case 'C': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_C); break;
                    case 'D': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_D); break;
                    case 'E': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_E); break;
                    case 'F': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_F); break;
                    case 'G': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_G); break;
                    case 'H': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_H); break;
                    case 'I': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_I); break;
                    case 'J': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_J); break;
                    case 'K': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_K); break;
                    case 'L': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_L); break;
                    case 'M': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_M); break;
                    case 'N': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_N); break;
                    case 'O': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_O); break;
                    case 'P': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_P); break;
                    case 'Q': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_Q); break;
                    case 'R': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_R); break;
                    case 'S': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_S); break;
                    case 'T': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_T); break;
                    case 'U': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_U); break;
                    case 'V': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_V); break;
                    case 'W': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_W); break;
                    case 'X': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_X); break;
                    case 'Y': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_Y); break;
                    case 'Z': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_Z); break;
                    case '`': typeChar(KeyEvent.VK_BACK_QUOTE); break;
                    case '0': typeChar(KeyEvent.VK_0); break;
                    case '1': typeChar(KeyEvent.VK_1); break;
                    case '2': typeChar(KeyEvent.VK_2); break;
                    case '3': typeChar(KeyEvent.VK_3); break;
                    case '4': typeChar(KeyEvent.VK_4); break;
                    case '5': typeChar(KeyEvent.VK_5); break;
                    case '6': typeChar(KeyEvent.VK_6); break;
                    case '7': typeChar(KeyEvent.VK_7); break;
                    case '8': typeChar(KeyEvent.VK_8); break;
                    case '9': typeChar(KeyEvent.VK_9); break;
                    case '-': typeChar(KeyEvent.VK_MINUS); break;
                    case '=': typeChar(KeyEvent.VK_EQUALS); break;
                    case '~': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_BACK_QUOTE); break;
                    case '!': typeChar(KeyEvent.VK_EXCLAMATION_MARK); break;
                    case '@': typeChar(KeyEvent.VK_AT); break;
                    case '#': typeChar(KeyEvent.VK_NUMBER_SIGN); break;
                    case '$': typeChar(KeyEvent.VK_DOLLAR); break;
                    case '%': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_5); break;
                    case '^': typeChar(KeyEvent.VK_CIRCUMFLEX); break;
                    case '&': typeChar(KeyEvent.VK_AMPERSAND); break;
                    case '*': typeChar(KeyEvent.VK_ASTERISK); break;
                    case '(': typeChar(KeyEvent.VK_LEFT_PARENTHESIS); break;
                    case ')': typeChar(KeyEvent.VK_RIGHT_PARENTHESIS); break;
                    case '_': typeChar(KeyEvent.VK_UNDERSCORE); break;
                    case '+': typeChar(KeyEvent.VK_PLUS); break;
                    case '\t': typeChar(KeyEvent.VK_TAB); break;
                    case '\n': typeChar(KeyEvent.VK_ENTER); break;
                    case '[': typeChar(KeyEvent.VK_OPEN_BRACKET); break;
                    case ']': typeChar(KeyEvent.VK_CLOSE_BRACKET); break;
                    case '\\': typeChar(KeyEvent.VK_BACK_SLASH); break;
                    case '{': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_OPEN_BRACKET); break;
                    case '}': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_CLOSE_BRACKET); break;
                    case '|': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_BACK_SLASH); break;
                    case ';': typeChar(KeyEvent.VK_SEMICOLON); break;
                    case ':': typeChar(KeyEvent.VK_COLON); break;
                    case '\'': typeChar(KeyEvent.VK_QUOTE); break;
                    case '"': typeChar(KeyEvent.VK_QUOTEDBL); break;
                    case ',': typeChar(KeyEvent.VK_COMMA); break;
                    case '<': typeChar(KeyEvent.VK_LESS); break;
                    case '.': typeChar(KeyEvent.VK_PERIOD); break;
                    case '>': typeChar(KeyEvent.VK_GREATER); break;
                    case '/': typeChar(KeyEvent.VK_SLASH); break;
                    case '?': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_SLASH); break;
                    case ' ': typeChar(KeyEvent.VK_SPACE); break;
                }
            }
        }

        private void sl(long ms) {
            try {
                Thread.sleep(ms);
            } catch(Exception e) {

            }
        }
    }
}
