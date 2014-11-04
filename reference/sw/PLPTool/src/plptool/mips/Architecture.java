/*
    Copyright 2011-2014 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mips;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JMenuItem;
import plptool.*;
import plptool.gui.ProjectDriver;
import java.io.File;

/**
 * This is the PLP CPU implementation of the ISA meta class.
 *
 * @author wira
 */
public class Architecture extends PLPArchitecture {

    private boolean busMonitorAttached;
    private int busMonitorModulePosition;
    private plptool.mods.BusMonitor busMonitor;
    private plptool.mods.BusMonitorFrame busMonitorFrame;
    private plptool.mips.visualizer.CPUVisualization cpuVis;
    private SyntaxHighlightSupport syntaxHighlightSupport;
    private javax.swing.JMenuItem menuExportVerilogHex;
    private javax.swing.JCheckBoxMenuItem menuNexysBoard;
    
    public Architecture(int archID, ProjectDriver plp) {
        super(archID, "plpmips", plp);
        hasAssembler = true;
        hasSimCore = true;
        hasSimCoreGUI = true;
        hasProgrammer = true;
        hasSyntaxHighlightSupport = true;
        syntaxHighlightSupport = new SyntaxHighlightSupport();
        informationString = "PLP CPU ISA implementation";
        if(plp.g())
            plp.g_opts.setBuiltInISAOptions(true);
    }

