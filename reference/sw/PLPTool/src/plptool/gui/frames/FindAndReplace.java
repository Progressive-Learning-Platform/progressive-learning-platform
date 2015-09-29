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

package plptool.gui.frames;

import javax.swing.text.Document;
import plptool.Msg;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class FindAndReplace extends javax.swing.JFrame {

    private ProjectDriver plp;
    private boolean haveTriedFromTop;
    private int curIndex;
    private String searchStr;

    /** Creates new form FindAndReplace */
    public FindAndReplace(ProjectDriver plp) {
        initComponents();

        plptool.PLPToolbox.attachHideOnEscapeListener(this);
        haveTriedFromTop = false;
        this.plp = plp;
        curIndex = 0;
        searchStr = "";
    }

    public void setCurIndex(int curIndex) {
        this.curIndex = curIndex;
        haveTriedFromTop = false;
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        txtFind = new javax.swing.JTextField();
        txtReplace = new javax.swing.JTextField();
        btnGo = new javax.swing.JButton();
        jLabel1 = new javax.swing.JLabel();
        chkReplace = new javax.swing.JCheckBox();
        btnClose = new javax.swing.JButton();
        btnReplaceAll = new javax.swing.JButton();

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(FindAndReplace.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setAlwaysOnTop(true);
        setName("Form"); // NOI18N
        setResizable(false);

        txtFind.setText(resourceMap.getString("txtFind.text")); // NOI18N
        txtFind.setName("txtFind"); // NOI18N
        txtFind.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                txtFindFocusGained(evt);
            }
        });
        txtFind.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                txtFindKeyPressed(evt);
            }
        });

        txtReplace.setText(resourceMap.getString("txtReplace.text")); // NOI18N
        txtReplace.setName("txtReplace"); // NOI18N
        txtReplace.addFocusListener(new java.awt.event.FocusAdapter() {
            public void focusGained(java.awt.event.FocusEvent evt) {
                txtReplaceFocusGained(evt);
            }
        });

        btnGo.setMnemonic('N');
        btnGo.setText(resourceMap.getString("btnGo.text")); // NOI18N
        btnGo.setName("btnGo"); // NOI18N
        btnGo.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnGoActionPerformed(evt);
            }
        });

        jLabel1.setText(resourceMap.getString("jLabel1.text")); // NOI18N
        jLabel1.setName("jLabel1"); // NOI18N

        chkReplace.setMnemonic('R');
        chkReplace.setText(resourceMap.getString("chkReplace.text")); // NOI18N
        chkReplace.setName("chkReplace"); // NOI18N

        btnClose.setMnemonic('C');
        btnClose.setText(resourceMap.getString("btnClose.text")); // NOI18N
        btnClose.setName("btnClose"); // NOI18N
        btnClose.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCloseActionPerformed(evt);
            }
        });

        btnReplaceAll.setMnemonic('A');
        btnReplaceAll.setText(resourceMap.getString("btnReplaceAll.text")); // NOI18N
        btnReplaceAll.setName("btnReplaceAll"); // NOI18N
        btnReplaceAll.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnReplaceAllActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel1)
                            .addComponent(chkReplace))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 18, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(txtFind, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 353, Short.MAX_VALUE)
                            .addComponent(txtReplace, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 353, Short.MAX_VALUE)))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(btnClose)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnReplaceAll)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnGo, javax.swing.GroupLayout.PREFERRED_SIZE, 116, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(txtFind, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jLabel1))
                .addGap(7, 7, 7)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(txtReplace, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(chkReplace))
                .addGap(18, 18, 18)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btnGo)
                    .addComponent(btnReplaceAll)
                    .addComponent(btnClose))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void findAndReplaceString(int caret) {
        plp.g_dev.getEditor().setCaretPosition(caret);
        plp.g_dev.getEditor().setSelectionStart(caret);
        plp.g_dev.getEditor().setSelectionEnd(caret + txtFind.getText().length());
        curIndex = caret + txtFind.getText().length();

        if(chkReplace.isSelected()) {
            plp.setModified();
            plp.g_dev.getEditor().replaceSelection(txtReplace.getText());
            plp.g_dev.getEditor().setSelectionStart(caret);
            plp.g_dev.getEditor().setSelectionEnd(caret + txtReplace.getText().length());
        }
    }

    private void btnGoActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnGoActionPerformed
        plp.g_dev.setFocusToEditor();
        curIndex = plp.g_dev.getEditor().getCaretPosition();

        int caret = plp.g_dev.getEditor().getText().indexOf(txtFind.getText(), curIndex);

        if(!txtFind.getText().equals(searchStr)) {
            haveTriedFromTop = false;
            searchStr = txtFind.getText();
        }

        if(caret > -1) {
            haveTriedFromTop = false;
            findAndReplaceString(caret);
        } else if(!haveTriedFromTop) {
            caret = plp.g_dev.getEditor().getText().indexOf(txtFind.getText(), 0);

            if(caret > -1)
                findAndReplaceString(caret);
            else {
                Msg.println("String not found: \"" + txtFind.getText() + "\"");
                haveTriedFromTop = true;
            }
        }
        else
            Msg.println("String not found: \"" + txtFind.getText() + "\"");
    }//GEN-LAST:event_btnGoActionPerformed

    private void txtFindKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txtFindKeyPressed
        if(evt.getKeyCode() == java.awt.event.KeyEvent.VK_ENTER)
            btnGoActionPerformed(null);
    }//GEN-LAST:event_txtFindKeyPressed

    private void btnCloseActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCloseActionPerformed
        this.setVisible(false);
    }//GEN-LAST:event_btnCloseActionPerformed

    private void btnReplaceAllActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnReplaceAllActionPerformed
        curIndex = 0;    
        int caret;
        int replaceCount = 0;
        while((caret = plp.g_dev.getEditor().getText().indexOf(txtFind.getText(), curIndex)) > -1) {
            plp.g_dev.getEditor().setCaretPosition(caret);
            plp.g_dev.getEditor().setSelectionStart(caret);
            plp.g_dev.getEditor().setSelectionEnd(caret + txtFind.getText().length());
            curIndex = caret + txtFind.getText().length();

            if(chkReplace.isSelected()) {
                plp.setModified();
                plp.g_dev.getEditor().replaceSelection(txtReplace.getText());
                replaceCount++;
            }
        }

        Msg.println(replaceCount + " instances of \"" + txtFind.getText() + "\" replaced.");
    }//GEN-LAST:event_btnReplaceAllActionPerformed

    private void txtReplaceFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_txtReplaceFocusGained
        txtReplace.setSelectionStart(0);
        txtReplace.setSelectionEnd(txtReplace.getText().length());
    }//GEN-LAST:event_txtReplaceFocusGained

    private void txtFindFocusGained(java.awt.event.FocusEvent evt) {//GEN-FIRST:event_txtFindFocusGained
        txtFind.setSelectionStart(0);
        txtFind.setSelectionEnd(txtFind.getText().length());
    }//GEN-LAST:event_txtFindFocusGained

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnClose;
    private javax.swing.JButton btnGo;
    private javax.swing.JButton btnReplaceAll;
    private javax.swing.JCheckBox chkReplace;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JTextField txtFind;
    private javax.swing.JTextField txtReplace;
    // End of variables declaration//GEN-END:variables

}
