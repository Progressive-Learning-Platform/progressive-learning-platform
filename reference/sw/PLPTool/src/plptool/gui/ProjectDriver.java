/*
    Copyright 2010-2014 David Fritz, Brian Gordon, Wira Mulia

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

import static plptool.Config.getWindowParameters;
import static plptool.Constants.minimumJREMajorVersion;
import static plptool.Constants.minimumJREMinorVersion;

import java.awt.Rectangle;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

import javax.swing.table.DefaultTableModel;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;

import plptool.ArchRegistry;
import plptool.Config;
import plptool.Constants;
import plptool.Msg;
import plptool.PLPArchitecture;
import plptool.PLPAsm;
import plptool.PLPAsmSource;
import plptool.PLPBuildError;
import plptool.PLPLinker;
import plptool.PLPSerialProgrammer;
import plptool.PLPSimCore;
import plptool.PLPSimCoreGUI;
import plptool.PLPToolbox;
import plptool.Text;
import plptool.dmf.CallbackRegistry;
import plptool.gui.frames.ASMSimView;
import plptool.gui.frames.AboutBoxDialog;
import plptool.gui.frames.AsmNameDialog;
import plptool.gui.frames.Develop;
import plptool.gui.frames.FindAndReplace;
import plptool.gui.frames.IDE;
import plptool.gui.frames.IORegistryFrame;
import plptool.gui.frames.ISASelector;
import plptool.gui.frames.OptionsFrame;
import plptool.gui.frames.ProgrammerDialog;
import plptool.gui.frames.QuickRef;
import plptool.gui.frames.SimControl;
import plptool.gui.frames.SimErrorFrame;
import plptool.gui.frames.Watcher;
import plptool.mods.IORegistry;
import plptool.mods.Preset;

/**
 * This is the PLPTool application project backend. This class handles
 * manipulation of the PLP project file and drives the workflow of a
 * PLP tool user. This includes creating, opening, and saving project files.
 * Driving workflow functions such as assembling, simulating, and
 * programming the board.
 *
 * @author wira
 */
public final class ProjectDriver {
    private boolean                g;          // are we driving a GUI?
    private boolean                applet;     // are we driving an applet?
    
    private List<TarEntryNode> tarEntryStash;
    
    // These variables hold project status and data for this driver
    /** denotes whether the plpfile has been modified since opening */
    private boolean                modified;

    private boolean                dirty;

    /** current open ASM file in the gui */
    private int                    open_asm;

    /** active ISA for this project */
    private PLPArchitecture        arch;

    /** denotes whether the project is in simulation mode */
    private boolean                sim_mode;

    /** denotes whether the project needs to be assembled for other functions such as simulation or programming */
    private boolean                asm_req;

    /** list of source assembly files */
    private ArrayList<PLPAsmSource> asms;

    /**
     * Current PLP file that the project driver is working on
     */
    public File                    plpfile;

    /**
     * Current working directory for the project
     */
    public String                  curdir;

    /**
     * A table model handler for the watcher window entries
     */
    private DefaultTableModel       watcher;

    /**
     * Build error list from the previous failed build process
     */
    private ArrayList<PLPBuildError> buildErrorList;

    /**
     * The assembler attached to the project. This assembler will most likely
     * be null if the project is not assembled. It is advised to wrap code
     * segments that access this handle with an if block checking for
     * isAssembled() method.
     */
    public PLPAsm                  asm;

    /**
     * A handle to the linker. This is currently unused as of PLPTool 5.
     */
    public PLPLinker               lnkr;

    /**
     * Reference to the board programmer.
     */
    public PLPSerialProgrammer     prg;

    /**
     * Reference to the simulator (generic PLPSimCore type)
     */
    public PLPSimCore              sim;

    public IORegistry              ioreg;

    public PLPSimCoreGUI           g_sim;

    /**
     * Meta information string of the project
     */
    public String                  meta;

    /**
     * Saved module set for the project
     */
    public Preset                  smods;

    /*
     * PLP GUI Windows
     */ // --
    public IORegistryFrame         g_ioreg;    
    public Develop                 g_dev;      
    public SimErrorFrame           g_err;      
    public AboutBoxDialog          g_about;    
    public OptionsFrame            g_opts;     
    public ProgrammerDialog        g_prg;      
    public AsmNameDialog           g_fname;    
    public SimRunner               g_simrun;   
    public Watcher                 g_watcher;  
    public SimControl              g_simctrl;  
    public ASMSimView              g_asmview;  
    public QuickRef                g_qref;     
    public ISASelector             g_isaselect;
    public FindAndReplace          g_find;

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
    	validateJavaVersion();
    	
