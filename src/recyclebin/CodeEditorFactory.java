package recyclebin;

import java.io.File;
import java.io.IOException;

import edu.asu.plp.tool.prototype.view.CodeEditor;

public class CodeEditorFactory
{
	public CodeEditor createEditorWithSyntaxHighlighting()
	{
		CodeEditor editor = new CodeEditor();
		return editor;
	}
}
