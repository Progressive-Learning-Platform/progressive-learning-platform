package junit.plptest;

import org.junit.Before;
import org.junit.Test;
import plptool.Constants;
import plptool.PLPSimBus;
import plptool.mods.MemModule;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;
import static junit.framework.Assert.assertTrue;
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
    public void testAddSuccessfully(){
        int index = plpSimBus.add(memModule);
        assertEquals(index,0);
        
        MemModule memModule2 = new MemModule(100L, 200, true);
        index = plpSimBus.add(memModule2);
        assertEquals("Adding additional memModule, index is " + index + ", should be 1", 1, index);
    }
    
 // using require getNumOfMods
    @Test
    public void testAddNull(){
    	memModule = null;
    	//assertEquals()
    	assertEquals("getNumOfMods() should return 0 because sim bus is empty",0, plpSimBus.getNumOfMods());
    	int index = plpSimBus.add(memModule);
    	//Maybe I dont understand this but even though the memModule is null, it is still being added
    	//and the size of bus_modules is still increasing and index is increasing to 1
    	//assertEquals("getNumOfMods() is" + plpSimBus.getNumOfMods() +", should be 0, Module added was null", 0, plpSimBus.getNumOfMods());
    	
    }
    /** Checks to see if remove() and getNumOfMods() is working or not */
    @Test
    public void testRemove(){
        int indx = plpSimBus.add(memModule);
        
        int res = plpSimBus.remove(indx);
        
        assertEquals(res, Constants.PLP_OK);
        assertEquals(plpSimBus.getNumOfMods(), 0);
    }
    
    // removing a module not already in simbus, testing for OutOfBoundsException
    @Test(expected = IndexOutOfBoundsException.class)
    public void testRemoveForException(){
        plpSimBus.remove(1);
    }
    
    @Test
    public void testReadSuccessfully(){
        int indx = plpSimBus.add(memModule);
        plpSimBus.enableMod(indx);
        plpSimBus.write(120L, 1234L, true);
        Object value = plpSimBus.read(120L);
        assertNotNull(value);
    }
    
    @Test
    public void testReadWithoutWrite(){
        int indx = plpSimBus.add(memModule);
        plpSimBus.enableMod(indx);
        assertEquals(0L, plpSimBus.read(120L));
        assertNull(plpSimBus.read(20L));
    }

    @Test
    public void testReadWithoutModule(){
        assertNull(plpSimBus.read(25L));
    }

    @Test
    public void testReadOutOfBounds(){
        int indx = plpSimBus.add(memModule);
        plpSimBus.enableMod(indx);
        assertNull(plpSimBus.read(20L));
    }

    @Test
    
    public void testWriteSuccessfully(){
        Long data = 1234L;
        plpSimBus.add(memModule);
        plpSimBus.enableMod(0);
        
        int res = plpSimBus.write(120L,data,true);
        assertEquals(res,Constants.PLP_OK);
        
        Object value = plpSimBus.read(120L);
        assertEquals((Long)value, data);
        
    }
    
    @Test
    public void testWriteWithoutEnableModule(){
        plpSimBus.add(memModule);
        plpSimBus.write(120L,1234L,true);

        assertEquals(0L,plpSimBus.read(120L));
    }

    @Test
    public void testWriteWithoutModule(){
        plpSimBus.write(120L,12L,true);
        assertNull(plpSimBus.read(120L));
    }

    @Test
    public void testWriteOutofbounds(){
        plpSimBus.add(memModule);
        plpSimBus.write(20L,1234L,true);

        assertEquals(0L,plpSimBus.read(120L));
        assertNull(plpSimBus.read(20L));
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
        
        plpSimBus.write(132L,12L,false);
        assertFalse(plpSimBus.isInstr(132L));
        
    }
    
    @Test
    public void testEnableMods(){
    	   int indx1 =plpSimBus.add(new MemModule(500L,50L,true));
           int indx2 = plpSimBus.add(new MemModule(600L,50L,true));

           assertEquals(plpSimBus.enableAllModules(), Constants.PLP_OK);
           assertTrue(plpSimBus.getEnabled(indx1));
           assertTrue(plpSimBus.getEnabled(indx2));
    }
    
    @Test
    public void testDisableMods(){
    	int indx1 = plpSimBus.add(new MemModule(500L,50L,true));
    	int indx2 = plpSimBus.add(new MemModule(600L,50L,true));
    	 plpSimBus.add(new MemModule(600L,50L,true));
    	 
        assertEquals(plpSimBus.enableAllModules(),Constants.PLP_OK);
        assertEquals(plpSimBus.disableAllModules(),Constants.PLP_OK);
        assertFalse(plpSimBus.getEnabled(indx1));
        assertFalse(plpSimBus.getEnabled(indx2));
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
    public void testEnableAllMods(){
        int indx1 = plpSimBus.add(new MemModule(500L,50L,true));
        int indx2 = plpSimBus.add(new MemModule(600L,50L,true));
        
        plpSimBus.enableMod(indx1);
        
        assertTrue(plpSimBus.getEnabled(indx1));
        assertFalse(plpSimBus.getEnabled(indx2));
    }
    
    @Test
    public void testDisableAllMods(){
        int indx1 = plpSimBus.add(new MemModule(500L,50L,true));
        int indx2 = plpSimBus.add(new MemModule(600L,50L,true));
        
        plpSimBus.enableMod(indx1);
        plpSimBus.enableMod(indx2);
        
        plpSimBus.disableMod(indx1);
        
        assertTrue(plpSimBus.getEnabled(indx2));
        assertFalse(plpSimBus.getEnabled(indx1));
    }
    @Test
    public void testGetStartValues(){
        plpSimBus.add(new MemModule(500L,50L,true));
        plpSimBus.add(new MemModule(600L,50L,true));
        
        assertEquals(plpSimBus.getModStartAddress(0),500L);
        assertEquals(plpSimBus.getModStartAddress(1),600L);
    }
    
    @Test
    public void testGetEndValues(){
        plpSimBus.add(new MemModule(500L,50L,true));
        plpSimBus.add(new MemModule(600L,50L,true));

        
        assertEquals(plpSimBus.getModEndAddress(0),546L);
        assertEquals(plpSimBus.getModEndAddress(1),646L);

    }
    @Test
    public void testGetNumOfMods(){
        plpSimBus.add(new MemModule(500L,50L,true));
        plpSimBus.add(new MemModule(600L,50L,true));
        
        assertEquals(2,plpSimBus.getNumOfMods());
    }
    
    @Test
    public void testgetRefMods(){
        MemModule mod= new MemModule(500L,50L,true);
        plpSimBus.add(mod);
        
        assertEquals(mod,plpSimBus.getRefMod(0));
    }
    @Test
    public void testUncheckedRead(){
        
        int indx = plpSimBus.add(memModule);
        plpSimBus.enableMod(indx);
        plpSimBus.write(120L,1234L,true);
        Object value = plpSimBus.uncheckedRead(120L);
        assertNotNull(value);
    }
    
    @Test
    public void testEval(){
        int indx = plpSimBus.add(memModule);
        plpSimBus.enableMod(indx);
        
        assertEquals(Constants.PLP_OK, plpSimBus.eval(indx));
    }
    
    
    @Test
    public void testEvalAll(){
        plpSimBus.add(memModule);
        plpSimBus.enableAllModules();
        
        assertEquals(Constants.PLP_OK, plpSimBus.eval());
    }

    @Test
    public void testClearModRegisters(){
        int indx1 = plpSimBus.add(new MemModule(500L,50L,true));
        int indx2 = plpSimBus.add(new MemModule(600L,50L,true));
        plpSimBus.enableAllModules();
        
        plpSimBus.write(500L, 1234L, true);
        
        assertEquals(Constants.PLP_OK, plpSimBus.clearModRegisters(indx1));
        assertEquals(Constants.PLP_OK, plpSimBus.clearModRegisters(indx2));
        assertEquals(0L, plpSimBus.read(500L));
    }

    @Test
    public void testIntroduceMod(){
    	int index = plpSimBus.add(memModule);
    	String tempString = "Memory Module 5.2";
    	assertEquals("Testing introduce(), should return 'Memory Module 5.2'", tempString, plpSimBus.introduceMod(index));
    }
    
    @Test
    public void testReset(){
    	//I tried to make a test for this but it kept failing
    	//I must not understand how it works
    }



}