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
import plptool.gui.PLPToolApp;
import plptool.gui.ProjectDriver;
import plptool.gui.ProjectEvent;
import javax.swing.JMenuItem;
import javax.swing.JMenu;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 *
 * @author wira
 */
public class PLPToolConnector implements PLPGenericModule {
    private ProjectDriver plp;
    private boolean processed = false;
    private PLPCPUProgram flowchart;
    private ExportDOT exportFrame;
    private String dotPath;

    public Object hook(Object param) {
        if(param instanceof ProjectDriver) {
            this.plp = (ProjectDriver) param;
            init();
        } else if(param instanceof String) {
            String cmd = (String) param;
            if(cmd.equals("process")) {
                if(plp.isAssembled()) {
                    flowchart = new PLPCPUProgram(plp);
                    processed = true;
                }
            } else if(cmd.equals("print")) {
                if(flowchart != null)
                    for(int i = 0; i < flowchart.getNumberOfRoutines(); i++)
                        flowchart.printProgram(i);
            } else if(cmd.equals("dot")) {
                if(processed) {
                    for(int i = 0; i < flowchart.getNumberOfRoutines(); i++) {
                        Msg.I("Flowchart for " + flowchart.getRoutine(i).getHead().getLabel(), this);
                        Msg.P(flowchart.generateDOT(i));
                    }
                }
            }
        } else if(param instanceof ProjectEvent) {
            ProjectEvent ev = (ProjectEvent) param;
            switch(ev.getIdentifier()) {
                case ProjectEvent.CONFIG_SAVE:
                    java.io.FileWriter out = (java.io.FileWriter) ev.getParameters();
                    try {
                        out.write("flowchart_dotpath::" + dotPath + "\n");
                    } catch(java.io.IOException e) {
                        return Msg.E("Unable to save configuration.",
                                Constants.PLP_GENERAL_IO_ERROR, this);
                    }
                    return true;
            }
        }

        return null;
    }

    private void init() {
        Msg.I("<em>Flowchart Generator</em> is ready &mdash; This module can be accessed through the <b>Tools" +
                "</b>&rarr;<b>Flowchart Generator</b> menu",
                null);
        this.dotPath = PLPToolApp.getAttributes().get("flowchart_dotpath");
        exportFrame = new ExportDOT(plp.g_dev);
        JMenu menuFlowchart = new JMenu("Flowchart Generator");
        JMenuItem menuFlowchartDisplay = new JMenuItem("Display flowchart for the project");
        menuFlowchartDisplay.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if(plp.isAssembled()) {
                    flowchart = new PLPCPUProgram(plp);
                    //exportFrame.update(flowchart);
                    //exportFrame.setVisible(true);
                } else
                    Msg.I("The project needs to be assembled first.", null);
            }
        });
        JMenuItem menuFlowchartSetupDOT = new JMenuItem("Setup DOT for flowchart generation in PLPTool...");
        menuFlowchartSetupDOT.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {

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

    public String getVersion() {
        return "0.1";
    }
}
