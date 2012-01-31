/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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

package plptool.mods;

import plptool.PLPToolbox;

import javax.swing.ImageIcon;

/**
 *
 * @author wira
 */
public class GPIOFrame extends javax.swing.JFrame {

    private GPIO mod;
    private ImageIcon gpio_in_low;
    private ImageIcon gpio_in_high;
    private ImageIcon gpio_out_low;
    private ImageIcon gpio_out_high;
    private boolean[] statesA = {false, false, false, false, false, false, false, false};
    private boolean[] statesB = {false, false, false, false, false, false, false, false};


    /** Creates new form GPIOFrame */
    public GPIOFrame(GPIO mod) {
        this.setIconImage(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/toolbar_sim_gpio.png")));
        initComponents();
        this.mod = mod;

        gpio_in_low = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_in_low.png")));
        gpio_in_high = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_input.png")));
        gpio_out_low = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_out_low.png")));
        gpio_out_high = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_output.png")));
    }

    public void setTristateRegisterValues(long val) {
        txtTristateReg.setText(PLPToolbox.format32Hex(val));
    }

    public void updateOutputs() {
        boolean dir, data;

        // A0
        dir = (((Long) mod.read(mod.startAddr()) >> 0) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 0) & 1L) == 1L ? true : false;
        if(dir & data)
            A0.setIcon(gpio_out_high);
        else if(dir & !data)
            A0.setIcon(gpio_out_low);
        else if(!dir & data)
            A0.setIcon(gpio_in_high);
        else if(!dir & !data)
            A0.setIcon(gpio_in_low);

        // A1
        dir = (((Long) mod.read(mod.startAddr()) >> 1) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 1) & 1L) == 1L ? true : false;
        if(dir & data)
            A1.setIcon(gpio_out_high);
        else if(dir & !data)
            A1.setIcon(gpio_out_low);
        else if(!dir & data)
            A1.setIcon(gpio_in_high);
        else if(!dir & !data)
            A1.setIcon(gpio_in_low);

