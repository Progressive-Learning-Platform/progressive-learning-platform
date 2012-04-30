/*
    Copyright 2010-2012 David Fritz, Brian Gordon, Wira Mulia

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

/*
 * Apache Commons Compress
 * Copyright 2002-2010 The Apache Software Foundation
 *
 * This product includes software developed by
 * The Apache Software Foundation (http://www.apache.org/).
 */

package plptool.gui;

import plptool.*;
import plptool.gui.frames.*;
import plptool.mods.Preset;
import plptool.mods.IORegistry;

import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;

import java.util.Scanner;
import java.util.ArrayList;
import java.util.HashMap;

import javax.swing.tree.*;
import javax.swing.table.DefaultTableModel;

/**
 * This is the PLPTool application project backend. This class handles
 * manipulation of the PLP project file and drives the workflow of a
 * PLP tool user. This includes creating, opening, and saving project files.
 * Driving workflow functions such as assembling, simulating, and
 * programming the board.
 *
 * @author wira
 */
public class ProjectDriver {

    public PLPToolApp              app;        // App

    /*
     * These variables hold some project file information for this driver.
     * plpfile      - the path to the plpfile ProjectDriver will work with
     * modified     - denotes whether the plpfile is modified since opening
     * open_asm     - current open ASM file in the gui
     * curdir       - current working directory for the project
     * arch         - active ISA for this project
     * sim_mode	    - denotes whether the project is in simulation mode
     * replay       - denotes whether the project is in replay mode
     */ // --

    public File                    plpfile;
    private boolean                modified;
    private boolean                dirty;
    private int                    open_asm;
    public String                  curdir;
    private PLPArchitecture        arch;
    private boolean                sim_mode;
    private boolean                replay;
    private boolean                asm_req;

    /*
     * These variables hold data and information loaded from
     * the plp project file.
     */ // --

    public byte[]                  binimage;   // binary image
    public String                  hexstring;  // hex string
    public String                  meta;       // Meta String
    public Preset                  smods;      // Saved mods information
    public DefaultTableModel       watcher;    // Watcher entries

    /*
     * References to PLP configuration and messaging classes
     */ // --

    public Config                  cfg;        // Configuration
    public Msg                     msg;        // Messaging class

    private ArrayList<PLPAsmSource> asms;       // Assembly files

    private boolean                halt;       // critical error
    
    /*
     * References to the workflow framework objects
     */ // --

    public PLPAsm                  asm;        // Assembler
    public PLPAsm[]                asm_array;  // Asm array
    public PLPLinker               lnkr;       // Linker
    public PLPSerialProgrammer     prg;        // Programmer

    public PLPSimCore              sim;        // Simulation core
    public IORegistry              ioreg;      // I/O registry
    public PLPSimCoreGUI           g_sim;      // Sim Core GUI

    /*
     * PLP GUI Windows
     */ // --
    public IORegistryFrame         g_ioreg;    // I/O registry GUI
    public Develop                 g_dev;      // IDE GUI
    public SimErrorFrame           g_err;      // Error frame
    public AboutBoxDialog          g_about;    // About frame
    public OptionsFrame            g_opts;     // Options frame
    public ProgrammerDialog        g_prg;      // Programming dialog
    public AsmNameDialog           g_fname;    // ASM Name dialog
    public SimRunner               g_simrun;   // SimRunner thread
    public Watcher                 g_watcher;  // Watcher window
    public SimControl              g_simctrl;  // Simulation Control Frame
    public ASMSimView              g_asmview;  // ASM Sim viewer
    public QuickRef                g_qref;     // Quick Reference
    public ISASelector             g_isaselect;// ISA selector window
    public FindAndReplace          g_find;     // Find and Replace
    private boolean                g;          // are we driving a GUI?
    private boolean                applet;     // are we driving an applet?

    // Programmer
    private boolean                serial_support;
    public gnu.io.SerialPort       p_port;
    public int                     p_progress;
    public TimeoutWatcher          p_watchdog;

    // Others
    public SerialTerminal          term;        // Serial terminal
    public NumberConverter         nconv;       // Number converter

    // Miscellaneous project attributes persistence support
    private HashMap<String, Object> pAttrSet;

    /**
     * The constructor for the project driver.
     *
     * @param g Specifies whether we are driving a GUI or not
     * @param archID The ISA to use for this project
     */
    public ProjectDriver(int modes) {
        this.g = (modes & Constants.PLP_GUI_START_IDE) == Constants.PLP_GUI_START_IDE;
        this.applet = (modes & Constants.PLP_GUI_APPLET) == Constants.PLP_GUI_APPLET;
        
        arch = null;
        modified = false;
        plpfile = null;
        halt = false;
        sim_mode = false;
        replay = false;
        asm_req = false;

        if(applet) asms = new ArrayList<PLPAsmSource>();
        pAttrSet = new HashMap<String, Object>();

        this.ioreg = new IORegistry(this);
        if(!applet) this.curdir = (new java.io.File(".")).getAbsolutePath();
        
        if(g && !applet) {
            this.g_err = new SimErrorFrame();
            this.g_dev = new Develop(this);
            this.g_ioreg = new IORegistryFrame(this);
            this.g_about = new AboutBoxDialog(this.g_dev);
            this.g_opts = new OptionsFrame(this);
            this.g_prg = new ProgrammerDialog(this, this.g_dev, true);
            this.g_fname = new AsmNameDialog(this, this.g_dev, true);
            this.g_find = new FindAndReplace(this);
            this.g_isaselect = new ISASelector(this.g_dev, this);
            
            java.awt.Dimension screenResolution = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
            int X = Config.devWindowPositionX;
            int Y = Config.devWindowPositionY;
            int W = Config.devWindowWidth;
            int H = Config.devWindowHeight;
            if(X < 0 || Y < 0 || W < 0 || H < 0) {
                this.g_dev.setSize((int) (Config.relativeDefaultWindowWidth * screenResolution.width),
                              (int) (Config.relativeDefaultWindowHeight * screenResolution.height));
                this.g_dev.setLocationRelativeTo(null);
            } else if (X+W <= screenResolution.width && Y+H <= screenResolution.height) {
                this.g_dev.setSize(W, H);
                this.g_dev.setLocation(X, Y);
            } else {
                this.g_dev.setSize((int) (Config.relativeDefaultWindowWidth * screenResolution.width),
                              (int) (Config.relativeDefaultWindowHeight * screenResolution.height));
                this.g_dev.setLocationRelativeTo(null);
            }

            this.g_find.setLocationRelativeTo(null);

            this.g_dev.setTitle("PLP Software Tool " + Text.versionString);
            this.g_dev.setVisible(true);
        }

        // check for JRE version

        String tokens[] = System.getProperty("java.version").split("\\.");
        int major = Integer.parseInt(tokens[0]);
        int minor = Integer.parseInt(tokens[1]);

        if((major == Constants.minimumJREMajorVersion && minor < Constants.minimumJREMinorVersion) ||
            major  < Constants.minimumJREMajorVersion) {
            Msg.W("You are running an older Java Runtime Environment version." +
                  " Some functionalities may not work as intended. " +
                  "Please upgrade to at least JRE version 1.5", null);
        }

        // check for rxtx native libaries
        serial_support = true;

        try {
            if(!applet) gnu.io.RXTXVersion.getVersion();
        } catch(UnsatisfiedLinkError e) {
            Msg.W("Failed to detect native RXTX library. " +
                  "Functionality requiring serial communication will fail.", null);
            Msg.W(" - If you are running Linux, make sure that RXTX library is installed.", null);
            Msg.W(" - If you are running Windows, make sure that the .dll files are in the " +
                  "same directory and you run the batch file associated with " +
                  "your version of Windows (32- or 64-bit)", null);
            serial_support = false;
        } catch(NoClassDefFoundError e) {
            Msg.W("Unsatisfied RXTX link.", null);
        }
    }

