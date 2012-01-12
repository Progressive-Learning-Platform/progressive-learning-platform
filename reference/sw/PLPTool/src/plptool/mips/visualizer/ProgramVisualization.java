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

package plptool.mips.visualizer;
import plptool.gui.ProjectDriver;
import plptool.*;
import plptool.mips.*;
import java.io.*;
import java.util.ArrayList;
/* remove upon proper jung integration
import org.jgrapht.*;
import org.jgrapht.graph.*;
import org.jgrapht.ext.*;
import org.jgrapht.alg.*;
 *
 */

import javax.swing.*;

/**
 * Using JUNG, this will create a flowchart like visualization of a PLP program.
 * @author will
 
public class ProgramVisualization {
    private ProjectDriver plp;


    public ProgramVisualization(ProjectDriver plp){
        this.plp=plp;
    }

    public void printProgram(){
        //Msg.M("\nTest.");
        plptool.mips.Formatter progformat = new plptool.mips.Formatter();
        //plptool.mips.Asm myasm = new plptool.mips.Asm();

        plp.assemble();
        String label;
        String instr_str;
        String instr_array[];
        int jump_index;
        int branch_index;
        long branch_imm;

        //Msg.M(String.format("0x%08x",plp.asm.getAddrTable()[0]));

        //Msg.M(plptool.mips.Formatter.mipsInstrStr(plp.asm.getAddrTable()[0]));

        //Msg.M("array index\tinstr addr\tinstr str");
        long[] addr_table = plp.asm.getAddrTable();
        long[] obj_table = plp.asm.getObjectCode();

        //Msg.M(progformat.mipsInstrStr(obj_table[0]));


        for(int addindex=0; addindex < addr_table.length; addindex++){
            instr_str=progformat.mipsInstrStr(obj_table[addindex]);
            instr_array=instr_str.split(" ");
            //Msg.M(instr_str);
            //Msg.M(instr_array[addindex]);
            if(instr_array[0].equals("jal")){
                jump_index=Integer.parseInt(instr_array[5]);
                Msg.M("jal " + plp.asm.lookupLabel(addr_table[jump_index]));
            }

            if(instr_array[0].equals("j")){
                jump_index=Integer.parseInt(instr_array[5]);
                Msg.M("j " + plp.asm.lookupLabel(addr_table[jump_index]));
            }

            if(instr_array[0].equals("beq")){
                branch_imm=Integer.parseInt(instr_array[5]);
                branch_imm=(long)(short) branch_imm;

                //Msg.M("beq " + String.format("0x%08x",branch_imm));
                //Msg.M("beq" + branch_imm);
                branch_index=(int)branch_imm+addindex+1;
                Msg.M("beq " + plp.asm.lookupLabel(addr_table[branch_index]));
            }

            /*
            //Msg.M(myasm.lookupLabel(plp.asm.getAddrTable()[addindex]));
            System.out.print(addindex);
            Msg.m("\t");
            Msg.m(String.format("0x%08x",addr_table[addindex]));
            //System.out.print(addr_table[addindex]);
            Msg.m("\t");
            Msg.m(progformat.mipsInstrStr(obj_table[addindex]));
            Msg.m("\t\t");

            if((label = plp.asm.lookupLabel(addr_table[addindex])) != null) {
                Msg.m(label + "\n\t\t");
            } else {
                Msg.m("\t\t");
            }/

            //Msg.m(plp.asm.lookupLabel(addr_table[addindex]));
            //Msg.m("\n");
        }
    }
    public final class programGraph{
        // instance field
        //private JGraphModelAdapter<String, DefaultEdge> jgAdapter;
        
        //methods
        public void initGraph(){
            //ProgramVisualization.programGraph graph = new programGraph();
            //graph.buildGraph();
            DirectedGraph<String, DefaultEdge> programGraph = buildGraph();
            Msg.M(programGraph.toString());
        }
        //construct the graph
        public ListenableDirectedGraph<String, DefaultEdge> buildGraph(){

            plptool.mips.Formatter progformat = new plptool.mips.Formatter();
            plp.assemble();
            String label;
            String instr_str;
            String instr_array[];
            int jump_index;
            int branch_index;
            int previousVertex;
            long branch_imm;
            ArrayList<String> vertices = new ArrayList<String>();

            ListenableDirectedGraph<String, DefaultEdge> progGraph = new ListenableDirectedGraph<String, DefaultEdge>(DefaultEdge.class);
            //jgAdapter = new JGraphModelAdapter<String, DefaultEdge>(progGraph);
            //JGraph jgraph = new JGraph(jgAdapter);

            long[] addr_table = plp.asm.getAddrTable();
            long[] obj_table = plp.asm.getObjectCode();

            vertices.add("Begin");
            previousVertex=0;
            progGraph.addVertex(vertices.get(vertices.size()-1));

            for(int addindex=0; addindex < addr_table.length; addindex++){
                instr_str=progformat.mipsInstrStr(obj_table[addindex]);
                instr_array=instr_str.split(" ");

                if(instr_array[0].equals("jal")){
                    jump_index=Integer.parseInt(instr_array[5]);
                    vertices.add("jal " + plp.asm.lookupLabel(addr_table[jump_index]));
                    Msg.M(vertices.get(vertices.size()-1));
                    progGraph.addVertex(vertices.get(vertices.size()-1));
                }

                if(instr_array[0].equals("j")){
                    jump_index=Integer.parseInt(instr_array[5]);
                    vertices.add("j " + plp.asm.lookupLabel(addr_table[jump_index]));
                    Msg.M(vertices.get(vertices.size()-1));
                    progGraph.addVertex(vertices.get(vertices.size()-1));
                    progGraph.addEdge(vertices.get(previousVertex), vertices.get(vertices.size()-1));
                }

                if(instr_array[0].equals("beq")){
                    // calculate branch target
                    branch_imm=Integer.parseInt(instr_array[5]);
                    branch_imm=(long)(short) branch_imm;
                    branch_index=(int)branch_imm+addindex+1;
                    vertices.add("beq " + plp.asm.lookupLabel(addr_table[branch_index]));
                    Msg.M(vertices.get(vertices.size()-1));
                    progGraph.addVertex(vertices.get(vertices.size()-1));
                }
            }
            return progGraph;
        }
    }
}
*/