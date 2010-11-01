/*
David Fritz

memory map module

2.5.2010
*/

/* the memory map is as follows:
0x00000000      512             bootloader ROM
0x10000000      8192            RAM
0x20000000      4               rs/232
0x30000000      4               switches
0x40000000      4               leds
0x50000000      4               buttons
0x60000000      4               GPIO
0x70000000      ?               VGA framebuffer
0xf0000000	?		DDR
*/



module mm(addr, mod, eff_addr);
	input [31:0] addr; /* word aligned base address */
	output [3:0] mod; /* the module */
	output [31:0] eff_addr; /* effective address */

	assign mod = addr[31:28];
	assign eff_addr = {4'h0,addr[27:0]};
endmodule