    @Override
    /**
     * Add our specific menu items
     */
    public void init() {
        if(plp.g()) {
            menuExportVerilogHex = new JMenuItem("Export PLP CPU boot ROM Verilog Hex...");
            menuExportVerilogHex.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    String lines[];
                    String out = "";
                    String hex;
                    File path;
                    if(!plp.isAssembled()) {
                        Msg.E("Project needs to be assembled.", Constants.PLP_GENERIC_ERROR, null);
                    } else {
                        Msg.I("Generating Verilog hex-format...", null);
                        hex = plptool.mips.Formatter.writeVerilogHex(plp.asm.getObjectCode());
                        lines = hex.split("\n");
                        for(int i = 0; i < lines.length; i++) {
                            lines[i] = "ram[" + i + "] = " + lines[i] + ";";
                            out += lines[i] + "\n";
                        }
                        path = PLPToolbox.saveFileDialog(Constants.launchPath);
                        if(path != null) {
                            if(PLPToolbox.writeFile(out, path.getAbsolutePath()) == Constants.PLP_OK) {
                                Msg.I(path + " written.", null);
                            }
                        }
                    }
                }
            });
            
            menuNexysBoard = new javax.swing.JCheckBoxMenuItem("Use 57600 baud for programming (Nexys 2/3 board)");

            menuNexysBoard.setState(true);
            plp.g_dev.addToolsItem(menuNexysBoard);
            plp.g_dev.addToolsItem(menuExportVerilogHex);
        }
    }

    /**
     * Return a new instance of the assembler, using the asm source list array
     * of the ProjectDriver as input
     *
     * @return the assembler instance
     */
    public PLPAsm createAssembler() {
        SerialProgrammer.resetPreamble();
        return new plptool.mips.Asm(plp.getAsms());
    }

    /**
     * Return a new instance of the simulation core.
     *
     * @return SimCore instance of the ISA
     */
    public PLPSimCore createSimCore() {
        PLPSimCore sim = new plptool.mips.SimCore((plptool.mips.Asm) plp.asm,
                                    plp.asm.getAddrTable()[0]);
        sim.setStartAddr(plp.asm.getAddrTable()[0]);
        return sim;
    }
    
    public PLPSimCoreGUI createSimCoreGUI() {
        return new plptool.mips.SimCoreGUI(plp);
    }

    public PLPSerialProgrammer createProgrammer() {
        return new SerialProgrammer(plp);
    }

    @Override
    public PLPSyntaxHighlightSupport getSyntaxHighlightSupport() {
        return syntaxHighlightSupport;
    }

    /**
     * Perform simulator initialization
     */
    @Override
    public void simulatorInitialization() {
        busMonitorAttached = false;

        plp.sim.bus.add(new plptool.mods.InterruptController(0xf0700000L, plp.sim));
        plp.sim.bus.add(new plptool.mods.Button(8, 0xfffffff7L, plp.sim));
        if(plp.smods == null) // if no modules are defined, load the PLP preset
            plp.ioreg.loadPredefinedPreset(plp.g() ? 0 : 1);
        plp.sim.bus.enableAllModules();

        // add our button interrupt to g_dev toolbar
        if(plp.g()) {
            final javax.swing.JToggleButton btnInt = new javax.swing.JToggleButton();
            btnInt.setIcon(new javax.swing.ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(plp.g_dev.getClass().getResource("resources/toolbar_exclamation.png"))));
            btnInt.setToolTipText("Button Interrupt (Toggle button)");
            btnInt.setOpaque(false);
            btnInt.setMargin(new java.awt.Insets(2, 0, 2, 0));
            btnInt.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent e) {
                    ((plptool.mods.Button) plp.sim.bus.getRefMod(1)).setPressedState(btnInt.isSelected());
                }
            });

            plp.g_dev.addButton(btnInt);
        }

        // add our custom simulation tools menu entries
        if(plp.g()) {
            final javax.swing.JMenuItem menuMemoryVisualizer = new javax.swing.JMenuItem();
            menuMemoryVisualizer.setText("Create a PLP CPU Memory Visualizer");
            menuMemoryVisualizer.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent e) {
                    plptool.mips.visualizer.MemoryVisualization memvis = new plptool.mips.visualizer.MemoryVisualization(plp);
                    ((plptool.mips.SimCoreGUI) plp.g_sim).attachMemoryVisualizer(memvis);
                    memvis.setVisible(true);
                }
            });

            final javax.swing.JMenuItem menuForgetMemoryVisualizer = new javax.swing.JMenuItem();
            menuForgetMemoryVisualizer.setText("Remove Memory Visualizers from Project");
            menuForgetMemoryVisualizer.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent e) {
                    ((plptool.mips.SimCoreGUI) plp.g_sim).disposeMemoryVisualizers();
                    ((plptool.mips.SimCoreGUI) plp.g_sim).updateAttributeForMemoryVisualizers();
                }
            });

            // Add bus monitor checkbox menu
            final javax.swing.JCheckBoxMenuItem menuBusMonitor = new javax.swing.JCheckBoxMenuItem();
            menuBusMonitor.setText("Display Bus Monitor Timing Diagram");
            menuBusMonitor.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent e) {
                    if(menuBusMonitor.isSelected()) {
                        if(!busMonitorAttached) {
                            busMonitor = new plptool.mods.BusMonitor(plp.sim);
                            plp.sim.bus.add(busMonitor);
                            busMonitorModulePosition = plp.sim.bus.getNumOfMods() - 1;
                            plp.sim.bus.enableMod(busMonitorModulePosition);
                            busMonitorFrame = new plptool.mods.BusMonitorFrame((plptool.mods.BusMonitor)plp.sim.bus.getRefMod(busMonitorModulePosition), menuBusMonitor);
                            busMonitorAttached = true;
                        }
                        busMonitorFrame.setVisible(true);
                    } else
                        busMonitorFrame.setVisible(false);
                }
            });

            // Add CPU visualization checkbox menu
            final javax.swing.JCheckBoxMenuItem menuCpuVis = new javax.swing.JCheckBoxMenuItem();
            menuCpuVis.setText("Display CPU Visualization");
            menuCpuVis.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent e) {
                    if(menuCpuVis.isSelected()) {
                        cpuVis.setVisible(true);
                        ((SimCoreGUI) plp.g_sim).attachCPUVisualizer(cpuVis);
                    } else {
                        cpuVis.setVisible(false);
                    }
                }
            });
            cpuVis = new plptool.mips.visualizer.CPUVisualization((SimCore) plp.sim, menuCpuVis);

            // Restore saved timing diagram from project attributes, if it exists
            plptimingdiagram.TimingDiagram savedTD = (plptimingdiagram.TimingDiagram) plp.getProjectAttribute("plpmips_timingdiagram");
            if(savedTD != null) {
                plptool.Msg.D("Attempting to load timing diagram from project attributes.", 3, null);
                busMonitor = new plptool.mods.BusMonitor(plp.sim);
                busMonitor.setTimingDiagram(savedTD);
                plp.sim.bus.add(busMonitor);
                busMonitorModulePosition = plp.sim.bus.getNumOfMods() - 1;
                plp.sim.bus.enableMod(busMonitorModulePosition);
                busMonitorFrame = new plptool.mods.BusMonitorFrame((plptool.mods.BusMonitor)plp.sim.bus.getRefMod(busMonitorModulePosition), menuBusMonitor);
                Boolean b = (Boolean) plp.getProjectAttribute("plpmips_timingdiagram_framevisibility");
                if(b != null) {
                    busMonitorFrame.setVisible(b);
                    menuBusMonitor.setSelected(b);
                }
                busMonitorAttached = true;
                plptool.Msg.D("Timing diagram loaded!", 3, null);
            }

            plp.g_dev.addSimToolSeparator();
            plp.g_dev.addSimToolItem(menuMemoryVisualizer);
            plp.g_dev.addSimToolItem(menuForgetMemoryVisualizer);
            plp.g_dev.addSimToolItem(menuBusMonitor);
            
            //Disable for 4.1 release
            if(Constants.debugLevel >= 2) {
                plp.g_dev.addSimToolItem(menuCpuVis);
            }
        }
    }

    /**
     * Post-simulation routine, clean up our mess with Develop
     */
    @Override
    public void simulatorStop() {
        plptool.mips.SimCoreGUI g_sim = ((plptool.mips.SimCoreGUI) plp.g_sim);

        if(plp.g() && plp.g_dev != null) {
            plp.g_dev.removeLastButton();
            plp.g_dev.removeLastSimToolItem();
            plp.g_dev.removeLastSimToolItem();
            plp.g_dev.removeLastSimToolItem();
            plp.g_dev.removeLastSimToolItem();
            
            //Disable for 4.1 release
            if(Constants.debugLevel >= 2) {
                plp.g_dev.removeLastSimToolItem();
            }

            if(busMonitor != null && busMonitorAttached) {
                plp.addProjectAttribute("plpmips_timingdiagram", busMonitor.getTimingDiagram());
                plp.addProjectAttribute("plpmips_timingdiagram_framevisibility", busMonitorFrame.isVisible());
                busMonitorFrame.dispose();
                busMonitorFrame = null;
                busMonitor = null;
            }

            cpuVis.dispose();
            cpuVis = null;
            g_sim.disposeMemoryVisualizers();
        }
    }

    @Override
    public void launchSimulatorCLI() {
        SimCLI.simCL(plp);
    }

    @Override
    public String saveArchSpecificSimStates() {
        // check if we have saved memory visualizer entries in pAttrSet
        String ret = "";
        Object[][] attrSet = (Object[][]) plp.getProjectAttribute("plpmips_memory_visualizer");

        if(attrSet != null) {
            ret += "plpmips_memory_visualizer::";
            for(int i = 0; i < attrSet.length; i++) {
                ret += attrSet[i][0] + "-" + attrSet[i][1] + ":";
            }
        }
        ret += "\n";

        // check for bus monitor timing diagram settings
        plptimingdiagram.TimingDiagram tD = (plptimingdiagram.TimingDiagram) plp.getProjectAttribute("plpmips_timingdiagram");
        if(tD != null) {
            ret += "plpmips_timingdiagram::";
            for(int i = 0; i < tD.getNumberOfSignals(); i++) {
                ret += tD.getSignal(i).getName() + ":";
            }
        }
        ret += "\n";

        return ret;
    }

    @Override
    public void restoreArchSpecificSimStates(String[] configStr) {
        if(configStr[0].equals("plpmips_memory_visualizer")) {
            String[] tokens = configStr[1].split(":");
            Object[][] attrSet = new Object[tokens.length][2];
            for(int j = 0; j < tokens.length; j++) {
                String tempTokens[] = tokens[j].split("-");
                plptool.Msg.D("plpmips_memory_visualizer load: " + tempTokens[0] + "-" + tempTokens[1], 4, null);
                Long[] temp = new Long[2];
                temp[0] = new Long(Long.parseLong(tempTokens[0]));
                temp[1] = new Long(Long.parseLong(tempTokens[1]));
                attrSet[j] = temp;
            }
            plp.addProjectAttribute("plpmips_memory_visualizer", attrSet);
        } else if(configStr[0].equals("plpmips_timingdiagram") && configStr.length == 2) {
            plptimingdiagram.TimingDiagram tD = new plptimingdiagram.TimingDiagram();
            String[] tokens = configStr[1].split(":");
            for(int j = 0; j < tokens.length; j++) {
                plptimingdiagram.signals.Bus busSignal = new plptimingdiagram.signals.Bus();
                busSignal.setName(tokens[j]);
                tD.addSignal(busSignal);
                plptool.Msg.D("plpmips_timingdiagram load: " + tokens[j], 4, null);
            }
            plp.addProjectAttribute("plpmips_timingdiagram", tD);
        }
    }
    
    @Override
    public void simCLICommand(String cmd) {
        SimCLI.simCLCommand(cmd, plp);
    }

    @Override
    public String getQuickReferenceString() {
        String str = "";

        str += "<a name=\"top\" /><h1>PLP-5 Quick Reference Card</h1>";

        str += "<p>Instructions: <a href=\"#r\">R-type</a> " +
               "<a href=\"#i\">I-type</a> " +
               "<a href=\"#j\">J-type</a> " +
               "</p>";
        str += "<p><a href=\"#p\">Pseudo Instructions</a></p>";
        str += "<p><a href=\"#d\">Assembler Directives</a></p>";
        str += "<p><a href=\"#regs\">Registers Usage</a></p>";
        str += "<p><a href=\"#mmap\">I/O Memory Map</a></p>";

        str += "<a name=\"r\" /><h1>R-type Instructions</h1>";
        str += "<table border=1 width=\"100%\">";

        String[][] data = new String[15][2];

        data[0][0] = "<font color=blue><b>addu</b></font> $rd, $rs, $rt";      data[0][1] = "rd = rs + rt";
        data[1][0] = "<font color=blue><b>subu</b></font> $rd, $rs, $rt";      data[1][1] = "rd = rs - rt";
        data[2][0] = "<font color=blue><b>and</b></font>  $rd, $rs, $rt";      data[2][1] = "rd = rs &amp; rt";
        data[3][0] = "<font color=blue><b>or</b></font>   $rd, $rs, $rt";      data[3][1] = "rd = rs | rt";
        data[4][0] = "<font color=blue><b>nor</b></font>  $rd, $rs, $rt";      data[4][1] = "rd = ~(rs | rt)";
        data[5][0] = "<font color=blue><b>mullo</b></font> $rd, $rs, $rt";     data[5][1] = "rd = (rs * rt) &amp; 0xffffffff";
        data[6][0] = "<font color=blue><b>mulhi</b></font> $rd, $rs, $rt";     data[6][1] = "rd = (rs * rt) &gt;&gt; 32";
        data[7][0] = "<font color=blue><b>slt</b></font>  $rd, $rs, $rt";      data[7][1] = "rd = (rs &lt; rt) ? 1 : 0";
        data[8][0] = "<font color=blue><b>sltu</b></font> $rd, $rs, $rt";      data[8][1] = "rd = (rs &lt; rt) ? 1 : 0";
        data[9][0] = "<font color=blue><b>sll</b></font>  $rd, $rt, shamt";    data[9][1] = "rd = rt &lt;&lt; shamt";
        data[10][0] = "<font color=blue><b>srl</b></font>  $rd, $rt, shamt";   data[10][1] = "rd = rt &gt;&gt; shamt";
        data[11][0] = "<font color=blue><b>sllv</b></font>  $rd, $rs, $rt";    data[11][1] = "rd = rs &lt;&lt; rt";
        data[12][0] = "<font color=blue><b>srlv</b></font>  $rd, $rs, $rt";    data[12][1] = "rd = rs &gt;&gt; rt";
        data[13][0] = "<font color=blue><b>jr</b></font> $rs";                 data[13][1] = "PC = rs";
        data[14][0] = "<font color=blue><b>jalr</b></font> $rd, $rs";          data[14][1] = "rd = PC + 4; PC = rs";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"i\" /><h1>I-type Instructions</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[10][2];

        data[0][0] = "<font color=blue><b>addiu</b></font> $rt, $rs, imm";     data[0][1] = "rt = rs + SignExtend(imm)";
        data[1][0] = "<font color=blue><b>andi</b></font>  $rt, $rs, imm";     data[1][1] = "rt = rs & ZeroExtend(imm)";
        data[2][0] = "<font color=blue><b>ori</b></font>   $rt, $rs, imm";     data[2][1] = "rt = rs | ZeroExtend(imm)";
        data[3][0] = "<font color=blue><b>slti</b></font>  $rt, $rs, imm";     data[3][1] = "rt = (rs &lt; SignExtend(imm)) ? 1 : 0";
        data[4][0] = "<font color=blue><b>sltiu</b></font> $rt, $rs, imm";     data[4][1] = "rt = (rs &lt; SignExtend(imm)) ? 1 : 0";
        data[5][0] = "<font color=blue><b>lui</b></font>   $rt, imm";          data[5][1] = "rt = imm &lt;&lt; 16";
        data[6][0] = "<font color=blue><b>lw</b></font>    $rt, imm($rs)";     data[6][1] = "rt = mem[SignExtend(imm) + rs]";
        data[7][0] = "<font color=blue><b>sw</b></font>    $rt, imm($rs)";     data[7][1] = "mem[SignExtend(imm) + rs] = rt";
        data[8][0] = "<font color=blue><b>beq</b></font>   $rt, $rs, label";   data[8][1] = "if (rt == rs) PC = PC + 4 + imm";
        data[9][0] = "<font color=blue><b>bne</b></font>   $rt, $rs, label";   data[9][1] = "if (rt != rs) PC = PC + 4 + imm";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"j\" /><h1>J-type Instructions</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[2][2];

        data[0][0] = "<font color=blue><b>j</b></font>     label";             data[0][1] = "PC = label";
        data[1][0] = "<font color=blue><b>jal</b></font>   label";             data[1][1] = "ra = PC + 4; PC = label";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"p\" /><h1>Pseudo-operations</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[13][2];

        data[0][0] = "<font color=blue><b>nop</b></font>";                     data[0][1] = "sll $0, $0, 0";
        data[1][0] = "<font color=blue><b>b</b></font>     label";             data[1][1] = "beq $0, $0, label";
        data[2][0] = "<font color=blue><b>move</b></font>  $rd, $rs";          data[2][1] = "or  $rd, $0, $rs";
        data[3][0] = "<font color=blue><b>li</b></font>    $rd, imm32";        data[3][1] = "lui $rd, imm32 &gt;&gt; 16<br />";
                                                                               data[3][1] += "ori $rd, $rd, imm32 & 0xffff";
        data[4][0] = "<font color=blue><b>li</b></font>    $rd, label";        data[4][1] = "lui $rd, label[31:16]<br />";
                                                                               data[4][1] += "ori $rd, $rd, label[15:0]";
        data[5][0] = "<font color=blue><b>push</b></font>  $rt";               data[5][1] = "addiu $sp, $sp, -4<br />";
                                                                               data[5][1] += "sw $rt, 4($sp)";
        data[6][0] = "<font color=blue><b>pop</b></font>   $rt";               data[6][1] = "lw $rt, 4($sp)<br />";
                                                                               data[6][1] += "addiu $sp, $sp, 4";
        data[7][0] = "<font color=blue><b>call</b></font>  label";             data[7][1] = "Save $aX, $tX, and $ra to stack and call function";
        data[8][0] = "<font color=blue><b>return</b></font>";                  data[8][1] = "Restore $aX, $tX, and $ra from stack and return";
        data[9][0] = "<font color=blue><b>save</b></font>";                    data[9][1] = "Save all registers except for $zero to stack";
        data[10][0] = "<font color=blue><b>restore</b></font>";                data[10][1] = "Restore all registers saved by 'save' in reverse order";
        data[11][0] = "<font color=blue><b>lwm</b></font> $rt, imm32/label";         data[11][1] = "Load the value from a memory location into $rt";
        data[12][0] = "<font color=blue><b>swm</b></font> $rt, imm32/label";         data[12][1] = "Store the value in $rt to a memory location";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"d\" /><h1>Assembler Directives</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[8][2];

        data[0][0] = ".org <i>address</i>";                     data[0][1] = "Place subsequent statements starting from <i>address</i>";
        data[1][0] = "<i>label</i>:";                           data[1][1] = "Label current memory location as <i>label</i>";
        data[2][0] = ".word <i>value</i>";                      data[2][1] = "Write 32-bit <i>value</i> to the current address";
        data[3][0] = ".ascii \"<i>string</i>\"";                data[3][1] = "Place <i>string</i> starting from the current address";
        data[4][0] = ".asciiz \"<i>string</i>\"";               data[4][1] = "Place null-terminated <i>string</i> starting from the current address";
        data[5][0] = ".asciiw \"<i>string</i>\"";               data[5][1] = "Place word-aligned <i>string</i> starting from the current address";
        data[6][0] = ".space <i>value</i>";                     data[6][1] = "Reserve <i>value</i> words starting from the current address";
        data[7][0] = ".equ <i>symbol</i> <i>value</i>";         data[7][1] = "Add a symbol and its associated value to the symbol table (a constant)";


        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"regs\" /><h1>Registers Usage Guide</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[11][2];

        data[0][0] = "$0, $zero";               data[0][1] = "The zero register";
        data[1][0] = "$1, $at";                 data[1][1] = "Assembler temporary";
        data[2][0] = "$2-$3, $v0-$v1";          data[2][1] = "Return values";
        data[3][0] = "$4-$7, $a0-$a3";          data[3][1] = "Function arguments";
        data[4][0] = "$8-$17, $t0-$t9";         data[4][1] = "Temporaries";
        data[5][0] = "$18-$25, $s0-$s7";        data[5][1] = "Saved temporaries";
        data[6][0] = "$26-$27, $i0-$i1";        data[6][1] = "Interrupt temporaries";
        data[7][0] = "$28, $iv";                data[7][1] = "Interrupt vector";
        data[8][0] = "$29, $sp";                data[8][1] = "Stack pointer";
        data[9][0] = "$30, $ir";                data[9][1] = "Interrupt return address";
        data[10][0] = "$31, $ra";               data[10][1] = "Return address";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"mmap\" /><h1>I/O Memory Map</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[11][2];

        data[0][0] = "0x00000000";              data[0][1] = "Boot ROM";
        data[1][0] = "0x10000000";              data[1][1] = "RAM";
        data[2][0] = "0xf0000000";              data[2][1] = "UART";
        data[3][0] = "0xf0100000";              data[3][1] = "Switches";
        data[4][0] = "0xf0200000";              data[4][1] = "LEDs";
        data[5][0] = "0xf0300000";              data[5][1] = "GPIO";
        data[6][0] = "0xf0400000";              data[6][1] = "VGA";
        data[7][0] = "0xf0500000";              data[7][1] = "PLPID";
        data[8][0] = "0xf0600000";              data[8][1] = "Timer";
        data[9][0] = "0xf0a00000";              data[9][1] = "Seven Segments";
        data[10][0] = "0xf0700000";             data[10][1] = "Interrupt Controller";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td><font face=\"monospaced\" size=\"12pt\">" + data[i][0] + "</font></td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        return str;
    }

    public boolean isUsingNexysBoard() {
        if(plp.g() && menuNexysBoard != null) {
            return menuNexysBoard.getState();
        } else {
            return false;
        }
    }

    @Override
    public void newProject(ProjectDriver plp) {
        plp.getAsm(0).setAsmString("# main source file\n\n.org 0x10000000");
    }

    @Override
    public void cleanup() {
        if(plp.g()) {
            plp.g_opts.setBuiltInISAOptions(false);
            plp.g_dev.removeToolsItem(menuExportVerilogHex);
            plp.g_dev.removeToolsItem(menuNexysBoard);
        }
    }
}
