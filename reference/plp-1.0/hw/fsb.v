/*
David Fritz

front side bus for ECEN4243 SoC design projects

2.5.2010
*/
		
	/* 
	the fsb just routes signals to and from the various hardware that we add to the board,
	all of which is instantiated here.

	therefore the memory map is also contained here.
	 
	in order to simplify the memory map selection, we route the address lines into a memory map module,
	which returns an id of what device to map to, as well as an effective address.
	

	the memory map is as follows:
		0            	bootloader ROM
		1            	RAM
		2               rs/232
		3               switches
		4               leds
		5               buttons
		6               GPIO
		7               VGA framebuffer
		8		CPU ID
		9		Timer
		f               DDR
	*/

module fsb(clk, cpu_daddress, cpu_din, cpu_dout, cpu_drw, cpu_iaddress, cpu_iout, leds, rst, txd, rxd, switches, rgb, hs, vs, gpi);
	input clk, rst;

	input cpu_drw; /* data read/write. cpu_drw = 0 is read */
	input [31:0] cpu_daddress, cpu_iaddress; /* word aligned */
	input [31:0] cpu_din; /* cpu data coming from the cpu */
	output [31:0] cpu_dout, cpu_iout; /* cpu data going to the cpu */

	//module i/o
	output [7:0] leds;
	input rxd;
	output txd;
	input [3:0] switches;
	output [2:0] rgb;
	output hs,vs;
	input [7:0] gpi; 
	
	wire [3:0] imod;
	wire [31:0] ieff_addr;
	mm imm(cpu_iaddress, imod, ieff_addr);

	wire [3:0] dmod;
	wire [31:0] deff_addr;
	mm dmm(cpu_daddress, dmod, deff_addr);

	/* we have instruction and data enable pins for the memories, and data enable pins for the other i/o, as per the specification in ../module_spec */
	wire mod0_ie, mod0_de;
	wire mod1_ie, mod1_de;
	wire mod2_de;
	wire mod3_de;
	wire mod4_de;
	wire mod6_de;
	wire mod7_de;
	wire mod8_de;
	wire mod9_de;

	assign mod0_ie = imod == 0;
	assign mod1_ie = imod == 1;

	assign mod0_de = dmod == 0;
	assign mod1_de = dmod == 1;
	assign mod2_de = dmod == 2;
	assign mod3_de = dmod == 3;
	assign mod4_de = dmod == 4;
	assign mod6_de = dmod == 6;
	assign mod7_de = dmod == 7;
	assign mod8_de = dmod == 8;
	assign mod9_de = dmod == 9;

	/* instead of all of the modules sharing the output buses, and going highZ when they're not in use,
		we'll explicitly convert them to simple logic to speed things up.
	*/
	wire [31:0] cpu_iout0, cpu_iout1;    
	wire [31:0] cpu_dout0, cpu_dout1, cpu_dout2, cpu_dout3, cpu_dout4, cpu_dout6, cpu_dout7, cpu_dout8, cpu_dout9;

	assign cpu_iout = (cpu_iout0 | cpu_iout1);
	assign cpu_dout = (cpu_dout0 | cpu_dout1 | cpu_dout2 | cpu_dout3 | cpu_dout4 | cpu_dout6 | cpu_dout7 | cpu_dout8 | cpu_dout9);

	/* here we instantiate all of the modules */
	mod_rom 			rom_t     	(clk, mod0_ie, mod0_de, ieff_addr, deff_addr, cpu_drw, cpu_din, cpu_iout0, cpu_dout0);
	mod_ram 			ram_t     	(clk, mod1_ie, mod1_de, ieff_addr, deff_addr, cpu_drw, cpu_din, cpu_iout1, cpu_dout1);
	mod_uart			uart_t    	(clk, mod2_de, deff_addr, cpu_drw, cpu_din, cpu_dout2, txd, rxd, rst);
	mod_switches 	switches_t 	(mod3_de, cpu_dout3, switches);
	mod_leds			leds_t    	(clk, mod4_de, deff_addr, cpu_drw, cpu_din, cpu_dout4, leds, rst);
	mod_gpi 			gpi_t			(mod6_de, cpu_dout6, gpi);
	mod_vga			vga_t			(clk, mod7_de, deff_addr, cpu_drw, cpu_din, cpu_dout7, rst, rgb, hs, vs);
	mod_cpuid		cpuid_t   	(mod8_de, deff_addr, cpu_dout8);
	mod_timer		timer_t	  	(clk, mod9_de, cpu_drw, cpu_din, cpu_dout9, rst);
endmodule
