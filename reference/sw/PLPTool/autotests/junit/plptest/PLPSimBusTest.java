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
    
    @Test
    public void testIsInitialized(){
        
        int index = plpSimBus.add(memModule);
        assertFalse(plpSimBus.isInitialized(120L));
        plpSimBus.enableMod(index);
        
        plpSimBus.write(120L, 1234L, false);
        assertTrue(plpSimBus.isInitialized(120L));
        
    }
    
    @Test
    public void testIsMapped(){
        
        assertFalse(plpSimBus.isMapped(120L));
        
        int index = plpSimBus.add(memModule);
        plpSimBus.enableMod(index);
        
        plpSimBus.write(120L, 1234L, true);
        assertTrue(plpSimBus.isMapped(120L));
        
    }
    @Test
    public void testIsInstr(){
        
        int index = plpSimBus.add(memModule);
        plpSimBus.enableMod(index);
        
        plpSimBus.write(120L, 1234L, true);
        
        assertTrue(plpSimBus.isInstr(120L));
        
    }
    
    @Test
    public void testEnableMods(){
        plpSimBus.add(new MemModule(500L,50L,true));
        plpSimBus.add(new MemModule(600L,50L,true));
        
        assertEquals(plpSimBus.enableAllModules(), Constants.PLP_OK);
    }
    
    @Test
    public void testDisableMods(){
        plpSimBus.add(new MemModule(500L,50L,true));
        plpSimBus.add(new MemModule(600L,50L,true));
        
        assertEquals(plpSimBus.enableAllModules(),Constants.PLP_OK);
        assertEquals(plpSimBus.disableAllModules(),Constants.PLP_OK);
    }
    
    @Test
    public void testGetEnabled(){
        plpSimBus.add(new MemModule(500L,50L,true));
        plpSimBus.add(new MemModule(600L,50L,true));
        
        assertFalse(plpSimBus.getEnabled(0));
        plpSimBus.enableAllModules();
        
        assertTrue(plpSimBus.getEnabled(0));
        assertTrue(plpSimBus.getEnabled(1));
        
    }
    
    @Test
    public void testEnableMod(){
        int indx1 = plpSimBus.add(new MemModule(500L,50L,true));
        int indx2 = plpSimBus.add(new MemModule(600L,50L,true));
        
        plpSimBus.enableMod(indx1);
        
        assertTrue(plpSimBus.getEnabled(indx1));
        assertFalse(plpSimBus.getEnabled(indx2));
    }
    
    @Test
    public void testDisableMod(){
        int indx1 = plpSimBus.add(new MemModule(500L,50L,true));
        int indx2 = plpSimBus.add(new MemModule(600L,50L,true));
        
        plpSimBus.enableMod(indx1);
        plpSimBus.enableMod(indx2);
        
        plpSimBus.disableMod(indx1);
        
        assertTrue(plpSimBus.getEnabled(indx2));
        assertFalse(plpSimBus.getEnabled(indx1));
    }

}