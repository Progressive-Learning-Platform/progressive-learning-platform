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

/*
 * PLPErrorFrame.java
 *
 * Created on Dec 6, 2010, 6:37:51 PM
 */

package plptool.gui;

import java.awt.Color;

/**
 *
 * @author wira
 */
public class SimErrorFrame extends javax.swing.JInternalFrame {

    /** Creates new form PLPErrorFrame */
    public SimErrorFrame() {
        initComponents();
    }

    public void setError(int errNum) {
        lblError.setBackground(new Color((float) 0.5, (float) 0.0, (float) 0.0));
        lblError.setText("" + errNum);
        if(plptool.PLPMsg.lastPartyResponsible != null)
            lblPerson.setText(plptool.PLPMsg.lastPartyResponsible.toString());
        else
            lblPerson.setText("Static class.");
        this.setTitle("Error!");
    }
    
    public void clearError() {
        plptool.PLPMsg.lastError = 0;
        lblError.setBackground(new Color((float) 0.0, (float) 0.4, (float) 0.2));
        lblError.setText("" + 0);
        lblPerson.setText("Nobody");
        this.setTitle("No errors");
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        lblPerson = new javax.swing.JTextField();
        lblError = new javax.swing.JLabel();

        setIconifiable(true);
        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(SimErrorFrame.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N

        lblPerson.setText(resourceMap.getString("lblPerson.text")); // NOI18N
        lblPerson.setName("lblPerson"); // NOI18N

        lblError.setBackground(resourceMap.getColor("lblError.background")); // NOI18N
        lblError.setForeground(resourceMap.getColor("lblError.foreground")); // NOI18N
        lblError.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        lblError.setText(resourceMap.getString("lblError.text")); // NOI18N
        lblError.setName("lblError"); // NOI18N
        lblError.setOpaque(true);

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(lblError, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 136, Short.MAX_VALUE)
                    .addComponent(lblPerson, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 136, Short.MAX_VALUE))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(lblError, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 10, Short.MAX_VALUE)
                .addComponent(lblPerson, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel lblError;
    private javax.swing.JTextField lblPerson;
    // End of variables declaration//GEN-END:variables

}