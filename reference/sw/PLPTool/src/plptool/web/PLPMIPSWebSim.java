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

package plptool.web;

import plptool.*;
import plptool.mips.*;
import plptool.gui.ProjectDriver;
import javax.swing.text.html.*;
import javax.swing.text.*;

/**
 * Java applet interface for the PLP MIPS simulation.
 *
 * @author wira
 */
public class PLPMIPSWebSim extends javax.swing.JApplet {

    String oldStr;
    ProjectDriver plp;
    Runner runner;

    EditorKit startingEditorKit;
    Document startingDocument;

    /** Initializes the applet PLPMIPSWebSim */
    public void init() {
        try {
            java.awt.EventQueue.invokeAndWait(new Runnable() {
                public void run() {
                    initComponents();
                    btnStep.setEnabled(false);
                    btnExec.setEnabled(false);
                    tglRun.setEnabled(false);
                    txtCLI.setEnabled(false);
                    startingEditorKit = txtEditor.getEditorKit();
                    startingDocument = txtEditor.getDocument();
                    lblStatus.setText("Assemble whenever you're ready!");
                    txtEditor.setText(".org 0x10000000\n" +
                                      "\n" +
                                      "\tmove $t0, $0" +
                                      "\nloop:" +
                                      "\n\tj loop" +
                                      "\n\taddiu $t0, $t0, 1\t\t# increment $t0 by one");
                    Config.simRunnerDelay = 100;
                    Config.simFunctional = false;

                    // Instantiate a new projectdriver and attach a source object
                    plp = new ProjectDriver(Constants.PLP_GUI_APPLET);
                    plp.setArch(ArchRegistry.ISA_PLPMIPS);
                    plp.newAsm("WebApplet");
                }
            });
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private void assemble() {
        if(btnAssemble.getText().equals("Back to Editor")) {
            txtEditor.setText("");
            txtEditor.setEditorKit(startingEditorKit);
            txtEditor.setDocument(startingDocument);
            txtEditor.setText(oldStr);
            txtEditor.setEditable(true);
            btnAssemble.setText("Assemble");
            btnStep.setEnabled(false);
            lblStatus.setText("Assemble whenever you're ready!");
            txtCLI.setEnabled(false);
            btnExec.setEnabled(false);
            if(runner != null)
                runner.stepCount = 0;
            tglRun.setSelected(false);
            tglRun.setEnabled(false);
            return;
        }

        oldStr = txtEditor.getText();
        txtEditor.setText("");
        txtEditor.setEditorKit(new HTMLEditorKit());
        txtEditor.setDocument(new HTMLDocument());
        Msg.setOutput(txtEditor);
        Msg.M("PLPTool build: " + Version.stamp + "\n");

        plp.getAsm("WebApplet").setAsmString(oldStr);
        plp.assemble();

        if(plp.isAssembled()) {
            btnAssemble.setText("Back to Editor");
            txtEditor.setEditable(false);

            if (plp.simulate() != Constants.PLP_OK) {
                lblStatus.setText("Assembly failed, check your code!");
                return;
            }

            txtCLI.setEnabled(true);
            btnExec.setEnabled(true);
            btnStep.setEnabled(true);
            tglRun.setEnabled(true);
            lblStatus.setText("Hit step to advance the program");
        } else {
            btnAssemble.setText("Back to Editor");
            lblStatus.setText("Assembly failed, check your code!");
            txtEditor.setEditable(false);
        }
    }

    private int step() {
        txtEditor.setText("");
        int ret;
        ret = plp.sim.stepW();
        SimCore sc = (SimCore) plp.sim;

        Msg.p("Register File Contents");
        Msg.p("======================");

        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 4; j++)
                Msg.pn((i+8*j) + ((i+8*j < 10) ? "  " : " ") + String.format("%08x", sc.regfile.read(i+8*j)) + "  ");

            Msg.p("");
        }

        Msg.p("");
        Msg.p("Instructions in-flight");
        Msg.p("======================");

        Msg.p(sc.wb_stage.printinstr());
        Msg.p(sc.mem_stage.printinstr());
        Msg.p(sc.ex_stage.printinstr());
        Msg.p(sc.id_stage.printinstr());
        sc.printfrontend();
        Msg.P();

        return ret;
    }

