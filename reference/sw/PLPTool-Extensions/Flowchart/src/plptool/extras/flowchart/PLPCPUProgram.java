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
import plptool.mips.*;
import plptool.gui.ProjectDriver;
import java.util.ArrayList;

/**
 *
 * @author wira
 */
public class PLPCPUProgram {
    private Asm asm;
    private ArrayList<PLPAsmSource> asms;
    private Node head;
    private ArrayList<NodeCollection> routines;
    private int traverseCount;
    long[] obj;
    long[] addr;

    private final byte BEQ  = 0x04;
    private final byte BNE  = 0x05;
    private final byte J    = 0x02;
    private final byte JAL  = 0x03;

    private final byte JR   = 0x08;
    private final byte JALR = 0x09;
    

    public PLPCPUProgram(ProjectDriver plp) {
        this.asm = (Asm) plp.asm;
        this.asms = plp.getAsms();
        obj = asm.getObjectCode();
        addr = asm.getAddrTable();
        routines = new ArrayList<NodeCollection>();
        head = new Node("__entry", asm.getEntryPoint());
        routines.add(new NodeCollection());
        traverseCount = 0;
        process();
    }

    public final void process() {
        Msg.I("Processing PLP program...", this);
        traverse(head, routines.get(0), traverseCount);
        Msg.I("Done!", this);
    }

    public void printProgram(int routine) {
        NodeCollection collection = routines.get(routine);
        NodeCollection reachedNodes = new NodeCollection();
        Node next = collection.getHead();
        Msg.P("\n\n*************************************************************");
        Msg.P(    "   R O U T I N E");
        Msg.P(next.getLabel() + " | #nodes: " + collection.size());
        int i = 0;
        while(next != null) {
            reachedNodes.addNode(next);
            Msg.P("*************************************");
            Msg.pn("node[" + i + "]: ");
            if(next.getLabel() != null)
                Msg.pn(next.getLabel() + ":");
            Msg.pn(PLPToolbox.format32Hex(next.getAddress()));
            Msg.P();
            if(next instanceof ExitNode) {
                Msg.pn("**Exit node");
                Msg.P();
            } else if(next instanceof BranchNode) {
                BranchNode bNode = (BranchNode) next;
                Msg.pn("**Branch node | ");
                Msg.pn("branch on true: " + bNode.isBranchOnTrue() + " ");
                Msg.pn("destination: " +
                        bNode.getBranchDestination().getLabel() + ":" +
                        PLPToolbox.format32Hex(bNode.getBranchDestination().getAddress()));
                Msg.P();
            } else if(next instanceof JumpNode) {
                JumpNode jNode = (JumpNode) next;
                Msg.pn("**Jump node | ");
                Msg.pn("destination: " +
                        jNode.getDestination().getLabel() + ":" +
                        PLPToolbox.format32Hex(jNode.getDestination().getAddress()));
                Msg.P();
            } else if(next instanceof CallNode) {
                CallNode cNode = (CallNode) next;
                Msg.pn("**Call node | ");
                Msg.pn("destination: " +
                        cNode.getRoutineLabel() + ":" +
                        PLPToolbox.format32Hex(cNode.getRoutineAddress()));
                Msg.P();
            }
            Msg.P("==CODE========\n" +
                   next.getCode() +
                  "==============\n");
            next = next.getNext();
            i++;
        }
        int r = reachedNodes.size();
        int c = collection.size();
        if(r != c) {
            Msg.I("There were " + (c-r) + " non-merging branches.", this);
        }
    }

