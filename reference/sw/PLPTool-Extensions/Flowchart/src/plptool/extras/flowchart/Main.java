/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.extras.flowchart;

import plptool.Constants;
import plptool.gui.ProjectDriver;

/**
 *
 * @author wira
 */
public class Main {
    public static void main(String args[]) {
        System.out.println("Flowchart Test");
        if(args.length != 1)
            return;

        ProjectDriver plp = new ProjectDriver(Constants.PLP_DEFAULT);
        plp.open(args[0], true);
        if(!plp.isAssembled())
            plp.assemble();
        if(plp.isAssembled()) {
            PLPCPUProgram p = new PLPCPUProgram(plp);
            p.printProgram(0);
        }
    }
}