    /**
     * Set a new architecture for the project
     *
     * @param arch
     * @return
     */
    public int setArch(int archID) {
        this.arch = ArchRegistry.getArchitecture(this, archID);

        if(arch == null) {
            arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
            return Msg.E("Invalid ISA ID: " + archID + ". Defaulting to " +
                         "plpmips (id 0).",
                         Constants.PLP_ISA_INVALID_ARCHITECTURE_ID, this);
        } else
            return Constants.PLP_OK;
    }

    /**
     *
     * @return the current active ISA for the project
     */
    public PLPArchitecture getArch() {
        return arch;
    }

    /**
     * Attempt to load configuration from ~/.plp/config. Also checks whether
     * we have access to a user configuration directory (~/.plp)
     */
    public static void loadConfig() {
        File confDir = new File(PLPToolbox.getConfDir());
        if(!confDir.exists() && !confDir.mkdir()) {
            Msg.W("Unable to create configuration directory. User settings " +
                  "will not be saved and module functionality will fail!",
                  null);
        }

        if(confDir.exists() &&
                (!confDir.isDirectory() || !confDir.canWrite())) {
            Msg.W("Configuration directory is either a file or is not " +
                  "writable. User settings will not be saved and module " +
                  "functionality will fail!", null);
        }

        File config = new File(System.getProperty("user.home") + "/.plp/config");

        if(config.exists() && !config.isDirectory()) {
            Msg.D("Loading config from " + config.getAbsolutePath(), 2, null);
            try {
                FileInputStream in = new FileInputStream(config);

                String tokens[];
                byte[] str = new byte[(int) config.length()];
                in.read(str);
                in.close();
                String lines[] = new String(str).toString().split("\\r?\\n");

                for(int i = 0; i < lines.length; i++) {
                    tokens = lines[i].split("::", 2);

                    if(tokens[0].equals("devFont")) {
                        Config.devFont = tokens[1];
                    } else if(tokens[0].equals("devFontSize")) {
                        Config.devFontSize = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devSyntaxHighlighting")) {
                        Config.devSyntaxHighlighting = Boolean.parseBoolean(tokens[1]);
                    } else if(tokens[0].equals("simFunctional")) {
                        Config.simFunctional = Boolean.parseBoolean(tokens[1]);
                    } else if(tokens[0].equals("devWindowPositionX")) {
                        Config.devWindowPositionX = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devWindowPositionY")) {
                        Config.devWindowPositionY = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devWindowWidth")) {
                        Config.devWindowWidth = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("devWindowHeight")) {
                        Config.devWindowHeight = Integer.parseInt(tokens[1]);
                    } else if(tokens[0].equals("cfgAskBeforeAutoloadingModules")) {
                        Config.cfgAskBeforeAutoloadingModules = Boolean.parseBoolean(tokens[1]);
                    } else if(tokens[0].equals("cfgAskForISAForNewProjects")) {
                        Config.cfgAskForISAForNewProjects = Boolean.parseBoolean(tokens[1]);
                    } else
                        PLPToolApp.getAttributes().put(tokens[0], tokens.length==2 ? tokens[1] : null);
                }

            } catch(Exception e) {
                Msg.E("Failed to load PLPTool configuration from disk.",
                      Constants.PLP_BACKEND_LOAD_CONFIG_FAILED, null);
            }
        }
    }

    /**
     * Save PLPTool configuration to ~/.plp/config
     */
    public static void saveConfig() {
        File configDir = new File(System.getProperty("user.home") + "/.plp");

        if(!configDir.exists()) {
            try {
                if(!configDir.mkdir()) {
                    Msg.W("Failed to save PLPTool configuration: " +
                          "Unable to create directory $[USER]/.plp", null);
                }
            } catch(Exception e) {
                Msg.E("Failed to save PLPTool configuration to disk.",
                      Constants.PLP_BACKEND_SAVE_CONFIG_FAILED, null);
            }
        }

        File config = new File(System.getProperty("user.home") + "/.plp/config");

        if(config != null) {
            Msg.D("Saving config to " + config.getAbsolutePath(), 2, null);
            try {
                FileWriter out = new FileWriter(config);
                out.write("devFont::" + Config.devFont + "\n");
                out.write("devFontSize::" + Config.devFontSize + "\n");
                out.write("devSyntaxHighlighting::" + Config.devSyntaxHighlighting + "\n");
                out.write("simFunctional::" + Config.simFunctional + "\n");
                out.write("devWindowPositionX::" + Config.devWindowPositionX + "\n");
                out.write("devWindowPositionY::" + Config.devWindowPositionY + "\n");
                out.write("devWindowWidth::" + Config.devWindowWidth + "\n");
                out.write("devWindowHeight::" + Config.devWindowHeight + "\n");
                out.write("cfgAskBeforeAutoloadingModules::" + Config.cfgAskBeforeAutoloadingModules + "\n");
                out.write("cfgAskForISAForNewProjects::" + Config.cfgAskForISAForNewProjects + "\n");
                for(int i = 0; i < 5; i++) {
                    String key = "develop_recent_" + i;
                    if(PLPToolApp.getAttributes().containsKey(key))
                        out.write(key + "::" + PLPToolApp.getAttributes().get(key) + "\n");
                }
                // see if any modules want to save out their configuration
                // --- any configuration saved will be loaded by loadConfig to
                //     the application attributes, so no hook on loading is
                //     necessary. The converse is not true, the module will have
                //     to use this hook to keep its configuration
                DynamicModuleFramework.hook(new ProjectEvent(ProjectEvent.CONFIG_SAVE, -1, out));
                out.close();

            } catch(Exception e) {
                Msg.E("Failed to save PLPTool configuration to disk.",
                      Constants.PLP_BACKEND_SAVE_CONFIG_FAILED, null);
            }
        }
    }

    /**
     * Delete ~/.plp/config
     */
    public static void removeConfig() {
        File config = new File(System.getProperty("user.home") + "/.plp/config");

        if(config.exists()) {
            Msg.M("Removing " + config.getAbsolutePath());
            config.delete();
        }
    }

    /**
     * Initialize plp project data structures
     *
     * @return PLP_OK
     */
    public int create(int archID) {
        modified = true;
        asm_req = true;
        plpfile = new File("Unsaved Project");

        try {
            this.arch = ArchRegistry.getArchitecture(this, archID);
            if(arch == null) {
                Msg.W("Invalid architecture ID is specified, reverting to " +
                      "default (plpmips).", this);
                this.arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
            }
        } catch(Exception e) {
            Msg.E("FATAL ERROR: invalid arch ID during ProjectDriver" +
                  "create routine (archID: " + archID + ")",
                  Constants.PLP_FATAL_ERROR, null);
            System.exit(-1);
        }

        asms = new ArrayList<PLPAsmSource>();
        asms.add(new PLPAsmSource("# main source file\n\n.org 0x10000000", "main.asm", 0));
        open_asm = 0;
        smods = null;
        watcher = null;
        pAttrSet = new HashMap<String, Object>();

        meta =  "PLP-4.0\n";
        meta += "START=0x0\n";
        meta += "DIRTY=1\n\n";
        dirty = true;
        hookEvent(new ProjectEvent(ProjectEvent.NEW_PROJECT, -1));
        Msg.I("New project initialized.", null);

        if(g) {
            refreshProjectView(false);
            desimulate();
            g_dev.disableSimControls();
            g_dev.enableBuildControls();
        }

        return Constants.PLP_OK;
    }

