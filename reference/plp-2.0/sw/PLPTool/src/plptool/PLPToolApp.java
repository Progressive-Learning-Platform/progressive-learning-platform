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
        PLPAsm plpAssembler = null;
        int objCode[];
        int addrTable[];
        HashMap symTable;
        Iterator iterator;
        String key, value;
        
        if(args.length > 0 && args[0].equals("-cl")) {
            if(args.length != 3) {
                System.out.println("Usage: PLPTool -cl <ASM> <Start Address>");
                System.exit(-1);
            } else {
                plpAssembler = new PLPAsm(null, args[1], Integer.parseInt(args[2]));
                plpAssembler.preprocess(0);
                plpAssembler.assemble();

                if(plpAssembler.isAssembled()) {
                    objCode = plpAssembler.getObjectCode();
                    addrTable = plpAssembler.getAddrTable();
                    symTable = plpAssembler.getSymTable();

                    for(int i = 0; i < objCode.length; i++)
                        System.out.println(Integer.toHexString(addrTable[i]) +
                                "\t" + Integer.toBinaryString(objCode[i]));

                    iterator = symTable.keySet().iterator();

                    while(iterator.hasNext()) {
                        key = iterator.next().toString();
                        value = symTable.get(key).toString();

                        System.out.println(key + "\t:\t" + value);
                    }


                } else {
                    PLPMsg.PLPInfo("Assembly of " + args[1] + " failed.", null);
                }
            }
        } else {
            System.out.println("\nPLP Java Tool");
            System.out.println("Authors: David Fritz, Brian Gordon, Wira Mulia");
            System.out.println("\nProvide -cl option for command line assembler");
            launch(PLPToolApp.class, args);
        }
    }
}
