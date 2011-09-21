/*
    Copyright 2010 David Fritz, Brian Gordon, Wira Mulia

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */


/*
David Fritz

memory map module

*/

/* the memory map is as follows:

0x00000000      512             bootloader ROM
0x10000000	16777216	SRAM
0xf0000000      16              UART
0xf0100000      4               switches
0xf0200000      4               leds
0xf0300000	12		gpio
0xf0400000	8		VGA
0xf0500000      8               PLPID
0xf0600000      4               timer
0xf0700000      8               interrupt controller
0xf0800000 	?		pmc hardware
0xf0a00000	4		sseg
*/



module mm(addr, mod, eff_addr);
	input [31:0] addr; /* word aligned base address */
	output [7:0] mod; /* the module */
	output [31:0] eff_addr; /* effective address */

	assign mod = (addr[31:20] == 12'h000) ? 0 : /* mod_rom */
		     (addr[31:24] ==   8'h10) ? 1 : /* mod_ram */
		     (addr[31:20] == 12'hf00) ? 2 : /* mod_uart */
		     (addr[31:20] == 12'hf01) ? 3 : /* mod_switches */
		     (addr[31:20] == 12'hf02) ? 4 : /* mod_leds */
		     (addr[31:20] == 12'hf03) ? 5 : /* mod_gpio */
		     (addr[31:20] == 12'hf04) ? 6 : /* mod_vga */
		     (addr[31:20] == 12'hf05) ? 7 : /* mod_plpid */
		     (addr[31:20] == 12'hf06) ? 8 : /* mod_timer */
		     (addr[31:20] == 12'hf07) ? 10 : /* mod_interrupt */
		     (addr[31:20] == 12'hf08) ? 11 : /* mod_pmc */
		     (addr[31:20] == 12'hf0a) ? 9 : /* mod_sseg */
		     0;
	assign eff_addr = (mod == 8'h01) ? {8'h00,addr[23:0]} : {12'h000,addr[19:0]};
endmodule
