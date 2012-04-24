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
//import java.io.*;
import java.util.ArrayList;
import java.util.List;
// jung
import edu.uci.ics.jung.graph.*;
//import edu.uci.ics.jung.graph.DirectedOrderedSparseMultigraph;
import edu.uci.ics.jung.graph.util.*;
//import org.apache.commons.collections15.*;
//import javax.swing.*;

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
    // new data structure
    public class progTree{
        private progFunction root;

        public progTree(String rootLabel){
            root = new progFunction();
            root.data.setLabel(rootLabel);
        }

        private class progFunction{
            private List<progFunction> progChildren;
            private functionData data;
            private progFunction parent;

            progFunction(){
                data = null;
            }
            progFunction(functionData newData){
                data = newData;
            }
            public void addChild(progFunction newchild){
                progChildren.add(newchild);
            }
        }

        private class functionData {
            private String label;
            private long startAddress;
            private long endAddress;

            public functionData(){
            }
            public functionData(String newlabel){
                label = newlabel;
            }
            public functionData(String newlabel, long givenStartAddress, long givenEndAddress){
                label = newlabel;
                startAddress = givenStartAddress;
                endAddress = givenEndAddress;
            }
            public void setLabel(String newlabel){
                label = newlabel;
            }
            public void setStartAddress(long address){
                startAddress = address;
            }
            public void setEndAddress(long address){
                endAddress = address;
            }
            public long getStartAddress(){
                return startAddress;
            }
            public long getEndAddress(){
                return endAddress;
            }
            public String getLabel(){
                return label;
            }

        }
    }
    // end new data structure

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
        //Msg.M(asm.toString());
        for(int addindex=0; addindex < addr_table.length; addindex++){
            instr_str=progformat.mipsInstrStr(obj_table[addindex]);
            instr_array=instr_str.split(" ");
            Msg.M(asm.lookupLabel(addr_table[addindex]));
            
            Msg.M(instr_str);
            /*
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
        private long[] addr_table;
        private long[] obj_table;
        private ArrayList<String> vertices;
        private String currentLabel;
        private String previousLabel;
        plptool.mips.Formatter progformat = new plptool.mips.Formatter();
        //methods
        public void initGraph(){
            //ProgramVisualization.programGraph graph = new programGraph();
            //graph.buildGraph();
            Graph<String, String> programGraph = buildGraph();
            //Forest<String, String> programGraph = buildGraph();
            //Msg.M(programGraph.toString());
        }
        // populates address & object code tables for the program
        // used to build the graph object
        private void buildTables(){
            addr_table = asm.getAddrTable();
            obj_table = asm.getObjectCode();            
        }

        private void traverseProgram(int index, DirectedOrderedSparseMultigraph<String,String> progGraph) {
            boolean stop = false;
            String label;
            long instr;
            long addr;
            byte opcode;
            byte funct;
            String delimiters = "[ ,\t]+|[()]";
            
            while(!stop) {
                // traverse the program image starting from the specified index
                addr = addr_table[index];
                label = asm.lookupLabel(addr);
                if(label != null)
                    progGraph.addVertex(label);
                instr = obj_table[index];
                opcode = MIPSInstr.opcode(instr);
                funct = MIPSInstr.opcode(instr);

                // check if this instruction is a branch
                if(opcode == 0x04 || opcode == 0x05) {
                    int fileIndex = asm.getFileIndex(addr);
                    int lineNum = asm.getLineNum(addr);
                    plp.getAsm(fileIndex).getAsmLine(lineNum);
                    

                // check if this instruction is a j (j, jal, jr, or jalr)
                } else if((opcode == 0x02 || opcode == 0x03) ||
                        (opcode == 0 && (funct == 0x08 || funct == 0x09))) {

                }

                index++;
                if(index == obj_table.length || progGraph.containsVertex(label))
                    stop = true;
            }
        }

        // add every program label as a vertex to the graph
        private void addLabels(DirectedOrderedSparseMultigraph<String,String> progGraph){
            for(int addindex1=0; addindex1 < addr_table.length; addindex1++){
                currentLabel=asm.lookupLabel(addr_table[addindex1]);
                if(currentLabel!=null){
                    //Msg.M("Current Address " + addr_table[addindex1] + ": " + currentLabel);
                    vertices.add(currentLabel);
                    Msg.M("adding " + currentLabel);
                    progGraph.addVertex(currentLabel);
                }

            }
        }
        // for data & string labels
        // sort of works
        private void removeNonLabels(DirectedOrderedSparseMultigraph<String,String> progGraph){
            String instrStr;
            String instrArray[];
            for(int addindex1=0; addindex1 < addr_table.length; addindex1++){
                currentLabel=asm.lookupLabel(addr_table[addindex1]);
                if(obj_table.length > (addindex1+1)){
                    //Msg.M("obj index: " + addindex1+1);
                    instrStr=progformat.mipsInstrStr(obj_table[addindex1+1]);
                    //Msg.M(instrStr);
                    instrArray=instrStr.split(" ");
                    if(instrArray[0].equals("null") && (currentLabel!=null)){
                        Msg.M(currentLabel + "! Give, cur! Fall to thy betters!");
                        progGraph.removeVertex(currentLabel);
                    }
                }
            }
        }
        // add all the jumps/branches/traversals between labels
        // to the graph object as edges
        private void addEdges(DirectedOrderedSparseMultigraph<String,String> progGraph){
            String instr_str;
            String instr_array[];
            String testLabel;
            String jumpTargetLabel;
            String jalTargetLabel;
            String previousInstr=null;
            String twoInstrAgo=null;
            String twoInstrArray[];
            int jump_index;
            int branch_index;
            boolean jump=false;
            boolean jumpedAway=false;
            long branch_imm;
            
            // add edges
            previousLabel=null;
            currentLabel=null;
            for(int addindex=0; addindex < addr_table.length; addindex++){
                testLabel=asm.lookupLabel(addr_table[addindex]);
                instr_str=progformat.mipsInstrStr(obj_table[addindex]);
                instr_array=instr_str.split(" ");
                //Msg.M(instr_str);
                if(testLabel!=null){
                    currentLabel=testLabel;
                }
                else{
                    twoInstrAgo=previousInstr;
                }

                // determining a traversal from one label to another
                // without jumping

                // did the program jump away? (and account for branch delay slot)
                if(twoInstrAgo!=null){
                    twoInstrArray=twoInstrAgo.split(" ");
                    if(twoInstrArray[0].equals("j") || twoInstrArray[0].equals("beq") || twoInstrArray[0].equals("jr")){
                        jumpedAway=true;
                    }
                    else{
                        jumpedAway=false;
                    }
                    //Msg.M("two instr ago: " + twoInstrAgo);
                    //Msg.M("jumpedaway is " + jumpedAway);
                }
                // if not, then add non jump traversal
                if(currentLabel != null && previousLabel != null) {
                    if((currentLabel.equals(previousLabel)) && !(jump) && (testLabel!=null) && (previousLabel!=null) && !(jumpedAway)){
                        //Msg.M(instr_str);
                        //Msg.M("non jump traversal from " + previousLabel + " to " + currentLabel);
                        progGraph.addEdge("njt" + addindex, previousLabel, currentLabel, EdgeType.DIRECTED);
                    }
                }

                jump=false;
                if(instr_array[0].equals("jal")){
                    //Msg.M("jal, jump true");
                    jump=true;
                    jump_index=Integer.parseInt(instr_array[5]);
                    jalTargetLabel=asm.lookupLabel(addr_table[jump_index]);
                    if(currentLabel!=null && jalTargetLabel!=null){
                        progGraph.addEdge("jal" + addindex, currentLabel, jalTargetLabel, EdgeType.DIRECTED);
                    }
                    //progGraph.addEdge("jr" + addindex, jalTargetLabel, currentLabel, EdgeType.DIRECTED);
                }

                if(instr_array[0].equals("j")){
                    //Msg.M("j, jump true");
                    jump=true;
                    jump_index=Integer.parseInt(instr_array[5]);
                    jumpTargetLabel=asm.lookupLabel(addr_table[jump_index]);
                    if(currentLabel!=null && jumpTargetLabel!=null){
                        progGraph.addEdge("j" + addindex, currentLabel, jumpTargetLabel, EdgeType.DIRECTED);
                    }
                }

                if(instr_array[0].equals("beq")){
                    //Msg.M("beq, jump true");
                    jump=true;
                    // calculate branch target
                    branch_imm=Integer.parseInt(instr_array[5]);
                    branch_imm=(long)(short) branch_imm;
                    branch_index=(int)branch_imm+addindex+1;
                    currentLabel=asm.lookupLabel(addr_table[branch_index]);
                    if(currentLabel!=null && previousLabel!=null){
                        progGraph.addEdge("beq" + addindex, previousLabel, currentLabel, EdgeType.DIRECTED);
                    }
                }

                if(instr_array[0].equals("bne")){
                    //Msg.M("beq, jump true");
                    jump=true;
                    // calculate branch target
                    branch_imm=Integer.parseInt(instr_array[5]);
                    branch_imm=(long)(short) branch_imm;
                    branch_index=(int)branch_imm+addindex+1;
                    currentLabel=asm.lookupLabel(addr_table[branch_index]);
                    if(currentLabel!=null && previousLabel!=null){
                        progGraph.addEdge("bne" + addindex, previousLabel, currentLabel, EdgeType.DIRECTED);
                    }
                }

                if(instr_array[0].equals("jr")){
                    //Msg.M("jr, jump true");
                    jump=true;
                }

                previousInstr=instr_str;
                previousLabel=currentLabel;
            }
        }
        //construct the graph
        public DirectedOrderedSparseMultigraph<String, String> buildGraph(){
            vertices = new ArrayList<String>();
            DirectedOrderedSparseMultigraph<String, String> progGraph = new DirectedOrderedSparseMultigraph<String, String>();

            buildTables();
            addLabels(progGraph);
            addEdges(progGraph);
            //removeNonLabels(progGraph);
            //Msg.M(progGraph.toString());

            String vertex;
            Object[] verts = progGraph.getVertices().toArray();
            for(int i = 0; i < verts.length; i++) {
                vertex = (String) verts[i];
                if(progGraph.getIncidentEdges(vertex).isEmpty())
                    progGraph.removeVertex(vertex);
            }


            return progGraph;
        }
    }
}
