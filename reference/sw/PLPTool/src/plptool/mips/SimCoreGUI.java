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
 * PLPMIPSCoreGUI.java
 *
 * Created on Dec 4, 2010, 12:12:09 AM
 */

package plptool.mips;

import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.JTable;
import javax.swing.table.DefaultTableModel;
import javax.swing.JTextField;
import javax.swing.DefaultCellEditor;
import javax.swing.text.html.*;
import java.awt.event.FocusListener;
import java.awt.event.FocusEvent;
import java.util.ArrayList;
import plptool.PLPToolbox;
import plptool.mips.visualizer.*;
import plptool.Config;

/**
 *
 * @author wira
 */
public class SimCoreGUI extends plptool.PLPSimCoreGUI {

    private plptool.gui.ProjectDriver plp;
    private ArrayList<MemoryVisualization> memoryVisualizers;
    private CPUVisualization cpuVisualizer;
    private long old_pc;
    private String lastCLCommand = "";

    /** Creates new form PLPMIPSCoreGUI */
    public SimCoreGUI(plptool.gui.ProjectDriver plp) {
        super();
        this.sim = plp.sim;
        this.plp = plp;

        sim.bus.enableAllModules();
        sim.bus.eval();

        initComponents();

        memoryVisualizers = new ArrayList<MemoryVisualization>();

        renderer.setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
        tblRegFile.setDefaultRenderer(tblRegFile.getColumnClass(2), renderer);

        tblRegFile.setValueAt("0: $zero", 0, 0);
        tblRegFile.setValueAt("1: $at", 1, 0);
        tblRegFile.setValueAt("2: $v0", 2, 0);
        tblRegFile.setValueAt("3: $v1", 3, 0);
        tblRegFile.setValueAt("4: $a0", 4, 0);
        tblRegFile.setValueAt("5: $a1", 5, 0);
        tblRegFile.setValueAt("6: $a2", 6, 0);
        tblRegFile.setValueAt("7: $a3", 7, 0);
        tblRegFile.setValueAt("8: $t0", 8, 0);
        tblRegFile.setValueAt("9: $t1", 9, 0);
        tblRegFile.setValueAt("10: $t2", 10, 0);
        tblRegFile.setValueAt("11: $t3", 11, 0);
        tblRegFile.setValueAt("12: $t4", 12, 0);
        tblRegFile.setValueAt("13: $t5", 13, 0);
        tblRegFile.setValueAt("14: $t6", 14, 0);
        tblRegFile.setValueAt("15: $t7", 15, 0);
        tblRegFile.setValueAt("16: $t8", 16, 0);
        tblRegFile.setValueAt("17: $t9", 17, 0);
        tblRegFile.setValueAt("18: $s0", 18, 0);
        tblRegFile.setValueAt("19: $s1", 19, 0);
        tblRegFile.setValueAt("20: $s2", 20, 0);
        tblRegFile.setValueAt("21: $s3", 21, 0);
        tblRegFile.setValueAt("22: $s4", 22, 0);
        tblRegFile.setValueAt("23: $s5", 23, 0);
        tblRegFile.setValueAt("24: $s6", 24, 0);
        tblRegFile.setValueAt("25: $s7", 25, 0);
        tblRegFile.setValueAt("26: $i0", 26, 0);
        tblRegFile.setValueAt("27: $i1", 27, 0);
        tblRegFile.setValueAt("28: $iv", 28, 0);
        tblRegFile.setValueAt("29: $sp", 29, 0);
        tblRegFile.setValueAt("30: $ir", 30, 0);
        tblRegFile.setValueAt("31: $ra", 31, 0);

        CustomCellTextField textField = new CustomCellTextField(this);
        CustomCellEditor ce = new CustomCellEditor(textField);
        tblRegFile.setDefaultEditor(String.class, ce);

        this.setIconImage(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/toolbar_cpu.png")));

        old_pc = -1;

        clearProgramMemoryTable();
        fillProgramMemoryTable();

        // check if we have saved memory visualizer entries in pAttrSet
        Object[][] attrSet = (Object[][]) plp.getProjectAttribute("plpmips_memory_visualizer");
        
        if(attrSet != null) {
            plptool.Msg.D("we have " + attrSet.length + " memory visualizers saved in project driver.", 4, this);

            for(int i = 0; i < attrSet.length; i++) {
                plptool.Msg.D("attaching memory visualizer " + String.format("(%08x-%08x)", attrSet[i][0], attrSet[i][1]), 3, this);
                plptool.mips.visualizer.MemoryVisualization memvis = new plptool.mips.visualizer.MemoryVisualization(plp);
                memvis.setAddresses((Long[]) attrSet[i]);
                memvis.visualize();
                attachMemoryVisualizer(memvis);
                memvis.setVisible(true);
            }
        }

        updateComponents();

       simCLOutput.setDocument(new HTMLDocument());
       simCLOutput.setEditorKit(new HTMLEditorKit());

       updateFontSize();

        /*** 4.0 RELEASE ***/
        //coreMainPane.setSelectedIndex(2);
        coreMainPane.remove(0);
        coreMainPane.setSelectedIndex(1);
        /*******************/
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jLabel1 = new javax.swing.JLabel();
        PC = new javax.swing.JTextField();
        jLabel2 = new javax.swing.JLabel();
        nextInstr = new javax.swing.JTextField();
        coreMainPane = new javax.swing.JTabbedPane();
        coreVisPane = new javax.swing.JPanel();
        lblVis = new javax.swing.JLabel();
        coreRegFilePane = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        tblRegFile = new javax.swing.JTable();
        coreProgramPane = new javax.swing.JPanel();
        jScrollPane3 = new javax.swing.JScrollPane();
        tblProgram = new javax.swing.JTable();
        coreMemMapPane = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        tblMemMap = new javax.swing.JTable();
        coreSimOptsPane = new javax.swing.JPanel();
        lblArchOpts = new javax.swing.JLabel();
        chkEXEXFwdR = new javax.swing.JCheckBox();
        chkMEMEXFwdR = new javax.swing.JCheckBox();
        lblBranchPrdction = new javax.swing.JLabel();
        rdioBrAlways = new javax.swing.JRadioButton();
        rdioBrNever = new javax.swing.JRadioButton();
        rdioBrLast = new javax.swing.JRadioButton();
        rdioBrRandom = new javax.swing.JRadioButton();
        chkMEMEXFwdLW = new javax.swing.JCheckBox();
        coreConsolePane = new javax.swing.JPanel();
        jLabel5 = new javax.swing.JLabel();
        simCLConsole = new javax.swing.JTextField();
        simCLExec = new javax.swing.JButton();
        simCLClear = new javax.swing.JButton();
        jScrollPane4 = new javax.swing.JScrollPane();
        simCLOutput = new javax.swing.JTextPane();

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(SimCoreGUI.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N

        jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
        jLabel1.setName("jLabel1"); // NOI18N

        PC.setEditable(false);
        PC.setText(resourceMap.getString("PC.text")); // NOI18N
        PC.setName("PC"); // NOI18N

        jLabel2.setText(resourceMap.getString("jLabel2.text")); // NOI18N
        jLabel2.setName("jLabel2"); // NOI18N

        nextInstr.setEditable(false);
        nextInstr.setText(resourceMap.getString("nextInstr.text")); // NOI18N
        nextInstr.setName("nextInstr"); // NOI18N

        coreMainPane.setName("coreMainPane"); // NOI18N

        coreVisPane.setName("coreVisPane"); // NOI18N

        lblVis.setText(resourceMap.getString("lblVis.text")); // NOI18N
        lblVis.setName("lblVis"); // NOI18N

        javax.swing.GroupLayout coreVisPaneLayout = new javax.swing.GroupLayout(coreVisPane);
        coreVisPane.setLayout(coreVisPaneLayout);
        coreVisPaneLayout.setHorizontalGroup(
            coreVisPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreVisPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(lblVis)
                .addContainerGap(805, Short.MAX_VALUE))
        );
        coreVisPaneLayout.setVerticalGroup(
            coreVisPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreVisPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(lblVis)
                .addContainerGap(431, Short.MAX_VALUE))
        );

        coreMainPane.addTab(resourceMap.getString("coreVisPane.TabConstraints.tabTitle"), coreVisPane); // NOI18N

        coreRegFilePane.setName("coreRegFilePane"); // NOI18N

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        tblRegFile.setFont(resourceMap.getFont("tblRegFile.font")); // NOI18N
        tblRegFile.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null},
                {null, null, null}
            },
            new String [] {
                "Register", "Contents", "Edit Contents"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.String.class, java.lang.Long.class, java.lang.String.class
            };
            boolean[] canEdit = new boolean [] {
                false, false, true
            };

            public Class getColumnClass(int columnIndex) {
                return types [columnIndex];
            }

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblRegFile.setColumnSelectionAllowed(true);
        tblRegFile.setName("tblRegFile"); // NOI18N
        tblRegFile.getTableHeader().setReorderingAllowed(false);
        tblRegFile.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                tblRegFileKeyPressed(evt);
            }
        });
        jScrollPane1.setViewportView(tblRegFile);
        tblRegFile.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_SELECTION);
        tblRegFile.getColumnModel().getColumn(0).setResizable(false);
        tblRegFile.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblRegFile.columnModel.title0")); // NOI18N
        tblRegFile.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblRegFile.columnModel.title1")); // NOI18N
        tblRegFile.getColumnModel().getColumn(2).setHeaderValue(resourceMap.getString("tblRegFile.columnModel.title2")); // NOI18N

        javax.swing.GroupLayout coreRegFilePaneLayout = new javax.swing.GroupLayout(coreRegFilePane);
        coreRegFilePane.setLayout(coreRegFilePaneLayout);
        coreRegFilePaneLayout.setHorizontalGroup(
            coreRegFilePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreRegFilePaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 1023, Short.MAX_VALUE)
                .addContainerGap())
        );
        coreRegFilePaneLayout.setVerticalGroup(
            coreRegFilePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreRegFilePaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 434, Short.MAX_VALUE)
                .addContainerGap())
        );

        coreMainPane.addTab(resourceMap.getString("coreRegFilePane.TabConstraints.tabTitle"), coreRegFilePane); // NOI18N

        coreProgramPane.setName("coreProgramPane"); // NOI18N

        jScrollPane3.setName("jScrollPane3"); // NOI18N

        tblProgram.setFont(resourceMap.getFont("tblProgram.font")); // NOI18N
        tblProgram.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "PC", "Breakpoint", "Address", "Instruction (Hex)", "Instruction"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.Object.class, java.lang.Boolean.class, java.lang.String.class, java.lang.String.class, java.lang.String.class
            };
            boolean[] canEdit = new boolean [] {
                false, true, false, false, false
            };

            public Class getColumnClass(int columnIndex) {
                return types [columnIndex];
            }

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblProgram.setName("tblProgram"); // NOI18N
        tblProgram.getTableHeader().setReorderingAllowed(false);
        jScrollPane3.setViewportView(tblProgram);
        tblProgram.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblProgram.columnModel.title0")); // NOI18N
        tblProgram.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblProgram.columnModel.title1")); // NOI18N
        tblProgram.getColumnModel().getColumn(2).setHeaderValue(resourceMap.getString("tblProgram.columnModel.title2")); // NOI18N
        tblProgram.getColumnModel().getColumn(3).setHeaderValue(resourceMap.getString("tblProgram.columnModel.title3")); // NOI18N
        tblProgram.getColumnModel().getColumn(4).setHeaderValue(resourceMap.getString("tblProgram.columnModel.title4")); // NOI18N

        javax.swing.GroupLayout coreProgramPaneLayout = new javax.swing.GroupLayout(coreProgramPane);
        coreProgramPane.setLayout(coreProgramPaneLayout);
        coreProgramPaneLayout.setHorizontalGroup(
            coreProgramPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreProgramPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane3, javax.swing.GroupLayout.DEFAULT_SIZE, 1023, Short.MAX_VALUE)
                .addContainerGap())
        );
        coreProgramPaneLayout.setVerticalGroup(
            coreProgramPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreProgramPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane3, javax.swing.GroupLayout.DEFAULT_SIZE, 434, Short.MAX_VALUE)
                .addContainerGap())
        );

        coreMainPane.addTab(resourceMap.getString("coreProgramPane.TabConstraints.tabTitle"), coreProgramPane); // NOI18N

        coreMemMapPane.setName("coreMemMapPane"); // NOI18N

        jScrollPane2.setName("jScrollPane2"); // NOI18N

        tblMemMap.setFont(resourceMap.getFont("tblMemMap.font")); // NOI18N
        tblMemMap.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Index", "Module Name", "Start Address", "End Address", "Enabled"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.String.class, java.lang.String.class, java.lang.String.class, java.lang.String.class, java.lang.Boolean.class
            };
            boolean[] canEdit = new boolean [] {
                false, false, false, false, true
            };

            public Class getColumnClass(int columnIndex) {
                return types [columnIndex];
            }

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblMemMap.setName("tblMemMap"); // NOI18N
        tblMemMap.getTableHeader().setReorderingAllowed(false);
        tblMemMap.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                tblMemMapMouseClicked(evt);
            }
            public void mousePressed(java.awt.event.MouseEvent evt) {
                tblMemMapMousePressed(evt);
            }
        });
        jScrollPane2.setViewportView(tblMemMap);
        tblMemMap.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblMemMap.columnModel.title0")); // NOI18N
        tblMemMap.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblMemMap.columnModel.title1")); // NOI18N
        tblMemMap.getColumnModel().getColumn(2).setHeaderValue(resourceMap.getString("tblMemMap.columnModel.title2")); // NOI18N
        tblMemMap.getColumnModel().getColumn(3).setHeaderValue(resourceMap.getString("tblMemMap.columnModel.title3")); // NOI18N
        tblMemMap.getColumnModel().getColumn(4).setHeaderValue(resourceMap.getString("tblMemMap.columnModel.title4")); // NOI18N

        javax.swing.GroupLayout coreMemMapPaneLayout = new javax.swing.GroupLayout(coreMemMapPane);
        coreMemMapPane.setLayout(coreMemMapPaneLayout);
        coreMemMapPaneLayout.setHorizontalGroup(
            coreMemMapPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreMemMapPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 1023, Short.MAX_VALUE)
                .addContainerGap())
        );
        coreMemMapPaneLayout.setVerticalGroup(
            coreMemMapPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreMemMapPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane2, javax.swing.GroupLayout.DEFAULT_SIZE, 434, Short.MAX_VALUE)
                .addContainerGap())
        );

        coreMainPane.addTab(resourceMap.getString("coreMemMapPane.TabConstraints.tabTitle"), coreMemMapPane); // NOI18N

        coreSimOptsPane.setName("coreSimOptsPane"); // NOI18N

        lblArchOpts.setText(resourceMap.getString("lblArchOpts.text")); // NOI18N
        lblArchOpts.setName("lblArchOpts"); // NOI18N

        chkEXEXFwdR.setSelected(true);
        chkEXEXFwdR.setText(resourceMap.getString("chkEXEXFwdR.text")); // NOI18N
        chkEXEXFwdR.setName("chkEXEXFwdR"); // NOI18N
        chkEXEXFwdR.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                chkEXEXFwdRStateChanged(evt);
            }
        });

        chkMEMEXFwdR.setSelected(true);
        chkMEMEXFwdR.setText(resourceMap.getString("chkMEMEXFwdR.text")); // NOI18N
        chkMEMEXFwdR.setName("chkMEMEXFwdR"); // NOI18N
        chkMEMEXFwdR.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                chkMEMEXFwdRStateChanged(evt);
            }
        });

        lblBranchPrdction.setText(resourceMap.getString("lblBranchPrdction.text")); // NOI18N
        lblBranchPrdction.setName("lblBranchPrdction"); // NOI18N

        rdioBrAlways.setText(resourceMap.getString("rdioBrAlways.text")); // NOI18N
        rdioBrAlways.setEnabled(false);
        rdioBrAlways.setName("rdioBrAlways"); // NOI18N

        rdioBrNever.setSelected(true);
        rdioBrNever.setText(resourceMap.getString("rdioBrNever.text")); // NOI18N
        rdioBrNever.setEnabled(false);
        rdioBrNever.setName("rdioBrNever"); // NOI18N

        rdioBrLast.setText(resourceMap.getString("rdioBrLast.text")); // NOI18N
        rdioBrLast.setEnabled(false);
        rdioBrLast.setName("rdioBrLast"); // NOI18N

        rdioBrRandom.setText(resourceMap.getString("rdioBrRandom.text")); // NOI18N
        rdioBrRandom.setEnabled(false);
        rdioBrRandom.setName("rdioBrRandom"); // NOI18N

        chkMEMEXFwdLW.setSelected(true);
        chkMEMEXFwdLW.setText(resourceMap.getString("chkMEMEXFwdLW.text")); // NOI18N
        chkMEMEXFwdLW.setName("chkMEMEXFwdLW"); // NOI18N
        chkMEMEXFwdLW.addChangeListener(new javax.swing.event.ChangeListener() {
            public void stateChanged(javax.swing.event.ChangeEvent evt) {
                chkMEMEXFwdLWStateChanged(evt);
            }
        });

        javax.swing.GroupLayout coreSimOptsPaneLayout = new javax.swing.GroupLayout(coreSimOptsPane);
        coreSimOptsPane.setLayout(coreSimOptsPaneLayout);
        coreSimOptsPaneLayout.setHorizontalGroup(
            coreSimOptsPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreSimOptsPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(coreSimOptsPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(chkMEMEXFwdLW)
                    .addComponent(chkMEMEXFwdR)
                    .addComponent(lblArchOpts)
                    .addComponent(chkEXEXFwdR)
                    .addComponent(rdioBrRandom)
                    .addComponent(rdioBrLast)
                    .addComponent(rdioBrNever)
                    .addComponent(lblBranchPrdction)
                    .addComponent(rdioBrAlways))
                .addContainerGap(672, Short.MAX_VALUE))
        );
        coreSimOptsPaneLayout.setVerticalGroup(
            coreSimOptsPaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreSimOptsPaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(lblArchOpts)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkEXEXFwdR)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkMEMEXFwdR)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkMEMEXFwdLW)
                .addGap(18, 18, 18)
                .addComponent(lblBranchPrdction)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(rdioBrAlways)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(rdioBrNever)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(rdioBrLast)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(rdioBrRandom)
                .addContainerGap(233, Short.MAX_VALUE))
        );

        coreMainPane.addTab(resourceMap.getString("coreSimOptsPane.TabConstraints.tabTitle"), coreSimOptsPane); // NOI18N

        coreConsolePane.setName("coreConsolePane"); // NOI18N

        jLabel5.setText(resourceMap.getString("jLabel5.text")); // NOI18N
        jLabel5.setName("jLabel5"); // NOI18N

        simCLConsole.setText(resourceMap.getString("simCLConsole.text")); // NOI18N
        simCLConsole.setName("simCLConsole"); // NOI18N
        simCLConsole.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                simCLConsoleKeyPressed(evt);
            }
        });

        simCLExec.setText(resourceMap.getString("simCLExec.text")); // NOI18N
        simCLExec.setName("simCLExec"); // NOI18N
        simCLExec.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                simCLExecActionPerformed(evt);
            }
        });

        simCLClear.setText(resourceMap.getString("simCLClear.text")); // NOI18N
        simCLClear.setName("simCLClear"); // NOI18N
        simCLClear.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                simCLClearActionPerformed(evt);
            }
        });

        jScrollPane4.setName("jScrollPane4"); // NOI18N

        simCLOutput.setFont(resourceMap.getFont("simCLOutput.font")); // NOI18N
        simCLOutput.setName("simCLOutput"); // NOI18N
        jScrollPane4.setViewportView(simCLOutput);

        javax.swing.GroupLayout coreConsolePaneLayout = new javax.swing.GroupLayout(coreConsolePane);
        coreConsolePane.setLayout(coreConsolePaneLayout);
        coreConsolePaneLayout.setHorizontalGroup(
            coreConsolePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(coreConsolePaneLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(coreConsolePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane4, javax.swing.GroupLayout.DEFAULT_SIZE, 1023, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, coreConsolePaneLayout.createSequentialGroup()
                        .addComponent(simCLConsole, javax.swing.GroupLayout.DEFAULT_SIZE, 851, Short.MAX_VALUE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(simCLExec)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(simCLClear))
                    .addComponent(jLabel5))
                .addContainerGap())
        );
        coreConsolePaneLayout.setVerticalGroup(
            coreConsolePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, coreConsolePaneLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel5)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane4, javax.swing.GroupLayout.DEFAULT_SIZE, 376, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(coreConsolePaneLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(simCLClear)
                    .addComponent(simCLExec)
                    .addComponent(simCLConsole, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );

        coreMainPane.addTab(resourceMap.getString("coreConsolePane.TabConstraints.tabTitle"), coreConsolePane); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(jLabel1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(PC, javax.swing.GroupLayout.PREFERRED_SIZE, 94, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jLabel2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(nextInstr, javax.swing.GroupLayout.DEFAULT_SIZE, 678, Short.MAX_VALUE))
                    .addComponent(coreMainPane, javax.swing.GroupLayout.DEFAULT_SIZE, 1052, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(PC, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(nextInstr, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel2))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(coreMainPane, javax.swing.GroupLayout.DEFAULT_SIZE, 485, Short.MAX_VALUE)
                .addGap(12, 12, 12))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void simCLClearActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_simCLClearActionPerformed
        // TODO add your handling code here:
        simCLOutput.setText("");
}//GEN-LAST:event_simCLClearActionPerformed

    private void simCLExecActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_simCLExecActionPerformed
        plptool.Msg.setOutput(simCLOutput);
        lastCLCommand = simCLConsole.getText();
        appendOutput("exec: " + simCLConsole.getText() + "\n");
        SimCLI.simCLCommand(simCLConsole.getText(), plp);
        if(simCLConsole.getText().trim().startsWith("asm")) {
            clearProgramMemoryTable();
            fillProgramMemoryTable();
        }
        appendOutput("\n");
        simCLConsole.setText("");
        updateComponents();
        plptool.Msg.setOutput(plp.g_dev.getOutput());
}//GEN-LAST:event_simCLExecActionPerformed


    private void appendOutput(String txt) {
        simCLOutput.setText(simCLOutput.getText() + txt);
    }

    private void simCLConsoleKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_simCLConsoleKeyPressed
        if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_ENTER)
            simCLExecActionPerformed(null);
        else if(simCLConsole.getText().equals("") && evt.getKeyCode() == java.awt.event.KeyEvent.VK_UP)
            simCLConsole.setText(lastCLCommand);
}//GEN-LAST:event_simCLConsoleKeyPressed

    private void formInternalFrameActivated(javax.swing.event.InternalFrameEvent evt) {//GEN-FIRST:event_formInternalFrameActivated
    }//GEN-LAST:event_formInternalFrameActivated

    private void tblRegFileKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_tblRegFileKeyPressed

    }//GEN-LAST:event_tblRegFileKeyPressed

    private void tblMemMapMousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_tblMemMapMousePressed

    }//GEN-LAST:event_tblMemMapMousePressed

    private void tblMemMapMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_tblMemMapMouseClicked
        for(int row = 0; row < tblMemMap.getRowCount(); row++) {
            if((Boolean) tblMemMap.getModel().getValueAt(row, 4))
                plp.sim.bus.enableMod(row);
            else
                plp.sim.bus.disableMod(row);
        }
    }//GEN-LAST:event_tblMemMapMouseClicked

    private void chkEXEXFwdRStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_chkEXEXFwdRStateChanged
        if(chkEXEXFwdR.isSelected())
            ((SimCore) plp.sim).ex_ex = true;
        else
            ((SimCore) plp.sim).ex_ex = false;
    }//GEN-LAST:event_chkEXEXFwdRStateChanged

    private void chkMEMEXFwdRStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_chkMEMEXFwdRStateChanged
        if(chkMEMEXFwdR.isSelected())
            ((SimCore) plp.sim).mem_ex = true;
        else
            ((SimCore) plp.sim).mem_ex = false;
    }//GEN-LAST:event_chkMEMEXFwdRStateChanged

    private void chkMEMEXFwdLWStateChanged(javax.swing.event.ChangeEvent evt) {//GEN-FIRST:event_chkMEMEXFwdLWStateChanged
        if(chkMEMEXFwdLW.isSelected())
            ((SimCore) plp.sim).mem_ex_lw = true;
        else
            ((SimCore) plp.sim).mem_ex_lw = false;
    }//GEN-LAST:event_chkMEMEXFwdLWStateChanged

    public final void updateFontSize() {
        tblRegFile.setFont(tblRegFile.getFont().deriveFont(Config.devFontSize + 0.0f));
        tblRegFile.setRowHeight(tblRegFile.getFontMetrics(tblRegFile.getFont()).getHeight() + 5);
        tblProgram.setFont(tblProgram.getFont().deriveFont(Config.devFontSize + 0.0f));
        tblProgram.setRowHeight(tblProgram.getFontMetrics(tblProgram.getFont()).getHeight() + 5);
        tblMemMap.setFont(tblMemMap.getFont().deriveFont(Config.devFontSize + 0.0f));
        tblMemMap.setRowHeight(tblMemMap.getFontMetrics(tblMemMap.getFont()).getHeight() + 5);
    }
    
    private void updateRegisters() {

        for(int regIndex = 0; regIndex < 32; regIndex++) {
            String val = (String) tblRegFile.getModel().getValueAt(regIndex, 2);
            plptool.Msg.lastError = 0;
            long longVal = PLPToolbox.parseNum(val);
            if(plptool.Msg.lastError == plptool.Constants.PLP_OK)
                ((SimCore) plp.sim).regfile.write(regIndex, longVal, false);
        }

        updateComponents();
    }

    @Override
    public final void updateBusTable() {
        javax.swing.table.DefaultTableModel memMap = (javax.swing.table.DefaultTableModel) tblMemMap.getModel();

        while(memMap.getRowCount() > 0)
                memMap.removeRow(0);

        for(int i = 0; i < sim.bus.getNumOfMods(); i++) {
            long startAddr = sim.bus.getModStartAddress(i);
            long endAddr = sim.bus.getModEndAddress(i);
            Object row[] = new Object[] {i, sim.bus.getRefMod(i),
                                         (startAddr < 0 ? "Unmapped" : String.format("0x%08x", startAddr)),
                                         (endAddr < 0 ? "Unmapped" : String.format("0x%08x", endAddr)),
                                         sim.bus.getEnabled(i) };
            memMap.addRow(row);
        }
        tblMemMap.setModel(memMap);
    }

    @Override
    public final void updateComponents() {
        long pc = ((SimCore)sim).id_stage.i_instrAddr;
        if(pc >= 0) {
            PC.setText(String.format("0x%08x", pc));
            nextInstr.setText(MIPSInstr.format(((SimCore)sim).id_stage.i_instruction));
        } else {
            PC.setText(String.format("0xXXXXXXXX"));
            nextInstr.setText("STALL");
        }

        for(int i = 0; i < 32; i++) {
            tblRegFile.setValueAt(((SimCore)sim).regfile.read(i).toString(), i, 2);
            tblRegFile.setValueAt(String.format("0x%08x", ((SimCore)sim).regfile.read(i) & 0xffffffffL), i, 1);
        }

        for(int i = 0; i < memoryVisualizers.size(); i++)
            memoryVisualizers.get(i).updateVisualization();

        if(cpuVisualizer != null && cpuVisualizer.isVisible())
            cpuVisualizer.update();

        updateProgramMemoryTablePC();
    }

    /**
     * Attach a memory visualization frame
     *
     * @param memVis Reference to the memory visualizer
     */
    public void attachMemoryVisualizer(MemoryVisualization memVis) {
        plptool.Msg.D("Attaching a memory visualizer", 4, this);
        memVis.setFrameID(memoryVisualizers.size());
        memoryVisualizers.add(memVis);
        updateAttributeForMemoryVisualizers();
    }

    public void disposeMemoryVisualizers() {
        for(int i = 0; i < memoryVisualizers.size(); i++) {
            plptool.Msg.D("Disposing memory visualizer " + i, 4, this);
            memoryVisualizers.get(i).dispose();
        }

        memoryVisualizers.clear();
    }

    public void disposeMemoryVisualizer(int index) {
        memoryVisualizers.get(index).dispose();
        for(int i = index + 1; i < memoryVisualizers.size(); i++)
            memoryVisualizers.get(i).setFrameID(i - 1);
        memoryVisualizers.remove(index);
        updateAttributeForMemoryVisualizers();
    }

    public void updateAttributeForMemoryVisualizers() {
        plp.deleteProjectAttribute("plpmips_memory_visualizer");
        Object[][] attrSet = new Object[memoryVisualizers.size()][];
        for(int i = 0; i < memoryVisualizers.size(); i++) {
            attrSet[i] = memoryVisualizers.get(i).getAddresses();
            plptool.Msg.D("adding attribute " + String.format("(%08x-%08x)", attrSet[i][0], attrSet[i][1]), 3, this);
        }

        if(memoryVisualizers.size() > 0)
            plp.addProjectAttribute("plpmips_memory_visualizer", attrSet);
    }

    public ArrayList<MemoryVisualization> getMemoryVisualizerArray() {
        return memoryVisualizers;
    }

    public void attachCPUVisualizer(CPUVisualization cpuVisualizer) {
        this.cpuVisualizer = cpuVisualizer;
    }

    /** release build features disabler **/
    public final void disableFeatures() {
        coreVisPane.setVisible(false);
    }

    public final void clearProgramMemoryTable() {
        javax.swing.table.DefaultTableModel program = (javax.swing.table.DefaultTableModel) tblProgram.getModel();

        while(program.getRowCount() > 0)
            program.removeRow(0);

        tblProgram.setModel(program);
    }

    public final void fillProgramMemoryTable() {
        javax.swing.table.DefaultTableModel program = (javax.swing.table.DefaultTableModel) tblProgram.getModel();
        int mainMemIndex = -1;

        for(int i = 0; i < plp.ioreg.getNumOfModsAttached(); i++) {
            if(plp.ioreg.getType(i) == 0 && plp.ioreg.getStartAddr(i) != 0) {
                mainMemIndex = i;
            }
        }

        if(mainMemIndex < 0) {
            return;
        }

        Object[][] objCode = ((plptool.mods.MemModule)plp.ioreg.getModule(mainMemIndex)).getValueSet();
        Object row[];
        for(int i = 0; i < objCode.length; i++) {
            if((Boolean) objCode[i][2]) {
                row = new Object[]
                      {"", false,
                       String.format("0x%08x", objCode[i][0]),
                       String.format("0x%08x", objCode[i][1]),
                       MIPSInstr.format((Long) objCode[i][1])};
                program.addRow(row);
            }
        }

        tblProgram.setModel(program);
    }

    public final void updateProgramMemoryTablePC() {
        if(((SimCore)sim).pc.eval() != old_pc) {
            javax.swing.table.DefaultTableModel program = (javax.swing.table.DefaultTableModel) tblProgram.getModel();

            for(int i = 0; i < program.getRowCount(); i++) {
                if(((SimCore)sim).id_stage.instrAddr == Long.parseLong(((String) program.getValueAt(i, 2)).substring(2), 16) &&
                   ((SimCore)sim).ex_stage.hot)
                    program.setValueAt("ID>", i, 0);
                else if(((SimCore)sim).ex_stage.instrAddr == Long.parseLong(((String) program.getValueAt(i, 2)).substring(2), 16) &&
                   ((SimCore)sim).mem_stage.hot)
                    program.setValueAt("EX>", i, 0);
                else if(((SimCore)sim).mem_stage.instrAddr == Long.parseLong(((String) program.getValueAt(i, 2)).substring(2), 16) &&
                   ((SimCore)sim).wb_stage.hot)
                    program.setValueAt("MEM>", i, 0);
                else if(((SimCore)sim).wb_stage.instrAddr == Long.parseLong(((String) program.getValueAt(i, 2)).substring(2), 16) &&
                   ((SimCore)sim).wb_stage.instr_retired)
                    program.setValueAt("WB>", i, 0);
                else
                    program.setValueAt("", i, 0);

                if(PC.getText().equals(program.getValueAt(i, 2)) && !((SimCore)sim).if_stall) {
                    program.setValueAt("IF>>>", i, 0);
                    if(((Boolean) program.getValueAt(i, 1)) == true)
                        plp.g_simrun.stepCount = -1;
                }
            }

            tblProgram.setModel(program);
            
            old_pc = ((SimCore)sim).pc.eval();
        }
    }

    public JTable getRegFileTable()  {
        return tblRegFile;
    }

    public DefaultTableModel getRegFileValues() {
        return (DefaultTableModel) tblRegFile.getModel();
    }

    @Override
    public String toString() {
        return "plptool.mips.SimCoreGUI";
    }

    private javax.swing.table.DefaultTableCellRenderer renderer = new DefaultTableCellRenderer();

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextField PC;
    private javax.swing.JCheckBox chkEXEXFwdR;
    private javax.swing.JCheckBox chkMEMEXFwdLW;
    private javax.swing.JCheckBox chkMEMEXFwdR;
    private javax.swing.JPanel coreConsolePane;
    private javax.swing.JTabbedPane coreMainPane;
    private javax.swing.JPanel coreMemMapPane;
    private javax.swing.JPanel coreProgramPane;
    private javax.swing.JPanel coreRegFilePane;
    private javax.swing.JPanel coreSimOptsPane;
    private javax.swing.JPanel coreVisPane;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JScrollPane jScrollPane4;
    private javax.swing.JLabel lblArchOpts;
    private javax.swing.JLabel lblBranchPrdction;
    private javax.swing.JLabel lblVis;
    private javax.swing.JTextField nextInstr;
    private javax.swing.JRadioButton rdioBrAlways;
    private javax.swing.JRadioButton rdioBrLast;
    private javax.swing.JRadioButton rdioBrNever;
    private javax.swing.JRadioButton rdioBrRandom;
    private javax.swing.JButton simCLClear;
    private javax.swing.JTextField simCLConsole;
    private javax.swing.JButton simCLExec;
    private javax.swing.JTextPane simCLOutput;
    private javax.swing.JTable tblMemMap;
    private javax.swing.JTable tblProgram;
    private javax.swing.JTable tblRegFile;
    // End of variables declaration//GEN-END:variables

    class CustomCellTextField extends JTextField {

        public CustomCellTextField(SimCoreGUI g_sim) {
            super();

            addFocusListener(
                new CellFocusListener(g_sim)
            );
        }
    }

    class CustomCellEditor extends DefaultCellEditor {
        CustomCellTextField textField;

        public CustomCellEditor(CustomCellTextField textField) {
            super(textField);
            this.textField = textField;
        }
    }

    class CellFocusListener implements FocusListener {

        private SimCoreGUI g_sim;
        private DefaultTableModel values;
        private int row;
        private int col;
        Object oldVal;

        public CellFocusListener(SimCoreGUI g_sim) {
            this.g_sim = g_sim;
        }

        public void focusGained(FocusEvent e) {
            row = g_sim.getRegFileTable().getSelectedRow();
            col = g_sim.getRegFileTable().getSelectedColumn();
            oldVal = g_sim.getRegFileValues().getValueAt(row, col);
        }

        public void focusLost(FocusEvent e) {
            // update simulator state
            values = g_sim.getRegFileValues();

            long newVal = PLPToolbox.parseNum((String) values.getValueAt(row, col));
            if(newVal == -1) {
                values.setValueAt(oldVal, row, col);
                g_sim.getRegFileTable().setModel(values);
                return;

            }  else
                ((plptool.mips.SimCore) (plp.sim)).regfile.write(row, newVal, false);

            plp.updateComponents(true);
        }
    }
}
