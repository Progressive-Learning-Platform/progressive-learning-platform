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
        Msg.I("<em>Flowchart Generator</em> is ready &mdash; This module can be accessed through the <b>Tools" +
                "</b>&rarr;<b>Flowchart Generator</b> menu",
                null);
        this.dotPath = PLPToolApp.getAttributes().get("flowchart_dotpath");
        exportFrame = new ExportDOT(plp.g_dev);
        setupFrame = new SetupDOT(plp.g_dev, this);
        displayFrame = new DisplayFlowchart(plp.g_dev);
        JMenu menuFlowchart = new JMenu("Flowchart Generator");
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
        menuFlowchart.add(menuFlowchartSetupDOT);
        menuFlowchart.add(menuFlowchartExportDOT);
        plp.g_dev.addToolsItem(menuFlowchart);
        CallbackRegistry.register_Save_Config(new Callback_Save_Config(this));
        CallbackRegistry.register_Command(new Callback_Command(this));
        Callback_Clear cb = new Callback_Clear(displayFrame);
        CallbackRegistry.register_Project_New(cb);
        CallbackRegistry.register_Project_Open_Successful(cb);
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
}

class Callback_Save_Config implements Callback {
    private PLPToolConnector c;

    public Callback_Save_Config(PLPToolConnector c) {
        this.c = c;
    }

    public boolean callback(Object param) {
        java.io.FileWriter out = (java.io.FileWriter) param;
        try {
            Msg.I("Saving configuration.", c);
            out.write("flowchart_dotpath::" + c.getDotPath() + "\n");
            return true;
        } catch(java.io.IOException e) {
            Msg.E("Unable to save configuration.",
                    Constants.PLP_GENERAL_IO_ERROR, this);
        }

        return false;
    }
}

class Callback_Command implements Callback {
    private PLPToolConnector c;

    public Callback_Command(PLPToolConnector c) {
        this.c = c;
    }

    public boolean callback(Object param) {
        c.command((String) param);
        return true;
    }
}

class Callback_Clear implements Callback {
    private DisplayFlowchart displayFrame;

    public Callback_Clear(DisplayFlowchart displayFrame) {
        this.displayFrame = displayFrame;
    }

    public boolean callback(Object param) {
        Msg.D("Callback: clear", 2, null);
        
        displayFrame.clearCanvas();
        return true;
    }
}