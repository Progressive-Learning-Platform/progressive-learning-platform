/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * ProgramVisualizationFrame.java
 *
 * Created on Nov 10, 2011, 1:35:41 PM
 */

package plptool.mips.visualizer;
import plptool.gui.ProjectDriver;
import plptool.*;
import plptool.mips.*;
import java.util.Arrays;
import java.lang.String;
import java.awt.BorderLayout;

import java.awt.Color;
import java.awt.Paint;
/*
import java.awt.Container;
 *
 */
import java.awt.Dimension;
import java.awt.Shape;
import java.awt.geom.AffineTransform;
import java.awt.geom.Ellipse2D;
/*
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import edu.uci.ics.jung.graph.*;
 *
 */
import edu.uci.ics.jung.algorithms.layout.*;
import edu.uci.ics.jung.visualization.*;
import edu.uci.ics.jung.visualization.decorators.*;
import edu.uci.ics.jung.graph.DirectedOrderedSparseMultigraph;
import edu.uci.ics.jung.visualization.renderers.Renderer.VertexLabel.Position;
import edu.uci.ics.jung.visualization.renderers.VertexLabelAsShapeRenderer;
import edu.uci.ics.jung.visualization.renderers.*;
import edu.uci.ics.jung.visualization.control.DefaultModalGraphMouse;
import edu.uci.ics.jung.visualization.control.ModalGraphMouse;
import java.util.ArrayList;
import java.util.List;
import java.awt.geom.Point2D;
import org.apache.commons.collections15.Transformer;

/**
 * Create the JUNG Visualization graphically in a pop-up window.
 *
 * @author will
 */
public class ProgramVisualizationFrame extends javax.swing.JFrame {

    private ProgramVisualization progVis;
    private ProgramVisualization.programGraph progGraph;
    private ProgVisUpdateThread repaintThread;
    private ProjectDriver plp;

    private Layout<String, String> layout;
    private VisualizationViewer<String,String> progVisServ;
    private vertexRecolor<String> vertexRecolor;

    private String currentLabel;
    private String jrTargetLabel;

    // changes the color of drawn vertices
    protected final class vertexRecolor<String> implements Transformer<String,Paint>{
            private String PaintMe;
            private String unPaintMe;
            public void setPaintMe(String vertex){
                this.PaintMe = vertex;
            }
            public void unPaintMe(String vertex){
                this.unPaintMe = vertex;
            }
            public Paint transform(String vertex) {
                if(vertex.equals(PaintMe)){
                    return Color.GREEN;
                }
                else if(vertex.equals(unPaintMe)){
                    return Color.BLUE;
                }
                else{
                    return Color.RED;
                }
            }
        };

    // changes the size of drawn vertices
    private Transformer<String,Shape> vertexResizer = new Transformer<String,Shape>(){
        public Shape transform(String vertex){
            Ellipse2D vertexShape = new Ellipse2D.Double(-5, -5, 15, 15);
            return vertexShape;
        }
    };

    /** Creates new form ProgramVisualizationFrame */
    public ProgramVisualizationFrame(ProgramVisualization progVis, ProgramVisualization.programGraph progGraph, ProjectDriver plp) {
        initComponents();
        int vertexYPos;
        this.progVis = progVis;
        this.progGraph = progGraph;
        this.plp = plp;
        vertexRecolor = new vertexRecolor<String>();
        // Vertical Layout
        layout = new StaticLayout<String,String>(progGraph.buildGraph());

        //layout.setSize(d);
        // Grab graph's vertices
        List<String> vertexList = new ArrayList<String>(layout.getGraph().getVertices());
        int maxStrLen=0;
        String vertName;
        int vertLength;
        for(int ind=0; ind<vertexList.size(); ind++){
                vertName=vertexList.get(ind);
                vertLength=vertName.length();
                if (vertLength>maxStrLen){
                    maxStrLen=vertLength;
                }
        }
        // Traverse vertices, arrange them vertically
        vertexYPos = 25;
        layout.setSize(new Dimension(maxStrLen*10,vertexList.size()*31));
        Dimension d = layout.getSize();
        if (d!=null){
            Msg.M("width = " + d.getWidth());
            Msg.M("height = " + d.getHeight());
        }
        else{
            Msg.M("d is null");
        }
        for(int i=0; i<vertexList.size(); i++){
                layout.setLocation(vertexList.get(i), new Point2D.Double(d.getWidth()/2, vertexYPos));
                vertexYPos+=30;
        }
        
        // create the vis viewer
        progVisServ = new VisualizationViewer<String,String>(layout);
        this.format();

        // create the pane
        final GraphZoomScrollPane progVisScrollPane = new GraphZoomScrollPane(progVisServ);
        //progVisServ.setPreferredSize(new Dimension(250,1000));
        progVisServ.setPreferredSize(d);
        //progVisScrollPane.setPreferredSize(new Dimension(600,600));
        //progVisServ.setSize(this.getSize());
        //progVisScrollPane.setSize(this.getSize());
        //this.setResizable(true);
        DefaultModalGraphMouse gm = new DefaultModalGraphMouse();
        gm.setMode(ModalGraphMouse.Mode.TRANSFORMING);
        progVisServ.setGraphMouse(gm);

        this.setLayout(new BorderLayout());
        getContentPane().add(progVisScrollPane, BorderLayout.CENTER);
        this.pack();
        
    }
    @SuppressWarnings("unchecked")
    // formatting
    private void format(){
        //vertexRecolor.transform("BEGIN");
        //progVisServ.getRenderContext().setVertexShapeTransformer(vertexResizer);
        progVisServ.getRenderContext().setVertexShapeTransformer(new VertexLabelAsShapeRenderer(progVisServ.getRenderContext()));
        progVisServ.getRenderContext().setVertexFillPaintTransformer(vertexRecolor);

        //vertexRecolor.setPaintMe("Begin");
        progVisServ.getRenderContext().setVertexLabelTransformer(new ToStringLabeller());
        progVisServ.getRenderer().getVertexLabelRenderer().setPosition(Position.E);

        progVisServ.getRenderContext().setEdgeShapeTransformer(new EdgeShape.Orthogonal());
        progVisServ.getRenderer().getVertexLabelRenderer().setPosition(Position.CNTR);

    }

