/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package plptool.hc11;

import plptool.*;
import plptool.gui.ProjectDriver;
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class Asm extends PLPAsm {
    private String[] lines;

    int PC;

    public Asm(ArrayList<PLPAsmSource> asms) {
        super(asms);
    }

    public int assemble() {
        StringBuilder str = new StringBuilder();
        for(int i = 0; i < sourceList.size(); i++)
            str.append(sourceList.get(i).getAsmString());
        lines = str.toString().split("\\r?\\n");
        setAssembled(true);
        return Constants.PLP_OK;
    }

    public int preprocess(int index) {
        /* In first pass, we populate our symbol table and map addresses to
         * the correspding source file and line number. For this, we also need
         * to figure out instruction lengths (damn variable length instructions!)
         */


        return Constants.PLP_OK;
    }

    public String[] getLines() {
        return lines;
    }
}
