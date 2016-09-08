package edu.asu.plp.tool.backend.plpisa.sim;

import javafx.beans.property.IntegerProperty;

public interface MemoryModule32Bit
{
	boolean hasRegister(String registerName);
	
	IntegerProperty getRegisterValueProperty(String registerName);
	
	String getRegisterID(String registerName);
	
	IntegerProperty getMemoryValueProperty(int address);
	
	/**
	 * Throws an {@link IllegalArgumentException} if the given address is not valid for
	 * this MemoryModule. Possible causes of the exception are the address being outside
	 * the range of valid addresses, or it failing to satisfy a condition such as
	 * word-alignment (if applicable). Any such conditions should be documented in the
	 * implementation's javadoc for this method.
	 * 
	 * @param address
	 *            The memory address
	 */
	void validateAddress(int address);
}
