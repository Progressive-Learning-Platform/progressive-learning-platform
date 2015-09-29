package plptool.gui.develop;

import java.util.ArrayList;

import javax.swing.undo.UndoManager;

import plptool.Config;
import plptool.Msg;

public class DevUndoManager extends UndoManager
{
	ArrayList<Boolean> editTypeList;
	int position;
	int lastUndoPosition;
	boolean busy;
	Develop g_dev;
	
	public DevUndoManager(Develop dev)
	{
		super();
		position = 0;
		lastUndoPosition = 0;
		busy = false;
		g_dev = dev;
		
		editTypeList = new ArrayList<>();
	}
	
	public synchronized boolean isBusy()
	{
		return busy;
	}
	
	private synchronized void setBusy(boolean val)
	{
		busy = val;
	}
	
	public boolean safeAddEdit(javax.swing.undo.UndoableEdit anEdit)
	{
		// if(g_dev.isHighlighting())
		// return false;
		
		editTypeList.add(position, !g_dev.isHighlighting());
		position++;
		Msg.D("++++ undo <shl:" + g_dev.isHighlighting() + "> position: " + position, 6,
				null);
		return super.addEdit(anEdit);
	}
	
	public void safeUndo()
	{
		setBusy(true);
		boolean oldSyntaxOption = Config.devSyntaxHighlighting;
		Config.devSyntaxHighlighting = false;
		
		while (g_dev.isHighlighting()); // / wait for highlighter to be done
		
		if (position <= 0 || !super.canUndo())
			return;
		
		// shed formatting events
		while (super.canUndo() && position > 0 && !editTypeList.get(position - 1))
		{
			position--;
			super.undo();
		}
		
		if (position > 0 && super.canUndo())
		{
			super.undo();
			position--;
			lastUndoPosition = position;
		}
		
		Config.devSyntaxHighlighting = oldSyntaxOption;
		
		Msg.D("<--- undo position: " + position, 6, null);
		setBusy(false);
	}
	
	public void dumpList()
	{
		System.out.println();
		for (int i = 0; i < editTypeList.size(); i++)
		{
			System.out.println(i + "\t" + editTypeList.get(i));
		}
		System.out.println();
	}
	
	public void safeRedo()
	{
		setBusy(true);
		boolean oldSyntaxOption = Config.devSyntaxHighlighting;
		Config.devSyntaxHighlighting = false;
		
		while (g_dev.isHighlighting()); // / wait for highlighter to be done
		
		if (position > editTypeList.size() || !super.canRedo())
			return;
		
		// shed formatting events
		while (super.canRedo() && position < editTypeList.size() - 1
				&& !editTypeList.get(position + 1))
		{
			super.redo();
			position++;
		}
		
		if (super.canRedo())
		{
			super.redo();
			position++;
		}
		
		Config.devSyntaxHighlighting = oldSyntaxOption;
		
		Msg.D("---> undo position: " + position, 6, null);
		setBusy(false);
	}
	
	public void reset()
	{
		editTypeList.clear();
		position = 0;
		super.discardAllEdits();
	}
	
	public void addEditType(boolean isHighlight)
	{
		
	}
	
	public boolean getNextEditType()
	{
		return false;
	}
	
	public boolean getPreviousEditType()
	{
		return false;
	}
}
