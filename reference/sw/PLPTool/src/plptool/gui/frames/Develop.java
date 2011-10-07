/*
    Copyright 2010-2011 David Fritz, Brian Gordon, Joshua Holland, Wira Mulia

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

package plptool.gui.frames;

import java.awt.Desktop;
import java.awt.Point;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import java.net.URI;



import java.io.File;

//For Syntax Highlighting
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import plptool.Msg;
import plptool.Constants;
import plptool.Config;
import plptool.PLPSimBusModule;
import plptool.mods.*;
import plptool.gui.ProjectDriver;
import plptool.gui.SerialTerminal;
import plptool.gui.NumberConverter;

/**
 *
 * @author wira
 */
public class Develop extends javax.swing.JFrame {
    final static int RTYPE = 0;
    final static int ITYPE = 1;
    final static int BRANCH = 2;
    final static int JUMP = 3;
    final static int MEMTYPE = 4;
    final static int NOP = 5;
    final static int REG = 6;
    final static int IMM = 7;
    final static int LABEL = 8;
    final static int COMMENT = 9;
    final static int SYS = 10;

    boolean trackChanges = false;
    private ProjectDriver plp;
    private DevUndoManager undoManager;
    private javax.swing.JPopupMenu popupProject;

    private TextLineNumber tln;
    private TextLineHighlighter tlh;

    // caret position
    private int line;

    private double hPaneSavedProportion = -1;
    private double vPaneSavedProportion = -1;

    /** Records number of non character keys pressed */
    int nonTextKeyPressed = 0;

    public SimpleAttributeSet[] styles = setupHighlighting();
    
    /** Creates new form PLPDevelop */
    public Develop(ProjectDriver plp) {
        this.plp = plp;
        initComponents();
        line = 0;

        DefaultMutableTreeNode projectRoot = new DefaultMutableTreeNode("No PLP Project Open");
        DefaultTreeModel treeModel = new DefaultTreeModel(projectRoot);
        treeProject.setModel(treeModel);
        
        splitterH.setDividerLocation(0.25);

        tlh = new TextLineHighlighter(txtEditor);
        tln = new TextLineNumber(txtEditor, tlh, plp);
        scroller.setRowHeaderView(tln);

        catchyPLP();

        Msg.output = txtOutput;
        scroller.setEnabled(false);
        txtOutput.setEditable(false);
        rootmenuProject.setEnabled(false);
        menuPrint.setEnabled(false);
        menuImportASM.setEnabled(false);
        menuNewASM.setEnabled(false);
        menuExportASM.setEnabled(false);
        menuDeleteASM.setEnabled(false);
        menuSave.setEnabled(false);
        menuSaveAs.setEnabled(false);
        btnAssemble.setEnabled(false);
        menuFindAndReplace.setEnabled(false);
        rootmenuEdit.setEnabled(false);
        btnSave.setEnabled(false);
        disableSimControls();

        treeProject.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);