    /**
     * Initialize project data structures and attempt to import the specified
     * assembly source file into the project.
     *
     * @param asmPath Path to ASM file to import
     * @return PLP_OK
     */
    public int create(String asmPath, int archID) {
        modified = true;
        asm_req = true;
        plpfile = new File("Unsaved Project");

        try {
            this.arch = ArchRegistry.getArchitecture(this, archID);
            if(arch == null) {
                Msg.W("Invalid architecture ID is specified, reverting to " +
                      "default (plpmips).", this);
                this.arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
            }
        } catch(Exception e) {
            Msg.E("FATAL ERROR: invalid arch ID during ProjectDriver" +
                  "create routine (archID: " + archID + ")",
                  Constants.PLP_FATAL_ERROR, null);
            System.exit(-1);
        }

        asms = new ArrayList<PLPAsmSource>();
        if(importAsm(asmPath) != Constants.PLP_OK) {
            asms.add(new PLPAsmSource("# main source file\n\n.org 0x10000000", "main.asm", 0));
        }
        open_asm = 0;
        smods = null;
        watcher = null;
        pAttrSet = new HashMap<String, Object>();

        meta =  "PLP-4.0\n";
        meta += "START=0x0\n";
        meta += "DIRTY=1\n\n";
        dirty = true;

        Msg.I("New project initialized.", null);

        if(g) {
            refreshProjectView(false);
            desimulate();
            g_dev.disableSimControls();
            g_dev.enableBuildControls();
        }

        return Constants.PLP_OK;
    }

    /**
     * Save current project state to the file specified by plpfile.
     *
     * @return PLP_OK on successful save, error code otherwise
     */
    public int save() {

        if(sim_mode) {
            smods = ioreg.createPreset();
            watcher = g_watcher.getEntries();
        }

        // commit changes of currently open source file
        if(g) updateAsm(open_asm, g_dev.getEditorText());
        //assemble();

        if(plpfile == null || plpfile.getName().equals("Unsaved Project"))
            return Msg.E("No PLP project file is open. Use Save As.",
                            Constants.PLP_FILE_USE_SAVE_AS, null);

        ArrayList<PLPAsmSource> sourceList;
        String verilogHex = "";
        long[] objCode = null;
        PLPAsmSource temp;
        int i;

        try {

        File outFile = plpfile;

        meta = "PLP-4.0\n";

        if(asm != null && asm.isAssembled()) {
            objCode = asm.getObjectCode();
            if(arch.getID() == ArchRegistry.ISA_PLPMIPS) {
                Msg.D("Creating verilog hex code...", 2, this);
                verilogHex = plptool.mips.Formatter.writeVerilogHex(objCode);
            }
            if(objCode.length > 0)
                meta += "START=" + asm.getAddrTable()[0] + "\n";
            else
                meta += "START=0\n";
            meta += "DIRTY=0\n";
            dirty = false;
        } else {
            meta += "DIRTY=1\n";
            dirty = true;
        }

        meta += "ARCH=" + arch.getID() + "\n";

        meta += "\n";

        sourceList = asms;

        for(i = 0; i < sourceList.size(); i++) {
            temp = (PLPAsmSource) sourceList.get(i);
            meta += temp.getAsmFilePath() + "\n";
        }

        // Create plpfile (a tar archive)
        TarArchiveOutputStream tOut = new TarArchiveOutputStream(new FileOutputStream(outFile));

        Msg.D("Writing plp.metafile...", 2, this);
        TarArchiveEntry entry = new TarArchiveEntry("plp.metafile");
        entry.setSize(meta.length());
        tOut.putArchiveEntry(entry);
        byte[] data = meta.getBytes();
        tOut.write(data);
        tOut.flush();
        tOut.closeArchiveEntry();

        for(i = 0; i < sourceList.size(); i++) {
            PLPAsmSource asmFile = sourceList.get(i);
            Msg.D("Writing " + asmFile.getAsmFilePath() + "...", 2, this);
            entry = new TarArchiveEntry(asmFile.getAsmFilePath());
            
            // We are not expecting an .asm file with size greater than 4GiB
            // ... I hope...
            byte[] fileStr = asmFile.getAsmString().getBytes();
            entry.setSize(fileStr.length);
            tOut.putArchiveEntry(entry);
            tOut.write(fileStr);
            tOut.flush();
            tOut.closeArchiveEntry();
        }

        // Write simulation configuration
        Msg.D("Writing out simulation configuration...", 2, this);
        entry = new TarArchiveEntry("plp.simconfig");
        String str = "";

        str += "simRunnerDelay::" + Config.simRunnerDelay + "\n";
        str += "simAllowExecutionOfArbitraryMem::" + Config.simAllowExecutionOfArbitraryMem + "\n";
        str += "simBusReturnsZeroForUninitRegs::" + Config.simBusReturnsZeroForUninitRegs + "\n";
        str += "simDumpTraceOnFailedEvaluation::" + Config.simDumpTraceOnFailedEvaluation + "\n";


        if(watcher != null) {
            str += "WATCHER\n";

            for(i = 0; i < watcher.getRowCount(); i++) {
                str += watcher.getValueAt(i, 0) + "::";
                str += watcher.getValueAt(i, 1) + "\n";
            }

            str += "END\n";
        }

        Msg.D("-- saving mods info...", 2, this);

        if(ioreg != null && ioreg.getNumOfModsAttached() > 0)
            smods = ioreg.createPreset();

        if(smods != null && smods.size() > 0) {
            str += "MODS\n";

            for(i = 0; i < smods.size(); i++) {
                str += smods.getType(i) + "::";     //0
                str +="RESERVED_FIELD::";       //1
                str += smods.getAddress(i) + "::";      //2
                str += smods.getSize(i) + "::";  //3

                if(smods.getHasFrame(i)) {
                    str += "frame::" ;              //4
                    str += smods.getVisible(i) + "::"; //5
                    str += smods.getX(i) + "::";      //6
                    str += smods.getY(i) + "::";      //7
                    str += smods.getW(i) + "::";  //8
                    str += smods.getH(i);        //9
                } else {
                    str += "noframe";
                }
                str += "\n";
            }
            str += "END\n";
        }

        str += "ISASPECIFIC\n";
        str += arch.saveArchSpecificSimStates();
        str += "END\n";

        entry.setSize(str.getBytes().length);
        tOut.putArchiveEntry(entry);
        tOut.write(str.getBytes());
        tOut.flush();
        tOut.closeArchiveEntry();

        if(asm != null && asm.isAssembled() && objCode != null) {
            // Write hex image
            Msg.D("Writing out verilog hex code...", 2, this);
            entry = new TarArchiveEntry("plp.hex");
            entry.setSize(verilogHex.length());
            tOut.putArchiveEntry(entry);
            data = new byte[verilogHex.length()];
            for(i = 0; i < verilogHex.length(); i++) {
                data[i] = (byte) verilogHex.charAt(i);
            }
            tOut.write(data);
            tOut.flush();
            tOut.closeArchiveEntry();

            // Write binary image, 4-byte big-endian packs
            Msg.D("Writing out binary image...", 2, this);
            entry = new TarArchiveEntry("plp.image");
            entry.setSize(objCode.length * 4);
            tOut.putArchiveEntry(entry);
            data = new byte[objCode.length * 4];
            for(i = 0; i < objCode.length; i++) {
                data[4*i] = (byte) (objCode[i] >> 24);
                data[4*i+1] = (byte) (objCode[i] >> 16);
                data[4*i+2] = (byte) (objCode[i] >> 8);
                data[4*i+3] = (byte) (objCode[i]);
            }
            tOut.write(data);
            tOut.flush();
            tOut.closeArchiveEntry();

        } else if(binimage != null) {
            Msg.D("Writing out old (dirty) verilog hex code...", 2, this);
            entry = new TarArchiveEntry("plp.hex");
            entry.setSize(hexstring.length());
            tOut.putArchiveEntry(entry);
            tOut.write(hexstring.getBytes());
            tOut.flush();
            tOut.closeArchiveEntry();
            
            Msg.D("Writing out old (dirty) binary image...", 2, this);
            entry = new TarArchiveEntry("plp.image");
            entry.setSize(binimage.length);
            tOut.putArchiveEntry(entry);
            tOut.write(binimage);
            tOut.flush();
            tOut.closeArchiveEntry();
        }

        // Hook for project save
        DynamicModuleFramework.hook(new ProjectEvent(ProjectEvent.PROJECT_SAVE, -1, tOut));

        Msg.D("Closing tar archive...", 2, this);
        tOut.close();
        Msg.D("Project save completed", 2, this);

        modified = false;
        if(g) refreshProjectView(false);
        Msg.I(plpfile.getAbsolutePath() + " written", null);

        } catch(Exception e) {
            if(Constants.debugLevel >= 2) e.printStackTrace();
            Msg.E("save: Unable to write to " +
                    plpfile.getAbsolutePath() + ". " +
                    "Do you have access to the specified location?",
                    Constants.PLP_FILE_SAVE_ERROR, this);
            return Constants.PLP_FILE_SAVE_ERROR;
        }


        return Constants.PLP_OK;
    }

