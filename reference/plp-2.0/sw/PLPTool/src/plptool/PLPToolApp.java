/*
 * PLPToolApp.java
 */

package plptool;

import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;
import java.util.HashMap;

/**
 * The main class of the application.
 */
public class PLPToolApp extends SingleFrameApplication {

    /**
     * At startup create and show the main frame of the application.
     */
    @Override protected void startup() {
        show(new PLPToolView(this));
    }

    /**
     * This method is to initialize the specified window by injecting resources.
     * Windows shown in our application come fully initialized from the GUI
     * builder, so this additional configuration is not needed.
     */
    @Override protected void configureWindow(java.awt.Window root) {
    }

    /**
     * A convenient static getter for the application instance.
     * @return the instance of PLPToolApp
     */
    public static PLPToolApp getApplication() {
        return Application.getInstance(PLPToolApp.class);
    }

    /**
     * Main method launching the application.
     */
    public static void main(String[] args) {
        System.out.println("\nPLP Java Tool");
        System.out.println("Authors: David Fritz, Brian Gordon, Wira Mulia");
        System.out.println(PLPMsg.versionString + "\n");

        if(args.length > 0 && args[0].equals("-a")) {
            if(args.length != 4) {
                System.out.println("Usage: PLPTool -a <asm> <addr> <out>");
                System.exit(-1);
            } else {
                PLPAsmFormatter.genPLP(args[1], args[3], false);
            }
        }
        else if(args.length > 0 && args[0].equals("-af")) {
            if(args.length != 4) {
                System.out.println("Usage: PLPTool -af <asm> <addr> <out>");
                System.exit(-1);
            } else {
                PLPAsmFormatter.genPLP(args[1], args[3], true);
            }
        }

        else if(args.length == 0)
            launch(PLPToolApp.class, args);
        
        else {
            PLPMsg.E("Invalid argument(s).", PLPMsg.PLP_ERROR_GENERIC, null);
            System.out.println();
            System.out.println("Run PLPTool with no command line arguments to launch GUI tool.");
            System.out.println();
            System.out.println("Non-GUI options:\n");
            System.out.println("  -a  <asm> <addr> <out>");
            System.out.println("      Assemble <asm> with initial address <addr> and write plp");
            System.out.println("      output to <out>.plp.");
            System.out.println();
            System.out.println("  -af <asm> <addr> <out>");
            System.out.println("      Like -a, but overwrite existing output file without prompting.");
            System.out.println();
            System.out.println("  -p  <plpfile> <port> <baud>");
            System.out.println("      Program PLP target board with <plpfile> using serial port <port>");
            System.out.println("      and baud rate of <baud>.");
            System.out.println();
        }
    }
}
