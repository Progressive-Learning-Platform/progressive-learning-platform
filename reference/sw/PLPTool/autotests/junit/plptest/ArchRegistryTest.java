package junit.plptest;

import org.junit.Before;
import org.junit.Test;
import plptool.ArchRegistry;
import plptool.Constants;
import plptool.gui.ProjectDriver;
import plptool.mips.Architecture;

import static junit.framework.Assert.assertNull;
import static org.junit.Assert.assertEquals;

public class ArchRegistryTest {

    private ProjectDriver plp;
    private Architecture arch;
    private static final int ID = 1;

    @Before
    public void setup(){

        plp = new ProjectDriver(10);
        arch = new Architecture(0,plp);

    }

    // Success Case 
    @Test
    public void testRegisterArchSuccess(){

        int result = ArchRegistry.registerArchitecture(arch.getClass(), ID+1, "01", "test class");
        assertEquals(result, Constants.PLP_OK);

    }

    // Failure Case :  Invalid Class 
    @Test
    public void testRegisterArchWithInvalidClass(){

        int result = ArchRegistry.registerArchitecture(plp.getClass(), ID+1, "01", "test class");
        assertEquals(result, Constants.PLP_ISA_INVALID_METACLASS);

    }

    // Special Case :  ID = 0 
    @Test
    public void testRegisterArchWithZeroID(){

        int result = ArchRegistry.registerArchitecture(arch.getClass(), 0, "01", "test class");
        assertEquals(result, Constants.PLP_ISA_ALREADY_DEFINED);

    }


    // Success Case 
    @Test
    public void testgetArchitectureSuccess(){
        ArchRegistry.registerArchitecture(arch.getClass(), ID, "01", "test class");
        assertEquals(arch.getClass(), ArchRegistry.getArchitecture(plp, 0).getClass());
    }


    // Failure Case :  Invalid ID 
    @Test
    public void testgetInvalidArchitecture(){
        ArchRegistry.registerArchitecture(arch.getClass(), ID, "01", "test class");
        assertNull(ArchRegistry.getArchitecture(plp, 10));
    }

    // Success Case 
    @Test
    public void testGetRegisteredArchitectureClass(){
        ArchRegistry.registerArchitecture(arch.getClass(), ID, "01", "test class");
        assertEquals(ArchRegistry.getRegisteredArchitectureClass(ID),arch.getClass());
    }

    // Simple Cases 
    @Test
    public void testGetStringID(){
        ArchRegistry.registerArchitecture(arch.getClass(), ID, "01", "test class");
        assertEquals(ArchRegistry.getStringID(ID),"01");
    }

    
    // Simple Cases 
    @Test
    public void testGetDescription(){
        ArchRegistry.registerArchitecture(arch.getClass(), ID, "01", "test class");
        assertEquals(ArchRegistry.getDescription(ID),"test class");
    }
}