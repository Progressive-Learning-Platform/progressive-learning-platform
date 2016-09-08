package edu.asu.plp.tool.backend;

import com.google.common.eventbus.EventBus;

public class EventRegistry
{
	private static EventRegistry globalInstance;
	private EventBus eventBus;
	
	public static EventRegistry getGlobalRegistry()
	{
		if (globalInstance == null)
			globalInstance = new EventRegistry();
		
		return globalInstance;
	}
	
	public EventRegistry()
	{
		this.eventBus = new EventBus();
	}
	
	public void post(Object event)
	{
		eventBus.post(event);
	}
	
	public void register(Object subscriber)
	{
		eventBus.register(subscriber);
	}
	
	public void unregister(Object subscriber)
	{
		eventBus.unregister(subscriber);
	}
}
