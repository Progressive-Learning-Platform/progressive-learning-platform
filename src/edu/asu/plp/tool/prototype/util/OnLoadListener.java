package edu.asu.plp.tool.prototype.util;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.concurrent.Worker.State;
import moore.util.Subroutine;

public class OnLoadListener implements ChangeListener<State>
{
	private ObservableValue<State> observable;
	private Subroutine onLoadFunction;

	public static void register(Subroutine function, ObservableValue<State> target)
	{
		OnLoadListener listener = new OnLoadListener(function);
		listener.observable = target;
		target.addListener(listener);
	}
	
	public OnLoadListener(Subroutine onLoadFunction)
	{
		this.onLoadFunction = onLoadFunction;
	}
	
	public void changed(ObservableValue<? extends State> value, State oldState,
			State newState)
	{
		if (newState == State.SUCCEEDED)
		{
			onLoadFunction.perform();
			
			if (observable != null)
				observable.removeListener(this);
		}
	}
}
