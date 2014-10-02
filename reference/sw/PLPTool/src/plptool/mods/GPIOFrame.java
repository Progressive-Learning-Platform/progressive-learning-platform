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

    /** Creates new form GPIOFrame */
    public GPIOFrame(GPIO mod) {
        this.setIconImage(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/toolbar_sim_gpio.png")));
        initComponents();
        this.mod = mod;
        this.setTitle("GPIO Module at " + PLPToolbox.format32Hex(mod.startAddr()));

        gpio_in_low = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_in_low.png")));
        gpio_in_high = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_input.png")));
        gpio_out_low = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_out_low.png")));
        gpio_out_high = new ImageIcon(java.awt.Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("resources/gpio_output.png")));
    }

    public void setTristateRegisterValues(long val) {
        txtTristateReg.setText(PLPToolbox.format32Hex(val));
    }

    public final void updateOutputs() {
        boolean dir, data;

        // A0
        dir = (((Long) mod.read(mod.startAddr()) >> 0) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 0) & 1L) == 1L;
        if(dir & data)
            A0.setIcon(gpio_out_high);
        else if(dir & !data)
            A0.setIcon(gpio_out_low);
        else if(!dir & data)
            A0.setIcon(gpio_in_high);
        else if(!dir & !data)
            A0.setIcon(gpio_in_low);

        // A1
        dir = (((Long) mod.read(mod.startAddr()) >> 1) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 1) & 1L) == 1L;
        if(dir & data)
            A1.setIcon(gpio_out_high);
        else if(dir & !data)
            A1.setIcon(gpio_out_low);
        else if(!dir & data)
            A1.setIcon(gpio_in_high);
        else if(!dir & !data)
            A1.setIcon(gpio_in_low);

        // A2
        dir = (((Long) mod.read(mod.startAddr()) >> 2) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 2) & 1L) == 1L;
        if(dir & data)
            A2.setIcon(gpio_out_high);
        else if(dir & !data)
            A2.setIcon(gpio_out_low);
        else if(!dir & data)
            A2.setIcon(gpio_in_high);
        else if(!dir & !data)
            A2.setIcon(gpio_in_low);

        // A3
        dir = (((Long) mod.read(mod.startAddr()) >> 3) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 3) & 1L) == 1L;
        if(dir & data)
            A3.setIcon(gpio_out_high);
        else if(dir & !data)
            A3.setIcon(gpio_out_low);
        else if(!dir & data)
            A3.setIcon(gpio_in_high);
        else if(!dir & !data)
            A3.setIcon(gpio_in_low);

        // A4
        dir = (((Long) mod.read(mod.startAddr()) >> 4) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 4) & 1L) == 1L;
        if(dir & data)
            A4.setIcon(gpio_out_high);
        else if(dir & !data)
            A4.setIcon(gpio_out_low);
        else if(!dir & data)
            A4.setIcon(gpio_in_high);
        else if(!dir & !data)
            A4.setIcon(gpio_in_low);

        // A5
        dir = (((Long) mod.read(mod.startAddr()) >> 5) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 5) & 1L) == 1L;
        if(dir & data)
            A5.setIcon(gpio_out_high);
        else if(dir & !data)
            A5.setIcon(gpio_out_low);
        else if(!dir & data)
            A5.setIcon(gpio_in_high);
        else if(!dir & !data)
            A5.setIcon(gpio_in_low);

        // A6
        dir = (((Long) mod.read(mod.startAddr()) >> 6) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 6) & 1L) == 1L;
        if(dir & data)
            A6.setIcon(gpio_out_high);
        else if(dir & !data)
            A6.setIcon(gpio_out_low);
        else if(!dir & data)
            A6.setIcon(gpio_in_high);
        else if(!dir & !data)
            A6.setIcon(gpio_in_low);

        // A7
        dir = (((Long) mod.read(mod.startAddr()) >> 7) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+4) >> 7) & 1L) == 1L;
        if(dir & data)
            A7.setIcon(gpio_out_high);
        else if(dir & !data)
            A7.setIcon(gpio_out_low);
        else if(!dir & data)
            A7.setIcon(gpio_in_high);
        else if(!dir & !data)
            A7.setIcon(gpio_in_low);

        // B0
        dir = (((Long) mod.read(mod.startAddr()) >> 8) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 0) & 1L) == 1L;
        if(dir & data)
            B0.setIcon(gpio_out_high);
        else if(dir & !data)
            B0.setIcon(gpio_out_low);
        else if(!dir & data)
            B0.setIcon(gpio_in_high);
        else if(!dir & !data)
            B0.setIcon(gpio_in_low);

        // B1
        dir = (((Long) mod.read(mod.startAddr()) >> 9) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 1) & 1L) == 1L;
        if(dir & data)
            B1.setIcon(gpio_out_high);
        else if(dir & !data)
            B1.setIcon(gpio_out_low);
        else if(!dir & data)
            B1.setIcon(gpio_in_high);
        else if(!dir & !data)
            B1.setIcon(gpio_in_low);

        // B2
        dir = (((Long) mod.read(mod.startAddr()) >> 10) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 2) & 1L) == 1L;
        if(dir & data)
            B2.setIcon(gpio_out_high);
        else if(dir & !data)
            B2.setIcon(gpio_out_low);
        else if(!dir & data)
            B2.setIcon(gpio_in_high);
        else if(!dir & !data)
            B2.setIcon(gpio_in_low);

        // B3
        dir = (((Long) mod.read(mod.startAddr()) >> 11) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 3) & 1L) == 1L;
        if(dir & data)
            B3.setIcon(gpio_out_high);
        else if(dir & !data)
            B3.setIcon(gpio_out_low);
        else if(!dir & data)
            B3.setIcon(gpio_in_high);
        else if(!dir & !data)
            B3.setIcon(gpio_in_low);

        // B4
        dir = (((Long) mod.read(mod.startAddr()) >> 12) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 4) & 1L) == 1L;
        if(dir & data)
            B4.setIcon(gpio_out_high);
        else if(dir & !data)
            B4.setIcon(gpio_out_low);
        else if(!dir & data)
            B4.setIcon(gpio_in_high);
        else if(!dir & !data)
            B4.setIcon(gpio_in_low);

        // B5
        dir = (((Long) mod.read(mod.startAddr()) >> 13) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 5) & 1L) == 1L;
        if(dir & data)
            B5.setIcon(gpio_out_high);
        else if(dir & !data)
            B5.setIcon(gpio_out_low);
        else if(!dir & data)
            B5.setIcon(gpio_in_high);
        else if(!dir & !data)
            B5.setIcon(gpio_in_low);

        // B6
        dir = (((Long) mod.read(mod.startAddr()) >> 14) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 6) & 1L) == 1L;
        if(dir & data)
            B6.setIcon(gpio_out_high);
        else if(dir & !data)
            B6.setIcon(gpio_out_low);
        else if(!dir & data)
            B6.setIcon(gpio_in_high);
        else if(!dir & !data)
            B6.setIcon(gpio_in_low);

        // B7
        dir = (((Long) mod.read(mod.startAddr()) >> 15) & 1L) == 1L;
        data = (((Long) mod.read(mod.startAddr()+8) >> 7) & 1L) == 1L;
        if(dir & data)
            B7.setIcon(gpio_out_high);
        else if(dir & !data)
            B7.setIcon(gpio_out_low);
        else if(!dir & data)
            B7.setIcon(gpio_in_high);
        else if(!dir & !data)
            B7.setIcon(gpio_in_low);
    }

    private void updateModuleRegisters(int position) {
        boolean isInput = (((Long) mod.read(mod.startAddr()) >> position) & 1L) == 0L;
        int blockOffset = position < 8 ? 4 : 8;
        if(isInput) {
            long data = (Long) mod.read(mod.startAddr()+blockOffset);
            data ^= (1L << (blockOffset == 4 ? position : position - 8));
            mod.writeReg(mod.startAddr()+blockOffset, data, false);
            updateOutputs();
        }
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
        jSeparator1 = new javax.swing.JSeparator();

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
        A7.setPreferredSize(new java.awt.Dimension(30, 60));
        A7.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A7MousePressed(evt);
            }
        });

        A6.setIcon(resourceMap.getIcon("A6.icon")); // NOI18N
        A6.setName("A6"); // NOI18N
        A6.setPreferredSize(new java.awt.Dimension(30, 60));
        A6.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A6MousePressed(evt);
            }
        });

        A5.setIcon(resourceMap.getIcon("A5.icon")); // NOI18N
        A5.setName("A5"); // NOI18N
        A5.setPreferredSize(new java.awt.Dimension(30, 60));
        A5.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A5MousePressed(evt);
            }
        });

        A4.setIcon(resourceMap.getIcon("A4.icon")); // NOI18N
        A4.setName("A4"); // NOI18N
        A4.setPreferredSize(new java.awt.Dimension(30, 60));
        A4.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A4MousePressed(evt);
            }
        });

        A3.setIcon(resourceMap.getIcon("A3.icon")); // NOI18N
        A3.setName("A3"); // NOI18N
        A3.setPreferredSize(new java.awt.Dimension(30, 60));
        A3.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A3MousePressed(evt);
            }
        });

        A2.setIcon(resourceMap.getIcon("A2.icon")); // NOI18N
        A2.setName("A2"); // NOI18N
        A2.setPreferredSize(new java.awt.Dimension(30, 60));
        A2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A2MousePressed(evt);
            }
        });

        A1.setIcon(resourceMap.getIcon("A1.icon")); // NOI18N
        A1.setName("A1"); // NOI18N
        A1.setPreferredSize(new java.awt.Dimension(30, 60));
        A1.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A1MousePressed(evt);
            }
        });

        A0.setIcon(resourceMap.getIcon("A0.icon")); // NOI18N
        A0.setName("A0"); // NOI18N
        A0.setPreferredSize(new java.awt.Dimension(30, 60));
        A0.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                A0MousePressed(evt);
            }
        });

        lblPortB.setText(resourceMap.getString("lblPortB.text")); // NOI18N
        lblPortB.setName("lblPortB"); // NOI18N

        B7.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B7.setName("B7"); // NOI18N
        B7.setPreferredSize(new java.awt.Dimension(30, 60));
        B7.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B7MousePressed(evt);
            }
        });

        B6.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B6.setName("B6"); // NOI18N
        B6.setPreferredSize(new java.awt.Dimension(30, 60));
        B6.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B6MousePressed(evt);
            }
        });

        B5.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B5.setName("B5"); // NOI18N
        B5.setPreferredSize(new java.awt.Dimension(30, 60));
        B5.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B5MousePressed(evt);
            }
        });

        B4.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B4.setName("B4"); // NOI18N
        B4.setPreferredSize(new java.awt.Dimension(30, 60));
        B4.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B4MousePressed(evt);
            }
        });

        B3.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B3.setName("B3"); // NOI18N
        B3.setPreferredSize(new java.awt.Dimension(30, 60));
        B3.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B3MousePressed(evt);
            }
        });

        B2.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B2.setName("B2"); // NOI18N
        B2.setPreferredSize(new java.awt.Dimension(30, 60));
        B2.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B2MousePressed(evt);
            }
        });

        B1.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B1.setName("B1"); // NOI18N
        B1.setPreferredSize(new java.awt.Dimension(30, 60));
        B1.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B1MousePressed(evt);
            }
        });

        B0.setIcon(resourceMap.getIcon("B7.icon")); // NOI18N
        B0.setName("B0"); // NOI18N
        B0.setPreferredSize(new java.awt.Dimension(30, 60));
        B0.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mousePressed(java.awt.event.MouseEvent evt) {
                B0MousePressed(evt);
            }
        });

        jSeparator1.setName("jSeparator1"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jSeparator1, javax.swing.GroupLayout.DEFAULT_SIZE, 350, Short.MAX_VALUE)
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(lblTristateReg)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(txtTristateReg, javax.swing.GroupLayout.DEFAULT_SIZE, 145, Short.MAX_VALUE))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(lblPortA)
                        .addGap(20, 20, 20)
                        .addComponent(A7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A6, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A5, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(A0, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(layout.createSequentialGroup()
                        .addComponent(lblPortB)
                        .addGap(18, 18, 18)
                        .addComponent(B7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B6, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B5, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(B0, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)))
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
                    .addComponent(A6, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(A5, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(A4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(A3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(A2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(A1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(A0, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(lblPortA)
                            .addComponent(A7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))))
                .addGap(18, 18, 18)
                .addComponent(jSeparator1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(10, 10, 10)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(lblPortB)
                    .addComponent(B7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(B6, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(B5, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(B4, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(B3, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(B2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(B1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(B0, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(55, 55, 55))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void A7MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A7MousePressed
        updateModuleRegisters(7);
    }//GEN-LAST:event_A7MousePressed

    private void A6MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A6MousePressed
        updateModuleRegisters(6);
    }//GEN-LAST:event_A6MousePressed

    private void A5MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A5MousePressed
        updateModuleRegisters(5);
    }//GEN-LAST:event_A5MousePressed

    private void A4MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A4MousePressed
        updateModuleRegisters(4);
    }//GEN-LAST:event_A4MousePressed

    private void A3MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A3MousePressed
        updateModuleRegisters(3);
    }//GEN-LAST:event_A3MousePressed

    private void A2MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A2MousePressed
        updateModuleRegisters(2);
    }//GEN-LAST:event_A2MousePressed

    private void A1MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A1MousePressed
        updateModuleRegisters(1);
    }//GEN-LAST:event_A1MousePressed

    private void A0MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_A0MousePressed
        updateModuleRegisters(0);
    }//GEN-LAST:event_A0MousePressed

    private void B0MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B0MousePressed
        updateModuleRegisters(8);
    }//GEN-LAST:event_B0MousePressed

    private void B1MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B1MousePressed
        updateModuleRegisters(9);
    }//GEN-LAST:event_B1MousePressed

    private void B2MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B2MousePressed
        updateModuleRegisters(10);
    }//GEN-LAST:event_B2MousePressed

    private void B3MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B3MousePressed
        updateModuleRegisters(11);
    }//GEN-LAST:event_B3MousePressed

    private void B4MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B4MousePressed
        updateModuleRegisters(12);
    }//GEN-LAST:event_B4MousePressed

    private void B5MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B5MousePressed
        updateModuleRegisters(13);
    }//GEN-LAST:event_B5MousePressed

    private void B6MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B6MousePressed
        updateModuleRegisters(14);
    }//GEN-LAST:event_B6MousePressed

    private void B7MousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_B7MousePressed
        updateModuleRegisters(15);
    }//GEN-LAST:event_B7MousePressed

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
    private javax.swing.JSeparator jSeparator1;
    private javax.swing.JLabel lblPortA;
    private javax.swing.JLabel lblPortB;
    private javax.swing.JLabel lblTristateReg;
    private javax.swing.JTextField txtTristateReg;
    // End of variables declaration//GEN-END:variables

}