    // paint current label green
    public void vert_repaint(String vertexName){
        vertexRecolor.setPaintMe(vertexName);
    }
    // as you move to next label, repaint red
    public void vert_unpaint(String vertexName){
        vertexRecolor.unPaintMe(vertexName);
    }
    private String getFunction(long currentAddress){
        plptool.mips.Formatter progFormat = new plptool.mips.Formatter();
        int index = -1;
        long objTable[] = plp.asm.getObjectCode();
        long addrTable[] = plp.asm.getAddrTable();
        index = Arrays.binarySearch(addrTable, currentAddress);
        if (index > -1) {
            String instrStr = progFormat.mipsInstrStr(objTable[index]);
            String instrArray[]=instrStr.split(" ");
            return instrArray[0];
        } else {
            return null;
        }
    }

    public void updateComponents() {
        if(repaintThread == null) {
            repaintThread = new ProgVisUpdateThread(this);
            repaintThread.start();
        }
        repaintThread.scheduleUpdate();
    }

    // called by SimCoreGUI when there's a GUI update in simulation
    public void update() {
        long currentAddress = plp.sim.visibleAddr;
        Msg.M("Current Address: " + currentAddress);
        //currentAddress -= 4;
        //Msg.M("Current Address - 4: " + currentAddress);
        String testLabel = plp.asm.lookupLabel(currentAddress);
        Msg.M("currentLabel: " + currentLabel);
        if(testLabel != null){
            currentLabel = testLabel;
            this.vert_repaint(currentLabel);
        }
        String function = getFunction(currentAddress);
        //Msg.M(function);
        if(function != null && function.equals("jal")){
            Msg.M("JUMP AND LINK YALL");
            jrTargetLabel = currentLabel;
            Msg.M("jr Target: " + jrTargetLabel);
        } else if(function != null && function.equals("jr")){
            currentLabel = jrTargetLabel;
            this.vert_repaint(jrTargetLabel);
        }
        //this.vert_unpaint("Begin");
        this.repaint();
    }

    public void stopUpdateThread() {
        if(repaintThread != null)
            repaintThread.stopRepaint();
    }
    
    public class ProgVisUpdateThread extends Thread {
        private ProgramVisualizationFrame progVisFrame;
        private boolean update = false;
        private boolean stop = false;
        public ProgVisUpdateThread(ProgramVisualizationFrame progVisFrame){
            this.progVisFrame = progVisFrame;
        }

        @Override
        public void run() {
            while(!stop) {
                if(update) {
                    progVisFrame.update();
                    update = false;
                }
                try {
                    Thread.sleep(100);
                } catch(Exception e) { }
            }
        }

        public void scheduleUpdate() {
            update = true;
        }

        public void stopRepaint() {
            stop = true;
        }
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        org.jdesktop.application.ResourceMap resourceMap = org.jdesktop.application.Application.getInstance(plptool.gui.PLPToolApp.class).getContext().getResourceMap(ProgramVisualizationFrame.class);
        setTitle(resourceMap.getString("Form.title")); // NOI18N
        setName("Form"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 400, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 300, Short.MAX_VALUE)
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables

}