    public String generateDOT(int routine, boolean useColors) {
        Msg.I("Generating DOT output", this);
        NodeCollection c = routines.get(routine);
        String ret = "digraph G {\n";
        String code;
        
        // generate vertices
        for(int i = 0; i < c.size(); i++) {
            Node n = c.getNode(i);
            code = n.getCode().trim().replace("\n", "\\n");
            
            if(n.getLabel() != null)
                ret += "label_node" + i + " [shape=Mrecord] " +
                        (useColors ? "[style=filled fillcolor=green fontcolor=black]" : "") +
                        "[label=\"" + n.getLabel() + "\"];\n";
            if(n instanceof BranchNode) {
                ret += "code_node" + i + " [shape=diamond] " +
                        (useColors ? "[style=filled fillcolor=yellow fontcolor=black]" : "") +
                        "[label=\"" + code + "\"];\n";
            } else if(n instanceof JumpNode) {
                ret += "code_node" + i + " [shape=box] " +
                        (useColors ? "[style=filled fillcolor=yellow fontcolor=black]" : "") +""
                        + "[label=\"" + code + "\"];\n";
            } else if(n instanceof CallNode) {
                ret += "code_node" + i + " [shape=parallelogram] " +
                        (useColors ? "[style=filled fillcolor=blue fontcolor=white]" : "") +
                        "[label=\"" + code + "\"];\n";
            } else if(n instanceof ExitNode) {
                ret += "code_node" + i + " [shape=invhouse] " +
                        (useColors ? "[style=filled fillcolor=red fontcolor=white]" : "") +
                        "[label=\"" + code + "\"];\n";
            } else {// if(!code.equals("")) {
                ret += "code_node" + i + " [shape=box] [label=\"";
                ret += code;
                ret += "\"];\n";
            }
        }
        // create edges
        for(int i = 0; i < c.size(); i++) {
            Node n = c.getNode(i);
            Node next = n.getNext();
            if(n.getLabel() != null)
                ret += "label_node" + i + " -> code_node" + i + ";\n";
            if(next != null && !(n instanceof JumpNode)) {
                if(next.getLabel() != null)
                    ret += "code_node" + i + " -> label_node" +
                            c.hasNodeWithAddress(next.getAddress()) + ";\n";
                else
                    ret += "code_node" + i + " -> code_node" +
                            c.hasNodeWithAddress(next.getAddress()) + ";\n";
            }

            if(n instanceof BranchNode) {
                BranchNode b = (BranchNode) n;
                Node dest = b.getBranchDestination();
                ret += "code_node" + i + " -> label_node" + c.hasNodeWithAddress(dest.getAddress()) +
                       " [label=\"Yes\"] [color=blue];\n";
            } else if(n instanceof JumpNode) {
                JumpNode j = (JumpNode) n;
                Node dest = j.getDestination();
                ret += "code_node" + i + " -> label_node" + c.hasNodeWithAddress(dest.getAddress()) + ";\n";
            } else if(n instanceof CallNode) {

            } else if(n instanceof ExitNode) {

            } else {
                
            }
        }
        ret += "}";
        return ret;
    }

