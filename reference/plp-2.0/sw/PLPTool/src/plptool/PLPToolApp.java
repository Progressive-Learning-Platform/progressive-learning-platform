/*
 * PLPToolApp.java
 */

package plptool;

import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;
import java.util.HashMap;
import java.util.Iterator;

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

        PLPAsm plpAssembler = null;
        long objCode[];
        long addrTable[];
        HashMap symTable;
        
        if(args.length > 0 && args[0].equals("-a")) {
            if(args.length != 4) {
                System.out.println("Usage: PLPTool -a <asm> <addr> <out>");
                System.exit(-1);
            } else {
                PLPMsg.I("Assembling " + args[1] + ".", null);
                plpAssembler = new PLPAsm(null, args[1], Integer.parseInt(args[2]));
                if(plpAssembler.preprocess(0) == PLPMsg.PLP_OK)
                    plpAssembler.assemble();

                if(plpAssembler.isAssembled()) {
                    objCode = plpAssembler.getObjectCode();
                    addrTable = plpAssembler.getAddrTable();
                    symTable = plpAssembler.getSymTable();

                    PLPAsmFormatter.symTablePrettyPrint(symTable);
                    System.out.println();
                    PLPAsmFormatter.prettyPrint(plpAssembler);
                    PLPAsmFormatter.writeBin(objCode, args[3]);
                    PLPAsmFormatter.writeCOE(objCode, args[3]);
                    System.out.println();
                    PLPMsg.I("Assembly completed.", null);
                } else {
                    PLPMsg.E("Assembly of " + args[1] + " failed.",
                             PLPMsg.PLP_ERROR_RETURN, null);
                }
            }
        }
        else if(args.length == 0)
            launch(PLPToolApp.class, args);
        
        else {
            System.out.println("Invalid arguments.\n");
            System.out.println("Run PLPTool with no command line arguments to launch GUI tool.");
            System.out.println();
            System.out.println("Non-GUI options:\n");
            System.out.println("  -a  <asm> <addr> <out>");
            System.out.println("      Assemble <asm> with initial address <addr> and write binary");
            System.out.println("      output to <out>.bin");
            System.out.println();
            System.out.println("  -p  <bin> <port> <baud>");
            System.out.println("      Program PLP target board with <bin> using serial port <port>");
            System.out.println("      and baud rate of <baud>.");


        }
    }
}
