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

module top(clk,leds,rst,txd,rxd,switches,sseg_an,sseg_display);
	input clk;
	output [7:0] leds;
	input rst,rxd;
	output txd;
	input [7:0] switches;
	output [3:0] sseg_an;
	output [7:0] sseg_display;
	
	wire [31:0] daddr, dout, din, iaddr, iin;
	wire drw;

	cpu cpu_t(rst, clk, daddr, dout, din, drw, iaddr, iin);
        arbiter arbiter_t(rst, clk, daddr, dout, din, drw, iaddr, iin, leds, txd, rxd, switches, sseg_an, sseg_display);
endmodule
	