    private boolean traverse(Node node, NodeCollection collection, int count) {
        Msg.D("traverse[" + count + "]: starting with " + node.getLabel() +":" +
                PLPToolbox.format32Hex(node.getAddress()), 3, this);
        long pc, instr;
        int index, tempIndex;
        byte funct, opcode;
        long imm;
        long jaddr;
        long branch_destination;
        String addrLabel;
        boolean done = false;
        boolean first = true;
        boolean postBranch = false;
        Node tempNode;
        Node current = node;
        pc = current.getAddress();
        if(count == 0)
            collection.addNode(node);
        
        while(!done) {
            Msg.D("pc=" + PLPToolbox.format32Hex(pc), 3, this);
            index = asm.lookupAddrIndex(pc);
            if(index < 0) {
                // clean up
                Msg.W("traverse[" + count + "]: Looks like we fell off the program!", this);
                return true;
            }
            tempIndex = collection.hasNodeWithAddress(pc);
            if(!first && tempIndex >= 0) {
                // branches merged, we're done
                current.setNext(collection.getNode(tempIndex));
                Msg.D("traverse[" + count +"]: Merged branches, we're done.", 3, this);
                return true;
            }
            instr = obj[index];
            opcode = MIPSInstr.opcode(instr);
            addrLabel = asm.lookupLabel(pc);

            switch(opcode) {
                case BEQ:
                    Msg.D("BEQ", 3, this);
                    imm = (short) MIPSInstr.imm(instr);
                    branch_destination = (pc+4 + (imm<<2)) & ((long) 0xfffffff << 4 | 0xf);
                    tempNode = new BranchNode(addrLabel, pc, null, true);
                    current.setNext(tempNode);
                    if(current instanceof JumpNode)
                        ((JumpNode)current).setDestination(tempNode);
                    current = tempNode;
                    appendCode(current, pc);
                    appendCode(current, pc+4);
                    collection.addNode(current);

                    // traverse down the branch destination if we're not branching
                    // to a location already in the collection
                    tempIndex = collection.hasNodeWithAddress(branch_destination);
                    if(tempIndex < 0) {
                        tempNode = new Node(asm.lookupLabel(branch_destination), branch_destination);
                        traverseCount++;
                        traverse(tempNode, collection, traverseCount);
                    } else
                        tempNode = collection.getNode(tempIndex);
                    ((BranchNode)current).setBranchDestination(tempNode);
                    postBranch = true;
                    pc+=8;
                    break;
                case BNE:
                    Msg.D("BNE", 3, this);
                    imm = (short) MIPSInstr.imm(instr);
                    branch_destination = (pc+4 + (imm<<2)) & ((long) 0xfffffff << 4 | 0xf);
                    tempNode = new BranchNode(addrLabel, pc, null, false);
                    current.setNext(tempNode);
                    if(current instanceof JumpNode)
                        ((JumpNode)current).setDestination(tempNode);
                    current = tempNode;
                    appendCode(current, pc);
                    appendCode(current, pc+4);
                    collection.addNode(current);

                    // traverse down the branch destination if we're not branching
                    // to a location already in the collection
                    tempIndex = collection.hasNodeWithAddress(branch_destination);
                    if(tempIndex < 0) {
                        tempNode = new Node(asm.lookupLabel(branch_destination), branch_destination);
                        traverseCount++;
                        traverse(tempNode, collection, traverseCount);
                    } else
                        tempNode = collection.getNode(tempIndex);
                    ((BranchNode)current).setBranchDestination(tempNode);
                    postBranch = true;
                    pc+=8;
                    break;
                case J:
                    Msg.D("J", 3, this);
                    jaddr = ((pc+8) & 0xff000000L) | (MIPSInstr.jaddr(instr)<<2);
                    tempNode = new JumpNode(addrLabel, pc, null);
                    current.setNext(tempNode);
                    if(current instanceof JumpNode)
                        ((JumpNode)current).setDestination(tempNode);
                    current = tempNode;
                    appendCode(current, pc);
                    appendCode(current, pc+4);
                    collection.addNode(current);
                    tempIndex = collection.hasNodeWithAddress(jaddr);
                    if(tempIndex < 0) {
                        Msg.D("Jumping to unknown territory", 3, this);
                    } else { // we're jumping back, set destination and quit
                        Msg.D("traverse[" + count +"]: Jumping back, we're done", 3, this);
                        ((JumpNode)current).setDestination(collection.getNode(tempIndex));
                        return true;
                    }
                    pc=jaddr;
                    break;
                case JAL:
                    Msg.D("JAL", 3, this);
                    jaddr = ((pc+8) & 0xff000000L) | (MIPSInstr.jaddr(instr)<<2);
                    tempNode = new CallNode(addrLabel, pc, asm.lookupLabel(jaddr), jaddr);
                    current.setNext(tempNode);
                    if(current instanceof JumpNode)
                        ((JumpNode)current).setDestination(tempNode);
                    current = tempNode;
                    appendCode(current, pc);
                    appendCode(current, pc+4);
                    collection.addNode(current);
                    tempIndex = collection.hasNodeWithAddress(jaddr);
                    if(tempIndex < 0 && !routineExists(jaddr)) {
                        // if we're calling a routine that we haven't traversed
                        // yet, create a new node collection for this routine
                        // and traverse down this path
                        Msg.D("Brand new routine, recursing...", 3, this);
                        Node newRoutineHead = new Node(asm.lookupLabel(jaddr), jaddr);
                        routines.add(new NodeCollection());
                        traverseCount++;
                        traverse(newRoutineHead, routines.get(routines.size()-1), traverseCount);
                    } else // recursive, do nothing
                        ;
                    postBranch = true;
                    pc+=8;
                    break;

                case 0:
                    funct = MIPSInstr.funct(instr);
                    switch(funct) {
                        case JR:
                            Msg.D("JR", 3, this);
                            if(MIPSInstr.rs(instr) != 31) {
                                // there is no way to figure out where we go
                                // without actually executing the program, just
                                // give a warning and return
                                Msg.W("jr using other than $ra is NOT supported" +
                                        ". Assuming exit node.", this);
                            }
                            // this routine is done
                            tempNode = new ExitNode(addrLabel, pc);
                            current.setNext(tempNode);
                            if(current instanceof JumpNode)
                                ((JumpNode)current).setDestination(tempNode);
                            current = tempNode;
                            appendCode(current, pc);
                            appendCode(current, pc+4);
                            collection.addNode(current);
                            Msg.D("traverse[" + count +"]: Routine return, we're done", 3, this);
                            return true;
                           
                        case JALR:
                            Msg.W("jalr instruction is not supported", this);
                            return false;
                        default:
                            if(postBranch || addrLabel != null) {
                                tempNode = new Node(addrLabel, pc);
                                if(current instanceof JumpNode)
                                    ((JumpNode)current).setDestination(tempNode);
                                current.setNext(tempNode);
                                current = tempNode;
                                collection.addNode(current);
                            }
                            postBranch = false;
                            appendCode(current, pc);
                            pc+=4;
                    }
                    break;
                    
                default:
                    if(postBranch || addrLabel != null) {
                        tempNode = new Node(addrLabel, pc);
                        if(current instanceof JumpNode)
                            ((JumpNode)current).setDestination(tempNode);
                        current.setNext(tempNode);
                        current = tempNode;
                        collection.addNode(current);
                    }
                    postBranch = false;
                    appendCode(current, pc);
                    pc+=4;
            }

            first = false;
        }

        return false;
    }

    private void appendCode(Node node, long addr) {
        PLPAsmSource src = asms.get(asm.getFileIndex(addr));
        String tokens[] = src.getAsmLine(asm.getLineNum(addr)).trim().split("#", 2);
        node.append(tokens[0].trim());
    }

    public int getNumberOfRoutines() {
        return routines.size();
    }

    public boolean routineExists(long address) {
        for(int i = 0; i < routines.size(); i++) {
            if(routines.get(i).getHead().getAddress() == address)
                return true;
        }

        return false;
    }

    public NodeCollection getRoutine(int index) {
        return routines.get(index);
    }

    @Override
    public String toString() {
        return "PLPCPUProgram";
    }
}
