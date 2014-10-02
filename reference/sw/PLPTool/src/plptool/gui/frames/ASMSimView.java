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

package plptool.gui.frames;

import plptool.Config;
import plptool.PLPToolbox;
import plptool.Constants;
import javax.swing.table.DefaultTableModel;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class ASMSimView extends javax.swing.JFrame {

    private ProjectDriver plp;

    /** Creates new form ASMSimView */
    public ASMSimView(ProjectDriver plp) {
        initComponents();

        this.plp = plp;
        //plp.g_simsh.attachOptionSynchronizer(this, Constants.PLP_TOOLFRAME_ASMVIEW);
        updateTable();
        updateFontSize();
    }

    public final void updateFontSize() {
        tblASM.setFont(tblASM.getFont().deriveFont(Config.devFontSize + 0.0f));
        tblASM.setRowHeight(tblASM.getFontMetrics(tblASM.getFont()).getHeight() + 5);
    }

    public final void updateTable() {
        if(!plp.isAssembled()) {
            setTitle("Program Listing - Project is not assembled");
            return;
        } else
            setTitle("Program Listing");
        
        DefaultTableModel model = new DefaultTableModel() {
            @Override
            public boolean isCellEditable(int row, int col) {
                return false;
            }
        };

        while(model.getRowCount() > 0)
            model.removeRow(0);

        boolean[] dispType = new boolean[5];
        dispType[0] = chkHex.isSelected();
        dispType[1] = chkASCII.isSelected();
        dispType[2] = chkDecimal.isSelected();
        dispType[3] = chkBinary.isSelected();
        dispType[4] = chkSource.isSelected();

        int curF, curL;
        int prevF = -1, prevL = -1;

        long[] objCode = plp.asm.getObjectCode();
        String dispSource, dispHex, dispDecimal, dispBinary, dispASCII;
        int cols = 3;
        for(int i = 0; i < 5; i++)
            cols += (dispType[i] ? 1 : 0);

        model.addColumn("Label");
        model.addColumn("Address");
        for(int j = 2; j < cols-1; j++) {
            if(dispType[0]) {
                model.addColumn("Hex");
                dispType[0] = false;
            } else if (dispType[1]) {
                model.addColumn("ASCII");
                dispType[1] = false;
            } else if (dispType[2]) {
                model.addColumn("Dec");
                dispType[2] = false;
            } else if (dispType[3]) {
                model.addColumn("Bin");
                dispType[3] = false;
            } else if (dispType[4]) {
                model.addColumn("Source");
                dispType[4] = false;
            }
        }
        model.addColumn("File:Line");

        for(int i = 0; i < objCode.length; i++) {
            curF = plp.asm.getFileMapper()[i];
            curL = plp.asm.getLineNumMapper()[i];
            if(curF == prevF && curL == prevL)
                dispSource = "^";
            else
                dispSource = plp.asm.getAsmList().get(curF).getAsmLine(curL);
            prevF = curF;
            prevL = curL;
            dispHex = PLPToolbox.format32Hex(objCode[i]);
            dispDecimal = "" + objCode[i];
            dispBinary = String.format("%32s", Long.toBinaryString(objCode[i])).replace(" ", "0");
            dispASCII = PLPToolbox.asciiWord(objCode[i]);
            Object[] row = new Object[cols];
            row[0] =
                (plp.asm.lookupLabel(plp.asm.getAddrTable()[i]) != null ? plp.asm.lookupLabel(plp.asm.getAddrTable()[i]) : "");
            row[1] =
                String.format("0x%08x", plp.asm.getAddrTable()[i]) +
                        " " + ((plp.sim != null) ?
                            ((((plptool.mips.SimCore) plp.sim).pc.eval() == plp.asm.getAddrTable()[i]) ? "[PC]" : "")
                            : "");
            
            dispType[0] = chkHex.isSelected();
            dispType[1] = chkASCII.isSelected();
            dispType[2] = chkDecimal.isSelected();
            dispType[3] = chkBinary.isSelected();
            dispType[4] = chkSource.isSelected();

            for(int j = 2; j < cols-1; j++) {
                if(dispType[0]) {
                    row[j] = dispHex;
                    dispType[0] = false;
                } else if (dispType[1]) {
                    row[j] = dispASCII;
                    dispType[1] = false;
                } else if (dispType[2]) {
                    row[j] = dispDecimal;
                    dispType[2] = false;
                } else if (dispType[3]) {
                    row[j] = dispBinary;
                    dispType[3] = false;
                } else if (dispType[4]) {
                    row[j] = dispSource;
                    dispType[4] = false;
                }
            }
            row[cols-1] = plp.asm.getAsmList().get(plp.asm.getFileMapper()[i]).getAsmFilePath() +
                    ":" + String.valueOf(plp.asm.getLineNumMapper()[i]);

            model.addRow(row);
        }
        tblASM.setModel(model);
    }

    public final void updatePC() {
        DefaultTableModel model = (DefaultTableModel) tblASM.getModel();

        long[] objCode = plp.asm.getObjectCode();

        for(int i = 0; i < objCode.length; i++) {
            if(((plptool.mips.SimCore) plp.sim).pc.eval() == plp.asm.getAddrTable()[i]) {
                model.setValueAt(String.format("0x%08x",plp.asm.getAddrTable()[i]) + " [PC]", i, Constants.ASMVIEW_ADDR);
            } else {
                model.setValueAt(String.format("0x%08x",plp.asm.getAddrTable()[i]), i, Constants.ASMVIEW_ADDR);
            }
        }
    }

    public void copyTable(int start, int end) {
        String[] labels = new String[tblASM.getRowCount()];
        String[] source = new String[tblASM.getRowCount()];
        String out = "";
        String t;

        if(chkIncludeLabels.isSelected() || chkIncludeFile.isSelected()) {
            for(int i = start; i <= end; i++) {
                if(chkIncludeLabels.isSelected()) {
                    t = (String) tblASM.getValueAt(i, 0);
                    if(t.length() > 24)
                        labels[i] = t.substring(0, 21) + "...";
                    else {
                        labels[i] = t;
                        for(int j = 0; j < 24-t.length(); j++)
                            labels[i] += " ";
                    }
                }
                if(chkIncludeFile.isSelected()) {
                    t = (String) tblASM.getValueAt(i, tblASM.getColumnCount()-1);
                    if(t.length() > 24)
                        source[i] = t.substring(0, 21) + "...";
                    else {
                        source[i] = t;
                        for(int j = 0; j < 24-t.length(); j++)
                            source[i] += " ";
                    }
                }
            }
        }

        if(chkIncludeHeader.isSelected()) {
            out += "PLPTool " + plptool.Text.versionString + "\n";
            out += "Project: " + plp.plpfile.getAbsolutePath() + "\n\n";
        }

        t = "";
        for(int i = 0; i < 24; i++)
            t += " ";

        for(int i = start; i <= end; i++) {
            out += !chkIncludeFile.isSelected() ? "" :
                   i != start && source[i-1].equals(source[i]) ? t + " " : source[i] + " ";
            out += !chkIncludeLabels.isSelected() ? "" : labels[i] + " ";
            for(int j = 1; j < tblASM.getColumnCount()-1; j++) {
                if(tblASM.getColumnName(j).equals("Source"))
                    out += "| ";
                out += tblASM.getValueAt(i, j) + "\t";
            }
            out += "\n";
        }

        PLPToolbox.copy(out);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jScrollPane1 = new javax.swing.JScrollPane();
        tblASM = new javax.swing.JTable();
        lblDisplay = new javax.swing.JLabel();
        btnCopy = new javax.swing.JButton();
        chkSource = new javax.swing.JCheckBox();
        chkHex = new javax.swing.JCheckBox();
        chkDecimal = new javax.swing.JCheckBox();
        chkBinary = new javax.swing.JCheckBox();
        chkASCII = new javax.swing.JCheckBox();
        chkIncludeLabels = new javax.swing.JCheckBox();
        chkIncludeFile = new javax.swing.JCheckBox();
        chkIncludeHeader = new javax.swing.JCheckBox();
        btnCopyAll = new javax.swing.JButton();
        lblInfoMultiword = new javax.swing.JLabel();

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(ASMSimView.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        tblASM.setFont(resourceMap.getFont("tblASM.font")); // NOI18N
        tblASM.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {
                {null, null, null, null, null, null, null, null, null},
                {null, null, null, null, null, null, null, null, null},
                {null, null, null, null, null, null, null, null, null},
                {null, null, null, null, null, null, null, null, null}
            },
            new String [] {
                "Label", "Address", "Src", "Hex", "Dec", "Bin", "ASCII", "Source", "#"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.String.class, java.lang.String.class, java.lang.String.class, java.lang.Object.class, java.lang.Object.class, java.lang.Object.class, java.lang.Object.class, java.lang.String.class, java.lang.String.class
            };
            boolean[] canEdit = new boolean [] {
                false, false, false, false, false, false, false, false, false
            };

            public Class getColumnClass(int columnIndex) {
                return types [columnIndex];
            }

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblASM.setColumnSelectionAllowed(true);
        tblASM.setGridColor(resourceMap.getColor("tblASM.gridColor")); // NOI18N
        tblASM.setName("tblASM"); // NOI18N
        tblASM.setShowVerticalLines(false);
        tblASM.getTableHeader().setReorderingAllowed(false);
        jScrollPane1.setViewportView(tblASM);
        tblASM.getColumnModel().getSelectionModel().setSelectionMode(javax.swing.ListSelectionModel.SINGLE_INTERVAL_SELECTION);
        tblASM.getColumnModel().getColumn(0).setPreferredWidth(100);
        tblASM.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblASM.columnModel.title1")); // NOI18N
        tblASM.getColumnModel().getColumn(1).setPreferredWidth(100);
        tblASM.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblASM.columnModel.title0")); // NOI18N
        tblASM.getColumnModel().getColumn(2).setPreferredWidth(350);
        tblASM.getColumnModel().getColumn(2).setHeaderValue(resourceMap.getString("tblASM.columnModel.title4")); // NOI18N
        tblASM.getColumnModel().getColumn(3).setHeaderValue(resourceMap.getString("tblASM.columnModel.title5")); // NOI18N
        tblASM.getColumnModel().getColumn(4).setHeaderValue(resourceMap.getString("tblASM.columnModel.title6")); // NOI18N
        tblASM.getColumnModel().getColumn(5).setHeaderValue(resourceMap.getString("tblASM.columnModel.title7")); // NOI18N
        tblASM.getColumnModel().getColumn(6).setHeaderValue(resourceMap.getString("tblASM.columnModel.title8")); // NOI18N
        tblASM.getColumnModel().getColumn(7).setPreferredWidth(100);
        tblASM.getColumnModel().getColumn(7).setHeaderValue(resourceMap.getString("tblASM.columnModel.title2")); // NOI18N
        tblASM.getColumnModel().getColumn(8).setPreferredWidth(35);
        tblASM.getColumnModel().getColumn(8).setHeaderValue(resourceMap.getString("tblASM.columnModel.title3")); // NOI18N

        lblDisplay.setText(resourceMap.getString("lblDisplay.text")); // NOI18N
        lblDisplay.setName("lblDisplay"); // NOI18N

        btnCopy.setText(resourceMap.getString("btnCopy.text")); // NOI18N
        btnCopy.setName("btnCopy"); // NOI18N
        btnCopy.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCopyActionPerformed(evt);
            }
        });

        chkSource.setText(resourceMap.getString("chkSource.text")); // NOI18N
        chkSource.setName("chkSource"); // NOI18N
        chkSource.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                chkSourceActionPerformed(evt);
            }
        });

        chkHex.setSelected(true);
        chkHex.setText(resourceMap.getString("chkHex.text")); // NOI18N
        chkHex.setName("chkHex"); // NOI18N
        chkHex.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                chkHexActionPerformed(evt);
            }
        });

        chkDecimal.setText(resourceMap.getString("chkDecimal.text")); // NOI18N
        chkDecimal.setName("chkDecimal"); // NOI18N
        chkDecimal.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                chkDecimalActionPerformed(evt);
            }
        });

        chkBinary.setText(resourceMap.getString("chkBinary.text")); // NOI18N
        chkBinary.setName("chkBinary"); // NOI18N
        chkBinary.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                chkBinaryActionPerformed(evt);
            }
        });

        chkASCII.setSelected(true);
        chkASCII.setText(resourceMap.getString("chkASCII.text")); // NOI18N
        chkASCII.setName("chkASCII"); // NOI18N
        chkASCII.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                chkASCIIActionPerformed(evt);
            }
        });

        chkIncludeLabels.setSelected(true);
        chkIncludeLabels.setText(resourceMap.getString("chkIncludeLabels.text")); // NOI18N
        chkIncludeLabels.setName("chkIncludeLabels"); // NOI18N

        chkIncludeFile.setSelected(true);
        chkIncludeFile.setText(resourceMap.getString("chkIncludeFile.text")); // NOI18N
        chkIncludeFile.setName("chkIncludeFile"); // NOI18N

        chkIncludeHeader.setText(resourceMap.getString("chkIncludeHeader.text")); // NOI18N
        chkIncludeHeader.setName("chkIncludeHeader"); // NOI18N

        btnCopyAll.setText(resourceMap.getString("btnCopyAll.text")); // NOI18N
        btnCopyAll.setName("btnCopyAll"); // NOI18N
        btnCopyAll.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCopyAllActionPerformed(evt);
            }
        });

        lblInfoMultiword.setText(resourceMap.getString("lblInfoMultiword.text")); // NOI18N
        lblInfoMultiword.setName("lblInfoMultiword"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(lblDisplay)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkHex)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkASCII)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkDecimal)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkBinary)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkSource)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 331, Short.MAX_VALUE)
                .addComponent(lblInfoMultiword)
                .addContainerGap())
            .addGroup(layout.createSequentialGroup()
                .addComponent(btnCopy)
                .addGap(4, 4, 4)
                .addComponent(btnCopyAll)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(chkIncludeFile)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkIncludeLabels)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(chkIncludeHeader)
                .addGap(160, 160, 160))
            .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 797, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(lblDisplay)
                    .addComponent(chkHex)
                    .addComponent(chkASCII)
                    .addComponent(chkDecimal)
                    .addComponent(chkBinary)
                    .addComponent(chkSource)
                    .addComponent(lblInfoMultiword))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 385, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btnCopy)
                    .addComponent(btnCopyAll)
                    .addComponent(chkIncludeFile)
                    .addComponent(chkIncludeLabels)
                    .addComponent(chkIncludeHeader)))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void chkSourceActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_chkSourceActionPerformed
        updateTable();
    }//GEN-LAST:event_chkSourceActionPerformed

    private void chkHexActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_chkHexActionPerformed
        updateTable();
    }//GEN-LAST:event_chkHexActionPerformed

    private void chkDecimalActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_chkDecimalActionPerformed
        updateTable();
    }//GEN-LAST:event_chkDecimalActionPerformed

    private void chkBinaryActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_chkBinaryActionPerformed
        updateTable();
    }//GEN-LAST:event_chkBinaryActionPerformed

    private void chkASCIIActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_chkASCIIActionPerformed
        updateTable();
    }//GEN-LAST:event_chkASCIIActionPerformed

    private void btnCopyActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCopyActionPerformed
        int start, end;
        if(tblASM.getSelectedRowCount() == 0) {
            start = 0;
            end = tblASM.getRowCount()-1;
        } else {
            start = tblASM.getSelectedRows()[0];
            end = tblASM.getSelectedRows()[tblASM.getSelectedRows().length-1];
        }

        copyTable(start, end);
    }//GEN-LAST:event_btnCopyActionPerformed

    private void btnCopyAllActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCopyAllActionPerformed
        copyTable(0, tblASM.getRowCount()-1);
    }//GEN-LAST:event_btnCopyAllActionPerformed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnCopy;
    private javax.swing.JButton btnCopyAll;
    private javax.swing.JCheckBox chkASCII;
    private javax.swing.JCheckBox chkBinary;
    private javax.swing.JCheckBox chkDecimal;
    private javax.swing.JCheckBox chkHex;
    private javax.swing.JCheckBox chkIncludeFile;
    private javax.swing.JCheckBox chkIncludeHeader;
    private javax.swing.JCheckBox chkIncludeLabels;
    private javax.swing.JCheckBox chkSource;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel lblDisplay;
    private javax.swing.JLabel lblInfoMultiword;
    private javax.swing.JTable tblASM;
    // End of variables declaration//GEN-END:variables

}
