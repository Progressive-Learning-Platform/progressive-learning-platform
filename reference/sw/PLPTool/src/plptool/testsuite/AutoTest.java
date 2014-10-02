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
import plptool.gui.ProjectDriver;
import plptool.dmf.*;

import java.awt.event.KeyEvent;

/**
 * PLPTool unit test framework
 *
 * @author Wira
 */
public class AutoTest {
    public static ProjectDriver plp;
    private static DriverThread driver;
    private static GUITester guiTester;
    private static java.awt.Robot r;
    private static boolean gui = false;
    private static boolean force = false;
    private static boolean start = false;
    private static boolean load = false;
    private static boolean delay = true;
    private static long startTime;
    private static Tester t;
    public static long TYPING_DELAY = 50;

    public static String[] setup(String[] args) {
        String[] ret = args;
        startTime = System.currentTimeMillis();

        for(int i = 0; i < ret.length; i++)
            if(ret[i].equals("--autotest-gui")) {
                if(load) {
                    p("can not use built-in GUI tester and external tester at the same time!");
                    System.exit(-1);
                }
                ret = PLPToolbox.gobble(ret, i);
                i--;
                gui = true;
                p("configuring built-in GUI tester");
                configure();
                guiTester = new GUITester();
                guiTester.configure(r);                
            } else if(ret[i].equals("--autotest-force")) {
                ret = PLPToolbox.gobble(ret, i);
                i--;
                force = true;
            } else if(ret[i].equals("--autotest-no-delay")) {
                ret = PLPToolbox.gobble(ret, i);
                i--;
                delay = false;
            } else if(ret[i].equals("--autotest-force-start")) {
                ret = PLPToolbox.gobble(ret, i);
                i--;
                start = true;
            } else if(ret[i].equals("--autotest-disable-messages")) {
                ret = PLPToolbox.gobble(ret, i);
                i--;
                Msg.silent = true;
                Msg.suppressWarnings = true;
            } else if(ret[i].equals("--autotest-load") && (i+2) < ret.length) {
                if(gui || load) {
                    p("can not use multiple testers at the same time!");
                    System.exit(-1);
                }
                DynamicModuleFramework.disableWarning();
                if(!DynamicModuleFramework.loadModuleClass(ret[i+2], ret[i+1])) {
                    p("external tester class load failed.");
                    System.exit(-1);
                }
                int index = DynamicModuleFramework.isModuleClassRegistered(ret[i+1]);
                configure();
                try {
                    t = (Tester) DynamicModuleFramework.getDynamicModuleClass(index).newInstance();
                    p("configuring external tester '" + ret[i+1] + "' from '" +
                            ret[i+2] + "'");
                    t.configure(r);
                } catch(Exception e) {
                    p("external tester instantiation failed.");
                    e.printStackTrace();
                    System.exit(-1);
                }
                ret = PLPToolbox.gobble(ret, i);
                ret = PLPToolbox.gobble(ret, i);
                ret = PLPToolbox.gobble(ret, i);
                
                i--;
                load = true;
            } else if(ret[i].equals("--autotest-run")) {
                if(gui) {
                    p("can not use '--autotest-run' with '--autotest-gui'");
                    System.exit(-1);
                }

                p("debug execution path selected");
                ret[i] = "--debug-projectdriver";
            } else if(ret[i].equals("--autotest-help")) {
                printHelp();
                System.exit(0);
            }

        return ret;
    }

    public static void configure() {        
        AutoTestCallback cb = new AutoTestCallback();
        int[] nums = {
                    CallbackRegistry.START,
                    CallbackRegistry.LOAD_CONFIG_LINE,
                    CallbackRegistry.SAVE_CONFIG,
                    CallbackRegistry.COMMAND,
                    CallbackRegistry.EVENT,
                    CallbackRegistry.EXIT
                };
        try {
            r = new java.awt.Robot();
        } catch(Exception e) {
            p("failed to initialize robot - use of robot will crash the autotest");
        }
        p("installing autotest callback");
        CallbackRegistry.register(cb, nums);
    }

    public static void p(String str) {
        long time = System.currentTimeMillis() - startTime;
        Msg.S(String.format("%16d", time) + " autotest: " + str);
    }

    static class AutoTestCallback implements Callback {
        public boolean callback(int callbackNum, Object param) {
            switch(callbackNum) {
                case CallbackRegistry.START:
                    plp = (ProjectDriver) param;
                    AutoTest.p("*** handing control to autotest thread ***");
                    driver = new DriverThread();
                    driver.start();
                    return false;
            }
            return false;
        }
    }

    public static void typeChar(int...code) {
        for(int i = 0; i < code.length; i++)
            r.keyPress(code[i]);
        delay(TYPING_DELAY);
        for(int i = 0; i < code.length; i++)
            r.keyRelease(code[i]);
        delay(TYPING_DELAY);

    }

    public static boolean isForced() {
        return force;
    }

