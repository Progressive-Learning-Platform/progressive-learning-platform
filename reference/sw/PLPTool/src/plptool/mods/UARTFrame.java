/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * UARTFrame.java
 *
 * Created on Feb 5, 2011, 5:57:54 PM
 */

package plptool.mods;

/**
 *
 * @author fritz
 */
public class UARTFrame extends javax.swing.JFrame {
    UART u;
    
    /** Creates new form UARTFrame */
    public UARTFrame() {
        initComponents();

        
    }

    public void setUART(UART f) {
        u = f;
    }
    public void addText(long d) {
        txtUART.append(String.format("%c",(char)d));
        txtUART.setCaretPosition(txtUART.getText().length() - 1);
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
        txtUART = new javax.swing.JTextArea();
        btnClear = new javax.swing.JButton();
        lblRawByte = new javax.swing.JLabel();
        txtRawByte = new javax.swing.JTextField();
        btnSend = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE);
        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(UARTFrame.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N
        setResizable(false);

        jScrollPane1.setBackground(resourceMap.getColor("jScrollPane1.background")); // NOI18N
        jScrollPane1.setForeground(resourceMap.getColor("jScrollPane1.foreground")); // NOI18N
        jScrollPane1.setName("jScrollPane1"); // NOI18N

        txtUART.setBackground(resourceMap.getColor("txtUART.background")); // NOI18N
        txtUART.setColumns(20);
        txtUART.setEditable(false);
        txtUART.setFont(resourceMap.getFont("txtUART.font")); // NOI18N
        txtUART.setForeground(resourceMap.getColor("txtUART.foreground")); // NOI18N
        txtUART.setLineWrap(true);
        txtUART.setRows(5);
        txtUART.setDisabledTextColor(resourceMap.getColor("txtUART.disabledTextColor")); // NOI18N
        txtUART.setName("txtUART"); // NOI18N
        txtUART.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                txtUARTKeyTyped(evt);
            }
        });
        jScrollPane1.setViewportView(txtUART);

        btnClear.setText(resourceMap.getString("btnClear.text")); // NOI18N
        btnClear.setName("btnClear"); // NOI18N
        btnClear.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnClearActionPerformed(evt);
            }
        });

        lblRawByte.setText(resourceMap.getString("lblRawByte.text")); // NOI18N
        lblRawByte.setName("lblRawByte"); // NOI18N

        txtRawByte.setText(resourceMap.getString("txtRawByte.text")); // NOI18N
        txtRawByte.setName("txtRawByte"); // NOI18N
        txtRawByte.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                txtRawByteKeyPressed(evt);
            }
        });

        btnSend.setText(resourceMap.getString("btnSend.text")); // NOI18N
        btnSend.setName("btnSend"); // NOI18N
        btnSend.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnSendActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 522, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(lblRawByte)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(txtRawByte, javax.swing.GroupLayout.PREFERRED_SIZE, 132, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(btnSend)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 185, Short.MAX_VALUE)
                        .addComponent(btnClear)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 255, Short.MAX_VALUE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(btnClear)
                    .addComponent(lblRawByte)
                    .addComponent(txtRawByte, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(btnSend))
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void txtUARTKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txtUARTKeyTyped
         u.receivedData(evt.getKeyChar());
    }//GEN-LAST:event_txtUARTKeyTyped

    private void btnClearActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnClearActionPerformed
        txtUART.setText("");
    }//GEN-LAST:event_btnClearActionPerformed

    private void btnSendActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnSendActionPerformed
        u.receivedData((char) plptool.PLPToolbox.parseNumInt(txtRawByte.getText()));
    }//GEN-LAST:event_btnSendActionPerformed

    private void txtRawByteKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txtRawByteKeyPressed
        if(evt.getKeyCode() == evt.VK_ENTER) {
            u.receivedData((char) plptool.PLPToolbox.parseNumInt(txtRawByte.getText()));
        }
    }//GEN-LAST:event_txtRawByteKeyPressed


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnClear;
    private javax.swing.JButton btnSend;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel lblRawByte;
    private javax.swing.JTextField txtRawByte;
    private javax.swing.JTextArea txtUART;
    // End of variables declaration//GEN-END:variables

}
