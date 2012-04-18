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

package carriagereturnbugfixer;

import plptool.*;
import javax.swing.JOptionPane;
import java.io.File;
import javax.swing.JFileChooser;

/**
 *
 * @author wira
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        File jar = new File((new File(".")).getAbsolutePath() + "/CarriageReturnBugFixer.jar");
        if(!jar.exists()) {
            JFileChooser fc = new JFileChooser();
            fc.setCurrentDirectory(new File("."));
            if(fc.showOpenDialog(fc) == fc.APPROVE_OPTION) {
                jar = new File(fc.getSelectedFile().getAbsolutePath());
            } else
                System.exit(-1);
        }

        String title = DynamicModuleFramework.getManifestEntry(jar.getAbsolutePath(), "title");
        if(title == null || !title.equals("PLPTool 4.0 Carriage Return Bug Fixer")) {
            JOptionPane.showMessageDialog(null, "Wrong File!", "Wrong File", JOptionPane.ERROR_MESSAGE);
            System.exit(-1);
        }

        boolean ret = PLPToolbox.downloadJARForAutoload("file:///" + jar, null, false);
        if(ret)
            JOptionPane.showMessageDialog(null, "The Carriage Return bug fixer will start when PLPTool 4.1 launches.");
    }

}
