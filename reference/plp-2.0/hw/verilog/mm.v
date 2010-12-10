/*
David Fritz

memory map module

*/

/* the memory map is as follows:

0x00000000      512             bootloader ROM
0x10000000      8192            RAM
0xf0000000      16              UART
0xf0100000      4               switches
0xf0200000      4               leds

0xf0600000	?		VGA

0xf0800000      8               PLPID
0xf0900000      4               timer
0xf0a00000	4		sseg

*/



module mm(addr, mod, eff_addr);
	input [31:0] addr; /* word aligned base address */
	output [7:0] mod; /* the module */
	output [31:0] eff_addr; /* effective address */

	assign mod = (addr[31:20] == 12'h000) ? 0 : /* mod_rom */
		     (addr[31:20] == 12'h100) ? 1 : /* mod_ram */
		     (addr[31:20] == 12'hf00) ? 2 : /* mod_uart */
		     (addr[31:20] == 12'hf01) ? 3 : /* mod_switches */
		     (addr[31:20] == 12'hf02) ? 4 : /* mod_leds */
		     (addr[31:20] == 12'hf04) ? 6 : /* mod_vga */
		     (addr[31:20] == 12'hf05) ? 8 : /* mod_plpid */
		     (addr[31:20] == 12'hf06) ? 9 : /* mod_timer */
		     (addr[31:20] == 12'hf0a) ? 10 : /* mod_sseg */
		     0;
	assign eff_addr = {12'h000,addr[19:0]};
endmodule
