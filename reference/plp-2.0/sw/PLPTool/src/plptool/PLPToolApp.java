/*
 * PLPToolApp.java
 */

package plptool;

import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;

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
            if(args.length != 3) {
                System.out.println("Usage: PLPTool -a <asm> <out>");
                System.exit(-1);
            } else {
                PLPAsmFormatter.genPLP(args[1], args[2], false);
            }
        }
        else if(args.length > 0 && args[0].equals("-af")) {
            if(args.length != 3) {
                System.out.println("Usage: PLPTool -af <asm> <out>");
                System.exit(-1);
            } else {
                PLPAsmFormatter.genPLP(args[1], args[2], true);
            }
        }
        else if(args.length > 0 && args[0].equals("-e")) {
            if(args.length != 2) {
                System.out.println("Usage: PLPTool -e <asm>");
            } else
                PLPEmuCL.emuCL(args[1]);
        }

        else if(args.length == 0)
            launch(PLPToolApp.class, args);
        
        else {
            PLPMsg.E("Invalid argument(s).", PLPMsg.PLP_ERROR_GENERIC, null);
            System.out.println();
            System.out.println("Run PLPTool with no command line arguments to launch GUI tool.");
            System.out.println();
            System.out.println("Non-GUI options:\n");
            System.out.println("  -a  <asm> <out>");
            System.out.println("      Assemble <asm> and write plp output to <out>.plp.");
            System.out.println();
            System.out.println("  -af <asm> <out>");
            System.out.println("      Like -a, but overwrite existing output file without prompting.");
            System.out.println();
            System.out.println("  -p  <plpfile> <port> <baud>");
            System.out.println("      Program PLP target board with <plpfile> using serial port <port>");
            System.out.println("      and baud rate of <baud>.");
            System.out.println();
            System.out.println("  -e  <asm>");
            System.out.println("      Launches the command line emulator to simulate <asm>.");
            System.out.println();
        }
    }
}
