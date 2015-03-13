


---


# Overview #

This document is geared towards users who are interested in the workings of the software suite or who are interested in developing PLPTool or PLPTool modules. Although some knowledge of Java and programming in general are helpful, everybody is welcome to use this document. Please refer to our [WCAE 2011 paper](http://plp.okstate.edu/papers.html) for an overview of how this software tool fits into the PLP ecosystem.

PLPTool is a collection of software written in Java to allow users to assemble assembly files for the target platform, run a program on the simulator, and to program the target board. PLPTool is also a framework that developers can use to _port_ an Intruction Set Architecture (ISA) to PLP. PLPTool was conceived with cross-platform and openness in mind, hence it is written in Java and licensed under the GPLv3.

[PLPTool 2.2 Code Repository](http://code.google.com/p/progressive-learning-platform/source/browse/#hg%2Freference%2Fplp-2.2%2Fsw%2FPLPTool%2Fsrc%2Fplptool)

[PLPTool Javadoc](http://plp.okstate.edu/javadoc)


---


# ISA Framework #

The root `plptool` and the `plptool.gui` packages constitute the PLP ISA framework. The [plptool](http://plp.okstate.edu/javadoc/plptool/package-summary.html) package contains abstract classes that developers can extend to _port_ an ISA to the PLP software system (program development, simulation, and hardware programming). The [plptool.gui](http://plp.okstate.edu/javadoc/plptool/gui/package-summary.html) package contains classes that present the framework and ISA implementations to the user.


---


## Abstract and Helper Classes ##

The three classes that constitute the framework for the ISA are [PLPAsm](http://plp.okstate.edu/javadoc/plptool/PLPAsm.html), [PLPSimCore](http://plp.okstate.edu/javadoc/plptool/PLPSimCore.html), and [PLPSerialProgrammer](http://plp.okstate.edu/javadoc/plptool/PLPSerialProgrammer.html). Developers wishing to port an ISA to PLPTool should create a plptool._ISA_ package within the PLPTool source directory (`reference/plp-x.x/sw/PLPTool/src`) and extend these three classes. An ISA registry is being worked on to register additional ISAs to the rest of the PLPTool system (currently everything is hardcoded as if-statements).

Refer to the MIPS-like ISA implementation on how these classes can be extended.

### Assembler Abstract ###

[PLPAsm](http://plp.okstate.edu/daily/javadoc/plptool/PLPAsm.html) is the abstract for the assembler. All ISAs ported to PLP will need to extend this abstract. Implementations of this class will need to implement the `preprocess` and `assemble` methods. Two superclass constructors are available:

```
public PLPAsm (String strAsm, String strFilePath)
```

and

```
public PLPAsm (ArrayList<PLPAsmSource> asms)
```

The first constructor takes a string and a filepath. This will instantiate a `PLPAsmSource` object and attach it to the arraylist `SourceList`. The second constructor takes an ArrayList of [PLPAsmSource](http://plp.okstate.edu/daily/javadoc/plptool/PLPAsmSource.html). After this point, it depends on the developer on how this class should be extended. The rest of the framework expects an address table, an object code array, and file / line number mappings to be populated if `assemble` succeeded. Refer to the javadoc for other members of this class.

### Simulation Core Abstract ###

Refer to the [javadoc documentation](http://plp.okstate.edu/daily/javadoc/plptool/PLPSimCore.html) and the PLP MIPS-like implementation section of SimCore.

### Board Programmer Abstract ###

[PLPSerialProgrammer](http://plp.okstate.edu/daily/javadoc/plptool/PLPSerialProgrammer.html) is an abstract extending the Thread class. Developers will need to implement the `connect` method and `programWithAsm`. The framework will run this thread when the user calls the program function of the ProjectDriver.

### Simulation Bus ###

[PLPSimBus](http://plp.okstate.edu/javadoc/plptool/PLPSimBus.html) is a simulated bus attached to all PLPSimCore objects. It provides a facility for the simulation core to interact with memory-mapped I/O modules. Refer to the [Module Framework](http://code.google.com/p/progressive-learning-platform/wiki/PLPTool#Module_Framework) section on how modules are written and used in the system.

The simulation bus provides read and write methods for the simulation core. These methods also do address-space checking. Overlapping address space is allowed. A read access will be propagated to all modules with overlapping address space, but only the value from the lowest indexed module will be returned. A write access issued to the bus will be issued to all modules with overlapping address space.

The I/O registry uses the bus' add and remove methods to attach and detach modules to and from the simulation.

Simulation core can use the eval method to issue an evaluation logic of the modules. This function should be called every cycle by the simulation core.

### Simulation Bus Module Abstract ###

Refer to the [Module Framework](http://code.google.com/p/progressive-learning-platform/wiki/PLPTool#Module_Framework) section.

### I/O Registry ###

Refer to the [Module Framework](http://code.google.com/p/progressive-learning-platform/wiki/PLPTool#Module_Framework) section.

### Constants Class ###

[Constants](http://plp.okstate.edu/javadoc/plptool/Constants.html) defines error codes, masks and other constants that PLPTool uses. Refer to the javadoc for constant values.

### Messaging, Debugging and Error Handling ###

[Msg](http://plp.okstate.edu/javadoc/plptool/Msg.html) is the PLPTool message/output utility class. Depending on what mode PLPTool is running in, it will either display messages to standard out or to a text block on the GUI. Msg also defines error / return codes for the plptool package.

There are three message types that should be routed through PLPMsg: error (the `Msg.E` function), information (the `Msg.I` function) and debug (the `Msg.D` function). Error messages take three parameters: message string, error number, and calling object (use null for static calls). Debug messages also take three parameters: message string, debug level, and calling object. Information string only takes message string and calling object. Debug level is also defined in this class.

### User Configuration ###

The `Config` class holds runtime configuration values for PLPTool. Any user-modifiable configuration for the program should be implemented here so we can have a centralized configuration keeper.

### Toolbox and Miscellaneous Methods ###

`PLPToolbox` class contains some utility methods such as number parsing.


---


## User Interface ##

The [plptool.gui](http://plp.okstate.edu/javadoc/plptool/gui/package-summary.html) package contains the classes needed that present the framework and ISA implementations to the user. [ProjectDriver](http://plp.okstate.edu/javadoc/plptool/gui/ProjectDriver.html) class handles all operations that the user may want to do with the PLP project file. This includes creating, opening, saving, and other manipulations to the project file. `PLPToolApp` is the main program of the suite where the main method resides. The rest of the classes in the package are frames and UI support classes.

[Develop](http://plp.okstate.edu/javadoc/plptool/gui/Develop.html) is the development frame, and is the frame that gets launched when PLPTool is run in GUI mode. The user can write source code and manage his/her source files in the project, and assemble the program. After the program is assembled, the user can either program the board with the program, or simulate it. If the user decides to simulate the program, [SimShell](http://plp.okstate.edu/javadoc/plptool/gui/SimShell.html) will launch. [SimShell](http://plp.okstate.edu/javadoc/plptool/gui/SimShell.html) provides an interface for simulation control (such as stepping, resetting, etc).

### Project Driver Class ###

[ProjectDriver](http://plp.okstate.edu/javadoc/plptool/gui/ProjectDriver.html) is the PLP project file model used in the application. It handles all possible manipulations that can be done to the project file. It also manages all references to UI elements of the program. Reference to the currently active `ProjectDriver` instance should be called `plp` for consistency. For example, to step the [PLPSimCore](http://plp.okstate.edu/javadoc/plptool/PLPSimCore.html) instance attached to the project, one can call `plp.sim.step()`. Refer to the [ProjectDriver javadoc page](http://plp.okstate.edu/javadoc/plptool/gui/ProjectDriver.html) for its members and methods.

`ProjectDriver` is first instantiated when `PLPToolApp` is executed. **All actions of the user in the program should center around the ProjectDriver**. This is to achieve separation between the UI and the driving backend. All GUI-specific actions in the `ProjectDriver` should check for the `g` boolean variable.

### Integrated Development Environment (IDE) ###

The [Develop](http://plp.okstate.edu/javadoc/plptool/gui/Develop.html) class provides an interface to `ProjectDriver`'s methods involving source file management, editing, assembling, and provides a way for the user to start the simulator or to program the board with the assembled program.

The Develop window has a tree control on the left side to show the structure of the currently open PLP project file. Users can use the Project root menu to manipulate the source files, such as importing, exporting, adding new source file, removing a source file, and assign a source file as the top level program (entry point during assembly). These commands directly call the methods in the currently active `ProjectDriver`.

### Simulator Shell ###

The [SimShell](http://plp.okstate.edu/javadoc/plptool/gui/SimShell.html) class is a frame with an instance of `JDesktopPane` where all simulation GUI elements are found. It also provides simulation controls for the users.

### Command Line Interface ###

`ProjectFileManipulator` class handles the command line arguments dealing with a PLP project file. This class is invoked by running PLPTool with `-plp <plpfile> [options]` command. The main PLPToolApp class will pass on the command line arguments after the 0th index (`args[1]` to `args[x]` will be passed to ProjectFileManipulator).

| **Command Line Option** | **ProjectDriver method used** |
|:------------------------|:------------------------------|
| `-c <asm>` | `create(String asmpath)` |
| `-p <port>` | `program(String port)` |
| `-a` | `assemble()` |
| `-i <asm 1> <asm 2> ...` | `importAsm(String asm_x)` |
| `-d <directory>` | `importAsm(String asm)` |
| `-e <index> <file>` | `exportAsm(int index, String path)` |
| `-r <index>` | `removeAsm(int index)` |
| `-s <index>` | `setMainAsm(int index)` |

Refer to [ProjectDriver javadoc](http://plp.okstate.edu/javadoc/plptool/gui/ProjectDriver.html) for what these methods do. Invoking the `-plp <plpfile>` command without giving the options above will open the project file and list the files contained in the project file to the terminal.

### Board Programming ###

Board programming is done by the serial programmer GUI frame ([ProgrammerDialog](http://plp.okstate.edu/javadoc/plptool/gui/ProgrammerDialog.html)). It calls the `program` method of the `ProjectDriver` class and passes on the programming port that the user specified.


---


# PLP MIPS-like ISA Implementation #

The [plptool.mips](http://plp.okstate.edu/javadoc/plptool/mips/package-summary.html) package contains an implementation of our PLP MIPS-like ISA using the PLP framework. This section describes how each part of the implementation is designed.


---


## Assembler ##

[plptool.mips.Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) is a Java class that implements an assembler for the MIPS-like target architecture that PLP implements. The assembler can either be invoked through command line arguments or through the GUI. Internally, [Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) is a Java class that can be called from anywhere within the plptool package. The [Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) data structure includes a list of assembly files attached, the object code, symbol table, address table, and instruction->opcode mapping.

[Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) has two constructors:

```
public Asm (String strAsm, String strFilePath)
```

and

```
public Asm (ArrayList<PLPAsmSource> asms)
```

In the first constructor, if `strAsm` is `null`, the file pointed by `strFilePath` will be opened. You can also pass an `ArrayList` of [PLPAsmSource](http://plp.okstate.edu/javadoc/plptool/PLPAsmSource.html) objects. `PLPAsmSource` represents an assembly file and contains the contents of the file and its path.

[Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) implements the abstract methods `preprocess` and `assemble` from the [PLPAsm](http://plp.okstate.edu/javadoc/plptool/PLPAsm.html) superclass.

Once there are source files attached to the assembler object, `preprocess(int index)` can be called to perform first pass on the source file with the given index. This method will return PLP\_OK (or 0) if it succeeds. Then the `assemble()` method can be called to perform the second pass to generate the object code.

The preprocess stage prepares the assembly file for translation to object code:
  * Executes directives such as including other sources and data allocation
  * Resolves pseudo-ops
  * Checks for invalid instructions
  * Strips comments

Below is a code snippet from the [ProjectDriver](http://plp.okstate.edu/javadoc/plptool/gui/ProjectDriver.html) class where the user requests that the source files in the project file to be assembled:

```
if(arch.equals("plpmips")) {
    asm = new plptool.mips.Asm(asms);

    if(asm.preprocess(0) == Constants.PLP_OK)
        asm.assemble();
}
```

`asms` is an ArrayList of `PLPAsmSource` maintained by the [ProjectDriver](http://plp.okstate.edu/javadoc/plptool/gui/ProjectDriver.html) class. After the code above successfully executes, the [Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) object can be passed to a simulator core to be simulated, or to a programmer class to be programmed to the board.

### Code/data sections and linking ###

[Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) supports basic code/data partitioning using the `org` directive. The programmer can specify where in memory the subsequent entries will be stored by using this directive. [Asm](http://plp.okstate.edu/javadoc/plptool/mips/Asm.html) (will) also support(s) `text` and `data` sectioning to meet plp file specifications. This allows users to use multiple assembly files without having to worry where to place data and code in memory.

### Development Notes and To Do list ###

  * Support annotation of the object code in respect to source assembly files.
  * Implement `.text` and `.data` directives


---


## Simulation Core ##

[plptool.mips.SimCore](http://plp.okstate.edu/javadoc/plptool/mips/SimCore.html) implements a 5-stage MIPS-like processor with accurate simulation of the version 2.2 PLP target hardware core.

### Pipelining and Signals Convention ###

SimCore is organized like the real hardware is. Each pipeline stage other than IF is structured as a class, and each signal line is a `long` / 64-bit integer variable. Datapath signals are prefixed with `data_` and control signals are prefixed with `ctl_`. Signals that don't affect a stage (i.e. they are forwarded for a stage following the current one) are prefixed with `fwd_`. Signals on the input-side of the pipeline registers are prefixed with `i_`. These values are propagated to the output side when that stage is clocked by the `step()` method. IF stage is implemented by the `fetch()` method and at the end of the `step()` implementation.

### Bus Interfacing ###

SimCore has an instance of `PLPSimBus` attached during instantiation, and this object is identifed as `bus`. It will also instantiate a memory module and attach it to the bus for program memory. The simulation core issues bus reads (`bus.read` method) during IF stage to fetch the next instruction. It issues bus reads or writes (`bus.write` method) in the MEM stage to access the data memory.

### Instantiation ###

SimCore takes an instance of Asm during instantiation, and loads the object code to the memory by issuing write accesses to the bus. `loadProgram` method implements this functionality. Once `loadProgram` finishes, the core can be stepped using the `step()` method to advance the simulation by one cycle.

### Hazards and Forwarding ###

The simulation core differs from the hardware design in the way it handles forwarding and hazards. Instead of implementing muxes in places where values may need to be overridden, the `step()` method calls on the `mod_forwarding` class to check for hazardous conditions and replace values and/or stall appropriately. This allows for enabling and disabling forwarding events during runtime for educational purposes.

### GUI Extensions ###

The PLP MIPS-like implementation also includes a simulation core GUI (SimCoreGUI) that allows users to access the registers of the CPU, set breakpoints, view the program memory, enable/disable simulation options, manipulate the bus, and a command line interface to the simulation core for more features.

### Development Notes and To Do Llist ###
  * ~~Implement forwarding to avoid some hazards~~
  * ~~Implement I/O simulation~~
  * ~~Register the program counter to make it consistent with the simulated hardware~~
  * Implement PROPER breakpoints


---


## Serial Programmer ##

The `SerialProgrammer` class implements the programming function of PLP Tool. It uses rxtx from http://www.rxtx.org/ to interface with the host OS serial device. The programmer can currently be invoked from the command line by using the -p option. This class implements the BootloaderProtocol of the PLP system.


---


## Formatter Class ##

The formatter class contains few utility functions that involve displaying `plptool.mips.Asm` output in a human-readable manner. ~~This class also implements the `genPLP` function that generates the PLP file output.~~

To do
  * ~~Update genPLP to support data/code region mapping - support new plp file format specs~~


---


## `SimCL` ##

`plptool.mips.SimCL` is the command line interface to the PLPTool MIPS Simulator. Currently it can be used by running the command below:

```
java -jar PLPTool.jar -s <plpfile>
```

This class is also exposed by the simulation core GUI frame (SimCoreGUI) in the Console tab.


---


# Module Framework #

[Diagram of the PLPSimBusModule architecture](http://progressive-learning-platform.googlecode.com/hg/images/plptool/PLPBusModuleArchitecture.png)

A PLPTool simulator module is implemented by extending the [PLPSimBusModule](http://plp.okstate.edu/javadoc/plptool/PLPSimBusModule.html) abstract class defined in the plptool package. [PLPSimBusModule](http://plp.okstate.edu/javadoc/plptool/PLPSimBusModule.html) defines how the module is instantiated, handles register read/writes, and provides overridable methods that developers can use to integrate their device model to PLP simulator. All PLP simulator modules should be placed within the [plptool.mods](http://plp.okstate.edu/javadoc/plptool/mods/package-summary.html) package.

The PLPSimBusModule superclass contains the following abstract methods that the subclass must implement:
  * A constructor
  * `eval()`
  * `gui_eval(Object x)`
  * `introduce()`

The superclass has writeReg and readReg methods to write and read the registers, respectively.
```
public int writeReg(long address, Object data, boolean isInstr)
```
```
public Object readReg(long address)
```

Additionally, the subclass can override the superclass register read/write wrapper methods to intercept a read or write access to the module's registers.
```
public int write(long address, Object data, boolean isInstr)
```
```
public Object read(long address)
```

The superclass implementation of these wrapper methods only calls writeReg and readReg methods directly. Although the subclass has access to the registers directly, it's advisable to use writeReg and readReg to access the registers because these methods do error checks for you.


---


## I/O registry ##

The I/O registry ([plptool.mods.IORegistry](http://plp.okstate.edu/javadoc/plptool/mods/IORegistry.html)) is where the simulation shell goes to to determine how a particular module is loaded/removed. Module developers will have to add their module information to `IORegistry.java` for users to be able to use it. There are three locations in the program where the developers will have to update:

```
private final int NUMBER_OF_MODULES = 10;    
```

`NUMBER_OF_MODULES` needs to reflect how many modules are defined in the I/O registry. Below is an example of how the seven segments is registered into the registry:

```
mods[9][0] = "Seven Segment LEDs";
mods[9][1] = false;
mods[9][2] = 1;
mods[9][3] = "Simulated seven segments LED.";
mods[9][4] = true;
mods[9][5] = true;
```

The first dimension of the array denotes the module index (9 in this case, or the 10th module registered in the I/O registry). Below is a description for each field:

  * 0: Name
  * 1: Whether the module can have variable number of registers
  * 2: Number of registers (doesn't matter if field 1 is true)
  * 3: Information string to be displayed by the simulator GUI
  * 4: Registers MUST be aligned
  * 5: The module has a GUI frame (gui\_eval is implemented)

The last section of the registry that needs to be modified is the instantiation code in the `attachModuleToBus` method. Below is an example on how the seven segments module is instantiated:

```
case 9:
    module = new SevenSegments(addr);
    moduleFrame = new SevenSegmentsFrame();

    break;
```

It is up to the developers on what information the module and its GUI frame should get during instantiation. `attachModuleToBus` method has access to the currently running simulation core ([PLPSimCore](http://plp.okstate.edu/javadoc/plptool/PLPSimCore.html) class). This means that the developer can have access to the whole simulation during instantiation (simulation core and its members such as the bus itself, etc). The implication is that the module can actually change the simulation states if needed. The philosophy here is to not pass on references to objects your modules won't need!


---


## Constructor Methods ##

Below is the constructor for the seven segments module:

```
public SevenSegments(long addr) {
    super(addr, addr, true);
}
```

And the constructor for the [PLPSimBusModule](http://plp.okstate.edu/javadoc/plptool/PLPSimBusModule.html) abstract class is defined as:

```
public PLPSimBusModule(long startAddr, long endAddr, boolean wordAligned)
```

The superclass constructor takes the starting address, the **final** address, and whether the registers are word-aligned (NOTE: PLP has no notion of addressing granularity, only that it checks whether the address is word-aligned during an access and throws an exception if it's not if this field is true). In this case, the first and last addresses are the same because the seven segments only use 1 register.

Below is the header for `attachModuleToBus` method:

```
public int attachModuleToBus(int index, long addr, long size,
                             PLPSimCore sim, javax.swing.JDesktopPane simDesktop)
```

Here it takes size instead of final address. Size here is defined as the number of bytes. For example, the developer may need to do `addr + (size - 4)` for the module final address to pass on to [PLPSimBusModule](http://plp.okstate.edu/javadoc/plptool/PLPSimBusModule.html) constructor if the module registers are word-aligned (4 bytes / word) and it assumes that size is in bytes. The reason why this function takes register file size instead of just the final address is because most modules do not have large address spaces, and it's easier to enter the size of the register file instead of making the users calculate what the final address is.


---


## `eval()` and `gui_eval(Object x)` ##

The evaluation function `eval()` is called every cycle by the simulation core. This is used to define the module's behavior during simulation. Abstraction of a cycle depends on the simulation core. `gui_eval(Object x)` is called by the simulation shell whenever it needs to update its graphical user interface. The I/O registry will pass on the (typically) frame object `x` when a GUI update is called.

Keep in mind that the eval() function of all modules is called every cycle, and this function may slow down the simulation considerably. It is not advisable to perform any kind of GUI update in this function.


---


## Simulator Modules as Threads ##

The [PLPSimBusModule](http://plp.okstate.edu/javadoc/plptool/PLPSimBusModule.html) class extends the Java [Thread](http://download.oracle.com/javase/1.5.0/docs/api/java/lang/Thread.html) class, making it possible for PLPTool simulator modules to be threaded. This is especially useful for modules that perform heavy graphical updates (e.g. VGA module) or modules that do heavy work that does not need to be cycle-accurate, and that will slow down the simulation considerably if it is implemented in `eval()`. [PLPSimBusModule](http://plp.okstate.edu/javadoc/plptool/PLPSimBusModule.html) also has the `threaded` boolean that developers can use to allow a non-threaded mode execution of the module. Currently, the only place to start a module thread is the `attachModuleToBus` method where the module gets instantiated (or from other modules... but let's not go there).


---


## Dynamic Module Loading ##

Work is being done to allow users to develop PLPTool Simulator modules without having to modify the I/O registry and recompile the whole software suite. This feature is slated for PLP 3.0.


---


# Using PLPTool #

PLPTool includes a suite of command line options to allow non-interactive execution.

Run PLPTool with no command line arguments to launch GUI tool.

Non-GUI options:
  * Assemble _asm_ and write plp output to _out_.plp:
```
java -jar PLPTool.jar -a <asm> <out>
```
  * Program PLP target board with _plpfile_ using serial port _port_ and baud rate of _baud_.
```
java -jar PLPTool.jar -p  <plpfile> <port> <baud>
```
  * Launch the command line simulator to simulate _plp_.
```
java -jar PLPTool.jar -s <plp>
```


---


## Graphical User Interface (GUI) ##

PLPTool also includes a GUI for each component of the suite.


---


# Others #

## MIPSInstr ##

`MIPSInstr` is a static class that is used throughout plptool.mips to dissect a MIPS instruction. e.g., `MIPSInstr` has an _rd_ function that returns the rd field of an instruction. `MIPSInstr` is located in `plptool.mips.Formatter.java` source.