    /**
     * Open plp file specified by path.
     *
     * @param path Path to project file to load.
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int open(String path, boolean assemble) {
        File plpFile = new File(path);
        
        if(!plpFile.exists())
            return Msg.E("open(" + path + "): File not found.",
                            Constants.PLP_BACKEND_PLP_OPEN_ERROR, null);

        boolean metafileFound = false;
        dirty = true;

        Msg.I("Opening " + path, null);

        arch = null;

        asms = new ArrayList<PLPAsmSource>();
        smods = null;
        watcher = null;
        pAttrSet = new HashMap<String, Object>();
        HashMap<String, Integer> asmFileOrder = new HashMap<String, Integer>();

        try {

        TarArchiveInputStream tIn = new TarArchiveInputStream(new FileInputStream(plpFile));
        TarArchiveEntry entry;
        byte[] image;
        String metaStr;

        // Find meta file first
        while((entry = tIn.getNextTarEntry()) != null) {
            if(entry.getName().equals("plp.metafile")) {
                image = new byte[(int) entry.getSize()];
                tIn.read(image, 0, (int) entry.getSize());
                metaStr = new String(image);

                metafileFound = true;
                meta = metaStr;
                Scanner metaScanner;

                String lines[] = meta.split("\\r?\\n");
                if(lines[0].equals("PLP-4.0"))  {

                } else {
                    Msg.W("This is not a PLP-4.0 project file. Opening anyways.", this);
                }

                metaScanner = new Scanner(meta);
                metaScanner.findWithinHorizon("DIRTY=", 0);
                if(metaScanner.nextInt() == 0)
                    dirty = false;
                if(metaScanner.findWithinHorizon("ARCH=", 0) != null) {
                    String temp = metaScanner.nextLine();
                    if(Config.cfgOverrideISA >= 0) { // ISA ID override, ignore the metafile
                        arch = ArchRegistry.getArchitecture(this, Config.cfgOverrideISA);
                    } else if (temp.equals("plpmips")) {
                        Msg.W("This project file is made by PLPTool version 3 or earlier. " +
                              "Meta data for this project will be updated " +
                              "with the default ISA (plpmips) when the project " +
                              "file is saved.", this);
                        arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
                    } else {
                        arch = ArchRegistry.getArchitecture(this, Integer.parseInt(temp));
                        if(arch == null) {
                            Msg.W("Invalid ISA ID is specified in the project file: '" + temp +
                                  "'. Assuming plpmips.", this);
                            arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
                        }
                    }
                    arch.hook(this);
                }

                // get asm files order
                int asmOrder = 0;
                while(metaScanner.hasNext()) {
                    String asmName = metaScanner.nextLine();
                    if(asmName.endsWith(".asm")) {
                        asmFileOrder.put(asmName, new Integer(asmOrder));
                        asmOrder++;
                        asms.add(null);
                    }
                }
            }
        }

        if(!metafileFound)
            return Msg.E("No PLP metadata found.",
                    Constants.PLP_BACKEND_INVALID_PLP_FILE, this);

        // reset the tar input stream
        tIn = new TarArchiveInputStream(new FileInputStream(plpFile));

        while((entry = tIn.getNextTarEntry()) != null) {
            boolean handled = false;
            image = new byte[(int) entry.getSize()];
            tIn.read(image, 0, (int) entry.getSize());
            metaStr = new String(image);

            // Hook for project open for each entry
            Object[] eParams = {entry.getName(), image, plpFile};
            for(int i = 0; i < DynamicModuleFramework.getNumberOfGenericModuleInstances(); i++) {
                Object ret = DynamicModuleFramework.hook(i, new ProjectEvent(ProjectEvent.PROJECT_OPEN_ENTRY, -1, eParams));
                if(ret != null && ret instanceof Boolean)
                    handled = (Boolean) ret;
            }

            if(entry.getName().endsWith("asm") && !entry.getName().startsWith("plp.")) {
                Integer order = (Integer) asmFileOrder.get(entry.getName());
                if(order == null)
                    Msg.W("The file '" + entry.getName() + "' is not listed in " +
                          "the meta file. This file will be removed when the project " +
                          "is saved.", this);
                else {
                    asms.set(order, new PLPAsmSource(metaStr, entry.getName(), order));
                    hookEvent(new ProjectEvent(ProjectEvent.OPEN_ASM_ENTRY, -1, order));
                }

            } else if(entry.getName().equals("plp.metafile")) {
                // we've done reading the metafile

            } else if (entry.getName().equals("plp.image")) {
                binimage = new byte[(int) entry.getSize()];
                binimage = image;

            } else if(entry.getName().equals("plp.hex")) {
                hexstring = new String(image);
            
            // Restore bus modules states
            } else if (entry.getName().equals("plp.simconfig")) {
                Msg.D("simconfig:\n" + metaStr + "\n", 4, this);
                String lines[] = metaStr.split("\\r?\\n");
                int i;

                for(i = 0; i < lines.length; i++) {
                    String tokens[] = lines[i].split("::");

                    if(lines[i].startsWith("simRunnerDelay")) {
                        Config.simRunnerDelay = Integer.parseInt(tokens[1]);
                    }

                    if(lines[i].equals("MODS")) {
                        i++;
                        this.smods = new Preset();

                        while(i < lines.length && !lines[i].equals("END")) {
                            tokens = lines[i].split("::");
                            if(tokens.length > 4 && tokens[4].equals("noframe"))
                                smods.addModuleDefinition(Integer.parseInt(tokens[0]),
                                        Long.parseLong(tokens[2]),
                                        Long.parseLong(tokens[3]), false, false);
                            else if(tokens.length > 4)
                                smods.addModuleDefinition(Integer.parseInt(tokens[0]),
                                        Long.parseLong(tokens[2]),
                                        Long.parseLong(tokens[3]), true,
                                        Boolean.parseBoolean(tokens[5]));

                            i++;
                        }
                    }

                    if(lines[i].equals("WATCHER")) {
                        i++;
                        this.watcher = Watcher.getTableInitialModel();

                        while(i < lines.length && !lines[i].equals("END")) {
                            tokens = lines[i].split("::");
                            Object row[] = {tokens[0], tokens[1], null, null};
                            watcher.addRow(row);
                            i++;
                        }
                    }

                    if(lines[i].equals("ISASPECIFIC")) {
                        i++;

                        while(i < lines.length && !lines[i].equals("END")) {
                            tokens = lines[i].split("::");
                            arch.restoreArchSpecificSimStates(tokens);
                            i++;
                        }
                    }
                }
            } else if(handled) {

            } else {
                Msg.W("open(" + path + "): unable to process entry: " +
                        entry.getName() + ". This file will be removed when"
                        + " you save the project.", this);
            }
        }

        tIn.close();
        
        if(asmFileOrder.isEmpty()) {
            return Msg.E("open(" + path + "): no .asm files found.",
                            Constants.PLP_BACKEND_INVALID_PLP_FILE, null);
        }

        }
        catch(Exception e) {
            if(Constants.debugLevel >= 2) e.printStackTrace();
            return Msg.E("open(" + path + "): Invalid PLP archive.",
                            Constants.PLP_BACKEND_INVALID_PLP_FILE, null);
        }

        if(arch == null) {
            Msg.W("No ISA information specified in the archive, assuming plpmips", this);
            arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
        }

        plpfile = new File(path);
        modified = false;
        open_asm = 0;

        for(int i = 0; i < asms.size(); i++)
            Msg.I(i + ": " + asms.get(i).getAsmFilePath(), null);

        if(g) refreshProjectView(false);
        if(!dirty && assemble) {
            assemble();
            asm_req = false;
        } else
            asm_req = true;

        if(g) {
            g_opts.restoreSavedOpts();
            desimulate();

            if(asm != null && asm.isAssembled())
                g_dev.enableSimControls();
            else
                g_dev.disableSimControls();
            
            this.setUnModified();
            updateWindowTitle();
            g_dev.updateDevelopRecentProjectList(plpFile.getAbsolutePath());
        }

        hookEvent(new ProjectEvent(ProjectEvent.PROJECT_OPEN, -1, plpFile));
        return Constants.PLP_OK;
    }

    /**
     * Get the project attributes set hashmap
     *
     * @return Project attributes hashmap
     */
    public HashMap<String, Object> getProjectAttributeSet() {
        return pAttrSet;
    }