        this.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent we) {
                exit();
            }
        });

        txtEditor.getDocument().addDocumentListener(new javax.swing.event.DocumentListener() {
            public void changedUpdate(javax.swing.event.DocumentEvent e) {
                notifyplpModified();
            }
            public void removeUpdate(javax.swing.event.DocumentEvent e) {
                notifyplpModified();
            }
            public void insertUpdate(javax.swing.event.DocumentEvent e) {
                notifyplpModified();
            }
        });


        this.setDefaultCloseOperation(javax.swing.JFrame.DO_NOTHING_ON_CLOSE);

        undoManager = new DevUndoManager();
        undoManager.setLimit(Config.devMaxUndoEntries);
        
        initPopupMenus();
        
        this.setLocationRelativeTo(null);
        this.setIconImage(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/appicon.png")));

        Msg.M(Constants.copyrightString);
    }

    public void setLblSimStatText(String txt) {
        lblSimStat.setText(txt);
    }

    public void updateIOFramesVisibility() {
        if(!plp.isSimulating())
            return;

        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            PLPSimBusModule mod = plp.ioreg.getModule(i);
            JFrame frame;
            if(plp.ioreg.getModuleFrame(i) instanceof JFrame) {
                frame = (JFrame) plp.ioreg.getModuleFrame(i);
                boolean frameVisible = frame.isVisible();

                if(mod instanceof LEDArray) {
                    btnSimLEDs.setSelected(frameVisible);
                    menuSimLEDs.setSelected(frameVisible);

                } else if(mod instanceof Switches) {
                    btnSimSwitches.setSelected(frameVisible);
                    menuSimSwitches.setSelected(frameVisible);

                } else if(mod instanceof SevenSegments) {
                    btnSimSevenSegments.setSelected(frameVisible);
                    menuSimSevenSegments.setSelected(frameVisible);

                } else if(mod instanceof UART) {
                    btnSimUART.setSelected(frameVisible);
                    menuSimUART.setSelected(frameVisible);

                } else if(mod instanceof VGA) {
                    btnSimVGA.setSelected(frameVisible);
                    menuSimVGA.setSelected(frameVisible);

                } else if(mod instanceof PLPID) {
                    btnSimPLPID.setSelected(frameVisible);
                    menuSimPLPID.setSelected(frameVisible);
                }
            }
        }
    }

    public void updateComponents() {
        try {
        if(plp.isSimulating()) {

            SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run() {
                        plptool.mips.SimCore sim = (plptool.mips.SimCore) plp.sim;
                        int pc_index = plp.asm.lookupAddrIndex(sim.id_stage.i_instrAddr);
                        if(pc_index == -1 || sim.isStalled()) {
                            tln.setHighlight(-1);
                            return;
                        }

                        updateStatText();
                        
                        final int lineNum = plp.asm.getLineNumMapper()[pc_index];
                        int fileNum = plp.asm.getFileMapper()[pc_index];

                        int yPos = (lineNum - 1) * txtEditor.getFontMetrics(txtEditor.getFont()).getHeight();
                        int viewPortY = scroller.getViewport().getViewPosition().y;

                        if(yPos > (viewPortY + scroller.getHeight()) || yPos < viewPortY)
                            scroller.getViewport().setViewPosition(new Point(0, yPos - scroller.getSize().height / 2));

                        if(plp.open_asm != fileNum) {
                            plp.open_asm = fileNum;
                            //plp.refreshProjectView(false);
                            safeRefresh(false);
                        } else {
                            tln.setHighlight(lineNum - 1);
                            repaintLater();
                        }
                    }
                });
        }
        } catch(Exception e) {
            JOptionPane.showMessageDialog(this, "Looks like the GUI is being refreshed too fast!\n" +
                                          "Slow down simulation speed or turn off IDE refresh in simulation options to prevent this error.", "PLPTool Error", JOptionPane.ERROR_MESSAGE);

            if(Constants.debugLevel >= 5)
                e.printStackTrace();

            if(plp.g_simrun != null)
                plp.g_simrun.stepCount = -1;
        }
    }

    public void updateStatText() {
        lblSimStat.setText(
                    (Config.simFunctional ? "Functional " : "") + "Simulation Mode - " +
                    "Cycles / step: " + Config.simCyclesPerStep + " - " +
                    "Cycle: " + plp.sim.getInstrCount()
                );
    }

    private void repaintLater() {
        SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run() {
                        tlh.repaint();
                        tln.repaint();
                    }
                });
    }

    private void repaintNow() {
        tlh.repaint();
        tln.repaint();
    }

    public void safeRefresh(final boolean commit) {
        SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run() {
                        plp.refreshProjectView(commit);
                    }
                });
    }

    public void changeFormatting() {
        java.awt.Font newFont = new java.awt.Font(Config.devFont, java.awt.Font.PLAIN, Config.devFontSize);
        txtEditor.setFont(newFont);
        txtEditor.setBackground(Config.devBackground);
        txtEditor.setForeground(Config.devForeground);
        styles = setupHighlighting();

        safeRefresh(true);
        repaintLater();
    }

    public void notifyplpModified() {
        if(trackChanges) {
            if(Config.nothighlighting) {
                //plp.setModified();
            }
        }
    }

    public javax.swing.JTextPane getOutput() {
        return txtOutput;
    }

    public javax.swing.JScrollPane getScroller() {
        return scroller;
    }

    public javax.swing.JEditorPane getEditor() {
        return txtEditor;
    }

    public void setEditorText(String str) {
        txtEditor.setContentType("text");
        trackChanges = false;
        if(!str.equals(txtEditor.getText())) {
            txtEditor.setText(str);
            if(Config.devSyntaxHighlighting && str.length() <= Config.filetoolarge)
                syntaxHighlight();
        }
        trackChanges = true;
        undoManager = new DevUndoManager();
        undoManager.setLimit(Config.devMaxUndoEntries);

        txtEditor.getDocument().addUndoableEditListener(new UndoableEditListener() {

            @Override
            public void undoableEditHappened(UndoableEditEvent evt) {
                undoManager.safeAddEdit(evt.getEdit());
            }
        });
    }

    public void setFocusToEditor() {
        txtEditor.requestFocusInWindow();
    }

    public String getEditorText() {
        return txtEditor.getText();
    }

    public void setCurFile(String path) {
        txtCurFile.setText(path);
    }

    public javax.swing.JTree getProjectTree() {
        return treeProject;
    }

    public void setToolbarVisible(boolean b) {
        toolbar.setVisible(b);
    }

    public final void enableBuildControls() {
        rootmenuProject.setEnabled(true);
        menuImportASM.setEnabled(true);
        menuNewASM.setEnabled(true);
        menuSave.setEnabled(true);
        menuSaveAs.setEnabled(true);
        btnAssemble.setEnabled(true);
        menuDeleteASM.setEnabled(true);
        menuExportASM.setEnabled(true);
        rootmenuEdit.setEnabled(true);
        menuFindAndReplace.setEnabled(true);
        menuPrint.setEnabled(true);
        btnSave.setEnabled(true);
    }

    public final void closeProject() {
        plp.plpfile = null;
        plp.setUnModified();
        txtCurFile.setText("No file open");
        simEnd();
        //plp.refreshProjectView(false);
        safeRefresh(false);
    }

    public final void disableBuildControls() {
        disableSimControls();
        menuSave.setEnabled(false);
        menuSaveAs.setEnabled(false);
        menuPrint.setEnabled(false);
        rootmenuProject.setEnabled(false);
        rootmenuEdit.setEnabled(false);
        txtEditor.setEnabled(false);
    }

    public final void disableSimControls() {
        menuSimulate.setEnabled(false);
        menuProgram.setEnabled(false);
        btnSimulate.setEnabled(false);
        btnProgram.setEnabled(false);
        menuQuickProgram.setEnabled(false);
        simEnd();
     }

    public final  void enableSimControls() {
        menuSimulate.setEnabled(true);
        menuProgram.setEnabled(true);
        btnSimulate.setEnabled(true);
        btnProgram.setEnabled(true);
        menuQuickProgram.setEnabled(true);
    }

    public int save() {
        
        return Constants.PLP_OK;
    }

    public void exit() {
        switch(askSaveFirst("exit", "Exit")) {
            case 2:
                return;
            default:
                System.exit(0);
        }
    }

    public void newPLPFile() {
        switch(askSaveFirst("create a new project", "Create a new project")) {
            case 2:
                return;
            default:
                if(plp.isSimulating())
                    simEnd();
                plp.create();
                //undoManager = new DoManager(txtEditor.getText());
        }
    }

    public void openPLPFile() {
        switch(askSaveFirst("open a project", "Open a project")) {
            case 2:
                return;
            default:
                Msg.output = txtOutput;

                final javax.swing.JFileChooser fc = new javax.swing.JFileChooser();
                fc.setFileFilter(new PlpFilter());
                fc.setAcceptAllFileFilterUsed(false);
                fc.setCurrentDirectory(new File(plp.curdir));

                int retVal = fc.showOpenDialog(null);

                if(retVal == javax.swing.JFileChooser.APPROVE_OPTION) {
                    if(plp.isSimulating())
                        simEnd();
                    plp.curdir = fc.getSelectedFile().getParent();
                    plp.open(fc.getSelectedFile().getAbsolutePath(), true);
                }

                //undoManager = new DoManager(txtEditor.getText());
        }
    }

    public int savePLPFileAs() {
        Msg.output = txtOutput;

        final javax.swing.JFileChooser fc = new javax.swing.JFileChooser();
        fc.setFileFilter(new PlpFilter());
        fc.setAcceptAllFileFilterUsed(false);
        fc.setCurrentDirectory(new File(plp.curdir));

        int retVal = fc.showSaveDialog(null);

        if(retVal == javax.swing.JFileChooser.APPROVE_OPTION) {
            File fOut = new File(fc.getSelectedFile().getAbsolutePath());

            if(fOut.exists()) {
                int ret = JOptionPane.showConfirmDialog(this, fOut.getName() +
                        " already exists. Overwrite?", "File exists",
                        JOptionPane.YES_NO_OPTION);
            
                if(ret == JOptionPane.NO_OPTION)
                    return -1;
            }

            plp.plpfile = fOut;
            plp.curdir = fc.getSelectedFile().getParent();
            if(!plp.plpfile.getName().endsWith(".plp"))
                plp.plpfile = new File(plp.plpfile.getAbsolutePath() + ".plp");
            plp.save();
            plp.open(plp.plpfile.getAbsolutePath(), true);
        }

        return retVal;
    }

    public int askSaveFirst(String action, String capAction) {
        int ret = javax.swing.JFileChooser.APPROVE_OPTION;

        if(plp.isModified()) {
            Object[] options = {"Save and " + action,
                    capAction + " without saving",
                    "Cancel"};
            int n = javax.swing.JOptionPane.showOptionDialog(this,
                "HALT! The project is modified and you are trying to " +
                action + ". How would you like to proceed?",
                "Project is modified and not saved",
                javax.swing.JOptionPane.YES_NO_CANCEL_OPTION,
                javax.swing.JOptionPane.QUESTION_MESSAGE,
                null,
                options,
                options[2]);

            if(n == 0)
                if(plp.plpfile.getName().equals("Unsaved Project"))
                    ret = this.savePLPFileAs();
                else
                    plp.save();

            // check if user hit the close window button (X button on the corner)
            else if(n == javax.swing.JOptionPane.CLOSED_OPTION)
                return 2;

            if(ret == javax.swing.JFileChooser.APPROVE_OPTION)
                return n;
            else
                return 2;
        }

        return -1;
    }

    public int deleteASM() {
        DefaultMutableTreeNode node = (DefaultMutableTreeNode) treeProject.getLastSelectedPathComponent();

        if(node == null)
            return Constants.PLP_GENERIC_ERROR;

        if(node.isLeaf()) {
            String nodeStr = (String) node.getUserObject();

            if(nodeStr.endsWith("asm")) {

                if(plp.asms.size() <= 1) {
                    Msg.E("Can not delete last source file.",
                             Constants.PLP_GENERIC_ERROR, null);

                    return Constants.PLP_GENERIC_ERROR;
                }

                String[] tokens = nodeStr.split(": ");

                int remove_asm = Integer.parseInt(tokens[0]);
                plp.removeAsm(remove_asm);
            }
        }

        return Constants.PLP_OK;
    }

    public int importASM() {
        Msg.output = txtOutput;

        final javax.swing.JFileChooser fc = new javax.swing.JFileChooser();
        fc.setFileFilter(new AsmFilter());
        fc.setAcceptAllFileFilterUsed(false);
        fc.setCurrentDirectory(new File(plp.curdir));

        int retVal = fc.showOpenDialog(null);

        if(retVal == javax.swing.JFileChooser.APPROVE_OPTION) {
            plp.importAsm(fc.getSelectedFile().getAbsolutePath());
        }

        return Constants.PLP_OK;
    }

    public int exportASM() {
        Msg.output = txtOutput;
        int indexToExport = -1;
        DefaultMutableTreeNode node = (DefaultMutableTreeNode) treeProject.getLastSelectedPathComponent();

        if(node == null)
            return Constants.PLP_GENERIC_ERROR;

        if(node.isLeaf()) {
            String nodeStr = (String) node.getUserObject();

            if(nodeStr.endsWith("asm")) {
                String[] tokens = nodeStr.split(": ");
                indexToExport = Integer.parseInt(tokens[0]);
            }
        }

        if(indexToExport >= 0) {
            final javax.swing.JFileChooser fc = new javax.swing.JFileChooser();
            fc.setFileFilter(new AsmFilter());
            fc.setAcceptAllFileFilterUsed(false);
            fc.setCurrentDirectory(new File(plp.curdir));

            int retVal = fc.showSaveDialog(null);

            if(retVal == javax.swing.JFileChooser.APPROVE_OPTION)
                plp.exportAsm(indexToExport, fc.getSelectedFile().getAbsolutePath());
        }

        return Constants.PLP_OK;
    }

    public void about() {
        plp.g_about.setVisible(true);
    }

    private void syntaxHighlight() {
        Config.nothighlighting = false;
        int currpos = 0;
        String lines[] = txtEditor.getText().split("\\r?\\n");
        int doclength = lines.length;

        for(int i=0;i<doclength;i++) {
            String currline = lines[i];
            syntaxHighlight(currline, currpos, styles);
            currpos += lines[i].length() + 1;
        }
        Config.nothighlighting = true;
    }

    private void redo() {
        Msg.D("redo()", 10, this);

        if(undoManager.canRedo()) {
            undoManager.safeRedo();
        }
    }

    private void undo() {
        if(undoManager.canUndo()) {
            undoManager.safeUndo();
        }
    }

    public void syntaxHighlight(int line) {
        Config.nothighlighting = false;
        try {
            String currline = txtEditor.getText().split("\\r?\\n")[line];
            int currpos = 0;
            for(int i=0;i<line;i++) {
                currpos += txtEditor.getText().split("\\r?\\n")[i].length() + 1;
            }
            syntaxHighlight(currline, currpos, setupHighlighting());
        } catch (java.lang.ArrayIndexOutOfBoundsException aioobe) {
        }
        Config.nothighlighting = true;
    }

    //Do not call this class without setting highlighting to true
    //Or without recording selected text
    private void syntaxHighlight(String text, int position, SimpleAttributeSet[] styles) {
        StyledDocument doc = txtEditor.getStyledDocument();
        int currentposition = 0;
        int startposition = 0;
        int texttype = -1;
        String currtext = "";
        while (currentposition < text.length()) {
            StringBuilder currtextbuffer = new StringBuilder();
            while (currentposition < text.length() && (text.charAt(currentposition) == '\t' || text.charAt(currentposition) == ' ' || text.charAt(currentposition) == ',')) {
                    currentposition++;
            }
            startposition = currentposition;
            while (currentposition < text.length() && (text.charAt(currentposition) != '\t' && text.charAt(currentposition) != ' ')) {
                    currtextbuffer.append(text.charAt(currentposition));
                    currentposition++;
            }
            currtext = currtextbuffer.toString();

            if(texttype == COMMENT || currtext.contains("#")) {
                    texttype = COMMENT;
            } else if (texttype == SYS || currtext.contains(".")) {
                    texttype = SYS;
            } else if(currtext.contains(":")) {
                    texttype = LABEL;
            } else if(currtext.contains("$")) {
                    texttype = REG;
            } else if(isimmediate(currtext)) {
                    texttype = IMM;
            } else if(currtext.equals("nop")) {
                    texttype = NOP;
            } else if (texttype == -1) {
                if(currtext.contains("w")) {
                        texttype = MEMTYPE;
                } else if(currtext.contains("j")) {
                        texttype = JUMP;
                } else if(currtext.contains("b")) {
                        texttype = BRANCH;
                } else if(currtext.contains("i")) {
                        texttype = ITYPE;
                } else {
                        texttype = RTYPE;
                }
            } else {
                texttype = LABEL;
            }

            try {
                doc.remove(startposition+position,currentposition-startposition);
            } catch (BadLocationException ble) {
                System.err.println("Deletion error   position:" + position + "," + currtext.length());
            }

            try {
                doc.insertString(startposition+position,currtext,styles[texttype]);
            } catch (BadLocationException ble) {
                System.err.println("Insertion error   position:" + position);
            }
            
            currentposition++;
        }
    }

    private boolean isimmediate(String num) {
        Pattern pattern0 = Pattern.compile("-?[0-9]*");
        Matcher matcher0 = pattern0.matcher(num);
        Pattern pattern1 = Pattern.compile("0x[0-9a-fA-F]*");
        Matcher matcher1 = pattern1.matcher(num);
        Pattern pattern2 = Pattern.compile("0b[0-1]*");
        Matcher matcher2 = pattern2.matcher(num);
        Pattern pattern3 = Pattern.compile("'\\S'");
        Matcher matcher3 = pattern3.matcher(num);
        return (matcher0.matches() || matcher1.matches() ||
                matcher2.matches() || matcher3.matches());
    }

    //Called whenever syntax styles change
    private SimpleAttributeSet[] setupHighlighting() {
        Style def = StyleContext.getDefaultStyleContext().getStyle(StyleContext.DEFAULT_STYLE);
        StyleConstants.setFontFamily(def,Config.devFont);
        StyleConstants.setFontSize(def,Config.devFontSize);
        SimpleAttributeSet[] styleSetup = new SimpleAttributeSet[11];
        for(int i=0;i<11;i++) {
            styleSetup[i] = new SimpleAttributeSet(def);
            StyleConstants.setForeground(styleSetup[i],Config.syntaxColors[i]);
            StyleConstants.setBold(styleSetup[i], Config.syntaxBold[i]);
            StyleConstants.setItalic(styleSetup[i], Config.syntaxItalic[i]);
        }
        return styleSetup;
    }

    public void catchyPLP() {
        String catchyStr;

        catchyStr =  "Progressive Learning Platform\n\n";
        catchyStr += "You can start by creating a new project or opening an existing one.\n\n";

        catchyStr += "Build: " + plptool.Version.stamp + "\n";
        catchyStr += "OS/arch: " + System.getProperty("os.name")
                      + "/" + System.getProperty("os.arch") + "\n";

        boolean savedConfig = Config.devSyntaxHighlighting;
        Config.devSyntaxHighlighting = false;
        txtEditor.setText(catchyStr);
        Config.devSyntaxHighlighting = savedConfig;
    }

    public DevUndoManager getUndoManager() {
        return undoManager;
    }

    private void assemble() {
        Msg.output = txtOutput;

        if(plp.plpfile != null)
            plp.assemble();

        if(Config.devSyntaxHighlightOnAssemble) {
            Config.nothighlighting = false;
            syntaxHighlight();
            Config.nothighlighting = true;
            Config.devSyntaxHighlightOnAssemble = false;
        }
    }

    private void simReset() {
        if(plp.g_simrun != null)
            plp.stopSimulation();
        plp.sim.reset();

        tln.setHighlight(-1);
        tlh.setY(-1);
        repaintLater();
        plp.open_asm = 0;
        plp.updateComponents(true);
        //plp.refreshProjectView(false);
        safeRefresh(false);
    }

    public void simBegin() {
        if(plp.simulate() == Constants.PLP_OK) {
            menuSimulate.setSelected(true);
            txtEditor.setEditable(false);
            rootmenuSim.setEnabled(true);
            btnSimulate.setSelected(true);
            btnSimRun.setVisible(true);
            separatorSim.setVisible(true);
            btnSimControl.setVisible(true);
            btnSimReset.setVisible(true);
            btnSimStep.setVisible(true);
            btnWatcher.setVisible(true);
            btnCPU.setVisible(true);
            btnSimLEDs.setVisible(true);
            btnSimSwitches.setVisible(true);
            btnSimSevenSegments.setVisible(true);
            btnSimUART.setVisible(true);
            btnSimVGA.setVisible(true);
            btnSimPLPID.setVisible(true);
            separatorSimControl.setVisible(true);
            lblSimStat.setText("Simulation Mode");
            updateIOFramesVisibility();
        } else
            simEnd();
    }

    public void simEnd() {

        txtEditor.setEditable(true);
        txtEditor.getCaret().setVisible(true);
        menuSimulate.setSelected(false);
        menuSimRun.setSelected(false);
        menuSimView.setSelected(false);
        menuSimWatcher.setSelected(false);
        menuSimIO.setSelected(false);
        tln.setHighlight(-1);
        tlh.setY(-1);
        repaintLater();
        if(plp.isSimulating()) {
            plp.stopSimulation();
            plp.desimulate();
        }
        btnSimControl.setVisible(false);
        rootmenuSim.setEnabled(false);
        btnSimulate.setSelected(false);
        btnSimRun.setSelected(false);
        btnCPU.setSelected(false);
        btnWatcher.setSelected(false);
        btnSimStep.setVisible(false);
        btnSimReset.setVisible(false);
        btnSimRun.setVisible(false);
        btnWatcher.setVisible(false);
        separatorSim.setVisible(false);
        btnCPU.setVisible(false);
        btnSimLEDs.setVisible(false);
        btnSimSwitches.setVisible(false);
        btnSimSevenSegments.setVisible(false);
        btnSimUART.setVisible(false);
        btnSimVGA.setVisible(false);
        btnSimPLPID.setVisible(false);
        separatorSimControl.setVisible(false);
        lblSimStat.setText("Editor Mode");
    }

    public void stopRunState() {
        menuSimRun.setSelected(false);
        btnSimRun.setSelected(false);
    }

    public javax.swing.JCheckBoxMenuItem getToolCheckboxMenu(int index) {
        switch(index) {
            case Constants.PLP_TOOLFRAME_IOREGISTRY:
                return menuSimIO;

            case Constants.PLP_TOOLFRAME_SIMCPU:
                return menuSimView;

            case Constants.PLP_TOOLFRAME_WATCHER:
                return menuSimWatcher;

            case Constants.PLP_TOOLFRAME_SIMRUN:
                return menuSimRun;

            case Constants.PLP_TOOLFRAME_SIMCTRL:
                return menuSimControl;

            case Constants.PLP_TOOLFRAME_SIMLEDS:
                return menuSimLEDs;

            case Constants.PLP_TOOLFRAME_SIMSWITCHES:
                return menuSimSwitches;

            case Constants.PLP_TOOLFRAME_SIMUART:
                return menuSimUART;

            case Constants.PLP_TOOLFRAME_SIMVGA:
                return menuSimVGA;

            case Constants.PLP_TOOLFRAME_SIMPLPID:
                return menuSimPLPID;

            case Constants.PLP_TOOLFRAME_SIMSEVENSEGMENTS:
                return menuSimSevenSegments;

            default:
                return null;
        }
    }

    public javax.swing.JToggleButton getToolToggleButton(int index) {
        switch(index) {
            case Constants.PLP_TOOLFRAME_IOREGISTRY:
                return null;

            case Constants.PLP_TOOLFRAME_SIMCPU:
                return btnCPU;

            case Constants.PLP_TOOLFRAME_WATCHER:
                return btnWatcher;

            case Constants.PLP_TOOLFRAME_SIMRUN:
                return btnSimRun;

            case Constants.PLP_TOOLFRAME_SIMCTRL:
                return btnSimControl;

            case Constants.PLP_TOOLFRAME_SIMLEDS:
                return btnSimLEDs;

            case Constants.PLP_TOOLFRAME_SIMSWITCHES:
                return btnSimSwitches;

            case Constants.PLP_TOOLFRAME_SIMUART:
                return btnSimUART;

            case Constants.PLP_TOOLFRAME_SIMVGA:
                return btnSimVGA;

            case Constants.PLP_TOOLFRAME_SIMPLPID:
                return btnSimPLPID;

            case Constants.PLP_TOOLFRAME_SIMSEVENSEGMENTS:
                return btnSimSevenSegments;

            default:
                return null;
        }
    }

    /**
     * Attach listeners to the specified module frame x so it deselects the
     * corresponding control menu and toggle button when closing
     *
     * @param x Module frame to attach the listener to
     * @param int Menu type requested
     */
    public void attachModuleFrameListeners(final javax.swing.JFrame x, final int menu) {
        x.addWindowListener(new java.awt.event.WindowListener() {
            @Override
            public void windowClosing(java.awt.event.WindowEvent evt) {
                x.setVisible(false);
                getToolCheckboxMenu(menu).setSelected(false);

                JToggleButton btn = getToolToggleButton(menu);
                if(btn != null)
                    btn.setSelected(false);
            }

            @Override public void windowOpened(java.awt.event.WindowEvent evt) { }
            @Override public void windowDeactivated(java.awt.event.WindowEvent evt) { }
            @Override public void windowActivated(java.awt.event.WindowEvent evt) { }
            @Override public void windowDeiconified(java.awt.event.WindowEvent evt) { }
            @Override public void windowIconified(java.awt.event.WindowEvent evt) { }
            @Override public void windowClosed(java.awt.event.WindowEvent evt) { }
        });
    }

    private void setLEDsFrame(boolean v) {
        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            plptool.PLPSimBusModule module = plp.ioreg.getModule(i);

            if(module instanceof plptool.mods.LEDArray) {
                ((JFrame)plp.ioreg.getModuleFrame(i)).setVisible(v);
                menuSimLEDs.setSelected(v);
                btnSimLEDs.setSelected(v);
                plp.updateComponents(false);
            }
        }
    }

    private void setSwitchesFrame(boolean v) {
        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            plptool.PLPSimBusModule module = plp.ioreg.getModule(i);

            if(module instanceof plptool.mods.Switches) {
                ((JFrame)plp.ioreg.getModuleFrame(i)).setVisible(v);
                menuSimSwitches.setSelected(v);
                btnSimSwitches.setSelected(v);
                plp.updateComponents(false);
            }
        }
    }

    private void setSevenSegmentsFrame(boolean v) {
        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            plptool.PLPSimBusModule module = plp.ioreg.getModule(i);

            if(module instanceof plptool.mods.SevenSegments) {
                ((JFrame)plp.ioreg.getModuleFrame(i)).setVisible(v);
                menuSimSevenSegments.setSelected(v);
                btnSimSevenSegments.setSelected(v);
                plp.updateComponents(false);
            }
        }
    }

    private void setUARTFrame(boolean v) {
        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            plptool.PLPSimBusModule module = plp.ioreg.getModule(i);

            if(module instanceof plptool.mods.UART) {
                ((JFrame)plp.ioreg.getModuleFrame(i)).setVisible(v);
                menuSimUART.setSelected(v);
                btnSimUART.setSelected(v);
                plp.updateComponents(false);
            }
        }
    }

    private void setVGAFrame(boolean v) {
        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            plptool.PLPSimBusModule module = plp.ioreg.getModule(i);

            if(module instanceof plptool.mods.VGA) {
                ((JFrame)plp.ioreg.getModuleFrame(i)).setVisible(v);
                menuSimVGA.setSelected(v);
                btnSimVGA.setSelected(v);
                plp.updateComponents(false);
            }
        }
    }

    private void setPLPIDFrame(boolean v) {
        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            plptool.PLPSimBusModule module = plp.ioreg.getModule(i);

            if(module instanceof plptool.mods.PLPID) {
                ((JFrame)plp.ioreg.getModuleFrame(i)).setVisible(v);
                menuSimPLPID.setSelected(v);
                btnSimPLPID.setSelected(v);
                plp.updateComponents(false);
            }
        }
    }
    
    private void simStep() {
        boolean breakpoint = false;
        for(int i = 0; i < Config.simCyclesPerStep && !breakpoint; i++) {
            plp.sim.step();
            if(plp.sim.breakpoints.hasBreakpoint() && plp.sim.breakpoints.isBreakpoint(plp.sim.visibleAddr)) {
                Msg.M("--- breakpoint encountered: " + String.format("0x%02x", plp.sim.visibleAddr));
                breakpoint = true;
            }
        }
        plp.updateComponents(true);
    }

    private void clearBreakpoints() {
        plp.sim.breakpoints.clear();
        repaintLater();
    }

    public void clearLineHighlights() {
        tlh.setY(-1);
        repaintLater();
    }

    public void simRun() {
        tlh.setY(-1);
        repaintLater();
        menuSimRun.setSelected(true);
        btnSimRun.setSelected(true);
        menuSimStep.setEnabled(false);
        btnSimStep.setEnabled(false);
    }

    public void stopSimState() {
        menuSimRun.setSelected(false);
        btnSimRun.setSelected(false);
        menuSimStep.setEnabled(true);
        btnSimStep.setEnabled(true);
        repaintLater();
    }

    public void addButton(javax.swing.AbstractButton btn) {
        toolbar.add(btn);
    }

    public void removeLastButton() {
        Msg.D("Removing toolbar button.", 4, null);

        toolbar.remove(toolbar.getComponentCount() - 1);

        SwingUtilities.invokeLater(new Runnable() {
                            @Override
                            public void run() {
                                toolbar.revalidate();
                                toolbar.repaint();
                            }
                        });


    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        grpSteps = new javax.swing.ButtonGroup();
        devMainPane = new javax.swing.JPanel();
        splitterV = new javax.swing.JSplitPane();
        splitterH = new javax.swing.JSplitPane();
        jScrollPane2 = new javax.swing.JScrollPane();
        treeProject = new javax.swing.JTree();
        jPanel1 = new javax.swing.JPanel();
        txtCurFile = new javax.swing.JLabel();
        scroller = new javax.swing.JScrollPane();
        txtEditor = new javax.swing.JTextPane();
        lblPosition = new javax.swing.JLabel();
        lblSimStat = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        txtOutput = new javax.swing.JTextPane();
        toolbar = new javax.swing.JToolBar();
        btnNew = new javax.swing.JButton();
        btnOpen = new javax.swing.JButton();
        btnSave = new javax.swing.JButton();
        jSeparator4 = new javax.swing.JToolBar.Separator();
        btnAssemble = new javax.swing.JButton();
        btnSimulate = new javax.swing.JToggleButton();
        btnProgram = new javax.swing.JButton();
        separatorSim = new javax.swing.JToolBar.Separator();
        btnSimStep = new javax.swing.JButton();
        btnSimRun = new javax.swing.JToggleButton();
        btnSimReset = new javax.swing.JButton();
        btnSimControl = new javax.swing.JToggleButton();
        separatorSimControl = new javax.swing.JToolBar.Separator();
        btnCPU = new javax.swing.JToggleButton();
        btnWatcher = new javax.swing.JToggleButton();
        btnSimLEDs = new javax.swing.JToggleButton();
        btnSimSwitches = new javax.swing.JToggleButton();
        btnSimSevenSegments = new javax.swing.JToggleButton();
        btnSimUART = new javax.swing.JToggleButton();
        btnSimVGA = new javax.swing.JToggleButton();
        btnSimPLPID = new javax.swing.JToggleButton();
        jMenuBar1 = new javax.swing.JMenuBar();
        rootmenuFile = new javax.swing.JMenu();
        menuNew = new javax.swing.JMenuItem();
        menuSeparator1 = new javax.swing.JPopupMenu.Separator();
        menuOpen = new javax.swing.JMenuItem();
        menuSave = new javax.swing.JMenuItem();
        menuSaveAs = new javax.swing.JMenuItem();
        jSeparator8 = new javax.swing.JPopupMenu.Separator();
        menuPrint = new javax.swing.JMenuItem();
        menuSeparator3 = new javax.swing.JPopupMenu.Separator();
        menuExit = new javax.swing.JMenuItem();
        rootmenuEdit = new javax.swing.JMenu();
        menuCopy = new javax.swing.JMenuItem();
        menuCut = new javax.swing.JMenuItem();
        menuPaste = new javax.swing.JMenuItem();
        menuSeparator4 = new javax.swing.JPopupMenu.Separator();
        menuFindAndReplace = new javax.swing.JMenuItem();
        menuSeparator5 = new javax.swing.JPopupMenu.Separator();
        menuUndo = new javax.swing.JMenuItem();
        menuRedo = new javax.swing.JMenuItem();
        rootmenuView = new javax.swing.JMenu();
        menuToolbar = new javax.swing.JCheckBoxMenuItem();
        menuProjectPane = new javax.swing.JCheckBoxMenuItem();
        menuOutputPane = new javax.swing.JCheckBoxMenuItem();
        menuClearOutputPane = new javax.swing.JMenuItem();
        rootmenuProject = new javax.swing.JMenu();
        menuAssemble = new javax.swing.JMenuItem();
        menuSimulate = new javax.swing.JCheckBoxMenuItem();
        menuProgram = new javax.swing.JMenuItem();
        menuQuickProgram = new javax.swing.JMenuItem();
        jSeparator1 = new javax.swing.JPopupMenu.Separator();
        menuNewASM = new javax.swing.JMenuItem();
        menuImportASM = new javax.swing.JMenuItem();
        menuExportASM = new javax.swing.JMenuItem();
        menuDeleteASM = new javax.swing.JMenuItem();
        jSeparator2 = new javax.swing.JPopupMenu.Separator();
        menuSetMainProgram = new javax.swing.JMenuItem();
        rootmenuTools = new javax.swing.JMenu();
        menuOptions = new javax.swing.JMenuItem();
        jSeparator7 = new javax.swing.JPopupMenu.Separator();
        menuSerialTerminal = new javax.swing.JMenuItem();
        menuNumberConverter = new javax.swing.JMenuItem();
        rootmenuSim = new javax.swing.JMenu();
        menuSimStep = new javax.swing.JMenuItem();
        menuSimReset = new javax.swing.JMenuItem();
        menuSimRun = new javax.swing.JCheckBoxMenuItem();
        menuStepSize = new javax.swing.JMenu();
        menuStep1 = new javax.swing.JRadioButtonMenuItem();
        menuStep2 = new javax.swing.JRadioButtonMenuItem();
        menuStep3 = new javax.swing.JRadioButtonMenuItem();
        menuStep4 = new javax.swing.JRadioButtonMenuItem();
        menuStep5 = new javax.swing.JRadioButtonMenuItem();
        menuClearBreakpoints = new javax.swing.JMenuItem();
        jSeparator9 = new javax.swing.JPopupMenu.Separator();
        menuSimTools = new javax.swing.JMenu();
        menuSimView = new javax.swing.JCheckBoxMenuItem();
        menuSimWatcher = new javax.swing.JCheckBoxMenuItem();
        menuSimControl = new javax.swing.JCheckBoxMenuItem();
        jSeparator3 = new javax.swing.JPopupMenu.Separator();
        menuSimIO = new javax.swing.JCheckBoxMenuItem();
        menuSimAsmView = new javax.swing.JMenuItem();
        menuIOReg = new javax.swing.JMenu();
        menuSimLEDs = new javax.swing.JCheckBoxMenuItem();
        menuSimSwitches = new javax.swing.JCheckBoxMenuItem();
        menuSimSevenSegments = new javax.swing.JCheckBoxMenuItem();
        menuSimUART = new javax.swing.JCheckBoxMenuItem();
        menuSimVGA = new javax.swing.JCheckBoxMenuItem();
        menuSimPLPID = new javax.swing.JCheckBoxMenuItem();
        jSeparator10 = new javax.swing.JPopupMenu.Separator();
        menuExitSim = new javax.swing.JMenuItem();
        rootmenuHelp = new javax.swing.JMenu();
        menuQuickRef = new javax.swing.JMenuItem();
        menuManual = new javax.swing.JMenuItem();
        jSeparator5 = new javax.swing.JPopupMenu.Separator();
        menuIssues = new javax.swing.JMenuItem();
        menuIssuesPage = new javax.swing.JMenuItem();
        jSeparator6 = new javax.swing.JPopupMenu.Separator();
        menuAbout = new javax.swing.JMenuItem();

        setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE);
        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(Develop.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setIconImage(java.awt.Toolkit.getDefaultToolkit().getImage("gui/frames/resources/plp.png"));
        setName("Form"); // NOI18N
        addWindowFocusListener(new java.awt.event.WindowFocusListener() {
            public void windowGainedFocus(java.awt.event.WindowEvent evt) {
                formWindowGainedFocus(evt);
            }
            public void windowLostFocus(java.awt.event.WindowEvent evt) {
            }
        });

        devMainPane.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        devMainPane.setName("devMainPane"); // NOI18N

        splitterV.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
        splitterV.setOrientation(javax.swing.JSplitPane.VERTICAL_SPLIT);
        splitterV.setResizeWeight(0.7);
        splitterV.setName("splitterV"); // NOI18N

        splitterH.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
        splitterH.setName("splitterH"); // NOI18N

        jScrollPane2.setName("jScrollPane2"); // NOI18N

        treeProject.setName("treeProject"); // NOI18N
        treeProject.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                treeProjectMousePressed(evt);
            }
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                treeProjectMouseClicked(evt);
            }
        });
        jScrollPane2.setViewportView(treeProject);

        splitterH.setLeftComponent(jScrollPane2);

        jPanel1.setName("jPanel1"); // NOI18N

        txtCurFile.setFont(resourceMap.getFont("txtCurFile.font")); // NOI18N
        txtCurFile.setText(resourceMap.getString("txtCurFile.text")); // NOI18N
        txtCurFile.setName("txtCurFile"); // NOI18N

        scroller.setName("scroller"); // NOI18N

        txtEditor.setBorder(javax.swing.BorderFactory.createEmptyBorder(1, 1, 1, 1));
        txtEditor.setFont(resourceMap.getFont("txtEditor.font")); // NOI18N
        txtEditor.setEnabled(false);
        txtEditor.setName("txtEditor"); // NOI18N
        txtEditor.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                txtEditorMousePressed(evt);
            }
        });
        txtEditor.addCaretListener(new javax.swing.event.CaretListener() {
            public void caretUpdate(javax.swing.event.CaretEvent evt) {
                txtEditorCaretUpdate(evt);
            }
        });
        txtEditor.addInputMethodListener(new java.awt.event.InputMethodListener() {
            public void caretPositionChanged(java.awt.event.InputMethodEvent evt) {
                txtEditorCaretPositionChanged(evt);
            }
            public void inputMethodTextChanged(java.awt.event.InputMethodEvent evt) {
            }
        });
        txtEditor.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                txtEditorKeyTyped(evt);
            }
        });
        scroller.setViewportView(txtEditor);

        lblPosition.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        lblPosition.setText(resourceMap.getString("lblPosition.text")); // NOI18N
        lblPosition.setName("lblPosition"); // NOI18N

        lblSimStat.setText(resourceMap.getString("lblSimStat.text")); // NOI18N
        lblSimStat.setName("lblSimStat"); // NOI18N

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(scroller, javax.swing.GroupLayout.DEFAULT_SIZE, 830, Short.MAX_VALUE)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(txtCurFile)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 625, Short.MAX_VALUE)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(lblPosition, javax.swing.GroupLayout.PREFERRED_SIZE, 119, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(lblSimStat))
                .addContainerGap())
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(txtCurFile)
                    .addComponent(lblPosition)
                    .addComponent(lblSimStat))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(scroller, javax.swing.GroupLayout.DEFAULT_SIZE, 406, Short.MAX_VALUE))
        );

        splitterH.setRightComponent(jPanel1);

        splitterV.setTopComponent(splitterH);

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        txtOutput.setFont(resourceMap.getFont("txtOutput.font")); // NOI18N
        txtOutput.setName("txtOutput"); // NOI18N
        jScrollPane1.setViewportView(txtOutput);

        splitterV.setRightComponent(jScrollPane1);

        javax.swing.GroupLayout devMainPaneLayout = new javax.swing.GroupLayout(devMainPane);
        devMainPane.setLayout(devMainPaneLayout);
        devMainPaneLayout.setHorizontalGroup(
            devMainPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(splitterV, javax.swing.GroupLayout.DEFAULT_SIZE, 863, Short.MAX_VALUE)
        );
        devMainPaneLayout.setVerticalGroup(
            devMainPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(splitterV, javax.swing.GroupLayout.DEFAULT_SIZE, 473, Short.MAX_VALUE)
        );

        getContentPane().add(devMainPane, java.awt.BorderLayout.CENTER);

        toolbar.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        toolbar.setFloatable(false);
        toolbar.setRollover(true);
        toolbar.setName("toolbar"); // NOI18N

        btnNew.setIcon(resourceMap.getIcon("btnNew.icon")); // NOI18N
        btnNew.setText(resourceMap.getString("btnNew.text")); // NOI18N
        btnNew.setToolTipText(resourceMap.getString("btnNew.toolTipText")); // NOI18N
        btnNew.setFocusable(false);
        btnNew.setHorizontalTextPosition(javax.swing.SwingConstants.LEFT);
        btnNew.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnNew.setName("btnNew"); // NOI18N
        btnNew.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnNew.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnNewActionPerformed(evt);
            }
        });
        toolbar.add(btnNew);

        btnOpen.setIcon(resourceMap.getIcon("btnOpen.icon")); // NOI18N
        btnOpen.setText(resourceMap.getString("btnOpen.text")); // NOI18N
        btnOpen.setToolTipText(resourceMap.getString("btnOpen.toolTipText")); // NOI18N
        btnOpen.setFocusable(false);
        btnOpen.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnOpen.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnOpen.setName("btnOpen"); // NOI18N
        btnOpen.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnOpen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnOpenActionPerformed(evt);
            }
        });
        toolbar.add(btnOpen);

        btnSave.setIcon(resourceMap.getIcon("btnSave.icon")); // NOI18N
        btnSave.setText(resourceMap.getString("btnSave.text")); // NOI18N
        btnSave.setToolTipText(resourceMap.getString("btnSave.toolTipText")); // NOI18N
        btnSave.setFocusable(false);
        btnSave.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSave.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSave.setName("btnSave"); // NOI18N
        btnSave.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSaveActionPerformed(evt);
            }
        });
        toolbar.add(btnSave);

        jSeparator4.setName("jSeparator4"); // NOI18N
        toolbar.add(jSeparator4);

        btnAssemble.setIcon(resourceMap.getIcon("btnAssemble.icon")); // NOI18N
        btnAssemble.setText(resourceMap.getString("btnAssemble.text")); // NOI18N
        btnAssemble.setToolTipText(resourceMap.getString("btnAssemble.toolTipText")); // NOI18N
        btnAssemble.setFocusable(false);
        btnAssemble.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnAssemble.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnAssemble.setName("btnAssemble"); // NOI18N
        btnAssemble.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnAssemble.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnAssembleActionPerformed(evt);
            }
        });
        toolbar.add(btnAssemble);

        btnSimulate.setIcon(resourceMap.getIcon("btnSimulate.icon")); // NOI18N
        btnSimulate.setText(resourceMap.getString("btnSimulate.text")); // NOI18N
        btnSimulate.setToolTipText(resourceMap.getString("btnSimulate.toolTipText")); // NOI18N
        btnSimulate.setFocusable(false);
        btnSimulate.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimulate.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimulate.setName("btnSimulate"); // NOI18N
        btnSimulate.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimulate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimulateActionPerformed(evt);
            }
        });
        toolbar.add(btnSimulate);

        btnProgram.setIcon(resourceMap.getIcon("btnProgram.icon")); // NOI18N
        btnProgram.setText(resourceMap.getString("btnProgram.text")); // NOI18N
        btnProgram.setToolTipText(resourceMap.getString("btnProgram.toolTipText")); // NOI18N
        btnProgram.setFocusable(false);
        btnProgram.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnProgram.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnProgram.setName("btnProgram"); // NOI18N
        btnProgram.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnProgram.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnProgramActionPerformed(evt);
            }
        });
        toolbar.add(btnProgram);

        separatorSim.setName("separatorSim"); // NOI18N
        toolbar.add(separatorSim);

        btnSimStep.setIcon(resourceMap.getIcon("btnSimStep.icon")); // NOI18N
        btnSimStep.setText(resourceMap.getString("btnSimStep.text")); // NOI18N
        btnSimStep.setToolTipText(resourceMap.getString("btnSimStep.toolTipText")); // NOI18N
        btnSimStep.setFocusable(false);
        btnSimStep.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimStep.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimStep.setName("btnSimStep"); // NOI18N
        btnSimStep.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimStep.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimStepActionPerformed(evt);
            }
        });
        toolbar.add(btnSimStep);

        btnSimRun.setIcon(resourceMap.getIcon("btnSimRun.icon")); // NOI18N
        btnSimRun.setText(resourceMap.getString("btnSimRun.text")); // NOI18N
        btnSimRun.setToolTipText(resourceMap.getString("btnSimRun.toolTipText")); // NOI18N
        btnSimRun.setFocusable(false);
        btnSimRun.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimRun.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimRun.setName("btnSimRun"); // NOI18N
        btnSimRun.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimRun.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimRunActionPerformed(evt);
            }
        });
        toolbar.add(btnSimRun);

        btnSimReset.setIcon(resourceMap.getIcon("btnSimReset.icon")); // NOI18N
        btnSimReset.setText(resourceMap.getString("btnSimReset.text")); // NOI18N
        btnSimReset.setToolTipText(resourceMap.getString("btnSimReset.toolTipText")); // NOI18N
        btnSimReset.setFocusable(false);
        btnSimReset.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimReset.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimReset.setName("btnSimReset"); // NOI18N
        btnSimReset.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimReset.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimResetActionPerformed(evt);
            }
        });
        toolbar.add(btnSimReset);

        btnSimControl.setIcon(resourceMap.getIcon("btnSimControl.icon")); // NOI18N
        btnSimControl.setText(resourceMap.getString("btnSimControl.text")); // NOI18N
        btnSimControl.setToolTipText(resourceMap.getString("btnSimControl.toolTipText")); // NOI18N
        btnSimControl.setFocusable(false);
        btnSimControl.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimControl.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimControl.setName("btnSimControl"); // NOI18N
        btnSimControl.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimControl.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimControlActionPerformed(evt);
            }
        });
        toolbar.add(btnSimControl);

        separatorSimControl.setName("separatorSimControl"); // NOI18N
        toolbar.add(separatorSimControl);

        btnCPU.setIcon(resourceMap.getIcon("btnCPU.icon")); // NOI18N
        btnCPU.setText(resourceMap.getString("btnCPU.text")); // NOI18N
        btnCPU.setToolTipText(resourceMap.getString("btnCPU.toolTipText")); // NOI18N
        btnCPU.setFocusable(false);
        btnCPU.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnCPU.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnCPU.setName("btnCPU"); // NOI18N
        btnCPU.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnCPU.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCPUActionPerformed(evt);
            }
        });
        toolbar.add(btnCPU);

        btnWatcher.setIcon(resourceMap.getIcon("btnWatcher.icon")); // NOI18N
        btnWatcher.setText(resourceMap.getString("btnWatcher.text")); // NOI18N
        btnWatcher.setToolTipText(resourceMap.getString("btnWatcher.toolTipText")); // NOI18N
        btnWatcher.setFocusable(false);
        btnWatcher.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnWatcher.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnWatcher.setName("btnWatcher"); // NOI18N
        btnWatcher.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnWatcher.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnWatcherActionPerformed(evt);
            }
        });
        toolbar.add(btnWatcher);

        btnSimLEDs.setIcon(resourceMap.getIcon("btnSimLEDs.icon")); // NOI18N
        btnSimLEDs.setText(resourceMap.getString("btnSimLEDs.text")); // NOI18N
        btnSimLEDs.setToolTipText(resourceMap.getString("btnSimLEDs.toolTipText")); // NOI18N
        btnSimLEDs.setFocusable(false);
        btnSimLEDs.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimLEDs.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimLEDs.setName("btnSimLEDs"); // NOI18N
        btnSimLEDs.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimLEDs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimLEDsActionPerformed(evt);
            }
        });
        toolbar.add(btnSimLEDs);

        btnSimSwitches.setIcon(resourceMap.getIcon("btnSimSwitches.icon")); // NOI18N
        btnSimSwitches.setText(resourceMap.getString("btnSimSwitches.text")); // NOI18N
        btnSimSwitches.setToolTipText(resourceMap.getString("btnSimSwitches.toolTipText")); // NOI18N
        btnSimSwitches.setFocusable(false);
        btnSimSwitches.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimSwitches.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimSwitches.setName("btnSimSwitches"); // NOI18N
        btnSimSwitches.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimSwitches.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimSwitchesActionPerformed(evt);
            }
        });
        toolbar.add(btnSimSwitches);

        btnSimSevenSegments.setIcon(resourceMap.getIcon("btnSimSevenSegments.icon")); // NOI18N
        btnSimSevenSegments.setText(resourceMap.getString("btnSimSevenSegments.text")); // NOI18N
        btnSimSevenSegments.setToolTipText(resourceMap.getString("btnSimSevenSegments.toolTipText")); // NOI18N
        btnSimSevenSegments.setFocusable(false);
        btnSimSevenSegments.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimSevenSegments.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimSevenSegments.setName("btnSimSevenSegments"); // NOI18N
        btnSimSevenSegments.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimSevenSegments.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimSevenSegmentsActionPerformed(evt);
            }
        });
        toolbar.add(btnSimSevenSegments);

        btnSimUART.setIcon(resourceMap.getIcon("btnSimUART.icon")); // NOI18N
        btnSimUART.setText(resourceMap.getString("btnSimUART.text")); // NOI18N
        btnSimUART.setToolTipText(resourceMap.getString("btnSimUART.toolTipText")); // NOI18N
        btnSimUART.setFocusable(false);
        btnSimUART.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimUART.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimUART.setName("btnSimUART"); // NOI18N
        btnSimUART.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimUART.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimUARTActionPerformed(evt);
            }
        });
        toolbar.add(btnSimUART);

        btnSimVGA.setIcon(resourceMap.getIcon("btnSimVGA.icon")); // NOI18N
        btnSimVGA.setText(resourceMap.getString("btnSimVGA.text")); // NOI18N
        btnSimVGA.setToolTipText(resourceMap.getString("btnSimVGA.toolTipText")); // NOI18N
        btnSimVGA.setFocusable(false);
        btnSimVGA.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimVGA.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimVGA.setName("btnSimVGA"); // NOI18N
        btnSimVGA.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimVGA.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimVGAActionPerformed(evt);
            }
        });
        toolbar.add(btnSimVGA);

        btnSimPLPID.setIcon(resourceMap.getIcon("btnSimPLPID.icon")); // NOI18N
        btnSimPLPID.setText(resourceMap.getString("btnSimPLPID.text")); // NOI18N
        btnSimPLPID.setToolTipText(resourceMap.getString("btnSimPLPID.toolTipText")); // NOI18N
        btnSimPLPID.setFocusable(false);
        btnSimPLPID.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
        btnSimPLPID.setMargin(new java.awt.Insets(2, 0, 2, 0));
        btnSimPLPID.setName("btnSimPLPID"); // NOI18N
        btnSimPLPID.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
        btnSimPLPID.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSimPLPIDActionPerformed(evt);
            }
        });
        toolbar.add(btnSimPLPID);

        getContentPane().add(toolbar, java.awt.BorderLayout.PAGE_START);

        jMenuBar1.setName("jMenuBar1"); // NOI18N

        rootmenuFile.setMnemonic('F');
        rootmenuFile.setText(resourceMap.getString("rootmenuFile.text")); // NOI18N

        menuNew.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_N, java.awt.event.InputEvent.CTRL_MASK));
        menuNew.setIcon(resourceMap.getIcon("menuNew.icon")); // NOI18N
        menuNew.setMnemonic('N');
        menuNew.setText(resourceMap.getString("menuNew.text")); // NOI18N
        menuNew.setName("menuNew"); // NOI18N
        menuNew.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuNewActionPerformed(evt);
            }
        });
        rootmenuFile.add(menuNew);

        menuSeparator1.setName("menuSeparator1"); // NOI18N
        rootmenuFile.add(menuSeparator1);

        menuOpen.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_O, java.awt.event.InputEvent.CTRL_MASK));
        menuOpen.setIcon(resourceMap.getIcon("menuOpen.icon")); // NOI18N
        menuOpen.setMnemonic('O');
        menuOpen.setText(resourceMap.getString("menuOpen.text")); // NOI18N
        menuOpen.setName("menuOpen"); // NOI18N
        menuOpen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuOpenActionPerformed(evt);
            }
        });
        rootmenuFile.add(menuOpen);

        menuSave.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S, java.awt.event.InputEvent.CTRL_MASK));
        menuSave.setIcon(resourceMap.getIcon("menuSave.icon")); // NOI18N
        menuSave.setMnemonic('S');
        menuSave.setText(resourceMap.getString("menuSave.text")); // NOI18N
        menuSave.setName("menuSave"); // NOI18N
        menuSave.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSaveActionPerformed(evt);
            }
        });
        rootmenuFile.add(menuSave);

        menuSaveAs.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_A, java.awt.event.InputEvent.SHIFT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        menuSaveAs.setMnemonic('A');
        menuSaveAs.setText(resourceMap.getString("menuSaveAs.text")); // NOI18N
        menuSaveAs.setName("menuSaveAs"); // NOI18N
        menuSaveAs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSaveAsActionPerformed(evt);
            }
        });
        rootmenuFile.add(menuSaveAs);

        jSeparator8.setName("jSeparator8"); // NOI18N
        rootmenuFile.add(jSeparator8);

        menuPrint.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_P, java.awt.event.InputEvent.CTRL_MASK));
        menuPrint.setMnemonic('P');
        menuPrint.setText(resourceMap.getString("menuPrint.text")); // NOI18N
        menuPrint.setName("menuPrint"); // NOI18N
        menuPrint.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuPrintActionPerformed(evt);
            }
        });
        rootmenuFile.add(menuPrint);

        menuSeparator3.setName("menuSeparator3"); // NOI18N
        rootmenuFile.add(menuSeparator3);

        menuExit.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Q, java.awt.event.InputEvent.CTRL_MASK));
        menuExit.setMnemonic('X');
        menuExit.setText(resourceMap.getString("menuExit.text")); // NOI18N
        menuExit.setName("menuExit"); // NOI18N
        menuExit.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuExitActionPerformed(evt);
            }
        });
        rootmenuFile.add(menuExit);

        jMenuBar1.add(rootmenuFile);

        rootmenuEdit.setMnemonic('E');
        rootmenuEdit.setText(resourceMap.getString("rootmenuEdit.text")); // NOI18N

        menuCopy.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_C, java.awt.event.InputEvent.CTRL_MASK));
        menuCopy.setMnemonic('C');
        menuCopy.setText(resourceMap.getString("menuCopy.text")); // NOI18N
        menuCopy.setName("menuCopy"); // NOI18N
        menuCopy.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuCopyActionPerformed(evt);
            }
        });
        rootmenuEdit.add(menuCopy);

        menuCut.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_X, java.awt.event.InputEvent.CTRL_MASK));
        menuCut.setMnemonic('t');
        menuCut.setText(resourceMap.getString("menuCut.text")); // NOI18N
        menuCut.setName("menuCut"); // NOI18N
        menuCut.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuCutActionPerformed(evt);
            }
        });
        rootmenuEdit.add(menuCut);

        menuPaste.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_V, java.awt.event.InputEvent.CTRL_MASK));
        menuPaste.setMnemonic('P');
        menuPaste.setText(resourceMap.getString("menuPaste.text")); // NOI18N
        menuPaste.setName("menuPaste"); // NOI18N
        menuPaste.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuPasteActionPerformed(evt);
            }
        });
        rootmenuEdit.add(menuPaste);

        menuSeparator4.setName("menuSeparator4"); // NOI18N
        rootmenuEdit.add(menuSeparator4);

        menuFindAndReplace.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F, java.awt.event.InputEvent.CTRL_MASK));
        menuFindAndReplace.setMnemonic('F');
        menuFindAndReplace.setText(resourceMap.getString("menuFindAndReplace.text")); // NOI18N
        menuFindAndReplace.setName("menuFindAndReplace"); // NOI18N
        menuFindAndReplace.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuFindAndReplaceActionPerformed(evt);
            }
        });
        rootmenuEdit.add(menuFindAndReplace);

        menuSeparator5.setName("menuSeparator5"); // NOI18N
        rootmenuEdit.add(menuSeparator5);

        menuUndo.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Z, java.awt.event.InputEvent.CTRL_MASK));
        menuUndo.setMnemonic('U');
        menuUndo.setText(resourceMap.getString("menuUndo.text")); // NOI18N
        menuUndo.setName("menuUndo"); // NOI18N
        menuUndo.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuUndoActionPerformed(evt);
            }
        });
        rootmenuEdit.add(menuUndo);

        menuRedo.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Y, java.awt.event.InputEvent.CTRL_MASK));
        menuRedo.setMnemonic('R');
        menuRedo.setText(resourceMap.getString("menuRedo.text")); // NOI18N
        menuRedo.setName("menuRedo"); // NOI18N
        menuRedo.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuRedoActionPerformed(evt);
            }
        });
        rootmenuEdit.add(menuRedo);

        jMenuBar1.add(rootmenuEdit);

        rootmenuView.setMnemonic('V');
        rootmenuView.setText(resourceMap.getString("rootmenuView.text")); // NOI18N
        rootmenuView.setName("rootmenuView"); // NOI18N

        menuToolbar.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_T, java.awt.event.InputEvent.ALT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        menuToolbar.setMnemonic('T');
        menuToolbar.setSelected(true);
        menuToolbar.setText(resourceMap.getString("menuToolbar.text")); // NOI18N
        menuToolbar.setName("menuToolbar"); // NOI18N
        menuToolbar.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuToolbarActionPerformed(evt);
            }
        });
        rootmenuView.add(menuToolbar);

        menuProjectPane.setMnemonic('R');
        menuProjectPane.setSelected(true);
        menuProjectPane.setText(resourceMap.getString("menuProjectPane.text")); // NOI18N
        menuProjectPane.setName("menuProjectPane"); // NOI18N
        menuProjectPane.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuProjectPaneActionPerformed(evt);
            }
        });
        rootmenuView.add(menuProjectPane);

        menuOutputPane.setMnemonic('O');
        menuOutputPane.setSelected(true);
        menuOutputPane.setText(resourceMap.getString("menuOutputPane.text")); // NOI18N
        menuOutputPane.setName("menuOutputPane"); // NOI18N
        menuOutputPane.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuOutputPaneActionPerformed(evt);
            }
        });
        rootmenuView.add(menuOutputPane);

        menuClearOutputPane.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_D, java.awt.event.InputEvent.CTRL_MASK));
        menuClearOutputPane.setMnemonic('C');
        menuClearOutputPane.setText(resourceMap.getString("menuClearOutputPane.text")); // NOI18N
        menuClearOutputPane.setName("menuClearOutputPane"); // NOI18N
        menuClearOutputPane.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuClearOutputPaneActionPerformed(evt);
            }
        });
        rootmenuView.add(menuClearOutputPane);

        jMenuBar1.add(rootmenuView);

        rootmenuProject.setMnemonic('R');
        rootmenuProject.setText(resourceMap.getString("rootmenuProject.text")); // NOI18N
        rootmenuProject.setName("rootmenuProject"); // NOI18N
        rootmenuProject.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                rootmenuProjectMouseClicked(evt);
            }
        });
        rootmenuProject.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                rootmenuProjectActionPerformed(evt);
            }
        });

        menuAssemble.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F2, 0));
        menuAssemble.setIcon(resourceMap.getIcon("menuAssemble.icon")); // NOI18N
        menuAssemble.setMnemonic('A');
        menuAssemble.setText(resourceMap.getString("menuAssemble.text")); // NOI18N
        menuAssemble.setName("menuAssemble"); // NOI18N
        menuAssemble.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuAssembleActionPerformed1(evt);
            }
        });
        rootmenuProject.add(menuAssemble);

        menuSimulate.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F3, 0));
        menuSimulate.setText(resourceMap.getString("menuSimulate.text")); // NOI18N
        menuSimulate.setIcon(resourceMap.getIcon("menuSimulate.icon")); // NOI18N
        menuSimulate.setName("menuSimulate"); // NOI18N
        menuSimulate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimulateActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuSimulate);

        menuProgram.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F4, java.awt.event.InputEvent.SHIFT_MASK));
        menuProgram.setIcon(resourceMap.getIcon("menuProgram.icon")); // NOI18N
        menuProgram.setMnemonic('P');
        menuProgram.setText(resourceMap.getString("menuProgram.text")); // NOI18N
        menuProgram.setName("menuProgram"); // NOI18N
        menuProgram.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuProgramActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuProgram);

        menuQuickProgram.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F4, 0));
        menuQuickProgram.setMnemonic('Q');
        menuQuickProgram.setText(resourceMap.getString("menuQuickProgram.text")); // NOI18N
        menuQuickProgram.setName("menuQuickProgram"); // NOI18N
        menuQuickProgram.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuQuickProgramActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuQuickProgram);

        jSeparator1.setName("jSeparator1"); // NOI18N
        rootmenuProject.add(jSeparator1);

        menuNewASM.setMnemonic('N');
        menuNewASM.setText(resourceMap.getString("menuNewASM.text")); // NOI18N
        menuNewASM.setName("menuNewASM"); // NOI18N
        menuNewASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuNewASMActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuNewASM);

        menuImportASM.setMnemonic('I');
        menuImportASM.setText(resourceMap.getString("menuImportASM.text")); // NOI18N
        menuImportASM.setName("menuImportASM"); // NOI18N
        menuImportASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuImportASMActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuImportASM);

        menuExportASM.setMnemonic('X');
        menuExportASM.setText(resourceMap.getString("menuExportASM.text")); // NOI18N
        menuExportASM.setName("menuExportASM"); // NOI18N
        menuExportASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuExportASMActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuExportASM);

        menuDeleteASM.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_E, java.awt.event.InputEvent.CTRL_MASK));
        menuDeleteASM.setMnemonic('R');
        menuDeleteASM.setText(resourceMap.getString("menuDeleteASM.text")); // NOI18N
        menuDeleteASM.setName("menuDeleteASM"); // NOI18N
        menuDeleteASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuDeleteASMActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuDeleteASM);

        jSeparator2.setName("jSeparator2"); // NOI18N
        rootmenuProject.add(jSeparator2);

        menuSetMainProgram.setMnemonic('C');
        menuSetMainProgram.setText(resourceMap.getString("menuSetMainProgram.text")); // NOI18N
        menuSetMainProgram.setName("menuSetMainProgram"); // NOI18N
        menuSetMainProgram.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSetMainProgramActionPerformed(evt);
            }
        });
        rootmenuProject.add(menuSetMainProgram);

        jMenuBar1.add(rootmenuProject);

        rootmenuTools.setMnemonic('T');
        rootmenuTools.setText(resourceMap.getString("rootmenuTools.text")); // NOI18N
        rootmenuTools.setName("rootmenuTools"); // NOI18N

        menuOptions.setMnemonic('O');
        menuOptions.setText(resourceMap.getString("menuOptions.text")); // NOI18N
        menuOptions.setName("menuOptions"); // NOI18N
        menuOptions.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuOptionsActionPerformed(evt);
            }
        });
        rootmenuTools.add(menuOptions);

        jSeparator7.setName("jSeparator7"); // NOI18N
        rootmenuTools.add(jSeparator7);

        menuSerialTerminal.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_T, java.awt.event.InputEvent.CTRL_MASK));
        menuSerialTerminal.setMnemonic('S');
        menuSerialTerminal.setText(resourceMap.getString("menuSerialTerminal.text")); // NOI18N
        menuSerialTerminal.setName("menuSerialTerminal"); // NOI18N
        menuSerialTerminal.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSerialTerminalActionPerformed(evt);
            }
        });
        rootmenuTools.add(menuSerialTerminal);

        menuNumberConverter.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F12, 0));
        menuNumberConverter.setMnemonic('C');
        menuNumberConverter.setText(resourceMap.getString("menuNumberConverter.text")); // NOI18N
        menuNumberConverter.setName("menuNumberConverter"); // NOI18N
        menuNumberConverter.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuNumberConverterActionPerformed(evt);
            }
        });
        rootmenuTools.add(menuNumberConverter);

        jMenuBar1.add(rootmenuTools);

        rootmenuSim.setMnemonic('S');
        rootmenuSim.setText(resourceMap.getString("rootmenuSim.text")); // NOI18N
        rootmenuSim.setName("rootmenuSim"); // NOI18N

        menuSimStep.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F5, 0));
        menuSimStep.setIcon(resourceMap.getIcon("menuSimStep.icon")); // NOI18N
        menuSimStep.setMnemonic('S');
        menuSimStep.setText(resourceMap.getString("menuSimStep.text")); // NOI18N
        menuSimStep.setName("menuSimStep"); // NOI18N
        menuSimStep.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimStepActionPerformed(evt);
            }
        });
        rootmenuSim.add(menuSimStep);

        menuSimReset.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F9, 0));
        menuSimReset.setIcon(resourceMap.getIcon("menuSimReset.icon")); // NOI18N
        menuSimReset.setMnemonic('E');
        menuSimReset.setText(resourceMap.getString("menuSimReset.text")); // NOI18N
        menuSimReset.setName("menuSimReset"); // NOI18N
        menuSimReset.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimResetActionPerformed(evt);
            }
        });
        rootmenuSim.add(menuSimReset);

        menuSimRun.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F7, 0));
        menuSimRun.setMnemonic('R');
        menuSimRun.setText(resourceMap.getString("menuSimRun.text")); // NOI18N
        menuSimRun.setName("menuSimRun"); // NOI18N
        menuSimRun.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimRunActionPerformed(evt);
            }
        });
        rootmenuSim.add(menuSimRun);

        menuStepSize.setMnemonic('C');
        menuStepSize.setText(resourceMap.getString("menuStepSize.text")); // NOI18N

        menuStep1.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_1, java.awt.event.InputEvent.ALT_MASK));
        grpSteps.add(menuStep1);
        menuStep1.setMnemonic('1');
        menuStep1.setSelected(true);
        menuStep1.setText(resourceMap.getString("menuStep1.text")); // NOI18N
        menuStep1.setName("menuStep1"); // NOI18N
        menuStep1.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuStep1ActionPerformed(evt);
            }
        });
        menuStepSize.add(menuStep1);

        menuStep2.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_2, java.awt.event.InputEvent.ALT_MASK));
        grpSteps.add(menuStep2);
        menuStep2.setMnemonic('5');
        menuStep2.setText(resourceMap.getString("menuStep2.text")); // NOI18N
        menuStep2.setName("menuStep2"); // NOI18N
        menuStep2.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuStep2ActionPerformed(evt);
            }
        });
        menuStepSize.add(menuStep2);

        menuStep3.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_3, java.awt.event.InputEvent.ALT_MASK));
        grpSteps.add(menuStep3);
        menuStep3.setMnemonic('2');
        menuStep3.setText(resourceMap.getString("menuStep3.text")); // NOI18N
        menuStep3.setName("menuStep3"); // NOI18N
        menuStep3.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuStep3ActionPerformed(evt);
            }
        });
        menuStepSize.add(menuStep3);

        menuStep4.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_4, java.awt.event.InputEvent.ALT_MASK));
        grpSteps.add(menuStep4);
        menuStep4.setMnemonic('0');
        menuStep4.setText(resourceMap.getString("menuStep4.text")); // NOI18N
        menuStep4.setName("menuStep4"); // NOI18N
        menuStep4.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuStep4ActionPerformed(evt);
            }
        });
        menuStepSize.add(menuStep4);

        menuStep5.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_5, java.awt.event.InputEvent.ALT_MASK));
        grpSteps.add(menuStep5);
        menuStep5.setMnemonic('5');
        menuStep5.setText(resourceMap.getString("menuStep5.text")); // NOI18N
        menuStep5.setName("menuStep5"); // NOI18N
        menuStep5.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuStep5ActionPerformed(evt);
            }
        });
        menuStepSize.add(menuStep5);

        rootmenuSim.add(menuStepSize);

        menuClearBreakpoints.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_B, java.awt.event.InputEvent.CTRL_MASK));
        menuClearBreakpoints.setMnemonic('B');
        menuClearBreakpoints.setText(resourceMap.getString("menuClearBreakpoints.text")); // NOI18N
        menuClearBreakpoints.setName("menuClearBreakpoints"); // NOI18N
        menuClearBreakpoints.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuClearBreakpointsActionPerformed(evt);
            }
        });
        rootmenuSim.add(menuClearBreakpoints);

        jSeparator9.setName("jSeparator9"); // NOI18N
        rootmenuSim.add(jSeparator9);

        menuSimTools.setMnemonic('T');
        menuSimTools.setText(resourceMap.getString("menuSimTools.text")); // NOI18N
        menuSimTools.setName("menuSimTools"); // NOI18N

        menuSimView.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_C, java.awt.event.InputEvent.SHIFT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        menuSimView.setMnemonic('C');
        menuSimView.setText(resourceMap.getString("menuSimView.text")); // NOI18N
        menuSimView.setIcon(resourceMap.getIcon("menuSimView.icon")); // NOI18N
        menuSimView.setName("menuSimView"); // NOI18N
        menuSimView.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimViewActionPerformed(evt);
            }
        });
        menuSimTools.add(menuSimView);

        menuSimWatcher.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_W, java.awt.event.InputEvent.SHIFT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        menuSimWatcher.setMnemonic('W');
        menuSimWatcher.setText(resourceMap.getString("menuSimWatcher.text")); // NOI18N
        menuSimWatcher.setIcon(resourceMap.getIcon("menuSimWatcher.icon")); // NOI18N
        menuSimWatcher.setName("menuSimWatcher"); // NOI18N
        menuSimWatcher.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimWatcherActionPerformed(evt);
            }
        });
        menuSimTools.add(menuSimWatcher);

        menuSimControl.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_R, java.awt.event.InputEvent.CTRL_MASK));
        menuSimControl.setText(resourceMap.getString("menuSimControl.text")); // NOI18N
        menuSimControl.setIcon(resourceMap.getIcon("menuSimControl.icon")); // NOI18N
        menuSimControl.setName("menuSimControl"); // NOI18N
        menuSimControl.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimControlActionPerformed(evt);
            }
        });
        menuSimTools.add(menuSimControl);

        jSeparator3.setName("jSeparator3"); // NOI18N
        menuSimTools.add(jSeparator3);

        menuSimIO.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_R, java.awt.event.InputEvent.SHIFT_MASK | java.awt.event.InputEvent.CTRL_MASK));
        menuSimIO.setMnemonic('R');
        menuSimIO.setText(resourceMap.getString("menuSimIO.text")); // NOI18N
        menuSimIO.setName("menuSimIO"); // NOI18N
        menuSimIO.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimIOActionPerformed(evt);
            }
        });
        menuSimTools.add(menuSimIO);

        menuSimAsmView.setText(resourceMap.getString("menuSimAsmView.text")); // NOI18N
        menuSimAsmView.setName("menuSimAsmView"); // NOI18N
        menuSimAsmView.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimAsmViewActionPerformed(evt);
            }
        });
        menuSimTools.add(menuSimAsmView);

        rootmenuSim.add(menuSimTools);

        menuIOReg.setMnemonic('D');
        menuIOReg.setText(resourceMap.getString("menuIOReg.text")); // NOI18N
        menuIOReg.setName("menuIOReg"); // NOI18N

        menuSimLEDs.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_1, java.awt.event.InputEvent.CTRL_MASK));
        menuSimLEDs.setMnemonic('1');
        menuSimLEDs.setText(resourceMap.getString("menuSimLEDs.text")); // NOI18N
        menuSimLEDs.setIcon(resourceMap.getIcon("menuSimLEDs.icon")); // NOI18N
        menuSimLEDs.setName("menuSimLEDs"); // NOI18N
        menuSimLEDs.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimLEDsActionPerformed(evt);
            }
        });
        menuIOReg.add(menuSimLEDs);

        menuSimSwitches.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_2, java.awt.event.InputEvent.CTRL_MASK));
        menuSimSwitches.setMnemonic('2');
        menuSimSwitches.setText(resourceMap.getString("menuSimSwitches.text")); // NOI18N
        menuSimSwitches.setIcon(resourceMap.getIcon("menuSimSwitches.icon")); // NOI18N
        menuSimSwitches.setName("menuSimSwitches"); // NOI18N
        menuSimSwitches.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimSwitchesActionPerformed(evt);
            }
        });
        menuIOReg.add(menuSimSwitches);

        menuSimSevenSegments.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_3, java.awt.event.InputEvent.CTRL_MASK));
        menuSimSevenSegments.setMnemonic('3');
        menuSimSevenSegments.setText(resourceMap.getString("menuSimSevenSegments.text")); // NOI18N
        menuSimSevenSegments.setIcon(resourceMap.getIcon("menuSimSevenSegments.icon")); // NOI18N
        menuSimSevenSegments.setName("menuSimSevenSegments"); // NOI18N
        menuSimSevenSegments.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimSevenSegmentsActionPerformed(evt);
            }
        });
        menuIOReg.add(menuSimSevenSegments);

        menuSimUART.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_4, java.awt.event.InputEvent.CTRL_MASK));
        menuSimUART.setMnemonic('4');
        menuSimUART.setText(resourceMap.getString("menuSimUART.text")); // NOI18N
        menuSimUART.setIcon(resourceMap.getIcon("menuSimUART.icon")); // NOI18N
        menuSimUART.setName("menuSimUART"); // NOI18N
        menuSimUART.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimUARTActionPerformed(evt);
            }
        });
        menuIOReg.add(menuSimUART);

        menuSimVGA.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_5, java.awt.event.InputEvent.CTRL_MASK));
        menuSimVGA.setMnemonic('5');
        menuSimVGA.setText(resourceMap.getString("menuSimVGA.text")); // NOI18N
        menuSimVGA.setIcon(resourceMap.getIcon("menuSimVGA.icon")); // NOI18N
        menuSimVGA.setName("menuSimVGA"); // NOI18N
        menuSimVGA.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimVGAActionPerformed(evt);
            }
        });
        menuIOReg.add(menuSimVGA);

        menuSimPLPID.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_6, java.awt.event.InputEvent.CTRL_MASK));
        menuSimPLPID.setMnemonic('6');
        menuSimPLPID.setText(resourceMap.getString("menuSimPLPID.text")); // NOI18N
        menuSimPLPID.setIcon(resourceMap.getIcon("menuSimPLPID.icon")); // NOI18N
        menuSimPLPID.setName("menuSimPLPID"); // NOI18N
        menuSimPLPID.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuSimPLPIDActionPerformed(evt);
            }
        });
        menuIOReg.add(menuSimPLPID);

        rootmenuSim.add(menuIOReg);

        jSeparator10.setName("jSeparator10"); // NOI18N
        rootmenuSim.add(jSeparator10);

        menuExitSim.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F11, 0));
        menuExitSim.setText(resourceMap.getString("menuExitSim.text")); // NOI18N
        menuExitSim.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuExitSimActionPerformed(evt);
            }
        });
        rootmenuSim.add(menuExitSim);

        jMenuBar1.add(rootmenuSim);

        rootmenuHelp.setMnemonic('H');
        rootmenuHelp.setText(resourceMap.getString("rootmenuHelp.text")); // NOI18N
        rootmenuHelp.setName("rootmenuHelp"); // NOI18N
        rootmenuHelp.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                rootmenuHelpActionPerformed(evt);
            }
        });

        menuQuickRef.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F1, 0));
        menuQuickRef.setMnemonic('Q');
        menuQuickRef.setText(resourceMap.getString("menuQuickRef.text")); // NOI18N
        menuQuickRef.setName("menuQuickRef"); // NOI18N
        menuQuickRef.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuQuickRefActionPerformed(evt);
            }
        });
        rootmenuHelp.add(menuQuickRef);

        menuManual.setMnemonic('M');
        menuManual.setText(resourceMap.getString("menuManual.text")); // NOI18N
        menuManual.setName("menuManual"); // NOI18N
        menuManual.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuManualActionPerformed(evt);
            }
        });
        rootmenuHelp.add(menuManual);

        jSeparator5.setName("jSeparator5"); // NOI18N
        rootmenuHelp.add(jSeparator5);

        menuIssues.setMnemonic('R');
        menuIssues.setText(resourceMap.getString("menuIssues.text")); // NOI18N
        menuIssues.setName("menuIssues"); // NOI18N
        menuIssues.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuIssuesActionPerformed(evt);
            }
        });
        rootmenuHelp.add(menuIssues);

        menuIssuesPage.setMnemonic('I');
        menuIssuesPage.setText(resourceMap.getString("menuIssuesPage.text")); // NOI18N
        menuIssuesPage.setName("menuIssuesPage"); // NOI18N
        menuIssuesPage.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuIssuesPageActionPerformed(evt);
            }
        });
        rootmenuHelp.add(menuIssuesPage);

        jSeparator6.setName("jSeparator6"); // NOI18N
        rootmenuHelp.add(jSeparator6);

        menuAbout.setIcon(resourceMap.getIcon("menuAbout.icon")); // NOI18N
        menuAbout.setMnemonic('A');
        menuAbout.setText(resourceMap.getString("menuAbout.text")); // NOI18N
        menuAbout.setName("menuAbout"); // NOI18N
        menuAbout.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuAboutActionPerformed(evt);
            }
        });
        rootmenuHelp.add(menuAbout);

        jMenuBar1.add(rootmenuHelp);

        setJMenuBar(jMenuBar1);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void menuExitActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuExitActionPerformed
        exit();
    }//GEN-LAST:event_menuExitActionPerformed

    private void rootmenuHelpActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_rootmenuHelpActionPerformed

    }//GEN-LAST:event_rootmenuHelpActionPerformed

    private void menuOpenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuOpenActionPerformed
        openPLPFile();
    }//GEN-LAST:event_menuOpenActionPerformed

    private void menuNewActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuNewActionPerformed
        newPLPFile();
        if(Config.devSyntaxHighlighting)
            syntaxHighlight();
    }//GEN-LAST:event_menuNewActionPerformed

    private void menuAboutActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuAboutActionPerformed
        about();
    }//GEN-LAST:event_menuAboutActionPerformed

    private void menuSaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSaveActionPerformed
        if(plp.save() == Constants.PLP_FILE_USE_SAVE_AS)
            savePLPFileAs();
    }//GEN-LAST:event_menuSaveActionPerformed

    private void menuAssembleActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuAssembleActionPerformed
        Msg.output = txtOutput;

        if(plp.plpfile != null)
            plp.assemble();

        Config.nothighlighting = false;
        syntaxHighlight();
        Config.nothighlighting = true;
}//GEN-LAST:event_menuAssembleActionPerformed

    private void menuAssembleActionPerformed1(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuAssembleActionPerformed1
        assemble();
    }//GEN-LAST:event_menuAssembleActionPerformed1

    private void menuSaveAsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSaveAsActionPerformed
        savePLPFileAs();
    }//GEN-LAST:event_menuSaveAsActionPerformed

    private void menuImportASMActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuImportASMActionPerformed
        importASM();
    }//GEN-LAST:event_menuImportASMActionPerformed

    private void menuDeleteASMActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuDeleteASMActionPerformed
        deleteASM();
    }//GEN-LAST:event_menuDeleteASMActionPerformed

    private void rootmenuProjectActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_rootmenuProjectActionPerformed
    }//GEN-LAST:event_rootmenuProjectActionPerformed

    private void menuSetMainProgramActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSetMainProgramActionPerformed
        if(plp.open_asm != 0)
            plp.setMainAsm(plp.open_asm);
    }//GEN-LAST:event_menuSetMainProgramActionPerformed

    private void btnAssembleActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnAssembleActionPerformed
        assemble();
    }//GEN-LAST:event_btnAssembleActionPerformed

    private void menuExportASMActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuExportASMActionPerformed
        exportASM();
    }//GEN-LAST:event_menuExportASMActionPerformed

    private void formWindowGainedFocus(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowGainedFocus
        Msg.output = txtOutput;
    }//GEN-LAST:event_formWindowGainedFocus

    private void menuProgramActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuProgramActionPerformed
        plp.g_prg.setVisible(true);
    }//GEN-LAST:event_menuProgramActionPerformed

    private void menuNewASMActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuNewASMActionPerformed
        plp.g_fname.setMode(false);
        plp.g_fname.setVisible(true);
    }//GEN-LAST:event_menuNewASMActionPerformed

    private void menuCopyActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuCopyActionPerformed
        txtEditor.copy();
    }//GEN-LAST:event_menuCopyActionPerformed

    private void menuCutActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuCutActionPerformed
        //undoManager.modify("delete", "", txtEditor.getSelectionStart(), txtEditor.getSelectionEnd(), txtEditor.getSelectionStart(), txtEditor.getText().length());
        txtEditor.cut();
    }//GEN-LAST:event_menuCutActionPerformed

    private void menuPasteActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuPasteActionPerformed
        txtEditor.paste();
        Config.devSyntaxHighlightOnAssemble = true;
    }//GEN-LAST:event_menuPasteActionPerformed

    private void menuUndoActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuUndoActionPerformed
        undo();
    }//GEN-LAST:event_menuUndoActionPerformed

    private void menuRedoActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuRedoActionPerformed
        redo();
    }//GEN-LAST:event_menuRedoActionPerformed

    private void txtEditorCaretPositionChanged(java.awt.event.InputMethodEvent evt) {//GEN-FIRST:event_txtEditorCaretPositionChanged
        
    }//GEN-LAST:event_txtEditorCaretPositionChanged

    private void treeProjectMousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_treeProjectMousePressed
        if(evt.getClickCount() == 2) { // user double clicked the project tree
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) treeProject.getLastSelectedPathComponent();

            if(node == null)
                return;

            if(node.isLeaf()) {
                String nodeStr = (String) node.getUserObject();

                if(nodeStr.endsWith("asm")) {

                    String[] tokens = nodeStr.split(": ");

                    Msg.I("Opening " + nodeStr, null);

                    plp.updateAsm(plp.open_asm, txtEditor.getText());
                    plp.open_asm = Integer.parseInt(tokens[0]);
                    //plp.refreshProjectView(false);
                    safeRefresh(false);
                    repaintNow();
                    if (Config.devSyntaxHighlighting) {
                        SwingUtilities.invokeLater(new Runnable() {
                            @Override
                            public void run() {
                                syntaxHighlight();
                            }
                        });
                    }
                }
            }
        } else if(plp.plpfile != null && evt.getButton() == java.awt.event.MouseEvent.BUTTON3) {
            popupProject.show(treeProject, evt.getX(), evt.getY());
        }
    }//GEN-LAST:event_treeProjectMousePressed

    private void txtEditorCaretUpdate(javax.swing.event.CaretEvent evt) {//GEN-FIRST:event_txtEditorCaretUpdate
        if(plp.asms != null && plp.asms.size() > 0) {
            int caretPos = txtEditor.getCaretPosition();
            Element root = txtEditor.getDocument().getDefaultRootElement();
            line = root.getElementIndex(caretPos)+1;

            String fName = plp.asms.get(plp.open_asm).getAsmFilePath();
            txtCurFile.setText(fName + ":" + line + (plp.open_asm == 0 ? " <main program>" : ""));

            if(plp.isSimulating()) {
                long addr = plp.asm.getAddrFromFileMetadata(plp.open_asm, line);
                if(addr != -1)
                    txtCurFile.setText(txtCurFile.getText() + " " + String.format("0x%02x", addr));
            }
        }
    }//GEN-LAST:event_txtEditorCaretUpdate

    private void treeProjectMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_treeProjectMouseClicked

    }//GEN-LAST:event_treeProjectMouseClicked

    private void rootmenuProjectMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_rootmenuProjectMouseClicked
    }//GEN-LAST:event_rootmenuProjectMouseClicked

    private void txtEditorMousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_txtEditorMousePressed
        if(plp.plpfile != null && evt.getButton() == java.awt.event.MouseEvent.BUTTON3) {
            popupEdit.show(txtEditor, evt.getX(), evt.getY());
        }
    }//GEN-LAST:event_txtEditorMousePressed

    private void btnNewActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnNewActionPerformed
        menuNewActionPerformed(evt);
    }//GEN-LAST:event_btnNewActionPerformed

    private void btnOpenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnOpenActionPerformed
        menuOpenActionPerformed(evt);
    }//GEN-LAST:event_btnOpenActionPerformed

    private void btnSaveActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSaveActionPerformed
        menuSaveActionPerformed(evt);
    }//GEN-LAST:event_btnSaveActionPerformed

    private boolean deleteOccured;

    private void txtEditorKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txtEditorKeyTyped
        
        deleteOccured = false;
        boolean modified = false;

        if(evt.isAltDown())
            return;

        if(evt.isControlDown() && evt.getKeyChar() == 'y' ) {
            Msg.D("redo.", 10, this);
        } else if((int)evt.getKeyChar() == 10 || (int)evt.getKeyChar() > 31 && (int)evt.getKeyChar() < 127) {
            deleteOccured = (txtEditor.getSelectedText() != null) || (txtEditor.getSelectedText() != null && !txtEditor.getSelectedText().equals(""));
            modified = true;
        } else if (evt.getKeyCode() == evt.VK_DELETE || evt.getKeyCode() == evt.VK_BACK_SPACE) {
            modified = true;
        } else if ((int)evt.getKeyChar() == 127) {
            //deleteOccured = true;
            modified = true;
        } else if ((int)evt.getKeyChar() == 8) {
            //deleteOccured = true;
            modified = true;
        } else if ((int)evt.getKeyChar() == 24) {
            deleteOccured = true;
            modified = true;
        } else if ((int)evt.getKeyChar() == 22) {
            deleteOccured = (txtEditor.getSelectedText() == null) || (txtEditor.getSelectedText() != null && !txtEditor.getSelectedText().equals(""));
            Config.devSyntaxHighlightOnAssemble = true;
            
            /*** highlight now ***/
            Config.nothighlighting = false;
            syntaxHighlight();
            Config.nothighlighting = true;
            Config.devSyntaxHighlightOnAssemble = false;
            /*********************/
            
            modified = true;
        }

        if(modified) {
            Msg.D("Text has been modified.", 9, this);
            plp.setModified();

            if(txtEditor.isEditable()) {
                disableSimControls();
            }
            SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run() {
                    if(Config.devSyntaxHighlighting && !deleteOccured) {
                        Config.nothighlighting = false;
                        int caretPos = txtEditor.getCaretPosition();
                        syntaxHighlight(txtEditor.getText().substring(0, caretPos).split("\\r?\\n").length-1);
                        txtEditor.setCaretPosition(caretPos);
                        Config.nothighlighting = true;
                    }
                }
            });
        }
    }//GEN-LAST:event_txtEditorKeyTyped

    private void menuQuickProgramActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuQuickProgramActionPerformed
        plp.g_prg.program();
        plp.g_prg.setVisible(true);
    }//GEN-LAST:event_menuQuickProgramActionPerformed

    private void btnProgramActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnProgramActionPerformed
        menuProgramActionPerformed(null);
    }//GEN-LAST:event_btnProgramActionPerformed

    private void menuQuickRefActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuQuickRefActionPerformed
        plp.showQuickRef();
    }//GEN-LAST:event_menuQuickRefActionPerformed

    private void menuManualActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuManualActionPerformed
        Desktop desktop = Desktop.getDesktop();
        if(desktop.isSupported(Desktop.Action.BROWSE)) {
            try {
                URI uri = new URI(Constants.manualURI);
                desktop.browse(uri);
            } catch(Exception e) {}
        }
    }//GEN-LAST:event_menuManualActionPerformed

    private void menuIssuesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuIssuesActionPerformed
        Desktop desktop = Desktop.getDesktop();
        if(desktop.isSupported(Desktop.Action.BROWSE)) {
            try {
                URI uri = new URI(Constants.issueEntryURI);
                desktop.browse(uri);
            } catch(Exception e) {}
        }
    }//GEN-LAST:event_menuIssuesActionPerformed

    private void menuIssuesPageActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuIssuesPageActionPerformed
        Desktop desktop = Desktop.getDesktop();
        if(desktop.isSupported(Desktop.Action.BROWSE)) {
            try {
                URI uri = new URI(Constants.issueTrackerURI);
                desktop.browse(uri);
            } catch(Exception e) {}
        }
    }//GEN-LAST:event_menuIssuesPageActionPerformed

    private void menuFindAndReplaceActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuFindAndReplaceActionPerformed
        plp.g_find.setCurIndex(this.txtEditor.getCaretPosition());
        plp.g_find.setVisible(false);
        plp.g_find.setVisible(true);
    }//GEN-LAST:event_menuFindAndReplaceActionPerformed

    private void menuOptionsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuOptionsActionPerformed
        if(!plp.isSimulating())
            plp.g_opts.getTabs().setSelectedIndex(0);
        else
            plp.g_opts.getTabs().setSelectedIndex(1);

        plp.g_opts.setVisible(false);
        plp.g_opts.setVisible(true);
    }//GEN-LAST:event_menuOptionsActionPerformed

    private void menuSerialTerminalActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSerialTerminalActionPerformed
        if(plp.term == null)
            plp.term = new SerialTerminal(false);

        plp.term.setVisible(false);
        plp.term.setVisible(true);
    }//GEN-LAST:event_menuSerialTerminalActionPerformed

    private void menuPrintActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuPrintActionPerformed
        try {
            txtEditor.print();
        } catch(Exception e) {
            Msg.E("Failed to print currently open file.", Constants.PLP_PRINT_ERROR, this);
        }
    }//GEN-LAST:event_menuPrintActionPerformed

    private void menuSimStepActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimStepActionPerformed
        simStep();
    }//GEN-LAST:event_menuSimStepActionPerformed

    private void menuExitSimActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuExitSimActionPerformed
        simEnd();
        //plp.refreshProjectView(false);
        safeRefresh(false);
    }//GEN-LAST:event_menuExitSimActionPerformed

    private void menuSimRunActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimRunActionPerformed
        if(menuSimRun.isSelected()) {
            plp.runSimulation();
        } else {
            plp.stopSimulation();
        }
    }//GEN-LAST:event_menuSimRunActionPerformed

    private void menuSimIOActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimIOActionPerformed
        if(menuSimIO.isSelected()) {
            if(plp.g_ioreg == null) {
                plp.g_ioreg = new IORegistryFrame(plp);
                //plp.g_simsh.getSimDesktop().add(plp.g_ioreg);
            }

            plp.g_ioreg.setVisible(true);
        } else {
            if(plp.g_ioreg != null)
                plp.g_ioreg.setVisible(false);
        }
    }//GEN-LAST:event_menuSimIOActionPerformed

    private void menuSimWatcherActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimWatcherActionPerformed
        btnWatcher.setSelected(menuSimWatcher.isSelected());

        if(menuSimWatcher.isSelected()) {
            if(plp.g_watcher == null) {
                plp.g_watcher = new Watcher(plp);
                attachModuleFrameListeners(plp.g_watcher, Constants.PLP_TOOLFRAME_WATCHER);
            }

            plp.g_watcher.setVisible(true);

        } else {
            if(plp.g_watcher != null)
                plp.g_watcher.setVisible(false);
        }
    }//GEN-LAST:event_menuSimWatcherActionPerformed

    private void menuSimViewActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimViewActionPerformed
        btnCPU.setSelected(menuSimView.isSelected());

        if(menuSimView.isSelected()) {
            plp.g_sim.setVisible(true);
        } else {
            plp.g_sim.setVisible(false);
        }
    }//GEN-LAST:event_menuSimViewActionPerformed

    private void menuSimResetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimResetActionPerformed
        simReset();
    }//GEN-LAST:event_menuSimResetActionPerformed

    private void menuSimLEDsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimLEDsActionPerformed
        setLEDsFrame(menuSimLEDs.isSelected());
    }//GEN-LAST:event_menuSimLEDsActionPerformed

    private void menuSimSwitchesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimSwitchesActionPerformed
        setSwitchesFrame(menuSimSwitches.isSelected());
    }//GEN-LAST:event_menuSimSwitchesActionPerformed

    private void menuSimSevenSegmentsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimSevenSegmentsActionPerformed
        setSevenSegmentsFrame(menuSimSevenSegments.isSelected());
    }//GEN-LAST:event_menuSimSevenSegmentsActionPerformed

    private void menuSimUARTActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimUARTActionPerformed
        setUARTFrame(menuSimUART.isSelected());
    }//GEN-LAST:event_menuSimUARTActionPerformed

    private void menuSimVGAActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimVGAActionPerformed
        setVGAFrame(menuSimVGA.isSelected());
    }//GEN-LAST:event_menuSimVGAActionPerformed

    private void menuSimPLPIDActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimPLPIDActionPerformed
        setPLPIDFrame(menuSimPLPID.isSelected());
    }//GEN-LAST:event_menuSimPLPIDActionPerformed

    private void btnSimulateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimulateActionPerformed
        if(btnSimulate.isSelected()) {
            if(plp.isAssembled())
                simBegin();
        } else {
            simEnd();
            //plp.refreshProjectView(false);
            safeRefresh(false);
        }
    }//GEN-LAST:event_btnSimulateActionPerformed

    private void btnSimRunActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimRunActionPerformed
        if(btnSimRun.isSelected()) {
            plp.runSimulation();
        } else {
            plp.stopSimulation();
        }
    }//GEN-LAST:event_btnSimRunActionPerformed

    private void btnSimStepActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimStepActionPerformed
        simStep();
    }//GEN-LAST:event_btnSimStepActionPerformed

    private void btnSimResetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimResetActionPerformed
        simReset();
    }//GEN-LAST:event_btnSimResetActionPerformed

    private void menuStep1ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuStep1ActionPerformed
        if(menuStep1.isSelected()) {
            Config.simCyclesPerStep = 1;
            updateComponents();
        }
    }//GEN-LAST:event_menuStep1ActionPerformed

    private void menuStep2ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuStep2ActionPerformed
        if(menuStep2.isSelected()) {
            Config.simCyclesPerStep = 5;
            updateComponents();
        }
    }//GEN-LAST:event_menuStep2ActionPerformed

    private void menuStep3ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuStep3ActionPerformed
        if(menuStep3.isSelected()) {
            Config.simCyclesPerStep = 20;
            updateComponents();
        }
    }//GEN-LAST:event_menuStep3ActionPerformed

    private void menuStep4ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuStep4ActionPerformed
        if(menuStep4.isSelected()) {
            Config.simCyclesPerStep = 100;
            updateComponents();
        }

    }//GEN-LAST:event_menuStep4ActionPerformed

    private void menuStep5ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuStep5ActionPerformed
        if(menuStep5.isSelected()) {
            Config.simCyclesPerStep = 5000;
            updateComponents();
        }
    }//GEN-LAST:event_menuStep5ActionPerformed

    private void menuClearBreakpointsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuClearBreakpointsActionPerformed
        clearBreakpoints();
    }//GEN-LAST:event_menuClearBreakpointsActionPerformed

    private void menuToolbarActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuToolbarActionPerformed
        toolbar.setVisible(menuToolbar.isSelected());
    }//GEN-LAST:event_menuToolbarActionPerformed

    private void btnSimLEDsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimLEDsActionPerformed
        setLEDsFrame(btnSimLEDs.isSelected());
    }//GEN-LAST:event_btnSimLEDsActionPerformed

    private void btnSimSwitchesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimSwitchesActionPerformed
        setSwitchesFrame(btnSimSwitches.isSelected());
    }//GEN-LAST:event_btnSimSwitchesActionPerformed

    private void btnSimSevenSegmentsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimSevenSegmentsActionPerformed
        setSevenSegmentsFrame(btnSimSevenSegments.isSelected());
    }//GEN-LAST:event_btnSimSevenSegmentsActionPerformed

    private void btnSimUARTActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimUARTActionPerformed
        setUARTFrame(btnSimUART.isSelected());
    }//GEN-LAST:event_btnSimUARTActionPerformed

    private void btnSimVGAActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimVGAActionPerformed
        setVGAFrame(btnSimVGA.isSelected());
    }//GEN-LAST:event_btnSimVGAActionPerformed

    private void btnSimPLPIDActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimPLPIDActionPerformed
        setPLPIDFrame(btnSimPLPID.isSelected());
    }//GEN-LAST:event_btnSimPLPIDActionPerformed

    private void menuSimControlActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimControlActionPerformed
        plp.g_simctrl.setVisible(menuSimControl.isSelected());
        btnSimControl.setSelected(menuSimControl.isSelected());
    }//GEN-LAST:event_menuSimControlActionPerformed

    private void btnWatcherActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnWatcherActionPerformed
        menuSimWatcher.setSelected(btnWatcher.isSelected());

        if(btnWatcher.isSelected()) {
            if(plp.g_watcher == null) {
                plp.g_watcher = new Watcher(plp);
                attachModuleFrameListeners(plp.g_watcher, Constants.PLP_TOOLFRAME_WATCHER);
            }

            plp.g_watcher.setVisible(true);

        } else {
            if(plp.g_watcher != null)
                plp.g_watcher.setVisible(false);
        }
    }//GEN-LAST:event_btnWatcherActionPerformed

    private void btnCPUActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCPUActionPerformed
        menuSimView.setSelected(btnCPU.isSelected());

        if(btnCPU.isSelected()) {
            plp.g_sim.setVisible(true);
        } else {
            plp.g_sim.setVisible(false);
        }
    }//GEN-LAST:event_btnCPUActionPerformed

    private void menuSimAsmViewActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimAsmViewActionPerformed
        if(plp.g_asmview == null)
            plp.g_asmview = new ASMSimView(plp);

        plp.g_asmview.setVisible(true);
    }//GEN-LAST:event_menuSimAsmViewActionPerformed

    private void menuProjectPaneActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuProjectPaneActionPerformed
        boolean v;
        
        if(v = menuProjectPane.isSelected()) {
            if(hPaneSavedProportion > -1)
                splitterH.setDividerLocation(hPaneSavedProportion);
        } else {
            hPaneSavedProportion = splitterH.getDividerLocation() / (double) splitterH.getWidth();
            splitterH.setDividerLocation(0.0);
        }

        splitterH.setEnabled(v);
    }//GEN-LAST:event_menuProjectPaneActionPerformed

    private void menuOutputPaneActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuOutputPaneActionPerformed
        boolean v;

        if(v = menuOutputPane.isSelected()) {
            if(vPaneSavedProportion > -1)
                splitterV.setDividerLocation(vPaneSavedProportion);
        } else {
            vPaneSavedProportion = splitterV.getDividerLocation() / (double) splitterV.getHeight();
            splitterV.setDividerLocation(1.0);
        }

        splitterV.setEnabled(v);
    }//GEN-LAST:event_menuOutputPaneActionPerformed

    private void menuNumberConverterActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuNumberConverterActionPerformed
        if(plp.nconv == null)
                plp.nconv = new NumberConverter(false);

        plp.nconv.setVisible(false);
        plp.nconv.setVisible(true);
    }//GEN-LAST:event_menuNumberConverterActionPerformed

    private void menuSimulateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuSimulateActionPerformed
        if(menuSimulate.isSelected()) {
            if(plp.isAssembled())
                simBegin();
        } else {
            simEnd();
            //plp.refreshProjectView(false);
            safeRefresh(false);
        }
    }//GEN-LAST:event_menuSimulateActionPerformed

    private void menuClearOutputPaneActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_menuClearOutputPaneActionPerformed
        txtOutput.setText("");
    }//GEN-LAST:event_menuClearOutputPaneActionPerformed

    private void btnSimControlActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSimControlActionPerformed
        plp.g_simctrl.setVisible(btnSimControl.isSelected());
        menuSimControl.setSelected(btnSimControl.isSelected());
    }//GEN-LAST:event_btnSimControlActionPerformed

    private void initPopupMenus() {
        popupmenuNewASM = new javax.swing.JMenuItem();
        popupmenuNewASM.setText("New ASM file..."); // NOI18N
        popupmenuNewASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuNewASMActionPerformed(evt);
            }
        });

        popupmenuImportASM = new javax.swing.JMenuItem();
        popupmenuImportASM.setText("Import ASM file..."); // NOI18N
        popupmenuImportASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuImportASMActionPerformed(evt);
            }
        });

        popupmenuExportASM = new javax.swing.JMenuItem();
        popupmenuExportASM.setText("Export selected ASM file..."); // NOI18N
        popupmenuExportASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuExportASMActionPerformed(evt);
            }
        });

        popupmenuDeleteASM = new javax.swing.JMenuItem();
        popupmenuDeleteASM.setText("Remove selected ASM file"); // NOI18N
        popupmenuDeleteASM.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuDeleteASMActionPerformed(evt);
            }
        });

        popupProject = new javax.swing.JPopupMenu();
        popupProject.add(popupmenuNewASM);
        popupProject.add(popupmenuImportASM);
        popupProject.add(popupmenuExportASM);
        popupProject.add(popupmenuDeleteASM);


        popupmenuCopy = new javax.swing.JMenuItem();
        popupmenuCopy.setText("Copy"); // NOI18N
        popupmenuCopy.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuCopyActionPerformed(evt);
            }
        });

        popupmenuCut = new javax.swing.JMenuItem();
        popupmenuCut.setText("Cut"); // NOI18N
        popupmenuCut.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuCutActionPerformed(evt);
            }
        });

        popupmenuPaste = new javax.swing.JMenuItem();
        popupmenuPaste.setText("Paste"); // NOI18N
        popupmenuPaste.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                menuPasteActionPerformed(evt);
            }
        });

        popupEdit = new javax.swing.JPopupMenu();
        popupEdit.add(popupmenuCopy);
        popupEdit.add(popupmenuCut);
        popupEdit.add(popupmenuPaste);
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnAssemble;
    private javax.swing.JToggleButton btnCPU;
    private javax.swing.JButton btnNew;
    private javax.swing.JButton btnOpen;
    private javax.swing.JButton btnProgram;
    private javax.swing.JButton btnSave;
    private javax.swing.JToggleButton btnSimControl;
    private javax.swing.JToggleButton btnSimLEDs;
    private javax.swing.JToggleButton btnSimPLPID;
    private javax.swing.JButton btnSimReset;
    private javax.swing.JToggleButton btnSimRun;
    private javax.swing.JToggleButton btnSimSevenSegments;
    private javax.swing.JButton btnSimStep;
    private javax.swing.JToggleButton btnSimSwitches;
    private javax.swing.JToggleButton btnSimUART;
    private javax.swing.JToggleButton btnSimVGA;
    private javax.swing.JToggleButton btnSimulate;
    private javax.swing.JToggleButton btnWatcher;
    private javax.swing.JPanel devMainPane;
    private javax.swing.ButtonGroup grpSteps;
    private javax.swing.JMenuBar jMenuBar1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JPopupMenu.Separator jSeparator1;
    private javax.swing.JPopupMenu.Separator jSeparator10;
    private javax.swing.JPopupMenu.Separator jSeparator2;
    private javax.swing.JPopupMenu.Separator jSeparator3;
    private javax.swing.JToolBar.Separator jSeparator4;
    private javax.swing.JPopupMenu.Separator jSeparator5;
    private javax.swing.JPopupMenu.Separator jSeparator6;
    private javax.swing.JPopupMenu.Separator jSeparator7;
    private javax.swing.JPopupMenu.Separator jSeparator8;
    private javax.swing.JPopupMenu.Separator jSeparator9;
    private javax.swing.JLabel lblPosition;
    private javax.swing.JLabel lblSimStat;
    private javax.swing.JMenuItem menuAbout;
    private javax.swing.JMenuItem menuAssemble;
    private javax.swing.JMenuItem menuClearBreakpoints;
    private javax.swing.JMenuItem menuClearOutputPane;
    private javax.swing.JMenuItem menuCopy;
    private javax.swing.JMenuItem menuCut;
    private javax.swing.JMenuItem menuDeleteASM;
    private javax.swing.JMenuItem menuExit;
    private javax.swing.JMenuItem menuExitSim;
    private javax.swing.JMenuItem menuExportASM;
    private javax.swing.JMenuItem menuFindAndReplace;
    private javax.swing.JMenu menuIOReg;
    private javax.swing.JMenuItem menuImportASM;
    private javax.swing.JMenuItem menuIssues;
    private javax.swing.JMenuItem menuIssuesPage;
    private javax.swing.JMenuItem menuManual;
    private javax.swing.JMenuItem menuNew;
    private javax.swing.JMenuItem menuNewASM;
    private javax.swing.JMenuItem menuNumberConverter;
    private javax.swing.JMenuItem menuOpen;
    private javax.swing.JMenuItem menuOptions;
    private javax.swing.JCheckBoxMenuItem menuOutputPane;
    private javax.swing.JMenuItem menuPaste;
    private javax.swing.JMenuItem menuPrint;
    private javax.swing.JMenuItem menuProgram;
    private javax.swing.JCheckBoxMenuItem menuProjectPane;
    private javax.swing.JMenuItem menuQuickProgram;
    private javax.swing.JMenuItem menuQuickRef;
    private javax.swing.JMenuItem menuRedo;
    private javax.swing.JMenuItem menuSave;
    private javax.swing.JMenuItem menuSaveAs;
    private javax.swing.JPopupMenu.Separator menuSeparator1;
    private javax.swing.JPopupMenu.Separator menuSeparator3;
    private javax.swing.JPopupMenu.Separator menuSeparator4;
    private javax.swing.JPopupMenu.Separator menuSeparator5;
    private javax.swing.JMenuItem menuSerialTerminal;
    private javax.swing.JMenuItem menuSetMainProgram;
    private javax.swing.JMenuItem menuSimAsmView;
    private javax.swing.JCheckBoxMenuItem menuSimControl;
    private javax.swing.JCheckBoxMenuItem menuSimIO;
    private javax.swing.JCheckBoxMenuItem menuSimLEDs;
    private javax.swing.JCheckBoxMenuItem menuSimPLPID;
    private javax.swing.JMenuItem menuSimReset;
    private javax.swing.JCheckBoxMenuItem menuSimRun;
    private javax.swing.JCheckBoxMenuItem menuSimSevenSegments;
    private javax.swing.JMenuItem menuSimStep;
    private javax.swing.JCheckBoxMenuItem menuSimSwitches;
    private javax.swing.JMenu menuSimTools;
    private javax.swing.JCheckBoxMenuItem menuSimUART;
    private javax.swing.JCheckBoxMenuItem menuSimVGA;
    private javax.swing.JCheckBoxMenuItem menuSimView;
    private javax.swing.JCheckBoxMenuItem menuSimWatcher;
    private javax.swing.JCheckBoxMenuItem menuSimulate;
    private javax.swing.JRadioButtonMenuItem menuStep1;
    private javax.swing.JRadioButtonMenuItem menuStep2;
    private javax.swing.JRadioButtonMenuItem menuStep3;
    private javax.swing.JRadioButtonMenuItem menuStep4;
    private javax.swing.JRadioButtonMenuItem menuStep5;
    private javax.swing.JMenu menuStepSize;
    private javax.swing.JCheckBoxMenuItem menuToolbar;
    private javax.swing.JMenuItem menuUndo;
    private javax.swing.JMenu rootmenuEdit;
    private javax.swing.JMenu rootmenuFile;
    private javax.swing.JMenu rootmenuHelp;
    private javax.swing.JMenu rootmenuProject;
    private javax.swing.JMenu rootmenuSim;
    private javax.swing.JMenu rootmenuTools;
    private javax.swing.JMenu rootmenuView;
    private javax.swing.JScrollPane scroller;
    private javax.swing.JToolBar.Separator separatorSim;
    private javax.swing.JToolBar.Separator separatorSimControl;
    private javax.swing.JSplitPane splitterH;
    private javax.swing.JSplitPane splitterV;
    private javax.swing.JToolBar toolbar;
    private javax.swing.JTree treeProject;
    private javax.swing.JLabel txtCurFile;
    private javax.swing.JTextPane txtEditor;
    private javax.swing.JTextPane txtOutput;
    // End of variables declaration//GEN-END:variables

    //popup menu items
    private javax.swing.JMenuItem popupmenuDeleteASM;
    private javax.swing.JMenuItem popupmenuExportASM;
    private javax.swing.JMenuItem popupmenuImportASM;
    private javax.swing.JMenuItem popupmenuNewASM;

    private javax.swing.JMenuItem popupmenuCopy;
    private javax.swing.JMenuItem popupmenuCut;
    private javax.swing.JMenuItem popupmenuPaste;

    private javax.swing.JPopupMenu popupEdit;

    @Override
    public String toString() {
        return plp.plpfile != null ?
            "Develop(" + plp.plpfile.getName() +")" :
            "Develop";
    }

}

