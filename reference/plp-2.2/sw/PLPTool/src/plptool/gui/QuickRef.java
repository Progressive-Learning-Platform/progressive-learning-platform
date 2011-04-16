/*
    Copyright 2010-2011 David Fritz, Brian Gordon, Wira Mulia

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

import javax.swing.table.DefaultTableModel;

/**
 *
 * @author wira
 */
public class QuickRef extends javax.swing.JFrame {

    /** Creates new form QuickRef */
    public QuickRef() {
        initComponents();

        this.setTitle("PLP " + plptool.Constants.versionString + " Quick Reference");

        DefaultTableModel tbl = (DefaultTableModel) tblRType.getModel();

        String[][] data = new String[11][2];

        data[0][0] = "addu $rd, $rs, $rt";      data[0][1] = "rd = rs + rt";
        data[1][0] = "subu $rd, $rs, $rt";      data[1][1] = "rd = rs - rt";
        data[2][0] = "and  $rd, $rs, $rt";      data[2][1] = "rd = rs & rt";
        data[3][0] = "or   $rd, $rs, $rt";      data[3][1] = "rd = rs | rt";
        data[4][0] = "nor  $rd, $rs, $rt";      data[4][1] = "rd = ~(rs | rt)";
        data[5][0] = "slt  $rd, $rs, $rt";      data[5][1] = "rd = (rs < rt) ? 1 : 0";
        data[6][0] = "sltu $rd, $rs, $rt";      data[6][1] = "rd = (rs < rt) ? 1 : 0";
        data[7][0] = "sll  $rd, $rt, shamt";    data[7][1] = "rd = rt << shamt";
        data[8][0] = "slr  $rd, $rt, shamt";    data[8][1] = "rd = rt << shamt";
        data[9][0] = "jr $rs";                  data[9][1] = "PC = rs";
        data[10][0] = "jalr $rd, $rs";          data[10][1] = "rd = PC + 4; PC = rs";


        for(int i = 0; i < data.length; i++)
            tbl.addRow(data[i]);
        
        tbl = (DefaultTableModel) tblIType.getModel();

        data = new String[10][2];

        data[0][0] = "addiu $rt, $rs, imm";     data[0][1] = "rt = rs + SignExtend(imm)";
        data[1][0] = "andi  $rt, $rs, imm";     data[1][1] = "rt = rs & ZeroExtend(imm)";
        data[2][0] = "ori   $rt, $rs, imm";     data[2][1] = "rt = rs | ZeroExtend(imm)";
        data[3][0] = "slit  $rt, $rs, imm";     data[3][1] = "rt = (rs < SignExtend(imm)) ? 1 : 0";
        data[4][0] = "sltiu $rt, $rs, imm";     data[4][1] = "rt = (rs < SignExtend(imm)) ? 1 : 0";
        data[5][0] = "lui   $rt, imm";          data[5][1] = "rt = imm << 16";
        data[6][0] = "lw    $rt, imm($rs)";     data[6][1] = "rt = mem[SignExtend(imm) + rs]";
        data[7][0] = "sw    $rt, imm($rs)";     data[7][1] = "mem[SignExtend(imm) + rs] = rt";
        data[8][0] = "beq   $rt, $rs, label";   data[8][1] = "if (rt == rs) PC = PC + 4 + imm";
        data[9][0] = "bne   $rt, $rs, label";   data[9][1] = "if (rt != rs) PC = PC + 4 + imm";

        for(int i = 0; i < data.length; i++)
            tbl.addRow(data[i]);

        tbl = (DefaultTableModel) tblJType.getModel();

        data = new String[2][2];

        data[0][0] = "j     label";             data[0][1] = "PC = label";
        data[1][0] = "jal   label";             data[1][1] = "ra = PC + 4; PC = label";

        for(int i = 0; i < data.length; i++)
            tbl.addRow(data[i]);

        tbl = (DefaultTableModel) tblPseudos.getModel();

        data = new String[5][2];

        data[0][0] = "nop";                     data[0][1] = "sll $0, $0, 0";
        data[1][0] = "b     label";             data[1][1] = "beq $0, $0, label";
        data[2][0] = "move  $rd, $rs";          data[2][1] = "or  $rd, $0, $rs";
        data[3][0] = "li    $rd, imm32";        data[3][1] = "lui $rd, imm32 >> 16; ori $rd, $rd, imm & 0xffff";
        data[4][0] = "li    $rd, label";        data[4][1] = "lui $rd, label[31:16]; ori $rd, $rd, label[15:0]";

        for(int i = 0; i < data.length; i++)
            tbl.addRow(data[i]);

        tbl = (DefaultTableModel) tblMemMap.getModel();

        data = new String[10][2];

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

        for(int i = 0; i < data.length; i++)
            tbl.addRow(data[i]);


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
        jScrollPane1 = new javax.swing.JScrollPane();
        tblIType = new javax.swing.JTable();
        jLabel2 = new javax.swing.JLabel();
        jScrollPane2 = new javax.swing.JScrollPane();
        tblRType = new javax.swing.JTable();
        jLabel3 = new javax.swing.JLabel();
        jScrollPane3 = new javax.swing.JScrollPane();
        tblJType = new javax.swing.JTable();
        jScrollPane4 = new javax.swing.JScrollPane();
        tblMemMap = new javax.swing.JTable();
        jLabel4 = new javax.swing.JLabel();
        btnClose = new javax.swing.JButton();
        jLabel5 = new javax.swing.JLabel();
        jScrollPane5 = new javax.swing.JScrollPane();
        tblPseudos = new javax.swing.JTable();

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(QuickRef.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N
        setResizable(false);
        addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                formKeyPressed(evt);
            }
        });

        jLabel1.setFont(resourceMap.getFont("jLabel1.font")); // NOI18N
        jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
        jLabel1.setName("jLabel1"); // NOI18N

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        tblIType.setFont(resourceMap.getFont("tblJType.font")); // NOI18N
        tblIType.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Usage", "C Equiv."
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblIType.setName("tblIType"); // NOI18N
        jScrollPane1.setViewportView(tblIType);
        tblIType.getColumnModel().getColumn(0).setResizable(false);
        tblIType.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblIType.columnModel.title0")); // NOI18N
        tblIType.getColumnModel().getColumn(1).setResizable(false);
        tblIType.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblIType.columnModel.title1")); // NOI18N

        jLabel2.setFont(resourceMap.getFont("jLabel1.font")); // NOI18N
        jLabel2.setText(resourceMap.getString("jLabel2.text")); // NOI18N
        jLabel2.setName("jLabel2"); // NOI18N

        jScrollPane2.setName("jScrollPane2"); // NOI18N

        tblRType.setFont(resourceMap.getFont("tblJType.font")); // NOI18N
        tblRType.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Usage", "C Equiv."
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblRType.setName("tblRType"); // NOI18N
        jScrollPane2.setViewportView(tblRType);
        tblRType.getColumnModel().getColumn(0).setResizable(false);
        tblRType.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblIType.columnModel.title0")); // NOI18N
        tblRType.getColumnModel().getColumn(1).setResizable(false);
        tblRType.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblIType.columnModel.title1")); // NOI18N

        jLabel3.setFont(resourceMap.getFont("jLabel1.font")); // NOI18N
        jLabel3.setText(resourceMap.getString("jLabel3.text")); // NOI18N
        jLabel3.setName("jLabel3"); // NOI18N

        jScrollPane3.setName("jScrollPane3"); // NOI18N

        tblJType.setFont(resourceMap.getFont("tblJType.font")); // NOI18N
        tblJType.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Usage", "C Equiv."
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblJType.setName("tblJType"); // NOI18N
        jScrollPane3.setViewportView(tblJType);
        tblJType.getColumnModel().getColumn(0).setResizable(false);
        tblJType.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblIType.columnModel.title0")); // NOI18N
        tblJType.getColumnModel().getColumn(1).setResizable(false);
        tblJType.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblIType.columnModel.title1")); // NOI18N

        jScrollPane4.setName("jScrollPane4"); // NOI18N

        tblMemMap.setFont(resourceMap.getFont("tblJType.font")); // NOI18N
        tblMemMap.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Address", "I/O Module"
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblMemMap.setName("tblMemMap"); // NOI18N
        jScrollPane4.setViewportView(tblMemMap);
        tblMemMap.getColumnModel().getColumn(0).setResizable(false);
        tblMemMap.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblMemMap.columnModel.title0")); // NOI18N
        tblMemMap.getColumnModel().getColumn(1).setResizable(false);
        tblMemMap.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblMemMap.columnModel.title1")); // NOI18N

        jLabel4.setFont(resourceMap.getFont("jLabel1.font")); // NOI18N
        jLabel4.setText(resourceMap.getString("jLabel4.text")); // NOI18N
        jLabel4.setName("jLabel4"); // NOI18N

        btnClose.setText(resourceMap.getString("btnClose.text")); // NOI18N
        btnClose.setName("btnClose"); // NOI18N
        btnClose.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCloseActionPerformed(evt);
            }
        });

        jLabel5.setFont(resourceMap.getFont("jLabel5.font")); // NOI18N
        jLabel5.setText(resourceMap.getString("jLabel5.text")); // NOI18N
        jLabel5.setName("jLabel5"); // NOI18N

        jScrollPane5.setName("jScrollPane5"); // NOI18N

        tblPseudos.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Usage", "Equiv. Ops"
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        tblPseudos.setName("tblPseudos"); // NOI18N
        jScrollPane5.setViewportView(tblPseudos);
        tblPseudos.getColumnModel().getColumn(0).setResizable(false);
        tblPseudos.getColumnModel().getColumn(0).setHeaderValue(resourceMap.getString("tblPseudos.columnModel.title0")); // NOI18N
        tblPseudos.getColumnModel().getColumn(1).setResizable(false);
        tblPseudos.getColumnModel().getColumn(1).setHeaderValue(resourceMap.getString("tblPseudos.columnModel.title1")); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(jScrollPane3, javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel2, javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel1, javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jLabel3, javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane2, javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 506, Short.MAX_VALUE))
                .addGap(18, 18, 18)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addComponent(jLabel4)
                    .addComponent(jLabel5)
                    .addComponent(btnClose)
                    .addComponent(jScrollPane5)
                    .addComponent(jScrollPane4))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel1)
                    .addComponent(jLabel4))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING, false)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 179, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(jLabel2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 179, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(18, 18, 18)
                        .addComponent(jLabel3)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jScrollPane3, javax.swing.GroupLayout.PREFERRED_SIZE, 127, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(jScrollPane4, javax.swing.GroupLayout.PREFERRED_SIZE, 307, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(18, 18, 18)
                        .addComponent(jLabel5)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jScrollPane5, javax.swing.GroupLayout.PREFERRED_SIZE, 172, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(18, 18, 18)
                        .addComponent(btnClose)))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void formKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_formKeyPressed
        if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_F1 ||
           evt.getKeyCode() == java.awt.event.KeyEvent.VK_ESCAPE) {
            this.setVisible(false);
        }
    }//GEN-LAST:event_formKeyPressed

    private void btnCloseActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCloseActionPerformed
        this.setVisible(false);
    }//GEN-LAST:event_btnCloseActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnClose;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JScrollPane jScrollPane4;
    private javax.swing.JScrollPane jScrollPane5;
    private javax.swing.JTable tblIType;
    private javax.swing.JTable tblJType;
    private javax.swing.JTable tblMemMap;
    private javax.swing.JTable tblPseudos;
    private javax.swing.JTable tblRType;
    // End of variables declaration//GEN-END:variables

}
