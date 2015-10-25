package junit.plptest

import org.junit.Before;
import org.junit.Test;
import plptool.Constants;
import plptool.PLPSimBus;
import plptool.mods.MemModule;

import static junit.framework.Assert.assertEquals;
import static org.junit.Assert.*;


public class PLPSimBusTest {

    //Creating a word Aligned memModule
    private MemModule memModule;
    private PLPSimBus plpSimBus;
    
    @Before
    public void setup(){
        
        memModule = new MemModule(100L, 200, true);
        plpSimBus = new PLPSimBus();
    }
    
    @Test
    public void testAdd(){
        int index = plpSimBus.add(memModule);
        assertEquals(index,0);
        assertNotNull(index);
    }
    /** Checks to see if remove() and getNumOfMods() is working or not */
    @Test
    public void testRemove(){
        int indx = plpSimBus.add(memModule);
        
        int res = plpSimBus.remove(indx);
        
        assertEquals(res, Constants.PLP_OK);
        assertEquals(plpSimBus.getNumOfMods(), 0);
    }
    
    @Test
    public void testRead(){
        int indx = plpSimBus.add(memModule);
        plpSimBus.enableMod(indx);
        plpSimBus.write(120L, 1234L, true);
        Object value = plpSimBus.read(120L);
        assertNotNull(value);
    }
    
    @Test
    
    public void testWrite(){
        Long data = 1234L;
        plpSimBus.add(memModule);
        plpSimBus.enableMod(0);
        
        int res = plpSimBus.write(120L,data,true);
        assertEquals(res,Constants.PLP_OK);
        
        Object value = plpSimBus.read(120L);
        assertEquals((Long)value, data);
        
    }

}