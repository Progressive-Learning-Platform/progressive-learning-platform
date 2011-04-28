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

        plptool.PLPToolbox.attachHideOnEscapeListener(this);

        String str = "";
        txtHTML.setContentType("text/html");

        str += "<a name=\"r\" /><h1>R-type Instructions</h1>";
        str += "<table border=1 width=\"100%\">";

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

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td>" + data[i][0] + "</td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }
        
        str += "</table>";

        str += "<a name=\"i\" /><h1>I-type Instructions</h1>";
        str += "<table border=1 width=\"100%\"";

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

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td>" + data[i][0] + "</td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"j\" /><h1>J-type Instructions</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[2][2];

        data[0][0] = "j     label";             data[0][1] = "PC = label";
        data[1][0] = "jal   label";             data[1][1] = "ra = PC + 4; PC = label";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td>" + data[i][0] + "</td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"p\" /><h1>Pseudo-operations</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[7][2];

        data[0][0] = "nop";                     data[0][1] = "sll $0, $0, 0";
        data[1][0] = "b     label";             data[1][1] = "beq $0, $0, label";
        data[2][0] = "move  $rd, $rs";          data[2][1] = "or  $rd, $0, $rs";
        data[3][0] = "li    $rd, imm32";        data[3][1] = "lui $rd, imm32 >> 16;<br />";
                                                data[3][1] += "ori $rd, $rd, imm & 0xffff";
        data[4][0] = "li    $rd, label";        data[4][1] = "lui $rd, label[31:16];<br />";
                                                data[4][1] += "ori $rd, $rd, label[15:0]";
        data[5][0] = "push  $rt";               data[5][1] = "sw $rt, 0($sp);<br />";
                                                data[5][1] += "addiu $sp, $sp, -4";
        data[6][0] = "pop   $rt";               data[6][1] = "addiu $sp, $sp, 4;<br />";
                                                data[6][1] += "lw $rt, 0($sp)";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td>" + data[i][0] + "</td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"m\" /><h1>Assembler Directives</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[6][2];

        data[0][0] = ".org <i>address</i>";                data[0][1] = "Place subsequent statements starting from <i>address</i>";
        data[1][0] = "<i>label</i>:";                data[1][1] = "Label current memory location as <i>label</i>";
        data[2][0] = ".word <i>value</i>";           data[2][1] = "Write 32-bit <i>value</i> to the current address";
        data[3][0] = ".ascii \"<i>string</i>\"";     data[3][1] = "Place <i>string</i> starting from the current address";
        data[4][0] = ".asciiz \"<i>string</i>\"";    data[4][1] = "Place null-terminated <i>string</i> statring from the current address";
        data[5][0] = ".space <i>value</i>";          data[5][1] = "Reserve <i>value</i> words starting from the current address";


        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td>" + data[i][0] + "</td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"m\" /><h1>Registers Usage Guide</h1>";
        str += "<table border=1 width=\"100%\"";

        data = new String[11][2];

        data[0][0] = "$0, $zero";               data[0][1] = "The zero register";
        data[1][0] = "$1, $at";                 data[1][1] = "Assembler temporary";
        data[2][0] = "$2-$3, $v0-$v1";          data[2][1] = "Return values";
        data[3][0] = "$4-$7, $a0-$a3";          data[3][1] = "Function arguments";
        data[4][0] = "$8-$17, $t0-$t9";         data[4][1] = "Temporaries";
        data[5][0] = "$18-$25, $s0-$s7";        data[5][1] = "Saved temporaries";
        data[6][0] = "$26-$27, $k0-$k1";        data[6][1] = "Kernel";
        data[7][0] = "$28, $gp";                data[7][1] = "Global pointer";
        data[8][0] = "$29, $sp";                data[8][1] = "Stack pointer";
        data[9][0] = "$30, $fp";                data[9][1] = "Frame pointer";
        data[10][0] = "$31, $ra";               data[10][1] = "Return address";

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td>" + data[i][0] + "</td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        str += "<a name=\"m\" /><h1>I/O Memory Map</h1>";
        str += "<table border=1 width=\"100%\"";

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

        for(int i = 0; i < data.length; i++) {
            str += "<tr>";
            str += "<td>" + data[i][0] + "</td><td>" + data[i][1] + "</td>";
            str += "</tr>";
        }

        str += "</table>";

        txtHTML.setText(str);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        btnClose = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        txtHTML = new javax.swing.JTextPane();

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(QuickRef.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N
        addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                formKeyPressed(evt);
            }
        });

        btnClose.setText(resourceMap.getString("btnClose.text")); // NOI18N
        btnClose.setName("btnClose"); // NOI18N
        btnClose.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCloseActionPerformed(evt);
            }
        });

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        txtHTML.setEditable(false);
        txtHTML.setName("txtHTML"); // NOI18N
        jScrollPane1.setViewportView(txtHTML);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(btnClose, javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 585, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 462, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(btnClose)
                .addContainerGap())
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
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JTextPane txtHTML;
    // End of variables declaration//GEN-END:variables

}
