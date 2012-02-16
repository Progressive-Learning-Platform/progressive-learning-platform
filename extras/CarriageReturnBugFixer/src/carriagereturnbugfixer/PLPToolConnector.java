/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package carriagereturnbugfixer;

import plptool.*;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import javax.swing.JOptionPane;
import javax.swing.JMenuItem;
import java.io.File;
import java.util.regex.*;

/**
 *
 * @author wira
 */
public class PLPToolConnector implements PLPGenericModule {
    private ProjectDriver plp;
    private boolean init = false;
    private JMenuItem menuCleanFile;

    public Object hook(Object param) {
        if(param instanceof ProjectEvent && init && plp.g()) {
            ProjectEvent ev = (ProjectEvent) param;
            if(ev.getIdentifier() == ProjectEvent.EXIT) {
                int ret = JOptionPane.showConfirmDialog(plp.g_dev,
                        "Do you want to uninstall the " +
                        "carriage return bug fixer module so it won't run next time?",
                        "Remove Carriage Return Bug Fixer", JOptionPane.YES_NO_OPTION);
                if(ret == JOptionPane.YES_OPTION) {
                    File f = new File(PLPToolbox.getConfDir() + "/autoload/CarriageReturnBugFixer.jar");
                    f.delete();
                }
            }
        } else if(param instanceof ProjectDriver) {
            this.plp = (ProjectDriver) param;
            init = true;

            menuCleanFile = new JMenuItem();
            menuCleanFile.setText("Clean up this file, please!");
            menuCleanFile.addActionListener(new java.awt.event.ActionListener() {
                public void actionPerformed(java.awt.event.ActionEvent evt) {
                    String editorText = plp.g_dev.getEditorText();
                    Pattern p1 = Pattern.compile("\\r*");
                    Pattern p2 = Pattern.compile("\\r?\\n+");
                    Matcher m1 = p1.matcher(editorText);
                    Matcher m2 = p2.matcher(m1.replaceAll(""));
                    plp.g_dev.setEditorText(m2.replaceAll("\n"));
                    plp.setModified();
                }
            });
            plp.g_dev.addToolsItem(menuCleanFile);
        }

        return null;
    }

    public String getVersion() {
        return "1.0.0";
    }

    public String toString() {
        return "Carriage Return Bug Fixer";
    }
}