    /**
     * Add a project attribute to the set
     *
     * @param key Attribute key
     * @param value Attribute value
     */
    public void addProjectAttribute(String key, Object value) {
        if(pAttrSet.containsKey(key))
            pAttrSet.remove(key);

        Msg.D("add attr " + key + ":" + value, 3, this);
        pAttrSet.put(key, value);
    }

    /**
     * Set a new value to a project attribute
     *
     * @param key Attribute key
     * @param value New attribute value
     * @return True if successful, false if key doesn't exist
     */
    public boolean setProjectAttribute(String key, Object value) {
        if(pAttrSet.containsKey(key)) {
            Msg.D("set attr " + key + ":" + value, 3, this);
            pAttrSet.remove(key);
            pAttrSet.put(key, value);
            return true;
        }

        return false;
    }

    /**
     * Get a project attribute
     *
     * @param key Attribute key
     * @return Attribute value if attribute exists, null otherwise
     */
    public Object getProjectAttribute(String key) {
        Msg.D("get attr " + key, 3, this);
        return pAttrSet.get(key);
    }

    /**
     * Remove specified attribute
     *
     * @param key Attribute key
     */
    public void deleteProjectAttribute(String key) {
        if(pAttrSet.containsKey(key))
            pAttrSet.remove(key);
    }

    /**
     * GUI: update window title of the develop window
     *
     * @return PLP_OK;
     */
    public int updateWindowTitle() {
        if(plpfile == null)
            return Constants.PLP_GENERIC_ERROR;

        String windowTitle = plpfile.getName() + ((modified) ? "*" : "") +
                             (sim_mode ? " - Simulation Mode " : "") +
                             " - PLP Software Tool " + Text.versionString;
        g_dev.setTitle(windowTitle);

        return Constants.PLP_OK;
    }

    /**
     * GUI: update the develop window to reflect the current state of the
     * project driver (open file, etc.)
     *
     * @param commitCurrentAsm Whether to commit currently open asm file
     * in the editor before refreshing.
     * @return PLP_OK
     */
    public int refreshProjectView(boolean commitCurrentAsm) {
        Msg.D("Project view refresh...", 3, this);

        if(plpfile == null) {
            g_dev.disableBuildControls();
            g_dev.catchyPLP();

            g_dev.getProjectTree().setModel(new DefaultTreeModel(new DefaultMutableTreeNode("No project file open.")));
            
            return Constants.PLP_OK;
        }

        if(commitCurrentAsm)
            updateAsm(open_asm, g_dev.getEditorText());

        updateWindowTitle();

        DefaultMutableTreeNode root = new DefaultMutableTreeNode(plpfile.getName());
        DefaultMutableTreeNode srcRoot = new DefaultMutableTreeNode("Source Files");
        DefaultMutableTreeNode metaRoot = new DefaultMutableTreeNode("Meta Information");
        root.add(srcRoot);
        root.add(metaRoot);
        for(int i = 0; i < asms.size(); i++)
            srcRoot.add(new DefaultMutableTreeNode(i + ": " + asms.get(i).getAsmFilePath()));

        Scanner metaScanner = new Scanner(meta);
        metaScanner.findWithinHorizon("DIRTY=", 0);
        int meta_dirty =  metaScanner.nextInt();
        metaRoot.add(new DefaultMutableTreeNode("meta.DIRTY=" + meta_dirty));
        metaRoot.add(new DefaultMutableTreeNode("ISA=" + ArchRegistry.getStringID(arch.getID())));

        g_dev.getProjectTree().setModel(new DefaultTreeModel(root));
        for(int i = 0; i < g_dev.getProjectTree().getRowCount(); i++)
            g_dev.getProjectTree().expandRow(i);

        if(!asms.get(open_asm).getAsmString().equals(g_dev.getEditorText()))
            g_dev.setEditorText(asms.get(open_asm).getAsmString());

        g_dev.getEditor().setEnabled(true);
        g_dev.getEditor().setVisible(true);
        g_dev.getEditor().setCaretPosition(0);
        g_dev.enableBuildControls();

        String header = asms.get(open_asm).getAsmFilePath();

        if(open_asm == 0)
            header += " <main program>";

        g_dev.setCurFile(header);

        Msg.D("Done.", 3, this);
        return Constants.PLP_OK;
    }

    /**
     * Link all assembly objects
     *
     * @return
     */
    public int link() {

        return Constants.PLP_OK;
    }

    /**
     * Assemble the source files attached to this project, starting from
     * the main program (index 0)
     *
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int assemble() {
        hookEvent(new ProjectEvent(ProjectEvent.ASSEMBLE, -1));
        if(!arch.hasAssembler())
            return Msg.E("This ISA does not implement an assembler.",
                         Constants.PLP_ISA_NO_ASSEMBLER, this);

        Msg.I("Assembling...", null);
        Msg.errorCounter = 0;

        boolean wasAssembled = false;
        int caretPos = -1;

        if(asm != null) {
            wasAssembled = asm.isAssembled();
            asm.setAssembled(false);
        }

        if(g) {
            g_dev.disableSimControls();
            asms.get(open_asm).setAsmString(g_dev.getEditor().getText());
            caretPos = g_dev.getEditor().getCaretPosition();
        }

        if(asms == null || asms.isEmpty())
            return Msg.E("assemble(): No source files are open.",
                            Constants.PLP_BACKEND_EMPTY_ASM_LIST, this);

        // ...assemble asm objects... //
        asm = arch.createAssembler();

        int ret = 0;

        if(asm.preprocess(0) == Constants.PLP_OK)
            ret = asm.assemble();

        if(asm != null && asm.isAssembled() && ret == 0) {
            if(!wasAssembled)
                modified = true;
            Msg.I("Done.", null);
            if(g) g_dev.enableSimControls();
            asm_req = false;
        }
        else
            asm = null;

        if(g) { 
            refreshProjectView(false);
            g_dev.getEditor().setCaretPosition(caretPos);
        }

        hookEvent(new ProjectEvent(ProjectEvent.POST_ASSEMBLE, -1));
        return Constants.PLP_OK;
    }

    /**
     * Notify the project driver that the project will need to be assembled
     * prior to any simulation or programming operations
     */
    public void requireAssemble() {
       asm_req = true;
    }

