/*
    Copyright 2012 PLP Contributors

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

package plptool.extras.flowchart;

import plptool.*;
import plptool.dmf.*;
import plptool.gui.PLPToolApp;
import plptool.gui.ProjectDriver;
import javax.swing.JMenuItem;
import javax.swing.JMenu;
import javax.swing.ImageIcon;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 *
 * @author wira
 */
public class PLPToolConnector implements ModuleInterface5 {
    private ProjectDriver plp;
    private boolean processed = false;
    private PLPCPUProgram flowchart;
    private ExportDOT exportFrame;
    private SetupDOT setupFrame;
    private DisplayFlowchart displayFrame;
    private String dotPath;

    public int initialize(ProjectDriver plp) {
        this.plp = plp;
        return init();
    }

    public void command(String cmd) {
        if(cmd.equals("flowchart process")) {
            if(plp.isAssembled()) {
                flowchart = new PLPCPUProgram(plp);
                processed = true;
            }
        } else if(cmd.equals("flowchart print")) {
            if(flowchart != null)
                for(int i = 0; i < flowchart.getNumberOfRoutines(); i++)
                    flowchart.printProgram(i);
        } else if(cmd.equals("flowchart dot")) {
            if(processed) {
                for(int i = 0; i < flowchart.getNumberOfRoutines(); i++) {
                    Msg.I("Flowchart for " + flowchart.getRoutine(i).getHead().getLabel(), this);
                    Msg.P(flowchart.generateDOT(i, false));
                }
            }
        }
    }

    private int init() {
		if(!plp.g())
			return Constants.PLP_OK;
        Msg.I("<em>Flowchart Generator</em> is ready &mdash; This module can be accessed through the <b>Tools" +
                "</b>&rarr;<b>Flowchart Generator</b> menu",
                null);

        // Load config
        if(PLPToolbox.isFileReadable(PLPToolbox.getConfDir() + "/flowchart")) {
            Msg.D("flowchart generator: loading from config", 3, null);
            java.util.HashMap<String, String> config = PLPToolbox.parseConfig(PLPToolbox.getConfDir() + "/flowchart");
            if(config != null)
                dotPath = config.get("flowchart_dotpath");
                Msg.D("flowchart generator: saved dotpath is " + dotPath, 3, null);
        } else {
            dotPath = "";
        }
        PLPToolApp.getAttributes().put("flowchart_dotpath", dotPath);

        exportFrame = new ExportDOT(plp.g_dev);
        setupFrame = new SetupDOT(plp.g_dev, this);
        displayFrame = new DisplayFlowchart(plp.g_dev);
        displayFrame.setIconImage(PLPToolApp.getImage("plptool.extras.flowchart.icon.png"));
        JMenu menuFlowchart = new JMenu("Flowchart Generator");
        menuFlowchart.setIcon(new ImageIcon(PLPToolApp.getImage("plptool.extras.flowchart.icon.png")));
        JMenuItem menuFlowchartDisplay = new JMenuItem("Display flowchart for the project");
        menuFlowchartDisplay.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if(plp.isAssembled()) {
                    flowchart = new PLPCPUProgram(plp);
                    displayFrame.update(flowchart);
                    displayFrame.setVisible(true);
                } else
                    Msg.I("The project needs to be assembled first.", null);
            }
        });
        JMenuItem menuFlowchartSetupDOT = new JMenuItem("Setup DOT for flowchart generation in PLPTool...");
        menuFlowchartSetupDOT.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                setupFrame.setVisible(true);
            }
        });
        JMenuItem menuFlowchartExportDOT = new JMenuItem("Export a program routine as a .DOT file...");
        menuFlowchartExportDOT.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if(plp.isAssembled()) {
                    flowchart = new PLPCPUProgram(plp);
                    exportFrame.update(flowchart);
                    exportFrame.setVisible(true);
                } else
                    Msg.I("The project needs to be assembled first.", null);
            }
        });
        menuFlowchart.add(menuFlowchartDisplay);
        menuFlowchart.addSeparator();
        menuFlowchart.add(menuFlowchartSetupDOT);
        menuFlowchart.add(menuFlowchartExportDOT);
        if(plp.g())
            plp.g_dev.addToolsItem(menuFlowchart);
        CallbackRegistry.register(new Callback_Save_Config(), CallbackRegistry.SAVE_CONFIG  );
        CallbackRegistry.register(new Callback_Command(), CallbackRegistry.COMMAND);
        Callback_Clear cb = new Callback_Clear();
        CallbackRegistry.register(cb, 
                CallbackRegistry.PROJECT_NEW,
                CallbackRegistry.PROJECT_OPEN_SUCCESSFUL);
        return Constants.PLP_OK;
    }

    public void setDotPath(String dotPath) {
        this.dotPath = dotPath;
    }

    public String getDotPath() {
        return dotPath;
    }

    @Override
    public String toString() {
        return "Flowchart Generator";
    }

    public String getName() {
        return "Flowchart Generator";
    }

    public String getDescription() {
        return "Flowchart tool for PLPTool. Requires GraphViz.";
    }

    public int[] getMinimumPLPToolVersion() {
        int[] ret = {5, 0};
        return ret;
    }

    public int[] getVersion() {
        int[] ver = {1, 0};
        return ver;
    }


    class Callback_Save_Config implements Callback {
        public boolean callback(int callbackNum, Object param) {
            Msg.I("Saving configuration.", null);
            PLPToolbox.writeFile("flowchart_dotpath::" + dotPath + "\n",
                    PLPToolbox.getConfDir() + "/flowchart");
            return true;
        }
    }

    class Callback_Command implements Callback {
        public boolean callback(int callbackNum, Object param) {
            command((String) param);
            return true;
        }
    }

    class Callback_Clear implements Callback {
        public boolean callback(int callbackNum, Object param) {
            Msg.D("Callback: clear", 2, null);

            displayFrame.clearCanvas();
            return true;
        }
    }

}

