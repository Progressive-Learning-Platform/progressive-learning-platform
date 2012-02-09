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
// jung
import edu.uci.ics.jung.graph.*;
//import edu.uci.ics.jung.graph.DirectedOrderedSparseMultigraph;
import edu.uci.ics.jung.graph.util.*;
import org.apache.commons.collections15.*;
import javax.swing.*;

/**
 * Using JUNG, this will create a flowchart like visualization of a PLP program.
 * @author will
 *
 */
 
public class ProgramVisualization {
    private ProjectDriver plp;
    static Asm asm;

    public ProgramVisualization(ProjectDriver plp){
        this.plp=plp;
        asm = (Asm) plp.asm;
    }

    public void printProgram(){
        //Msg.M("\nTest.");
        plptool.mips.Formatter progformat = new plptool.mips.Formatter();
        //plptool.mips.Asm myasm = new plptool.mips.Asm();

        //plp.assemble();
        
        String label;
        String instr_str;
        String instr_array[];
        int jump_index;
        int branch_index;
        long branch_imm;

        //Msg.M(String.format("0x%08x",asm.getAddrTable()[0]));

        //Msg.M(plptool.mips.Formatter.mipsInstrStr(asm.getAddrTable()[0]));

        //Msg.M("array index\tinstr addr\tinstr str");
        long[] addr_table = asm.getAddrTable();
        long[] obj_table = asm.getObjectCode();

        //Msg.M(progformat.mipsInstrStr(obj_table[0]));


        for(int addindex=0; addindex < addr_table.length; addindex++){
            instr_str=progformat.mipsInstrStr(obj_table[addindex]);
            instr_array=instr_str.split(" ");
            Msg.M(asm.lookupLabel(addr_table[addindex]));
            /*
            Msg.M(instr_str);
            Msg.M(instr_array[addindex]);
            
            if(instr_array[0].equals("jal")){
                jump_index=Integer.parseInt(instr_array[5]);
                Msg.M("jal " + asm.lookupLabel(addr_table[jump_index]));
            }

            if(instr_array[0].equals("j")){
                jump_index=Integer.parseInt(instr_array[5]);
                Msg.M("j " + asm.lookupLabel(addr_table[jump_index]));
            }

            if(instr_array[0].equals("beq")){
                branch_imm=Integer.parseInt(instr_array[5]);
                branch_imm=(long)(short) branch_imm;

                //Msg.M("beq " + String.format("0x%08x",branch_imm));
                //Msg.M("beq" + branch_imm);
                branch_index=(int)branch_imm+addindex+1;
                Msg.M("beq " + asm.lookupLabel(addr_table[branch_index]));
            }

            */
            /*
            //Msg.M(myasm.lookupLabel(asm.getAddrTable()[addindex]));
            System.out.print(addindex);
            Msg.m("\t");
            Msg.m(String.format("0x%08x",addr_table[addindex]));
            //System.out.print(addr_table[addindex]);
            Msg.m("\t");
            Msg.m(progformat.mipsInstrStr(obj_table[addindex]));
            Msg.m("\t\t");

            if((label = asm.lookupLabel(addr_table[addindex])) != null) {
                Msg.m(label + "\n\t\t");
            } else {
                Msg.m("\t\t");
            }*/

            //Msg.m(asm.lookupLabel(addr_table[addindex]));
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
            Graph<String, String> programGraph = buildGraph();
            //Forest<String, String> programGraph = buildGraph();
            //Msg.M(programGraph.toString());
        }
        //construct the graph
        public DirectedOrderedSparseMultigraph<String, String> buildGraph(){
        //public DelegateForest<String, String> buildGraph(){

            plptool.mips.Formatter progformat = new plptool.mips.Formatter();
            //plp.assemble();
            //String label;
            String instr_str;
            String instr_array[];
            String currentLabel;
            String previousLabel;
            String testLabel;
            int jump_index;
            int branch_index;
            int previousVertex;
            long branch_imm;
            ArrayList<String> vertices = new ArrayList<String>();

            DirectedOrderedSparseMultigraph<String, String> progGraph = new DirectedOrderedSparseMultigraph<String, String>();
            //DelegateForest<String, String> progGraph = new DelegateForest<String, String>();

            long[] addr_table = asm.getAddrTable();
            long[] obj_table = asm.getObjectCode();

            vertices.add("Begin");
            previousVertex=0;
            //progGraph.addVertex(vertices.get(vertices.size()-1));
            //Msg.M(vertices.get(0));
            progGraph.addVertex(vertices.get(0));

            // add labels first
            //previousLabel=vertices.get(0);
            for(int addindex1=0; addindex1 < addr_table.length; addindex1++){
                currentLabel=asm.lookupLabel(addr_table[addindex1]);
                if(currentLabel!=null){
                    //Msg.M(currentLabel);
                    vertices.add(currentLabel);
                    progGraph.addVertex(currentLabel);
                    //progGraph.addEdge("" + addindex1, previousLabel, currentLabel, EdgeType.DIRECTED);
                    //previousLabel=currentLabel;
                }
            }
            progGraph.addVertex("End");
            //Msg.M(progGraph.toString());
            //progGraph.addEdge("FIGHT", "Begin", "start", EdgeType.DIRECTED);
            // add edges
            previousLabel=vertices.get(0);
            currentLabel=vertices.get(1);
            //Msg.M("previous1: " + previousLabel);
            //Msg.M("current1: " + currentLabel);
            for(int addindex=0; addindex < addr_table.length; addindex++){
                testLabel=asm.lookupLabel(addr_table[addindex]);
                if(testLabel!=null){
                    currentLabel=testLabel;
                }
                instr_str=progformat.mipsInstrStr(obj_table[addindex]);
                instr_array=instr_str.split(" ");

                if(instr_array[0].equals("jal")){
                    jump_index=Integer.parseInt(instr_array[5]);
                    //vertices.add("jal " + asm.lookupLabel(addr_table[jump_index]));
                    //Msg.M(vertices.get(vertices.size()-1));
                    //progGraph.addVertex(vertices.get(vertices.size()-1));
                    currentLabel=asm.lookupLabel(addr_table[jump_index]);
                    //progGraph.addEdge("jal" + addindex, previousLabel, currentLabel, EdgeType.DIRECTED);
                    //progGraph.addEdge("jr" + addindex, currentLabel, previousLabel, EdgeType.DIRECTED);
                }

                if(instr_array[0].equals("j")){
                    jump_index=Integer.parseInt(instr_array[5]);
                    //vertices.add("j " + asm.lookupLabel(addr_table[jump_index]));
                    //Msg.M(vertices.get(vertices.size()-1));
                    //progGraph.addVertex(vertices.get(vertices.size()-1));
                    //progGraph.addEdge(vertices.get(previousVertex) + " to " + vertices.get(vertices.size()-1), vertices.get(previousVertex), vertices.get(vertices.size()-1), EdgeType.DIRECTED);
                    currentLabel=asm.lookupLabel(addr_table[jump_index]);
                    //Msg.M("previous: " + previousLabel);
                    //Msg.M("current: " + currentLabel);
                    progGraph.addEdge("j" + addindex, previousLabel, currentLabel, EdgeType.DIRECTED);
                }

                if(instr_array[0].equals("beq")){
                    // calculate branch target
                    branch_imm=Integer.parseInt(instr_array[5]);
                    branch_imm=(long)(short) branch_imm;
                    branch_index=(int)branch_imm+addindex+1;
                    //vertices.add("beq " + asm.lookupLabel(addr_table[branch_index]));
                    currentLabel=asm.lookupLabel(addr_table[branch_index]);
                    //Msg.M(vertices.get(vertices.size()-1));
                    //progGraph.addVertex(vertices.get(vertices.size()-1));
                    progGraph.addEdge("beq" + addindex, previousLabel, currentLabel, EdgeType.DIRECTED);
                }
                previousLabel=currentLabel;
            }
            return progGraph;
        }
    }
}
