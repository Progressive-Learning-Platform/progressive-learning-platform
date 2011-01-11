/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.gui;

import org.jdesktop.application.Application;
import org.jdesktop.application.SingleFrameApplication;
import plptool.PLPMsg;
import plptool.Constants;
import plptool.mods.IORegistry;

/**
 * The main class of the application.
 */
public class PLPToolApp extends SingleFrameApplication {
    
    PLPSimShell simUI;
    static String plpFilePath = null;
    static boolean open = false;

    /**
     * At startup create and show the main frame of the application.
     */
    @Override protected void startup() {
        PLPBackend backend = new PLPBackend(true, "plpmips"); // default to plpmips for now
        backend.app = this;
        backend.ioreg = new IORegistry();
        backend.g_err = new PLPErrorFrame();
        backend.g_dev = new PLPDevelop(backend);
        backend.g_ioreg = new PLPIORegistry(backend);
        simUI = new PLPSimShell(backend);
        backend.g_simsh = simUI;
        backend.g_desktop = simUI.getSimDesktop();
        backend.g_about = new PLPToolAboutBox(backend.g_dev);
        backend.g_opts = new PLPOptions(backend);
        simUI.getSimDesktop().add(backend.g_ioreg);

        backend.g_dev.setVisible(true);

        PLPMsg.output = backend.g_dev.getOutput();
        if(plpFilePath != null)
            backend.openPLPFile(plpFilePath);
        // show(mainWindow);
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
        System.out.println("Copyright 2010 David Fritz, Brian Gordon, Wira Mulia");
        System.out.println(Constants.versionString + "\n");
        System.out.println("This software is licensed under GPLv3.");
        System.out.println();

        if(args.length > 0 && args[0].equals("-a")) {
            if(args.length != 3) {
                System.out.println("Usage: PLPTool -a <asm> <out>");
                System.exit(-1);
            } else {
                plptool.mips.Formatter.genPLP(args[1], args[2], false);
            }
        }
        else if(args.length > 0 && args[0].equals("-af")) {
            if(args.length != 3) {
                System.out.println("Usage: PLPTool -af <asm> <out>");
                System.exit(-1);
            } else {
                plptool.mips.Formatter.genPLP(args[1], args[2], true);
            }
        }
        else if(args.length > 0 && args[0].equals("-s")) {
            if(args.length == 1) {
                plptool.mips.SimCLI.simCL(null, null, null);
            }
            else if(args.length != 2) {
                System.out.println("Usage: PLPTool -s <asm>");
            } else
                plptool.mips.SimCLI.simCL(null, args[1], null);
        }
        else if(args.length > 0 && args[0].equals("-p")) {
            if(args.length != 4) {
                System.out.println("Usage: PLPTool -p <plpfile> <port> <baud>");
            } else {
                plptool.mips.SerialProgrammer plpProg = new plptool.mips.SerialProgrammer();
                try {
                    plpProg.connect(args[2], Integer.parseInt(args[3]));
                    plpProg.programWithPLPFile(args[1]);
                    plpProg.close();
                } catch(Exception e) {
                    
                }
            }
        }
        else if(args.length == 1) {
            open = true;
            plpFilePath = args[0];
            launch(PLPToolApp.class, args);
        }

        else if(args.length == 0) {
            launch(PLPToolApp.class, args);
        }
        else {
            PLPMsg.E("Invalid argument(s).", Constants.PLP_TOOLAPP_ERROR, null);
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
            System.out.println("  -s  <asm>");
            System.out.println("      Launches the command line emulator to simulate <asm>.");
            System.out.println();
        }
    }
}
