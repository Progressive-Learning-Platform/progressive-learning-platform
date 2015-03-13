

# Overview #

# Building PLP #

The reference design comes with a build script in `hw/scripts`. A successful build will generate everything in `hw/build/build_volatile`. A number of XST flags can break the design during synthesis, so pay attention to the reference XST parameters in `hw/build/build_skeleton/top.xst`.

To build PLP from a linux host, you will need to have the xilinx webpack ise installed. Navigate to the `hw` directory for the version of plp you want to build, source the xilinx settings (see xilinx documentation), and run the build script for your target board.

For example, if you were targeting the Nexys2 500k board with plp-2.3:

```
cd plp/reference/plp-2.3/hw
source /opt/Xilinx/12.4/ISE_DS/settings64.sh
sh scripts/build_500k_linux.sh
```

A lot of information from the xilinx tools will be generated, and hopefully you'll end up with a successful build.

The build files are located in the `hw/build/<board version>` directory. If you built the Nexys2 500k target, you would find the build files in
`hw/build/build_500k_volatile/`. Two files are of interest: `top.bit` and `top.mcs`. The mcs file is the prom image of the bit file.

From there you can configure the FPGA and have at it!

# CPU #

![http://progressive-learning-platform.googlecode.com/hg/images/CPUDiagram.png](http://progressive-learning-platform.googlecode.com/hg/images/CPUDiagram.png)

The CPU is a simple, 5-stage pipelined, MIPS-like machine. It implements a subset of the MIPS ISA, described in the user manual.

## Notes ##

The diagram reflects how the CPU is implemented on the FPGA.

The branch target calculation is abstracted in the diagram. Branch target is actually calculated by adding PC + 4 to the branch target which is sign extended and left shifted twice.

All instructions except for load word (`lw`) resolve in the EX stage. This has implications in potential data and control hazards. For example, we could eliminate the branch control hazard by moving the branch resolution hardware to the ID stage. The branch instruction already has a delay slot defined, so by resolving the branch in the ID stage, there would be no control hazard. However, this move introduces 5 additional data hazards, 3 of which require stalling the pipeline, including one that stalls for 2 cycles. Due to the likelihood of branch instructions depending on immediately previous instructions (calculate/branch), this stall would significantly impact performance. Therefore, branch instructions are resolved in the EX stage. There are also educational reasons for this configuration, as it incorporates a stall, several forwards, and a flush event which are valuable to learn about in the lab.

## Forwarding and Hazards ##

![http://progressive-learning-platform.googlecode.com/hg/images/ForwardsandHazards.png](http://progressive-learning-platform.googlecode.com/hg/images/ForwardsandHazards.png)

The PLP reference design forwards or stalls on all potential data hazards. Forwarding/Hazard logic is shown in the diagram in blue. Below is a table of all potential hazards, how each hazard is avoided, and an instruction sequence that exposes the hazard.

| **Hazard** | **Forward/Stall** | **Example Instruction Sequence** |
|:-----------|:------------------|:---------------------------------|
| EX->EX | Forward | `addu $t0, $t1, $t2`<br><code>addu $t3, $t0, $t1</code> <br>
<tr><td> MEM->MEM </td><td> Forward </td><td> <code>lw $t0, 0($t1)</code><br><code>sw $t0, 0($t2)</code> </td></tr>
<tr><td> MEM->EX </td><td> Forward </td><td> <code>lw $t0, 0($t1)</code><br><code>nop</code><br><code>addu $t3, $t0, $t1</code> </td></tr>
<tr><td> MEM->EX </td><td> Stall </td><td> <code>lw $t0, 0($t1)</code><br><code>addu $t3, $t0, $t1</code> </td></tr></tbody></table>

The savvy reader may note that there could be hazards originating from the WB stage. This would happen in a RAW hazard between two instructions that were four stages apart. In fact, the register file is written to on the negative edge of the clock, and so finishes half-way through the WB stage, allowing an asynchronous read to occur in the ID phase, eliminating all WB hazards.<br>
<br>
<h2>Interrupts</h2>

<b>Interrupt support has been pushed to the 3.0 build</b>

<h1>Bus Arbiter</h1>

<h1>Modules</h1>

<h2>Module Port and Timing</h2>

Modules have a standard port interface to facilitate a (later) module generator, and to make the arbiter code look pretty.<br>
<br>
All modules must have the following port spec:<br>
<br>
<pre><code>module mod_mymod(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);<br>
        input rst;<br>
        input clk;<br>
        input ie,de;<br>
        input [31:0] iaddr, daddr;<br>
        input drw;<br>
        input [31:0] din;<br>
        output [31:0] iout, dout;<br>
</code></pre>

<table><thead><th> port name </th><th> description </th></thead><tbody>
<tr><td> rst </td><td> synchronous reset </td></tr>
<tr><td> clk </td><td> system clock </td></tr>
<tr><td> ie </td><td> instruction port enable </td></tr>
<tr><td> de </td><td> data port enable </td></tr>
<tr><td> iaddr </td><td> instruction port address </td></tr>
<tr><td> daddr </td><td> data port address </td></tr>
<tr><td> drw </td><td> data port read/write. 0 = read, 1 = write </td></tr>
<tr><td> din </td><td> data port data in </td></tr>
<tr><td> iout </td><td> instruction port data out </td></tr>
<tr><td> dout </td><td> data port data out </td></tr></tbody></table>

Additional ports may be added to the <b>end</b> of the port list shown above, should that module have its own I/O.<br>
<br>
Modules must be negatively edge triggered. Reset is synchronous. Output on <code>iout</code> and <code>dout</code> when not enabled (via ie and de) should be HiZ. This is accomplished with:<br>
<br>
<pre><code>wire [31:0] idata, ddata;<br>
assign iout = ie ? idata : 32'hzzzzzzzz;<br>
assign dout = de ? ddata : 32'hzzzzzzzz;<br>
</code></pre>

Once modules are created, they can be added to the memory map by adding a line in the <code>mm</code> module. Addresses passed to the module are effective addresses, that is, the upper bits that designate the module are zeroed. That is, address 0xf0200050 would appear to the module as 0x00000050.<br>
<br>
Version 2.2 will have a module autogen script, that builds modules into the memory map with a special verilog comment:<br>
<br>
<br>
<pre><code>/* PLP2MODULE start:0xf0200000 length:0x00000100 */<br>
module my_module(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);<br>
</code></pre>

<h2>Boot ROM</h2>

The ROM is a 2KB (512 words) memory implemented on-chip (inferred by the synthesis tools) to facilitate initial program loading. It always exists in the memory map starting at address <code>0x00000000</code>, since the CPU is designed to have an initial PC value at 0. The ROM is hard-coded at synthesis time with the <code>fload</code> image, discussed in the software development section.<br>
<br>
The ROM uses the <code>initial</code> construct in Verilog to populate the ROM at synthesis time. In order for the synthesizer to correctly build the ROM, the entire memory must be specified, meaning that you may have to pack additional garbage (in our case zeros) at the end of the bootloader code.<br>
<br>
<h2>Memory Hierarchy</h2>

The Cellular RAM is used in asynchronous mode, using the standard SRAM interface. The Cellular RAM has a few additional ports that have to be used in special ways:<br>
<br>
<table><thead><th> Port </th><th> Description </th></thead><tbody>
<tr><td> MT-CLK </td><td> Clock signal used during synchronous mode. We must tie it static LOW. </td></tr>
<tr><td> MT-ADV </td><td> Address valid (active low). Held LOW since we latch the address in another place. </td></tr>
<tr><td> MT-CRE </td><td> Control register enable. Tie LOW. </td></tr>
<tr><td> MT-CE </td><td> Chip enable (active low). Tie LOW. </td></tr>
<tr><td> OE </td><td> Output enable (active low). Tie LOW. </td></tr>
<tr><td> WE </td><td> Write enable (active low). Tied low on write cycles. </td></tr>
<tr><td> MT-LB </td><td> Lower byte enable (active low). Tie LOW. </td></tr>
<tr><td> MT-UB </td><td> Upper byte enable (active low). Tie LOW. </td></tr></tbody></table>

Read operations occur by bringing CE, OE, LB, and UB low while leaving WE high. Writes occur by bringing CE, LB, UB, and WE low, and bringing OE high. Data to be written is latched on the rising edge of WE.<br>
<br>
Because the system clock is only 25MHz data writes take 6 cycles (two 16-bit writes). Normally this should take only four cycles, but the data words have to be latched. I didn't want to add a faster clock domain or mess with dual-edge triggered logic to make a higher precision SRAM interface, so I left the SRAM writes at 6 cycles. Data reads still only take 4 cycles, which is just fast enough for VGA reads to work.<br>
<br>
<h3>Cache</h3>

<img src='http://progressive-learning-platform.googlecode.com/hg/images/CACHE.png' />

Below the SRAM is a simple cache. This is used because reads to the SRAM are already 4 times slower than the CPU, and the VGA module can hold the SRAM for upwards of 640 cycles at a time, all of which the CPU spends stalled.<br>
<br>
The cache used right now is the simplest possible: Direct mapped, unified, write allocate, write through. We use direct mapped because it's the simplest to design, a unified cache because it eliminates coherency issues between caches, write allocate for performance, and write through for coherency with the SRAM. Write through also eliminates the need for eviction logic (we can always safely trash data being evicted). The only cache-able region will be the SRAM (0x10000000-0x10ffffff).<br>
<br>
<h2>UART</h2>

Not much to say about it's development. It's currently fixed at 9600 baud, 8N1.<br>
<br>
<h2>VGA</h2>

The VGA module implements 3 elements: The VGA timing controller, the VGA module interface and control registers, and the VGA-SRAM bypass.<br>
<br>
The VGA timing controller, which is probably the only logic retained from the 1.0 tree, implements a 640x480x60Hz VGA core. It requests an 8-bit color value during the write phase at 25MHz. The 8-bit pixel values are satisfied via the scan line buffer in the VGA-SRAM bypass.<br>
<br>
The VGA module interface and control registers allow the user to control the VGA module. The old module extended a way to read/write to VGA memory, but the VGA memory is in the SRAM now, and the user can just write to RAM to accomplish that. The VGA interface provides 2 registers: control and frame buffer pointer.<br>
<br>
The control register currently only has one bit in the LSB position: 0=disabled, 1=enabled. We provide this functionality because the VGA module can consume a lot of memory time if enabled when unneeded.<br>
<br>
The frame buffer pointer is simply the base address of the frame buffer to draw in memory. This makes double (or N) buffering possible.<br>
<br>
The VGA-SRAM bypass is a special module-to-module interconnect that will override any upstream memory requests. We do this because SRAM reads take 4 cycles, and we attempt to fill a buffer with a full scan line (640 bytes/pixels). We start a scan line read as soon as we enter the backporch, loading a 640 byte register in the VGA module. This is why the VGA module can <b>decimate</b> memory access time for the CPU. The VGA module consumes 640 cycles of every 800 cycle quantum (80%).<br>
<br>
<h2>PLPID</h2>

The PLPID module will eventually house a lot of other system data for use by libplp and other things when the system become fractured. Currently it stores the board revision (2.0=deadbeef) and the board frequency.<br>
<br>
<h2>Timer</h2>

<h2>Programmable Interrupt Controller</h2>

<h2>Switches</h2>

Not much to say, the module has 8 switches.<br>
<br>
<h2>LEDS</h2>

Not much to say, the module has 8 LEDs. We've run into issues with builds of the board having different and undocumented LED mappings.<br>
<br>
<h2>GPIO</h2>

<h2>Seven Segment Display</h2>

The seven segment display is a common anode, 4-unit display. We cycle through each of the 4 display units at 60Hz. To provide the most generic interface, the module simply provides raw access to each of the 8 segments (7+dot) for each of the 4 displays in a single word. libplp wrapper functions exist for various abstractions.