        g = (modes & Constants.PLP_GUI_START_IDE) == Constants.PLP_GUI_START_IDE;
        applet = (modes & Constants.PLP_GUI_APPLET) == Constants.PLP_GUI_APPLET;
        tarEntryStash = new LinkedList<>();
        
        arch = null;
        modified = false;
        plpfile = null;
        sim_mode = false;
        asm_req = false;
        pAttrSet = new HashMap<>();
        ioreg = new IORegistry(this);
        asms = new ArrayList<>();
        
        if (!applet) {
        	serial_support = isRXTXAvailable();
        	curdir = (new File(".")).getAbsolutePath();
        	if(g) 
        		initializeGraphicalComponents();
        }

        if(!CallbackRegistry.INITIALIZED)
            CallbackRegistry.setup(null);
    }

    private void validateJavaVersion()
	{
    	String tokens[] = System.getProperty("java.version").split("\\.");
        int major = Integer.parseInt(tokens[0]);
        int minor = Integer.parseInt(tokens[1]);

        if((major == minimumJREMajorVersion && minor < minimumJREMinorVersion) ||
            major  < minimumJREMajorVersion) {
        	String requiredVersion = Constants.minimumJREMajorVersion + "." + minimumJREMinorVersion;
            Msg.warning("You are running an older Java Runtime Environment version." +
                  " Some functionalities may not work as intended. " +
                  "Please upgrade to at least JRE version " + requiredVersion, null);
        }
	}

	private void initializeGraphicalComponents()
	{
    	g_err = new SimErrorFrame();
        g_dev = new Develop(this);
        g_ioreg = new IORegistryFrame(this);
        g_about = new AboutBoxDialog(g_dev);
        g_opts = new OptionsFrame(this);
        g_opts.setBuiltInISAOptions(false);
        g_prg = new ProgrammerDialog(this, g_dev, true);
        g_fname = new AsmNameDialog(this, g_dev, true);
        g_find = new FindAndReplace(this);
        g_isaselect = new ISASelector(g_dev, this);
        
        Rectangle windowParameters = getWindowParameters();
        g_dev.setSize(windowParameters.getSize());
        g_dev.setLocation(windowParameters.getLocation());
        g_dev.setLocationRelativeTo(null);

        g_find.setLocationRelativeTo(null);

        g_dev.setTitle("PLP Software Tool " + Text.versionString);
        if(PLPToolApp.getAttributes().containsKey("new_ide")) {
            IDE ide = new IDE(this);
            ide.setVisible(true);
        } else {
            g_dev.setVisible(true);
        }
	}

	private boolean isRXTXAvailable()
	{
        // check for rxtx native libaries
    	try {
            gnu.io.RXTXVersion.getVersion();
            return true;
        } catch(UnsatisfiedLinkError e) {
        	// TODO: extract warning messages
        	// TODO: replace with Java Logging API
            Msg.warning("Failed to detect native RXTX library. " +
                  "Functionality requiring serial communication will fail.", null);
            Msg.warning(" - If you are running Linux, make sure that RXTX library is installed.", null);
            Msg.warning(" - If you are running Windows, make sure that the .dll files are in the " +
                  "same directory and you run the batch file associated with " +
                  "your version of Windows (32- or 64-bit)", null);
        } catch(NoClassDefFoundError e) {
            Msg.warning("Unsatisfied RXTX link.", null);
        }
    	
    	return false;
	}

	/**
     * Set a new architecture for the project
     *
     * @param arch
     * @return
     */
    public int setArch(int archID) {
        if(arch != null)
            arch.cleanup();
        arch = ArchRegistry.getArchitecture(this, archID);
        if(arch == null) {
            arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
            arch.init();
            return Msg.error("Invalid ISA ID: " + archID + ". Defaulting to " +
                         "plpmips (id 0).",
                         Constants.PLP_ISA_INVALID_ARCHITECTURE_ID, this);
        } else {
            arch.init();    
            return Constants.PLP_OK;
        }
    }

    /**
     *
     * @return the current active ISA for the project
     */
    public PLPArchitecture getArch() {
        return arch;
    }    

    /**
     * Initialize plp project data structures. This function provides a default
     * empty source file and calls the architecture newProject() method.
     *
     * @param archID The ID of the ISA to use
     * @return PLP_OK
     */
    public int create(int archID) {
        return create(null, archID);
    }

    /**
     * Initialize project data structures and attempt to import the specified
     * assembly source file into the project.
     *
     * @param asmPath Path to ASM file to import
     * @param archID The ID of the ISA to use
     * @return PLP_OK
     */
    public int create(String asmPath, int archID) {
        modified = true;
        asm_req = true;
        plpfile = new File("Unsaved Project");
        if(arch != null)
            arch.cleanup();

        try {
            this.arch = ArchRegistry.getArchitecture(this, archID);
            if(arch == null) {
                Msg.warning("Invalid architecture ID is specified, reverting to " +
                      "default (plpmips).", this);
                this.arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
            }
            arch.init();
        } catch(Exception e) {
            Msg.error("FATAL ERROR: invalid arch ID during ProjectDriver" +
                  "create routine (archID: " + archID + ")",
                  Constants.PLP_FATAL_ERROR, null);
            System.exit(-1);
        }

        asm = null;
        asms = new ArrayList<PLPAsmSource>();
        if(importAsm(asmPath) != Constants.PLP_OK) {
            asms.add(new PLPAsmSource("", "main.asm", 0));
        }

        smods = null;
        watcher = null;
        pAttrSet = new HashMap<String, Object>();
        dirty = true;

        open_asm = 0;
        arch.newProject(this);
        Msg.info("New project initialized.", null);

        if(g) {
            refreshProjectView(false);
            desimulate();
            g_dev.disableSimControls();
            g_dev.enableBuildControls();

            if(g_asmview != null)
                g_asmview.dispose();
        }

        CallbackRegistry.callback(CallbackRegistry.PROJECT_NEW, null);
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
        if(g)
        	updateAsm(open_asm, g_dev.getEditorText());

        if(plpfile == null || plpfile.getName().equals("Unsaved Project"))
            return Msg.error("No PLP project file is open. Use Save As.",
                            Constants.PLP_FILE_USE_SAVE_AS, null);

        try {
        	String verilogHex = "";
	        File outFile = plpfile;
	        long[] objCode = null;
	        TarArchiveOutputStream tOut = 
	        		new TarArchiveOutputStream(new FileOutputStream(outFile));
	        
	        if(asm != null && asm.isAssembled()) {
	            objCode = asm.getObjectCode();
	            if(arch.getID() == ArchRegistry.ISA_PLPMIPS) {
	                Msg.debug("Creating verilog hex code...", 2, this);
	                verilogHex = plptool.mips.Formatter.writeVerilogHex(objCode);
	            }
	        }

	        meta = createMetaString(objCode);
	        writeMetaFile(tOut, meta);
	        writePLPSourceFiles(tOut);
	        writeSimulationConfigurationData(tOut);
	
	        if(asm != null && asm.isAssembled() && objCode != null) {
	            // Write hex image
	            Msg.debug("Writing out verilog hex code...", 2, this);
	            TarArchiveEntry entry = new TarArchiveEntry("plp.hex");
	            entry.setSize(verilogHex.length());
	            tOut.putArchiveEntry(entry);
	            byte[] data = new byte[verilogHex.length()];
	            for(int i = 0; i < verilogHex.length(); i++) {
	                data[i] = (byte) verilogHex.charAt(i);
	            }
	            tOut.write(data);
	            tOut.flush();
	            tOut.closeArchiveEntry();
	
	            // Write binary image, 4-byte big-endian packs
	            Msg.debug("Writing out binary image...", 2, this);
	            entry = new TarArchiveEntry("plp.image");
	            entry.setSize(objCode.length * 4);
	            tOut.putArchiveEntry(entry);
	            data = new byte[objCode.length * 4];
	            for(int i = 0; i < objCode.length; i++) {
	                data[4*i] = (byte) (objCode[i] >> 24);
	                data[4*i+1] = (byte) (objCode[i] >> 16);
	                data[4*i+2] = (byte) (objCode[i] >> 8);
	                data[4*i+3] = (byte) (objCode[i]);
	            }
	            tOut.write(data);
	            tOut.flush();
	            tOut.closeArchiveEntry();
	        } 
	        
	        // write entries that appear in the save but are not used by PLPTool
	        writeStashedEntries(tOut);
	
	        // Hook for project save
	        CallbackRegistry.callback(CallbackRegistry.PROJECT_SAVE, tOut);
	
	        Msg.debug("Closing tar archive...", 2, this);
	        tOut.close();
	        Msg.debug("Project save completed", 2, this);
	
	        modified = false;
	        if(g) refreshProjectView(false);
	        Msg.info(plpfile.getAbsolutePath() + " written", null);
        } catch(Exception e) {
            Msg.trace(e);
            Msg.error("save: Unable to write to " +
                    plpfile.getAbsolutePath() + ". " +
                    "Do you have access to the specified location?",
                    Constants.PLP_FILE_SAVE_ERROR, this);
            return Constants.PLP_FILE_SAVE_ERROR;
        }

        return Constants.PLP_OK;
    }

    private void writePLPSourceFiles(TarArchiveOutputStream tOut) throws IOException
	{
    	for(PLPAsmSource asmFile : asms) {
            Msg.debug("Writing " + asmFile.getAsmFilePath() + "...", 2, this);
            TarArchiveEntry entry = new TarArchiveEntry(asmFile.getAsmFilePath());
            
            // XXX: add to external documentation
            // We are not expecting an .asm file with size greater than 4GB
            byte[] fileStr = asmFile.getAsmString().getBytes();
            entry.setSize(fileStr.length);
            tOut.putArchiveEntry(entry);
            tOut.write(fileStr);
            tOut.flush();
            tOut.closeArchiveEntry();
        }
	}

	private String createMetaString(long[] objCode)
	{
    	// TODO: reduce scope of meta from global to local
		// TODO: remove dependencies on asm and dirty
    	// TODO: write proper PLP Version
    	String meta = "PLP-5.0\n";
    	
        if(asm != null && asm.isAssembled()) {
            if(objCode != null && objCode.length > 0)
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

        for(PLPAsmSource asmFile : asms)
            meta += asmFile.getAsmFilePath() + "\n";
        
        return meta;
	}

	private void writeMetaFile(TarArchiveOutputStream tOut, String meta) throws IOException
	{
    	Msg.debug("Writing plp.metafile...", 2, this);
        TarArchiveEntry entry = new TarArchiveEntry("plp.metafile");
        entry.setSize(meta.length());
        tOut.putArchiveEntry(entry);
        byte[] data = meta.getBytes();
        tOut.write(data);
        tOut.flush();
        tOut.closeArchiveEntry();
	}

	private void writeStashedEntries(TarArchiveOutputStream tOut) throws IOException
	{
    	for (TarEntryNode node : tarEntryStash) {
        	// TODO: verify this error code
            Msg.debug("Writing out old tar entry (" + node.entry.getName() + ")", 2, this);
            tOut.putArchiveEntry(node.entry);
            tOut.write(node.data);
            tOut.flush();
            tOut.closeArchiveEntry();
        }
	}

	private void writeSimulationConfigurationData(TarArchiveOutputStream tOut) throws IOException
	{
    	Msg.debug("Writing out simulation configuration...", 2, this);
    	TarArchiveEntry entry = new TarArchiveEntry("plp.simconfig");
        String str = "";
        str += "simRunnerDelay::" + Config.simRunnerDelay + "\n";
        str += "simAllowExecutionOfArbitraryMem::" + Config.simAllowExecutionOfArbitraryMem + "\n";
        str += "simBusReturnsZeroForUninitRegs::" + Config.simBusReturnsZeroForUninitRegs + "\n";
        str += "simDumpTraceOnFailedEvaluation::" + Config.simDumpTraceOnFailedEvaluation + "\n";


        if(watcher != null) {
            str += "WATCHER\n";

            for(int i = 0; i < watcher.getRowCount(); i++) {
                str += watcher.getValueAt(i, 0) + "::";
                str += watcher.getValueAt(i, 1) + "\n";
            }

            str += "END\n";
        }

        Msg.debug("-- saving mods info...", 2, this);

        if(ioreg != null && ioreg.getNumOfModsAttached() > 0)
            smods = ioreg.createPreset();

        if(smods != null && smods.size() > 0) {
            str += "MODS\n";

            for(int i = 0; i < smods.size(); i++) {
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
	}

	/**
     * Open plp file specified by path.
     *
     * @param path Path to project file to load.
     * @param assemble Attempt to assemble after opening (if dirty is not set)
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int open(String path, boolean assemble) {
        File plpFile = new File(path);
        CallbackRegistry.callback(CallbackRegistry.PROJECT_OPEN, plpFile);
        
        if(!plpFile.exists())
            return Msg.error("open(" + path + "): File not found.",
                            Constants.PLP_BACKEND_PLP_OPEN_ERROR, null);

        Msg.info("Opening " + path, null);

        if(arch != null)
            arch.cleanup();

        dirty = true;
        arch = null;
        asm = null;
        asms = new ArrayList<>();
        smods = null;
        watcher = null;
        pAttrSet = new HashMap<String, Object>();

        try {
	        byte[] image = extractMetafileImage(plpFile);
	        if(image == null)
	            return Msg.error("No PLP metadata found.", Constants.PLP_BACKEND_INVALID_PLP_FILE, this);
	
	        HashMap<String, Integer> asmFileOrder = loadMetafileEntry(image);
	        parsePLPArchive(plpFile, asmFileOrder);
	        
	        if(asmFileOrder.isEmpty())
	            return Msg.error("open(): no .asm files found.", Constants.PLP_BACKEND_INVALID_PLP_FILE, null);
        }
        catch(Exception e) {
            Msg.trace(e);
            return Msg.error("open(): Invalid PLP archive.",
                            Constants.PLP_BACKEND_INVALID_PLP_FILE, null);
        }

        if(arch == null) {
            Msg.warning("No ISA information specified in the archive, assuming plpmips", this);
            arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
            arch.init();
        }

        plpfile = plpFile;
        modified = false;
        open_asm = 0;

        for(int i = 0; i < asms.size(); i++)
            Msg.info(i + ": " + asms.get(i).getAsmFilePath(), null);
        
        if(!dirty && assemble) {
            assemble();
            asm_req = false;
        } else
            asm_req = true;

        if(g)
        	refreshGUI(plpFile);

        CallbackRegistry.callback(CallbackRegistry.PROJECT_OPEN_SUCCESSFUL, plpFile);
        return Constants.PLP_OK;
    }

    private void refreshGUI(File plpFile)
	{
    	refreshProjectView(false);
        g_opts.restoreSavedOpts();
        desimulate();

        if(asm != null && asm.isAssembled())
            g_dev.enableSimControls();
        else
            g_dev.disableSimControls();
        
        this.setUnModified();
        updateWindowTitle();
        // TODO: move this to the open method, and remove dependency on g_dev
        g_dev.updateDevelopRecentProjectList(plpFile.getAbsolutePath());
        if(g_asmview != null)
            g_asmview.dispose();
	}

	private void parsePLPArchive(File plpFile, HashMap<String, Integer> asmFileOrder) 
    		throws NumberFormatException, IOException
	{
        TarArchiveInputStream tIn = new TarArchiveInputStream(new FileInputStream(plpFile));
    	TarArchiveEntry entry;
        while((entry = tIn.getNextTarEntry()) != null) {
        	byte[] image = new byte[(int) entry.getSize()];
            tIn.read(image, 0, (int) entry.getSize());
            String metaStr = new String(image);

            // Hook for project open for each entry
            Object[] eParams = {entry.getName(), image, plpFile};
            boolean handled = CallbackRegistry.callback(CallbackRegistry.PROJECT_OPEN_ENTRY, eParams);

            if(entry.getName().endsWith("asm") && !entry.getName().startsWith("plp.")) {
                Integer order = (Integer) asmFileOrder.get(entry.getName());
                if(order == null)
                    Msg.warning("The file '" + entry.getName() + "' is not listed in " +
                          "the meta file. This file will be removed when the project " +
                          "is saved.", this);
                else {
                    asms.set(order, new PLPAsmSource(metaStr, entry.getName(), order));
                }

            } else if(entry.getName().equals("plp.metafile")) {
                // we've done reading the metafile
            // Restore bus modules states
            } else if (entry.getName().equals("plp.simconfig")) {
                Msg.debug("simconfig:\n" + metaStr + "\n", 4, this);
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
            	TarEntryNode entryNode = new TarEntryNode(entry, image);
            	tarEntryStash.add(entryNode);
                Msg.info("open(): found misc tarball entry: " + entry.getName(), this);
            }
        }
        
        tIn.close();
	}

	private byte[] extractMetafileImage(File plpFile) throws IOException
	{
    	TarArchiveInputStream tIn = new TarArchiveInputStream(new FileInputStream(plpFile));
        byte[] image = extractMetafileImage(tIn);
        tIn.close();
        
        return image;
	}

	private byte[] extractMetafileImage(TarArchiveInputStream tIn) throws IOException
	{
    	TarArchiveEntry entry;
        while((entry = tIn.getNextTarEntry()) != null) {
            if(entry.getName().equals("plp.metafile")) {
            	byte[] image = new byte[(int) entry.getSize()];
                tIn.read(image, 0, (int) entry.getSize());
                return image;
            }
        }
        
        return null;
	}

	private HashMap<String, Integer> loadMetafileEntry(byte[] image)
	{
        String metaStr = new String(image);
        
        meta = metaStr;
        Scanner metaScanner;

        String lines[] = meta.split("\\r?\\n");
        if(!lines[0].equals(Text.projectFileVersionString)) {
            Msg.warning("This is not a " + Text.projectFileVersionString +
                    " project file. Opening anyways.", this);
        }

        metaScanner = new Scanner(meta);
        metaScanner.findWithinHorizon("DIRTY=", 0);
        if(metaScanner.nextInt() == 0)
            dirty = false;
        if(metaScanner.findWithinHorizon("ARCH=", 0) != null) {
            String temp = metaScanner.nextLine();
            if(Config.cfgOverrideISA >= 0) { // ISA ID override, ignore the metafile
                arch = ArchRegistry.getArchitecture(this, Config.cfgOverrideISA);
                arch.init();
            } else if (temp.equals("plpmips")) {
                Msg.warning("This project file was created by PLPTool version 3 or earlier. " +
                      "Meta data for this project will be updated " +
                      "with the default ISA (plpmips) when the project " +
                      "file is saved.", this);
                arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
                arch.init();
            } else {
                arch = ArchRegistry.getArchitecture(this, Integer.parseInt(temp));
                if(arch == null) {
                    Msg.warning("Invalid ISA ID is specified in the project file: '" + temp +
                          "'. Assuming PLPCPU.", this);
                    arch = ArchRegistry.getArchitecture(this, ArchRegistry.ISA_PLPMIPS);
                }
                arch.init();
            }
            arch.hook(this);
        }

        // get asm files order
        int asmOrder = 0;
        HashMap<String, Integer> asmFileOrder = new HashMap<String, Integer>();
        while(metaScanner.hasNext()) {
            String asmName = metaScanner.nextLine();
            if(asmName.endsWith(".asm")) {
                asmFileOrder.put(asmName, new Integer(asmOrder));
                asmOrder++;
                asms.add(null);
            }
        }
        
        metaScanner.close();
        return asmFileOrder;
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

        Msg.debug("add attr " + key + ":" + value, 3, this);
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
            Msg.debug("set attr " + key + ":" + value, 3, this);
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
        Msg.debug("get attr " + key, 3, this);
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
        Msg.debug("Project view refresh...", 3, this);

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

        int meta_dirty =  dirty ? 1 : 0;
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

        CallbackRegistry.callback(CallbackRegistry.GUI_VIEW_REFRESH,
                commitCurrentAsm);
        Msg.debug("Done.", 3, this);
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
        CallbackRegistry.callback(CallbackRegistry.EVENT_ASSEMBLE, null);
        if(!arch.hasAssembler())
            return Msg.error("This ISA does not implement an assembler.",
                         Constants.PLP_ISA_NO_ASSEMBLER, this);

        Msg.info("Assembling...", null);
        Msg.errorCounter = 0;
        buildErrorList = null;

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
            return Msg.error("assemble(): No source files are open.",
                            Constants.PLP_BACKEND_EMPTY_ASM_LIST, this);

        // ...assemble asm objects... //
        asm = arch.createAssembler();
        CallbackRegistry.callback(CallbackRegistry.EVENT_ASSEMBLE_INIT, asm);

        int ret = 0;

        if(asm != null && asm.preprocess(0) == Constants.PLP_OK)
            ret = asm.assemble();

        if(asm != null && asm.isAssembled() && ret == 0) {
            if(!wasAssembled)
                modified = true;
            Msg.info("Done.", null);
            if(g) g_dev.enableSimControls();
            asm_req = false;
        }
        else if (asm != null) {
            buildErrorList = asm.getErrorList();
            asm = null;
        }

        if(g) { 
            refreshProjectView(false);
            g_dev.getEditor().setCaretPosition(caretPos);
        }

        if(g && asm != null && g_asmview != null)
            g_asmview.updateTable();
        CallbackRegistry.callback(CallbackRegistry.EVENT_POST_ASSEMBLE, null);
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
        CallbackRegistry.callback(CallbackRegistry.EVENT_SIMULATE, null);
        if(!arch.hasSimCore())
            return Msg.error("simulate: This ISA does not implement a simulation" +
                         " core.", Constants.PLP_ISA_NO_SIMCORE, this);

        if(asm_req) {
            int ret = assemble();
            if(ret != Constants.PLP_OK) return Constants.PLP_ASM_ASSEMBLE_FAILED;
        }

        Msg.info("Starting simulation...", null);

        if(g) this.updateAsm(open_asm, g_dev.getEditor().getText());

        if(asm == null || !asm.isAssembled())
            return Msg.error("simulate: The project is not assembled.",
                            Constants.PLP_BACKEND_NO_ASSEMBLED_OBJECT, this);

        int checkRet = asm.preSimulationCheck();
        if(checkRet != Constants.PLP_OK)
            return checkRet;

        if(g && sim_mode)
            desimulate();

        sim = arch.createSimCore();
        ioreg = new IORegistry(this);
        arch.simulatorInitialization();
        CallbackRegistry.callback(CallbackRegistry.EVENT_SIM_INIT, null);

        Msg.debug("I/O Modules: smods is " + (smods == null ? "null" : "not null")
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
        CallbackRegistry.callback(CallbackRegistry.EVENT_SIM_POST_INIT, null);
        return Constants.PLP_OK;
    }

    /**
     * Destroy current simulation.
     *
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int desimulate() {
        CallbackRegistry.callback(CallbackRegistry.EVENT_DESIMULATE, null);
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

        if(g_simctrl != null)
            g_simctrl.dispose();

        g_ioreg = null;
        g_watcher = null;
        g_simctrl = null;

        sim_mode = false;
        updateWindowTitle();

        CallbackRegistry.callback(CallbackRegistry.EVENT_SIM_POST_UNINIT, null);
        return Constants.PLP_OK;
    }

    /**
     * Run the simulation driver (SimRunner) thread
     *
     * @return PLP_OK on successful operation, error code otherwise
     */
    public int runSimulation() {
        CallbackRegistry.callback(CallbackRegistry.SIM_RUN_START, null);
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
        CallbackRegistry.callback(CallbackRegistry.SIM_RUN_STOP, null);
        if(g_simrun != null) {
            try {
                g_simrun.stepCount = 0;
                while(!g_simrun.isDone()) {
                    // wait for the thread to clean up
                }
                g_simrun.gracefullyQuit();
            } catch(Exception e) {}
        }

        if(g && g_simctrl != null) {
            g_dev.stopSimState();
            g_simctrl.stopSimState();
        }
        updateComponents(true);
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
        CallbackRegistry.callback(CallbackRegistry.EVENT_PROGRAM, port);
        if(!arch.hasProgrammer())
                return Msg.error("This ISA does not implement a board programmer.",
                             Constants.PLP_ISA_NO_PROGRAMMER, this);

        if(!serial_support)
            return Msg.error("No native serial libraries available.",
                         Constants.PLP_BACKEND_NO_NATIVE_SERIAL_LIBS, this);

        if(asm_req) {
            int ret = assemble();
            if(ret != Constants.PLP_OK) return Constants.PLP_ASM_ASSEMBLE_FAILED;
        }
     
        Msg.info("Programming to " + port, this);

        try {

        if(asm != null && asm.isAssembled()) {

            if(asm.getObjectCode().length < 1)
                return Msg.error("Empty program.",
                             Constants.PLP_PRG_EMPTY_PROGRAM, this);
            
            prg = arch.createProgrammer();

            if(prg == null)
                return Msg.error("The specified ISA does not have the serial " +
                             "programmer implemented.",
                             Constants.PLP_ISA_NO_PROGRAMMER, this);

            int ret = prg.connect(port);
            if(ret != Constants.PLP_OK)
                return ret;
            p_progress = 0;

            /*** RXTX Linux hack for the Nexys3 board ***/
            if(arch.equals("plpmips") && Config.prgNexys3ProgramWorkaround && PLPToolbox.isHostLinux()) {
                Msg.debug("program: Nexys 3 Linux RXTX workaround engaging...", 2, this);
                prg.close();
                prg = arch.createProgrammer();
                prg.connect(port);
            }
            
            if(g) {
                g_prg.disableControls();
                g_prg.getProgressBar().setMinimum(0);
                g_prg.getProgressBar().setMaximum(asm.getObjectCode().length - 1);
            }

            CallbackRegistry.callback(CallbackRegistry.EVENT_PROGRAM_INIT, prg);
            Msg.debug("Starting PLPSerialProgrammer thread", 2, this);
            prg.start();
            return Constants.PLP_OK;

        } else
            return Msg.error("No assembled sources.",
                            Constants.PLP_PRG_SOURCES_NOT_ASSEMBLED, this);

        } catch(Exception e) {
            Msg.trace(e);
            return Msg.error("Programming failed.\n" + e,
                            Constants.PLP_GENERIC_ERROR, this);
        }
    }

    /**
     * Alert the project driver that the project state has been changed and
     * may need saving.
     */
    public void setModified() {
        Msg.debug("Project has been modified.", 5, this);
        modified = true;
        if(g)
            this.updateWindowTitle();
    }

    public void setUnModified() {
        Msg.debug("Project has been unmodified.", 5, this);
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
            CallbackRegistry.callback(CallbackRegistry.PROJECT_OPENASM_CHANGE, open_asm);
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
            Msg.error("getAsm: Invalid index: " + index,
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
            return Msg.error("updateAsm: Invalid index: " + index,
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
                return Msg.error("The file '" + name + "' already exists in " +
                             "the project.", Constants.PLP_BACKEND_IMPORT_CONFLICT, this);
        }

        asms.add(new PLPAsmSource("# New ASM File", name, asms.size()));
        Object[] cParams = {asms.get(asms.size()-1), asms.size()-1};
        CallbackRegistry.callback(CallbackRegistry.PROJECT_NEW_ASM, cParams);
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
    	if (path == null)
    		return Constants.PLP_ERROR_RETURN;
    	
        File asmFile = new File(path);

        Msg.info("Importing " + path, null);

        if(!asmFile.exists())
            return Msg.error("importAsm(" + path + "): file not found.",
                            Constants.PLP_BACKEND_ASM_IMPORT_ERROR, this);

        if(!asmFile.getName().endsWith(".asm")) {
            return Msg.error("importAsm(" + path + "): imported source files " +
                         "must have a .asm extension.",
                         Constants.PLP_BACKEND_ASM_IMPORT_ERROR, this);
        }

        String existingPath;
        for(int i = 0; i < asms.size(); i++) {
            existingPath = asms.get(i).getAsmFilePath();

            if(existingPath.equals(path) ||
               existingPath.equals(asmFile.getName())) {
                return Msg.error("importAsm(" + path + "): File with the same name already exists.",
                                Constants.PLP_BACKEND_IMPORT_CONFLICT, this);
            }
        }

        asms.add(new PLPAsmSource(null, path, asms.size()));
        asms.get(asms.size() - 1).setAsmFilePath(asmFile.getName());
        Object[] cParams = {asms.get(asms.size() - 1), asms.size() - 1};
        CallbackRegistry.callback(CallbackRegistry.PROJECT_NEW_ASM, cParams);

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

        if(asms == null || index < 0 || index >= asms.size())
            return Msg.error("exportAsm: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        Msg.info("Exporting " + asms.get(index).getAsmFilePath() +
                 " to " + path, null);

        if(asmFile.exists()) {
            return Msg.error("exportAsm: " + path + " exists.",
                            Constants.PLP_FILE_SAVE_ERROR, this);
        }

        if(g && getOpenAsm() == index)
            updateAsm(index, g_dev.getEditorText());

        try {

        PLPToolbox.writeFile(asms.get(index).getAsmString(), path);

        } catch(Exception e) {
            Msg.trace(e);
            return Msg.error("exportAsm(" + asms.get(index).getAsmFilePath() +
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
        if(asms == null || index < 0 || index >= asms.size())
            return  Msg.error("removeAsm: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);
        
        if(asms.size() <= 1) {
            return  Msg.error("removeAsm: Can not delete last source file.",
                            Constants.PLP_BACKEND_DELETING_LAST_ASM_ERROR, this);
        }

        setModified();

        if(index < open_asm) {
            Msg.debug("removeAsm: index < open_asm: " + index + "<" + open_asm, 4, this);
            if(g) updateAsm(open_asm, g_dev.getEditorText());
            open_asm--;
        } else if(index == open_asm && open_asm != 0) {
            Msg.debug("removeAsm: index == open_asm: " + index + "==" + open_asm, 4, this);
            open_asm--;
        } else {
            Msg.debug("removeAsm: index > open_asm: " + index + ">" + open_asm, 4, this);
            if(g) updateAsm(open_asm, g_dev.getEditorText());
        }
            
        Msg.info("Removing " + asms.get(index).getAsmFilePath(), null);
        Object[] cParams = {asms.get(index), index};
        CallbackRegistry.callback(CallbackRegistry.PROJECT_REMOVE_ASM, cParams);
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

        return Msg.error("removeAsm: Can not find source file with the name \"" +
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
            return Msg.error("setMainAsm: Invalid index: " + index,
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
            return Msg.error("setAsmPosition: Invalid index: " + index,
                            Constants.PLP_BACKEND_BOUND_CHECK_FAILED, this);

        if(asms == null || newIndex < 0 || newIndex >= asms.size())
            return Msg.error("setAsmPosition: Invalid new index: " + newIndex,
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
     * Get the build error list from the previous failed build
     *
     * @return An arraylist of PLPBuildError objects
     */
    public ArrayList<PLPBuildError> getBuildErrorList() {
        return buildErrorList;
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

        CallbackRegistry.callback(CallbackRegistry.GUI_UPDATE, null);

        } catch(Exception e) {
            // GUI update error has occured
            System.out.println("GUI error has occured. Switch to debug level 2 or above to print stack trace.");
            Msg.trace(e);
        }
    }

    /**
     * Use this method for some unforeseen bug!
     */
    public void triggerCriticalError() {
        // XXX: removed "hault = true"; ensure processes are stopped
        System.err.println("[CRITICAL ERROR] " +
                    "This really, really, really, should not have happened.");
        System.err.println("[CRITICAL ERROR] " +
                    "PLPTool is now exiting. Please report this issue. Thanks!");
        if(g) {
            javax.swing.JOptionPane.showMessageDialog(g_dev,
                    "This really, really, really, should not have happened. " +
                    "PLPTool is now exiting. Please report this issue. Thanks!",
                    "CRITICAL ERROR", javax.swing.JOptionPane.ERROR_MESSAGE);
        }
        
        if(plpfile != null) {
            System.err.println("Saving current open project as ./dump.plp...");
            plpfile = new File("dump.plp");
            this.save();
        }

        CallbackRegistry.callback(CallbackRegistry.CRITICAL_ERROR, null);
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