        // A2
        dir = (((Long) mod.read(mod.startAddr()) >> 2) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 2) & 1L) == 1L ? true : false;
        if(dir & data)
            A2.setIcon(gpio_out_high);
        else if(dir & !data)
            A2.setIcon(gpio_out_low);
        else if(!dir & data)
            A2.setIcon(gpio_in_high);
        else if(!dir & !data)
            A2.setIcon(gpio_in_low);

        // A3
        dir = (((Long) mod.read(mod.startAddr()) >> 3) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 3) & 1L) == 1L ? true : false;
        if(dir & data)
            A3.setIcon(gpio_out_high);
        else if(dir & !data)
            A3.setIcon(gpio_out_low);
        else if(!dir & data)
            A3.setIcon(gpio_in_high);
        else if(!dir & !data)
            A3.setIcon(gpio_in_low);

        // A4
        dir = (((Long) mod.read(mod.startAddr()) >> 4) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 4) & 1L) == 1L ? true : false;
        if(dir & data)
            A4.setIcon(gpio_out_high);
        else if(dir & !data)
            A4.setIcon(gpio_out_low);
        else if(!dir & data)
            A4.setIcon(gpio_in_high);
        else if(!dir & !data)
            A4.setIcon(gpio_in_low);

        // A5
        dir = (((Long) mod.read(mod.startAddr()) >> 5) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 5) & 1L) == 1L ? true : false;
        if(dir & data)
            A5.setIcon(gpio_out_high);
        else if(dir & !data)
            A5.setIcon(gpio_out_low);
        else if(!dir & data)
            A5.setIcon(gpio_in_high);
        else if(!dir & !data)
            A5.setIcon(gpio_in_low);

        // A6
        dir = (((Long) mod.read(mod.startAddr()) >> 6) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 6) & 1L) == 1L ? true : false;
        if(dir & data)
            A6.setIcon(gpio_out_high);
        else if(dir & !data)
            A6.setIcon(gpio_out_low);
        else if(!dir & data)
            A6.setIcon(gpio_in_high);
        else if(!dir & !data)
            A6.setIcon(gpio_in_low);

        // A7
        dir = (((Long) mod.read(mod.startAddr()) >> 7) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+4) >> 7) & 1L) == 1L ? true : false;
        if(dir & data)
            A7.setIcon(gpio_out_high);
        else if(dir & !data)
            A7.setIcon(gpio_out_low);
        else if(!dir & data)
            A7.setIcon(gpio_in_high);
        else if(!dir & !data)
            A7.setIcon(gpio_in_low);

        // B0
        dir = (((Long) mod.read(mod.startAddr()) >> 8) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 0) & 1L) == 1L ? true : false;
        if(dir & data)
            B0.setIcon(gpio_out_high);
        else if(dir & !data)
            B0.setIcon(gpio_out_low);
        else if(!dir & data)
            B0.setIcon(gpio_in_high);
        else if(!dir & !data)
            B0.setIcon(gpio_in_low);

        // B1
        dir = (((Long) mod.read(mod.startAddr()) >> 9) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 1) & 1L) == 1L ? true : false;
        if(dir & data)
            B1.setIcon(gpio_out_high);
        else if(dir & !data)
            B1.setIcon(gpio_out_low);
        else if(!dir & data)
            B1.setIcon(gpio_in_high);
        else if(!dir & !data)
            B1.setIcon(gpio_in_low);

        // B2
        dir = (((Long) mod.read(mod.startAddr()) >> 10) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 2) & 1L) == 1L ? true : false;
        if(dir & data)
            B2.setIcon(gpio_out_high);
        else if(dir & !data)
            B2.setIcon(gpio_out_low);
        else if(!dir & data)
            B2.setIcon(gpio_in_high);
        else if(!dir & !data)
            B2.setIcon(gpio_in_low);

        // B3
        dir = (((Long) mod.read(mod.startAddr()) >> 11) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 3) & 1L) == 1L ? true : false;
        if(dir & data)
            B3.setIcon(gpio_out_high);
        else if(dir & !data)
            B3.setIcon(gpio_out_low);
        else if(!dir & data)
            B3.setIcon(gpio_in_high);
        else if(!dir & !data)
            B3.setIcon(gpio_in_low);

        // B4
        dir = (((Long) mod.read(mod.startAddr()) >> 12) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 4) & 1L) == 1L ? true : false;
        if(dir & data)
            B4.setIcon(gpio_out_high);
        else if(dir & !data)
            B4.setIcon(gpio_out_low);
        else if(!dir & data)
            B4.setIcon(gpio_in_high);
        else if(!dir & !data)
            B4.setIcon(gpio_in_low);

        // B5
        dir = (((Long) mod.read(mod.startAddr()) >> 13) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 5) & 1L) == 1L ? true : false;
        if(dir & data)
            B5.setIcon(gpio_out_high);
        else if(dir & !data)
            B5.setIcon(gpio_out_low);
        else if(!dir & data)
            B5.setIcon(gpio_in_high);
        else if(!dir & !data)
            B5.setIcon(gpio_in_low);

        // B6
        dir = (((Long) mod.read(mod.startAddr()) >> 14) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 6) & 1L) == 1L ? true : false;
        if(dir & data)
            B6.setIcon(gpio_out_high);
        else if(dir & !data)
            B6.setIcon(gpio_out_low);
        else if(!dir & data)
            B6.setIcon(gpio_in_high);
        else if(!dir & !data)
            B6.setIcon(gpio_in_low);

        // B7
        dir = (((Long) mod.read(mod.startAddr()) >> 15) & 1L) == 1L ? true : false;
        data = (((Long) mod.read(mod.startAddr()+8) >> 7) & 1L) == 1L ? true : false;
        if(dir & data)
            B7.setIcon(gpio_out_high);
        else if(dir & !data)
            B7.setIcon(gpio_out_low);
        else if(!dir & data)
            B7.setIcon(gpio_in_high);
        else if(!dir & !data)
            B7.setIcon(gpio_in_low);
    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        lblTristateReg = new javax.swing.JLabel();
        txtTristateReg = new javax.swing.JTextField();
        lblPortA = new javax.swing.JLabel();
        A7 = new javax.swing.JLabel();
        A6 = new javax.swing.JLabel();
        A5 = new javax.swing.JLabel();
        A4 = new javax.swing.JLabel();
        A3 = new javax.swing.JLabel();
        A2 = new javax.swing.JLabel();
        A1 = new javax.swing.JLabel();
        A0 = new javax.swing.JLabel();
        lblPortB = new javax.swing.JLabel();
        B7 = new javax.swing.JLabel();
        B6 = new javax.swing.JLabel();
        B5 = new javax.swing.JLabel();
        B4 = new javax.swing.JLabel();
        B3 = new javax.swing.JLabel();
        B2 = new javax.swing.JLabel();
        B1 = new javax.swing.JLabel();
        B0 = new javax.swing.JLabel();

        setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE);
        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(GPIOFrame.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N
        setResizable(false);

        lblTristateReg.setText(resourceMap.getString("lblTristateReg.text")); // NOI18N
        lblTristateReg.setName("lblTristateReg"); // NOI18N

        txtTristateReg.setEditable(false);
        txtTristateReg.setText(resourceMap.getString("txtTristateReg.text")); // NOI18N
        txtTristateReg.setName("txtTristateReg"); // NOI18N

        lblPortA.setText(resourceMap.getString("lblPortA.text")); // NOI18N
        lblPortA.setName("lblPortA"); // NOI18N

        A7.setIcon(resourceMap.getIcon("A7.icon")); // NOI18N
        A7.setText(resourceMap.getString("A7.text")); // NOI18N
        A7.setName("A7"); // NOI18N

        A6.setIcon(null);
        A6.setName("A6"); // NOI18N

        A5.setIcon(null);
        A5.setName("A5"); // NOI18N

        A4.setIcon(null);
        A4.setName("A4"); // NOI18N

        A3.setIcon(null);
        A3.setName("A3"); // NOI18N

        A2.setIcon(null);
        A2.setName("A2"); // NOI18N

        A1.setIcon(null);
        A1.setName("A1"); // NOI18N

        A0.setIcon(null);
        A0.setName("A0"); // NOI18N

        lblPortB.setText(resourceMap.getString("lblPortB.text")); // NOI18N
        lblPortB.setName("lblPortB"); // NOI18N

        B7.setIcon(null);
        B7.setName("B7"); // NOI18N

        B6.setIcon(null);
        B6.setName("B6"); // NOI18N

        B5.setIcon(null);
        B5.setName("B5"); // NOI18N

        B4.setIcon(null);
        B4.setName("B4"); // NOI18N

        B3.setIcon(null);
        B3.setName("B3"); // NOI18N

        B2.setIcon(null);
        B2.setName("B2"); // NOI18N

        B1.setIcon(null);
        B1.setName("B1"); // NOI18N

        B0.setIcon(null);
        B0.setName("B0"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(lblTristateReg)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(txtTristateReg, javax.swing.GroupLayout.DEFAULT_SIZE, 266, Short.MAX_VALUE))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(lblPortA)
                        .addGap(18, 18, 18)
                        .addComponent(A7)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A6)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A5)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A4)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A3)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A0))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(lblPortB)
                        .addGap(18, 18, 18)
                        .addComponent(B7)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B6)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B5)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B4)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B3)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B2)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B1)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B0)))
                .addContainerGap())
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(lblTristateReg)
                    .addComponent(txtTristateReg, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(lblPortA)
                        .addComponent(A7))
                    .addComponent(A6)
                    .addComponent(A5)
                    .addComponent(A4)
                    .addComponent(A3)
                    .addComponent(A2)
                    .addComponent(A1)
                    .addComponent(A0))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(lblPortB)
                    .addComponent(B7)
                    .addComponent(B6)
                    .addComponent(B5)
                    .addComponent(B4)
                    .addComponent(B3)
                    .addComponent(B2)
                    .addComponent(B1)
                    .addComponent(B0))
                .addContainerGap(46, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JLabel A0;
    private javax.swing.JLabel A1;
    private javax.swing.JLabel A2;
    private javax.swing.JLabel A3;
    private javax.swing.JLabel A4;
    private javax.swing.JLabel A5;
    private javax.swing.JLabel A6;
    private javax.swing.JLabel A7;
    private javax.swing.JLabel B0;
    private javax.swing.JLabel B1;
    private javax.swing.JLabel B2;
    private javax.swing.JLabel B3;
    private javax.swing.JLabel B4;
    private javax.swing.JLabel B5;
    private javax.swing.JLabel B6;
    private javax.swing.JLabel B7;
    private javax.swing.JLabel lblPortA;
    private javax.swing.JLabel lblPortB;
    private javax.swing.JLabel lblTristateReg;
    private javax.swing.JTextField txtTristateReg;
    // End of variables declaration//GEN-END:variables

}