    /**
     * Drive a simulation of assembly objects attached to this project.
     *
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int simulate() {
        hookEvent(new ProjectEvent(ProjectEvent.SIMULATE, -1));
        if(!arch.hasSimCore())
            return Msg.E("simulate: This ISA does not implement a simulation" +
                         " core.", Constants.PLP_ISA_NO_SIMCORE, this);

        if(asm_req && !isReplaying()) {
            int ret = assemble();
            if(ret != Constants.PLP_OK) return Constants.PLP_ASM_ASSEMBLE_FAILED;
        }

        Msg.I("Starting simulation...", null);

        if(g) this.updateAsm(open_asm, g_dev.getEditor().getText());

        if(asm == null || !asm.isAssembled())
            return Msg.E("simulate: The project is not assembled.",
                            Constants.PLP_BACKEND_NO_ASSEMBLED_OBJECT, this);

        int checkRet = asm.preSimulationCheck();
        if(checkRet != Constants.PLP_OK)
            return checkRet;

        if(g && sim_mode)
            desimulate();

        sim = arch.createSimCore();
        ioreg = new IORegistry(this);
        arch.simulatorInitialization();

        Msg.D("I/O Modules: smods is " + (smods == null ? "null" : "not null")
	      + " and g is " + g, 3, this);

        // The ISA is responsible for providing a default set of modules.
        // This can be achieved by overriding the simulatorInitialization()
        // method in the ISA architecture metaclass.
        // The I/O registry starts empty otherwise. 
        if(smods != null && !Config.simIgnoreSavedSimState)
            ioreg.loadPreset(smods);

        sim.setStartAddr(asm.getEntryPoint());
        sim.loadProgram(asm);

        sim.reset();
        sim_mode = true;

        if(g) {
            g_ioreg = new IORegistryFrame(this);
            g_sim = arch.createSimCoreGUI();
            g_ioreg.refreshModulesTable();
            g_dev.attachModuleFrameListeners(g_ioreg, Constants.PLP_TOOLFRAME_IOREGISTRY);
            if(g_sim != null) {
                g_sim.updateComponents();
                g_sim.updateBusTable();
                g_dev.attachModuleFrameListeners(g_sim, Constants.PLP_TOOLFRAME_SIMCPU);
            }
            g_watcher = new Watcher(this);
            Boolean watcher_visibility = (Boolean) getProjectAttribute("watcher_visibility");
            g_watcher.setVisible(watcher_visibility != null ? watcher_visibility : false);
            g_dev.attachModuleFrameListeners(g_watcher, Constants.PLP_TOOLFRAME_WATCHER);
            g_simctrl = new SimControl(this);
            g_simctrl.setVisible(false);
            g_dev.attachModuleFrameListeners(g_simctrl, Constants.PLP_TOOLFRAME_SIMCTRL);
            if(watcher != null) {
                g_watcher.setEntries(watcher);
                g_watcher.updateWatcher();
            }
            if(Constants.debugLevel >= 1)
                g_err.setVisible(true);

            /** 2.2 Release- disable unimplemented features **/
            if(arch.equals("plpmips"))
                ((plptool.mips.SimCoreGUI)g_sim).disableFeatures();
            /*************************************************/

