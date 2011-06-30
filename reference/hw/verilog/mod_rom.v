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

ROM module, which uses an inferred ram
*/

module mod_rom(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);
	input rst;
	input clk;
	input ie,de;
	input [31:0] iaddr, daddr;
	input [1:0] drw;
	input [31:0] din;
	output [31:0] iout, dout;

	/* by spec, the iout and dout signals must go hiZ when we're not using them */
	wire [31:0] idata, ddata;
	assign iout = idata;
	assign dout = ddata;

        inferred_rom rom(clk,clk,1'b1,1'b1,iaddr[10:2],daddr[10:2],idata,ddata);
endmodule