    private void toggleRun() {
        if(!tglRun.isSelected()) {
            lblStatus.setText("Stopped.");
            if(runner != null)
                runner.stepCount = 0;
            tglRun.setSelected(false);
            btnStep.setEnabled(true);
        }
        else {
            lblStatus.setText("Running...");
            runner = new Runner(plp, this);
            runner.stepCount = 1;
            btnStep.setEnabled(false);
            runner.start();
        }
    }

    private void execCLI() {
        txtEditor.setText("");
        SimCLI.simCLCommand(txtCLI.getText(), plp);
        txtCLI.setText("");
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
        txtCLI = new javax.swing.JTextField();
        btnExec = new javax.swing.JButton();
        tglRun = new javax.swing.JToggleButton();

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

        txtEditor.setFont(new java.awt.Font("Monospaced", 0, 12));
        txtEditor.setName("txtEditor"); // NOI18N
        jScrollPane1.setViewportView(txtEditor);

        lblStatus.setText("jLabel1");
        lblStatus.setName("lblStatus"); // NOI18N

        txtCLI.setName("txtCLI"); // NOI18N
        txtCLI.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                txtCLIKeyPressed(evt);
            }
        });

        btnExec.setText("Execute");
        btnExec.setName("btnExec"); // NOI18N
        btnExec.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnExecActionPerformed(evt);
            }
        });

        tglRun.setText("Run");
        tglRun.setName("tglRun"); // NOI18N
        tglRun.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                tglRunActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 606, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(btnAssemble)
                        .addGap(10, 10, 10)
                        .addComponent(lblStatus)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 365, Short.MAX_VALUE)
                        .addComponent(tglRun)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnStep))
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addComponent(txtCLI, javax.swing.GroupLayout.DEFAULT_SIZE, 523, Short.MAX_VALUE)
                        .addGap(10, 10, 10)
                        .addComponent(btnExec)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(txtCLI, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(btnExec))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 285, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btnStep)
                    .addComponent(lblStatus)
                    .addComponent(btnAssemble)
                    .addComponent(tglRun))
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents

    private void btnAssembleActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnAssembleActionPerformed
        assemble();
    }//GEN-LAST:event_btnAssembleActionPerformed

    private void btnStepActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnStepActionPerformed
        step();
    }//GEN-LAST:event_btnStepActionPerformed

    private void btnExecActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnExecActionPerformed
        execCLI();
    }//GEN-LAST:event_btnExecActionPerformed

    private void txtCLIKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txtCLIKeyPressed
        if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_ENTER)
            execCLI();
    }//GEN-LAST:event_txtCLIKeyPressed

    private void tglRunActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_tglRunActionPerformed
        toggleRun();
    }//GEN-LAST:event_tglRunActionPerformed

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnAssemble;
    private javax.swing.JButton btnExec;
    private javax.swing.JButton btnStep;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel lblStatus;
    private javax.swing.JToggleButton tglRun;
    private javax.swing.JTextField txtCLI;
    private javax.swing.JTextPane txtEditor;
    // End of variables declaration//GEN-END:variables

    class Runner extends plptool.gui.SimRunner {
        ProjectDriver plp;
        PLPMIPSWebSim applet;

        public Runner(ProjectDriver plp, PLPMIPSWebSim applet) {
            super(plp);
            this.plp = plp;
            this.applet = applet;
        }

        @Override
        public void run() {
            int ret;

            while(stepCount > 0) {
                ret = applet.step();
                try {
                    this.sleep(Config.simRunnerDelay);
                } catch(Exception e) {
                    stepCount = 0;
                }

                if(ret != Constants.PLP_OK)
                    applet.toggleRun();
            }
        }
    }
}
