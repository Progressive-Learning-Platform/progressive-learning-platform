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

}