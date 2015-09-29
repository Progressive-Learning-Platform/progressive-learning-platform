package plptool.gui.develop;

import javax.swing.event.DocumentListener;

import plptool.gui.ProjectDriver;

public class DevEditorDocListener implements DocumentListener {
    private Develop g_dev;
    private ProjectDriver plp;
    private HighlighterThread thread;
    private boolean enable;

    public DevEditorDocListener(ProjectDriver plp, HighlighterThread thread) {
        this.g_dev = plp.g_dev;
        this.plp = plp;
        this.thread = thread;
        enable = true;
    }

    public void disable() {
        enable = false;
    }

    public void changedUpdate(javax.swing.event.DocumentEvent e) {}

    public void removeUpdate(final javax.swing.event.DocumentEvent e) {
       if(g_dev.getUndoManager().isBusy() || !enable || g_dev.isHighlighting()) return;

        if(plp.plpfile != null) {
            plp.setModified();
            plp.requireAssemble();
        }
        thread.setScheduleHighlight(true);
    }

    public void insertUpdate(final javax.swing.event.DocumentEvent e) {
        if(g_dev.getUndoManager().isBusy() || !enable || g_dev.isHighlighting()) return;

        if(plp.plpfile != null) {
            plp.setModified();
            plp.requireAssemble();
        }
        thread.setScheduleHighlight(true);
    }
}
