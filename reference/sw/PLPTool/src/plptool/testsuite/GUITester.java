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

import java.io.*;
import java.awt.event.*;
import java.awt.*;

/**
 *
 * @author wira
 */
public class GUITester implements Tester {
    private Robot r;

    public void configure(Robot r) {
        try {
            this.r = r;
        } catch(Exception e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }

    public void run(ProjectDriver plp) {
        File f = new File(Constants.launchPath + "/test.plp");
        AutoTest.p("checking if " + f.getAbsolutePath() + " exists");
        if(f.exists()) {
            if(!AutoTest.isForced()) {
                AutoTest.p(f.getAbsolutePath() + " exists. Delete this file first before running GUI autotest.");
                System.exit(-1);
            } else
                f.delete();
        }
        AutoTest.TYPING_DELAY = 5;
        AutoTest.p("creating new project");
        plp.create(ArchRegistry.ISA_PLPMIPS);
        AutoTest.p("commencing tests");
        AutoTest.delay(1000);
        plp.g_dev.toFront();
        AutoTest.delay(500);
        Msg.E("Message test", Constants.PLP_ASM_GENERAL_SYNTAX_ERROR, plp);
        AutoTest.delay(500);
        Msg.W("Message test", plp);
        AutoTest.delay(500);
        Msg.M("Message test");
        Msg.p("<b>HTML message</b> test");
        AutoTest.delay(500);
        Msg.p("Preformatted buffer -");
        Msg.p(" with no newline");
        Msg.pn("");
        Msg.pn("With new");
        Msg.pn("lines");
        Msg.P();
        plp.g_dev.setEditorText("");
        plp.g_dev.getEditor().requestFocus();
        AutoTest.delay(1000);

        r.keyRelease(KeyEvent.VK_SHIFT);
        r.keyRelease(KeyEvent.VK_CONTROL);
        r.keyRelease(KeyEvent.VK_ALT);
        AutoTest.delay(100);
        AutoTest.type(Text.file1);
        AutoTest.delay(1000);
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Z);
        AutoTest.delay(1000);
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_Y);
        AutoTest.delay(1000);
        AutoTest.p("save test");
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_S);
        AutoTest.delay(1000);
        AutoTest.type("test.plp\n");
        //AutoTest.delay(100);
        //AutoTest.typeChar(KeyEvent.VK_ALT, KeyEvent.VK_S);
        AutoTest.delay(1000);
        AutoTest.p("assemble test");
        AutoTest.typeChar(KeyEvent.VK_F2);
        AutoTest.delay(500);
        AutoTest.p("entering simulation test and stepping");
        AutoTest.typeChar(KeyEvent.VK_F3);
        AutoTest.delay(1000);
        AutoTest.typeChar(KeyEvent.VK_F5);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_F5);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_F5);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_F5);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_F5);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_F5);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_F5);
        AutoTest.delay(1000);
        AutoTest.p("saving in simulation mode test");
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_S);
        AutoTest.delay(1000);

        AutoTest.p("sim test: run->wait->run");
        Config.simCyclesPerStep = 10000;
        Config.simRunnerDelay = 1;
        AutoTest.typeChar(KeyEvent.VK_F7);
        AutoTest.delay(5000);
        AutoTest.typeChar(KeyEvent.VK_F7);
        AutoTest.delay(1000);
        AutoTest.p("sim test: run->wait->reset");
        AutoTest.typeChar(KeyEvent.VK_F7);
        AutoTest.delay(5000);
        AutoTest.typeChar(KeyEvent.VK_F9);
        AutoTest.delay(1000);
        AutoTest.p("sim test: run->wait->new project");
        AutoTest.typeChar(KeyEvent.VK_F7);
        AutoTest.delay(5000);
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_N);
        AutoTest.delay(1000);
        AutoTest.p("opening test.plp using g_dev.open");
        plp.g_dev.open(f);
        AutoTest.delay(1000);
        AutoTest.p("opening test.plp using g_dev.openPLPFile");
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_O);
        AutoTest.delay(500);
        AutoTest.type("test.plp\n");
        AutoTest.delay(1000);
        AutoTest.p("import test");
        AutoTest.typeChar(KeyEvent.VK_ALT, KeyEvent.VK_R);
        AutoTest.delay(500);
        AutoTest.typeChar(KeyEvent.VK_I);
        AutoTest.delay(500);
        if(PLPToolbox.isHostLinux())
            AutoTest.type("../libplp/libplp_uart.asm\n");
        else
            AutoTest.type("..\\libplp\\libplp_uart.asm\n");
        AutoTest.delay(500);
        AutoTest.p("source order test");
        AutoTest.delay(500);
        plp.setAsmPosition(0, 1);
        AutoTest.delay(50);
        plp.refreshProjectView(false);
        AutoTest.delay(500);
        plp.setAsmPosition(0, 1);
        AutoTest.delay(50);
        plp.refreshProjectView(false);
        AutoTest.delay(500);
        AutoTest.p("source display test");
        plp.setOpenAsm(1);
        AutoTest.delay(50);
        plp.refreshProjectView(false);
        AutoTest.delay(500);
        plp.setOpenAsm(0);
        AutoTest.delay(50);
        plp.refreshProjectView(false);
        AutoTest.delay(500);
        AutoTest.p("source remove test");
        plp.g_dev.getProjectTree().requestFocus();
        plp.g_dev.getProjectTree().setSelectionRow(0);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_DOWN);
        AutoTest.typeChar(KeyEvent.VK_DOWN);
        AutoTest.typeChar(KeyEvent.VK_DOWN);
        AutoTest.delay(100);
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_E);
        AutoTest.delay(500);
        plp.g_dev.getEditor().requestFocus();
        AutoTest.delay(500);
        AutoTest.p("re-importing with plp.importAsm");
        plp.importAsm("../libplp/libplp_uart.asm");
        AutoTest.delay(500);
        AutoTest.p("uart test");
        plp.g_dev.setEditorText(Text.file2);
        AutoTest.delay(500);
        AutoTest.typeChar(KeyEvent.VK_F2);
        AutoTest.delay(500);
        AutoTest.p("sim test: UART");
        AutoTest.typeChar(KeyEvent.VK_F3);
        AutoTest.delay(1000);
        Config.simCyclesPerStep = 50;
        Config.simRunnerDelay = 100;
        AutoTest.typeChar(KeyEvent.VK_F7);
        AutoTest.delay(100);
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_4);
        AutoTest.delay(2000);
        plp.g_dev.getEditor().requestFocus();
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_CONTROL, KeyEvent.VK_4);
        AutoTest.delay(50);
        AutoTest.typeChar(KeyEvent.VK_F7);
    }

    static class Text {
    public final static String file1 =
            "#PLPTool autotest\n" +
            "#\n" +
            ".org 0x10000000\n\n" +
            "loop:\n" +
            "\tj loop\n" +
            "\tnop\n"
            ;

    public final static String file2 =
            "#PLPTool autotest 2\n" +
            "#\n" +
            ".org 0x10000000\n\n" +
            "\tli $a0, string\n\n" +
            "loop:\n" +
            "\tjal libplp_uart_write_string\n" +
            "\tnop\n\n" +
            "\tj loop\n" +
            "\tnop\n\n" +
            "string:\n" +
            "\t.asciiz \"Oh yeah!\"\n" +
            "";
    }
}
