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

top level module for mips design

2.6.2010
*/

module top(clk,leds,rst,txd,rxd,switches,sseg_an,sseg_display, mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub, mod_sram_data, mod_sram_addr, mod_vga_rgb, mod_vga_hs, mod_vga_vs, mod_gpio_gpio);
	input clk;
	output [7:0] leds;
	input rst,rxd;
	output txd;
	input [7:0] switches;
	output [3:0] sseg_an;
	output [7:0] sseg_display;
	output mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub;
	inout [15:0] mod_sram_data;
	output [23:1] mod_sram_addr;	
	output [7:0] mod_vga_rgb;
	output mod_vga_hs, mod_vga_vs;
	inout [15:0] mod_gpio_gpio;

	wire [31:0] daddr, dout, din, iaddr, iin;
	wire [1:0] drw;
	wire cpu_c, arbiter_c, cpu_stall;

	wire int;

	clock c_t(clk, c);
	cpu cpu_t(rst, c, cpu_stall, daddr, dout, din, drw, iaddr, iin, int);
        arbiter arbiter_t(rst, c, daddr, dout, din, drw, iaddr, iin, int, leds, txd, rxd, switches, sseg_an, sseg_display, cpu_stall, mod_sram_clk, mod_sram_adv, mod_sram_cre, mod_sram_ce, mod_sram_oe, mod_sram_we, mod_sram_lb, mod_sram_ub, mod_sram_data, mod_sram_addr, mod_vga_rgb, mod_vga_hs, mod_vga_vs, mod_gpio_gpio);
endmodule
	