    public static void type(String str) {
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
                case '!': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_1); break;
                case '@': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_2); break;
                case '#': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_3); break;
                case '$': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_4); break;
                case '%': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_5); break;
                case '^': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_6); break;
                case '&': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_7); break;
                case '*': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_8); break;
                case '(': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_9); break;
                case ')': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_0); break;
                case '_': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_MINUS); break;
                case '+': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_PLUS); break;
                case '\t': typeChar(KeyEvent.VK_TAB); break;
                case '\n': typeChar(KeyEvent.VK_ENTER); break;
                case '[': typeChar(KeyEvent.VK_OPEN_BRACKET); break;
                case ']': typeChar(KeyEvent.VK_CLOSE_BRACKET); break;
                case '\\': typeChar(KeyEvent.VK_BACK_SLASH); break;
                case '{': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_OPEN_BRACKET); break;
                case '}': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_CLOSE_BRACKET); break;
                case '|': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_BACK_SLASH); break;
                case ';': typeChar(KeyEvent.VK_SEMICOLON); break;
                case ':': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_SEMICOLON); break;
                case '\'': typeChar(KeyEvent.VK_QUOTE); break;
                case '"': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_QUOTE); break;
                case ',': typeChar(KeyEvent.VK_COMMA); break;
                case '<': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_COMMA); break;
                case '.': typeChar(KeyEvent.VK_PERIOD); break;
                case '>': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_PERIOD); break;
                case '/': typeChar(KeyEvent.VK_SLASH); break;
                case '?': typeChar(KeyEvent.VK_SHIFT, KeyEvent.VK_SLASH); break;
                case ' ': typeChar(KeyEvent.VK_SPACE); break;
            }
        }
    }

    public static void delay(long ms) {
        try {
            Thread.sleep(ms);
        } catch(Exception e) {

        }
    }

    static class DriverThread extends Thread {
        @Override
        public void run() {
            try {
                if(!start) {
                    p("press enter to start");
                    PLPToolbox.readLine();
                }
                if(delay) {
                    p("starting in 2 seconds");
                    delay(1000);
                }
                if(plp.g()) {
                    plp.g_dev.requestFocus();
                    plp.g_dev.toFront();
                }
                if(delay)
                    delay(1000);
                if(load) {
                    t.run(plp);
                } else {
                    guiTester.run(plp);
                }

            } catch(Exception e) {
                System.err.println("=====================================================");
                System.err.println("             Driver thread has crashed!              ");
                System.err.println("=====================================================");
                e.printStackTrace();
                if(plp.g())
                    PLPToolbox.showErrorDialog(null, "Autotest Failed. Reason: Crashed.");
                System.exit(-1);
            }

            p("autotest thread exiting");
        }
    }

    public static void printHelp() {
        System.out.println("");
        System.out.println("PLPTool has a built-in autotest framework that can be used to perform unit");
        System.out.println("tests and other automated tasks. The framework is located in plptool.testsuite");
        System.out.println("package of the PLPTool archive, and it consists of a utility class (AutoTest) ");
        System.out.println("and a test interface (Tester).");
        System.out.println("");
        System.out.println("A template for a class implementing the test interface is as follows:");
        System.out.println("");
        System.out.println("import plptool.*;");
        System.out.println("import plptool.testsuite.*;");
        System.out.println("import plptool.gui.ProjectDriver;");
        System.out.println("");
        System.out.println("public class Test1 implements Tester {");
        System.out.println("    public void configure(java.awt.Robot r) { }");
        System.out.println("    public void run(ProjectDriver plp)      { }");
        System.out.println("}");
        System.out.println("");
        System.out.println("The configure method is called immediately after PLPTool starts and before");
        System.out.println("non-autotest command line arguments are parsed. The run method is called");
        System.out.println("in a separate thread immediately after the ProjectDriver is instantiated.");
        System.out.println("");
        System.out.println("The tester class must NOT have subclasses and anonymous classes (it must ");
        System.out.println("compile into a single .class file).");
        System.out.println("");
        System.out.println("plptool.testsuite.AutoTest members:");
        System.out.println("  p(String s)             Print a message with a timestamp since the program");
        System.out.println("                            launched in milliseconds. This message will not be");
        System.out.println("                            disabled by --autotest-disable-messages");
        System.out.println("  delay(int t)            Wait for t milliseconds");
        System.out.println("  type(String s)          Type the string as if the user is using the keyboard");
        System.out.println("  typeChar(int... code)   Press and release the specified virtual keycodes.");
        System.out.println("                            See: java.awt.event.KeyEvent constants");
        System.out.println("  long TYPING_DELAY       Amount of time in milliseconds for: how long a key is");
        System.out.println("                            pressed, and the amount of time that must pass ");
        System.out.println("                            after it is released");
        System.out.println("");
        System.out.println("Autotest command line arguments:");
        System.out.println("  --autotest-load <class name> <class file path>");
        System.out.println("                          Load tester class (e.g. Test1 ./Test1.class)");
        System.out.println("  --autotest-run          Run debug-ProjectDriver for autotesting instead of");
        System.out.println("                            the default execution path");
        System.out.println("  --autotest-force-start  Run autotest without prompting");
        System.out.println("  --autotest-no-delay     Skip the 2-second delay before the autotest");
        System.out.println("  --autotest-disable-messages");
        System.out.println("                          Disable all messages except from AutoTest.p method");
        System.out.println("                            and System.out or System.err streams");
        System.out.println("");
        System.out.println("Compiling and running a test");
        System.out.println("");
        System.out.println("The PLPTool archive must be included in the classpath when the tester class is");
        System.out.println("compiled. E.g.:");
        System.out.println("  javac -cp ./PLPTool.jar Test1.java");
        System.out.println("");
        System.out.println("To run the test without starting up the GUI:");
        System.out.println("  java -jar ./PLPTool.jar --autotest-load Test1 Test1.class \\");
        System.out.println("                          --autotest-run");
        System.out.println("");

    }
}