class AsmFilter extends javax.swing.filechooser.FileFilter {
    public boolean accept(java.io.File f) {
        if(f.isDirectory())
            return true;

        if(f.getAbsolutePath().endsWith(".asm"))
            return true;

        return false;
    }

    public String getDescription() {
        return ".ASM files";
    }
}

class PlpFilter extends javax.swing.filechooser.FileFilter {
    public boolean accept(java.io.File f) {
        if(f.isDirectory())
            return true;

        if(f.getAbsolutePath().endsWith(".plp"))
            return true;

        return false;
    }

    public String getDescription() {
        return "PLP project files";
    }
}

class DevUndoManager extends javax.swing.undo.UndoManager{

    java.util.ArrayList<Boolean> editTypeList;
    int position;
    int lastUndoPosition;

    public DevUndoManager() {
        super();
        position = 0;
        lastUndoPosition = 0;

        editTypeList = new java.util.ArrayList<Boolean>();
    }

    public boolean safeAddEdit(javax.swing.undo.UndoableEdit anEdit) {
            editTypeList.add(position, Config.nothighlighting);
            position++;
            Msg.D("++++ " + Config.nothighlighting + " undo position: " + position, 5, null);
            return super.addEdit(anEdit);
    }
    
    public void safeUndo() {
        boolean oldSyntaxOption = Config.devSyntaxHighlighting;
        Config.devSyntaxHighlighting = false;

        if(position <= 0 || !super.canUndo())
            return;

        // shed formatting events
        while(super.canUndo() && position > 0 && !editTypeList.get(position - 1)) {
            position--;
            super.undo();
        }

        if(position > 0 && super.canUndo()) {
            super.undo();
            position--;
            lastUndoPosition = position;
        }

        Config.devSyntaxHighlighting = oldSyntaxOption;

        Msg.D("<--- undo position: " + position, 5, null);
    }

    public void dumpList() {
        System.out.println();
        for(int i = 0; i < editTypeList.size(); i++) {
            System.out.println(i + "\t" + editTypeList.get(i));
        }
        System.out.println();
    }
    
    public void safeRedo() {
        boolean oldSyntaxOption = Config.devSyntaxHighlighting;
        Config.devSyntaxHighlighting = false;

        if(position > editTypeList.size() || !super.canRedo())
            return;

        // shed formatting events
        while(super.canRedo() && position < editTypeList.size() - 1 && !editTypeList.get(position + 1)) {
            super.redo();
            position++;
        }

        if(super.canRedo()) {
            super.redo();
            position++;
        }
        
        Config.devSyntaxHighlighting = oldSyntaxOption;

        Msg.D("---> undo position: " + position, 5, null);
    }

    public void reset() {
        editTypeList.clear();
        position = 0;
        super.discardAllEdits();
    }

    public void addEditType(boolean isHighlight) {

    }

    public boolean getNextEditType() {
        return false;
    }

    public boolean getPreviousEditType() {
        return false;
    }
}
