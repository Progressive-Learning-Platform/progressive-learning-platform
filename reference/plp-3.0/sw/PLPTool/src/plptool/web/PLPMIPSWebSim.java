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

package plptool.web;

import plptool.*;
import plptool.mips.*;
import plptool.mods.*;

/**
 * Java applet interface for the PLP MIPS simulation.
 *
 * @author wira
 */
public class PLPMIPSWebSim extends javax.swing.JApplet {

    String oldStr;
    Asm asm;
    SimCore sim;

    /** Initializes the applet PLPMIPSWebSim */
    public void init() {
        try {
            java.awt.EventQueue.invokeAndWait(new Runnable() {
                public void run() {
                    initComponents();
                    btnStep.setEnabled(false);
                    lblStatus.setText("Assemble whenever you're ready!");
                    txtEditor.setText(".org 0x10000000");
                }
            });
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /** This method is called from within the init() method to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        btnAssemble = new javax.swing.JButton();
        btnStep = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        txtEditor = new javax.swing.JTextPane();
        lblStatus = new javax.swing.JLabel();

        setName("Form"); // NOI18N

        btnAssemble.setText("Assemble");
        btnAssemble.setName("btnAssemble"); // NOI18N
        btnAssemble.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnAssembleActionPerformed(evt);
            }
        });

        btnStep.setText("Step!");
        btnStep.setName("btnStep"); // NOI18N
        btnStep.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnStepActionPerformed(evt);
            }
        });

        jScrollPane1.setName("jScrollPane1"); // NOI18N

        txtEditor.setFont(new java.awt.Font("Monospaced", 0, 12)); // NOI18N
        txtEditor.setName("txtEditor"); // NOI18N
        jScrollPane1.setViewportView(txtEditor);

        lblStatus.setText("jLabel1");
        lblStatus.setName("lblStatus"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 590, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(btnAssemble)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(lblStatus)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 440, Short.MAX_VALUE)
                        .addComponent(btnStep)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 250, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btnStep)
                    .addComponent(btnAssemble)
                    .addComponent(lblStatus))
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btnAssembleActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnAssembleActionPerformed
        if(btnAssemble.getText().equals("Back to Editor")) {
            txtEditor.setEditable(true);
            txtEditor.setText(oldStr);
            btnAssemble.setText("Assemble");
            btnStep.setEnabled(false);
            lblStatus.setText("Assemble whenever you're ready!");
            return;
        }

        oldStr = txtEditor.getText();
        txtEditor.setText("");
        Msg.output = txtEditor;
        asm = new Asm(oldStr, "WebApplet");
        int ret = -1;
        if(asm.preprocess(0) == Constants.PLP_OK)
            ret = asm.assemble();

        if(asm.isAssembled()) {
            btnAssemble.setText("Back to Editor");
            sim = new SimCore(asm, asm.getEntryPoint());
            sim.bus.add(new InterruptController(0xf0700000L, (PLPSimCore) sim));
            sim.bus.add(new MemModule(0x10000000L, 0x1000000, true));
            sim.bus.enableAllModules();
            sim.loadProgram(asm);
            sim.reset();
            txtEditor.setEditable(false);
            btnStep.setEnabled(true);
            lblStatus.setText("Hit step to advance the program");
        } else {
            btnAssemble.setText("Back to Editor");
            lblStatus.setText("Assemble failed, check your code!");
            txtEditor.setEditable(false);
        }
    }//GEN-LAST:event_btnAssembleActionPerformed

    private void btnStepActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnStepActionPerformed
        txtEditor.setText("");
        sim.step();
        Msg.M("");
        sim.wb_stage.printinstr();
        sim.mem_stage.printinstr();
        sim.ex_stage.printinstr();
        sim.id_stage.printinstr();
        sim.printfrontend();
        Msg.M("-------------------------------------");
    }//GEN-LAST:event_btnStepActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnAssemble;
    private javax.swing.JButton btnStep;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel lblStatus;
    private javax.swing.JTextPane txtEditor;
    // End of variables declaration//GEN-END:variables

}