            updateWindowTitle();
        }
        else if (applet) {
            // do nothing
        }

        arch.simulatorInitializationFinal();
        hookEvent(new ProjectEvent(ProjectEvent.SIM_POST_INIT, -1));
        return Constants.PLP_OK;
    }

    /**
     * Destroy current simulation.
     *
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int desimulate() {
        hookEvent(new ProjectEvent(ProjectEvent.DESIMULATE, -1));
        if(!sim_mode)
            return Constants.PLP_OK;

        arch.simulatorStop();

        if(ioreg != null && ioreg.getNumOfModsAttached() > 0) {
            smods = ioreg.createPreset();
            ioreg.removeAllModules();
            ioreg = null;
        }

        if(sim != null) {
            if(g_sim != null)
                g_sim.dispose();
            g_err.dispose();
        }

        if(g_simrun != null) {
            g_simrun.stepCount = 0;
        }

        if(g_ioreg != null)
            g_ioreg.dispose();

        if(g_watcher != null) {
            watcher = g_watcher.getEntries();
            addProjectAttribute("watcher_visibility", g_watcher.isVisible());
            g_watcher.dispose();
        }

        if(g_asmview != null)
            g_asmview.dispose();

        if(g_simctrl != null)
            g_simctrl.dispose();

        g_ioreg = null;
        g_watcher = null;
        g_asmview = null;
        g_simctrl = null;

        sim_mode = false;
        updateWindowTitle();

        hookEvent(new ProjectEvent(ProjectEvent.SIM_POST_UNINIT, -1));
        return Constants.PLP_OK;
    }

    /**
     * Run the simulation driver (SimRunner) thread
     *
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int runSimulation() {
        hookEvent(new ProjectEvent(ProjectEvent.RUN_START, -1));
        g_simrun = new plptool.gui.SimRunner(this);
        g_simrun.start();

        if(g) {
            g_dev.simRun();
            g_simctrl.runSimState();
        }

        return Constants.PLP_OK;
    }

    /**
     * Stop the simulation driver (SimRunner) thread
     *
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int stopSimulation() {
        hookEvent(new ProjectEvent(ProjectEvent.RUN_END, -1));
        if(g_simrun != null) {
            try {
                g_simrun.stepCount = 0;
            } catch(Exception e) {}
        }

        if(g && g_simctrl != null) {
            g_dev.stopSimState();
            g_simctrl.stopSimState();
        }

        return Constants.PLP_OK;
    }

    /**
     * Return whether the project is in simulation mode
     *
     * @return boolean
     */
    public boolean isSimulating() {
        return sim_mode;
    }

    /**
     * Program the board with the object code produced by the assembler to
     * the specified port
     *
     * @param port Programming port to use
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int program(String port) {
        if(!arch.hasProgrammer())
                return Msg.E("This ISA does not implement a board programmer.",
                             Constants.PLP_ISA_NO_PROGRAMMER, this);

        if(!serial_support)
            return Msg.E("No native serial libraries available.",
                         Constants.PLP_BACKEND_NO_NATIVE_SERIAL_LIBS, this);

        if(asm_req) {
            int ret = assemble();
            if(ret != Constants.PLP_OK) return Constants.PLP_ASM_ASSEMBLE_FAILED;
        }
     
        Msg.I("Programming to " + port, this);

        try {

        if(asm != null && asm.isAssembled()) {

            if(asm.getObjectCode().length < 1)
                return Msg.E("Empty program.",
                             Constants.PLP_PRG_EMPTY_PROGRAM, this);
            
            prg = arch.createProgrammer();

            if(prg == null)
                return Msg.E("The specified ISA does not have the serial " +
                             "programmer implemented.",
                             Constants.PLP_ISA_NO_PROGRAMMER, this);

            int ret = prg.connect(port, Constants.PLP_BAUDRATE);
            if(ret != Constants.PLP_OK)
                return ret;
            p_progress = 0;

            /*** RXTX Linux hack for the Nexys3 board ***/
            if(Config.prgNexys3ProgramWorkaround && PLPToolbox.isHostLinux()) {
                Msg.D("program: Nexys 3 Linux RXTX workaround engaging...", 2, this);
                prg.close();
                prg = arch.createProgrammer();
                prg.connect(port, Constants.PLP_BAUDRATE);
            }
            
            if(g) {
                g_prg.disableControls();
                g_prg.getProgressBar().setMinimum(0);
                g_prg.getProgressBar().setMaximum(asm.getObjectCode().length - 1);
            }

            Msg.D("Starting worker threads", 2, this);
            prg.start();
            return Constants.PLP_OK;

        } else
            return Msg.E("No assembled sources.",
                            Constants.PLP_PRG_SOURCES_NOT_ASSEMBLED, this);

        } catch(Exception e) {
            e.printStackTrace();
            return Msg.E("Programming failed.\n" + e,
                            Constants.PLP_GENERIC_ERROR, this);
        }
    }

    /**
     * Alert the project driver that the project state has been changed and
     * may need saving.
     */
    public void setModified() {
        Msg.D("Project has been modified.", 5, this);
        modified = true;
        if(g)
            this.updateWindowTitle();
    }

    public void setUnModified() {
        Msg.D("Project has been unmodified.", 5, this);
        modified = false;
        if(g)
            this.updateWindowTitle();
    }

    public boolean isSerialSupported() {
        return serial_support;
    }

    /**
     *
     * @return boolean whether the project has been modified since opening
     */
    public boolean isModified() {
        return modified;
    }

    /**
     *
     * @return boolean whether the project is assembled
     */
    public boolean isAssembled() {
        if(asm != null)
            return asm.isAssembled();

        return false;
    }

    /**
     * Set current open source index
     *
     * @param index Index of the source file to open and display in editor
     */
    public void setOpenAsm(int index) {
        if(index >= 0 && index < asms.size()) {
            open_asm = index;
            hookEvent(new ProjectEvent(ProjectEvent.OPENASM_CHANGE, -1, open_asm));
        }
    }

    /**
     * Get the index of the currently open source file
     *
     * @return Index of the currently open source file
     */
    public int getOpenAsm() {
        return open_asm;
    }

    /**
     * Getter function for the source files array
     * 
     * @return ArrayList of the source files
     */
    public ArrayList<PLPAsmSource> getAsms() {
        return asms;
    }

    /**
     * Set new list of source files
     *
     * @param asms ArrayList of references to source files
     */
    public void setAsms(ArrayList<PLPAsmSource> asms) {
        this.asms = asms;
    }

    /**
     * Getter function for the asm source file with the specified index
     *
     * @param index Index of source file to get
     * @return PLPAsmSource object that corresponds to index
     */
    public PLPAsmSource getAsm(int index) {
        if(asms == null || index < 0 || index >= asms.size()) {
            Msg.E("getAsm: Invalid index: " + index,
                     Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);
            return null;
        }

        return asms.get(index);
    }

    /**
     * Getter function for the asm source file with the specified filename
     *
     * @param asmFileName identifier for the source asm
     * @return PLPAsmSource object that corresponds to index
     */
    public PLPAsmSource getAsm(String asmFileName) {
        for(int i = 0; i < asms.size(); i++) {
            if(asms.get(i).getAsmFilePath().equals(asmFileName))
                return asms.get(i);
        }

        return null;
    }

    /**
     * Getter function for the asm source file index with the specified filename
     *
     * @param asmFileName identifier for the source asm
     * @return int Index of the source file
     */
    public int getAsmIndex(String asmFileName) {
        for(int i = 0; i < asms.size(); i++) {
            if(asms.get(i).getAsmFilePath().equals(asmFileName))
                return i;
        }

        return -1;
    }

    /**
     * Update PLPAsmSource specified by index with new string.
     *
     * @param index Index of the source file to update
     * @param newStr New string to store
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int updateAsm(int index, String newStr) {
        if(asms == null || index < 0 || index >= asms.size())
            return Msg.E("updateAsm: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        //if(!asms.get(index).getAsmString().equals(newStr))
        //    setModified();

        asms.get(index).setAsmString(newStr);

        return Constants.PLP_OK;
    }

    /**
     * Create a new source file for the project
     *
     * @param name Identifier for the source file
     * @return PLP_OK
     */
    public int newAsm(String name) {
        for(int i = 0; i < asms.size(); i++) {
            if(asms.get(i).getAsmFilePath().equals(name))
                return Msg.E("The file '" + name + "' already exists in " +
                             "the project.", Constants.PLP_BACKEND_IMPORT_CONFLICT, this);
        }

        asms.add(new PLPAsmSource("# New ASM File", name, asms.size()));
        setModified();

        if(g) refreshProjectView(true);

        return Constants.PLP_OK;
    }

    /**
     * Import specified source file into the project
     *
     * @param path The path to the source file to import
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int importAsm(String path) {
        File asmFile = new File(path);

        Msg.I("Importing " + path, null);

        if(!asmFile.exists())
            return Msg.E("importAsm(" + path + "): file not found.",
                            Constants.PLP_BACKEND_ASM_IMPORT_ERROR, this);

        if(!asmFile.getName().endsWith(".asm")) {
            return Msg.E("importAsm(" + path + "): imported source files " +
                         "must have a .asm extension.",
                         Constants.PLP_BACKEND_ASM_IMPORT_ERROR, this);
        }

        String existingPath;
        for(int i = 0; i < asms.size(); i++) {
            existingPath = asms.get(i).getAsmFilePath();

            if(existingPath.equals(path) ||
               existingPath.equals(asmFile.getName())) {
                return Msg.E("importAsm(" + path + "): File with the same name already exists.",
                                Constants.PLP_BACKEND_IMPORT_CONFLICT, this);
            }
        }

        asms.add(new PLPAsmSource(null, path, asms.size()));
        asms.get(asms.size() - 1).setAsmFilePath(asmFile.getName());

        setModified();

        if(g) refreshProjectView(true);

        return Constants.PLP_OK;
    }

    /**
     * Export the source file specified by index and save it to the specified
     * output path.
     *
     * @param index Index of the source file to export
     * @param path The path of export destination
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int exportAsm(int index, String path) {
        File asmFile = new File(path);

        Msg.I("Exporting " + asms.get(index).getAsmFilePath() +
                 " to " + path, null);

        if(asms == null || index < 0 || index >= asms.size())
            return Msg.E("exportAsm: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        if(asmFile.exists()) {
            return Msg.E("exportAsm: " + path + " exists.",
                            Constants.PLP_FILE_SAVE_ERROR, this);
        }

        if(g && getOpenAsm() == index)
            updateAsm(index, g_dev.getEditorText());

        try {

        FileWriter asmWriter = new FileWriter(asmFile);
        asmWriter.write(asms.get(index).getAsmString());
        asmWriter.close();

        } catch(Exception e) {
            if(Constants.debugLevel >= 10) e.printStackTrace();
            return Msg.E("exportAsm(" + asms.get(index).getAsmFilePath() +
                            "): Unable to write to " + path + "\n",
                            Constants.PLP_FILE_SAVE_ERROR, this);
        }

        return Constants.PLP_OK;
    }

    /**
     * Remove the source file specified by index from the project.
     *
     * @param index Index of the source file to remove
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int removeAsm(int index) {
        if(asms.size() <= 1) {
            return  Msg.E("removeAsm: Can not delete last source file.",
                            Constants.PLP_BACKEND_DELETING_LAST_ASM_ERROR, this);
        }

        if(asms == null || index < 0 || index >= asms.size())
            return  Msg.E("removeAsm: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        setModified();

        if(index < open_asm) {
            Msg.D("removeAsm: index < open_asm: " + index + "<" + open_asm, 4, this);
            if(g) updateAsm(open_asm, g_dev.getEditorText());
            open_asm--;
        } else if(index == open_asm && open_asm != 0) {
            Msg.D("removeAsm: index == open_asm: " + index + "==" + open_asm, 4, this);
            open_asm--;
        } else {
            Msg.D("removeAsm: index > open_asm: " + index + ">" + open_asm, 4, this);
            if(g) updateAsm(open_asm, g_dev.getEditorText());
        }
            
        Msg.I("Removing " + asms.get(index).getAsmFilePath(), null);
        asms.remove(index);
        if(g) refreshProjectView(false);

        return Constants.PLP_OK;
    }
    /**
     * Remove the source file with the specified name
     *
     * @param asmFileName Name of the source file to remove
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int removeAsm(String asmFileName) {
        for(int i = 0; i < asms.size(); i++) {
            if (asms.get(i).getAsmFilePath().equals(asmFileName))
                return removeAsm(i);
        }

        return Msg.E("removeAsm: Can not find source file with the name \"" +
                     asmFileName + "\" within the project.",
                     Constants.PLP_BACKEND_ASM_DOES_NOT_EXIST, this);
    }

    /**
     * Move the specified source file to index 0, making it the top level
     * source file.
     *
     * @param index Index of the source file to be set as main program
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int setMainAsm(int index) {
        if(asms == null || index <= 0 || index >= asms.size())
            return Msg.E("setMainAsm: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        asms.add(0, asms.get(index));
        asms.remove(index + 1);
        
        setModified();
        
        if(open_asm == index)
            open_asm = 0;

        if(g) refreshProjectView(true);

        return Constants.PLP_OK;
    }

    /**
     * Set the position of the specified source file to a new one.
     *
     * @param index Index of the source file to be modified
     * @param newIndex The new position of the source file
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int setAsmPosition(int index, int newIndex) {
        if(asms == null || index < 0 || index >= asms.size())
            return Msg.E("setAsmPosition: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        if(asms == null || newIndex < 0 || newIndex >= asms.size())
            return Msg.E("setAsmPosition: Invalid new index: " + newIndex,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        if(newIndex <= index) {
            asms.add(newIndex, asms.get(index));
            asms.remove(index + 1);

        } else {
            asms.add(newIndex+1, asms.get(index));
            asms.remove(index);
        }

        return Constants.PLP_OK;
    }

    /**
     * Display the Quick Reference window
     */
    public void showQuickRef() {
        if(g_qref != null)
            g_qref.dispose();

        g_qref = new QuickRef(this);
        g_qref.setVisible(true);
    }

    /**
     * Is the binary files up to date.
     *
     * @return boolean that denotes whether the binary files are up to date
     */
    public boolean isDirty() {
        return dirty;
    }

    /**
     * Update GUI components
     */
    public void updateComponents(boolean updateDevelop) {
        try {
        if(g_sim != null)
            g_sim.updateComponents();
        
        if(updateDevelop)
            g_dev.updateComponents();

        if(sim_mode) {
            g_dev.updateStatText();
        }

        if(ioreg != null)
            ioreg.gui_eval();

        if(g_watcher != null)
            g_watcher.updateWatcher();

        if(g_asmview != null)
            g_asmview.updatePC();

        if(g_simctrl != null)
            g_simctrl.update();

        } catch(Exception e) {
            // GUI update error has occured
            System.out.println("GUI error has occured. Switch to debug level 2 or above to print stack trace.");
            if(Constants.debugLevel >= 2) {
                e.printStackTrace();
            }
        }
    }

    /**
     * Pass a project event to the hook function of dynamic modules
     *
     * @param e ProjectEvent to be passed on
     */
    public void hookEvent(ProjectEvent e) {
        if(!replay) DynamicModuleFramework.hook(e);
    }

    /**
     * Replay action as specified by the ProjectEvent instance passed to
     * this function
     *
     * @param e ProjectEvent to be replayed
     */
    public void replay(ProjectEvent e) {
        replay = true;
        switch(e.getIdentifier()) {
            
            case ProjectEvent.ASSEMBLE:
                g_dev.assemble();
                break;
                
            case ProjectEvent.SIMULATE:
                g_dev.simBegin();
                break;

            case ProjectEvent.DESIMULATE:
                g_dev.simEnd();
                break;

            case ProjectEvent.SINGLE_STEP:
                g_dev.simStep();
                break;

            case ProjectEvent.EDITOR_INSERT:
                try {
                    g_dev.getEditor().getDocument().insertString(
                            (Integer)((Object[])e.getParameters())[0],  //off
                            (String)((Object[])e.getParameters())[1],   //str
                            null);
                } catch(javax.swing.text.BadLocationException ble) {
                    ble.printStackTrace();
                }
                break;

            case ProjectEvent.EDITOR_REMOVE:
                try {
                    g_dev.getEditor().getDocument().remove(
                            (Integer)((Object[])e.getParameters())[0],  //off
                            (Integer)((Object[])e.getParameters())[1]); //len
                } catch(javax.swing.text.BadLocationException ble) {
                    ble.printStackTrace();
                }
                break;

            case ProjectEvent.RUN_START:
                runSimulation();
                break;

            case ProjectEvent.RUN_END:
                stopSimulation();
                break;

            case ProjectEvent.SIM_SPEED_CHANGED:
                int newDelay = (Integer) e.getParameters();
                Config.simRunnerDelay = newDelay;
                break;

            case ProjectEvent.SIM_STEPSIZE_CHANGED:
                int newStepsize = (Integer) e.getParameters();
                Config.simCyclesPerStep = newStepsize;
                break;

            case ProjectEvent.EDITOR_CHANGE:

                break;

            case ProjectEvent.OPENASM_CHANGE:
                updateAsm(open_asm, g_dev.getEditorText());
                open_asm = (Integer) e.getParameters();
                refreshProjectView(false);
                break;

            case ProjectEvent.SIM_WINDOW_VISIBILITY_TRUE:
                g_dev.setSimWindowVisibility((Integer) e.getParameters(), true);
                break;

            case ProjectEvent.SIM_WINDOW_VISIBILITY_FALSE:
                g_dev.setSimWindowVisibility((Integer) e.getParameters(), false);
                break;

            case ProjectEvent.GENERIC:
                break;

            default:
                Msg.D("Unknown event ID: " + e.getIdentifier(), 3, this);
        }
        replay = false;
    }

    /**
     * Return whether the project is replaying project events
     *
     * @return True if project is being replayed, false otherwise
     */
    public boolean isReplaying() {
        return replay;
    }

    /**
     * Use this method for some unforeseen bug!
     */
    public void triggerCriticalError() {
        halt = true;
        System.err.println("[CRITICAL ERROR] " +
                    "This really, really, really, should not have happened.");
        System.err.println("[CRITICAL ERROR] " +
                    "PLP Tool is now exiting. Please report this issue. Thanks!");
        if(g) {
            javax.swing.JOptionPane.showMessageDialog(g_dev,
                    "This really, really, really, should not have happened. " +
                    "PLP Tool is now exiting. Please report this issue. Thanks!",
                    "CRITICAL ERROR", javax.swing.JOptionPane.ERROR_MESSAGE);
        }
        
        if(plpfile != null) {
            System.err.println("Saving current open project as ./dump.plp...");
            plpfile = new File("dump.plp");
            this.save();
        }

        System.exit(-1);
    }

    /**
     * Denotes whether the project driver is driving GUI or not
     *
     * @return true if a GUI is active, false otherwise
     */
    public boolean g() {
        return g;
    }

    @Override
    public String toString() {
        return "PLP(" + ((plpfile != null) ? plpfile.getName() : "") + ")";
    }
}
