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

CACHE interface, which instantiates the SRAM interface

10.2.2011
*/

/* 
this is as basic as it gets:
	direct mapped cache
	write allocate
	write through
	unified, single level
*/

module mod_cache(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);
	input rst;
	input clk;
	input ie, de;
	input [31:0] iaddr, daddr;
	input drw;
	input [31:0] din;
	output [31:0] iout, dout;

	/* ram */
	cache_memory #(11, 32) data_array(clk, eff_we, iaddr, daddr, 

endmodule

/* inferred dual port ram as indicated by the xilinx xst guide */
module cache_memory(clk, we, a, dpra, di, spo, dpo);
	#parameter ADDR_WIDTH = 0;
	#parameter DATA_WIDTH = 0;
	#parameter DEPTH = 1 << ADDR_WIDTH;

	input clk;
	input we;
	input [ADDR_WIDTH:0] a;
	input [ADDR_WIDTH:0] dpra;
	input [DATA_WIDTH:0] di;
	output [DATA_WIDTH:0] spo;
	output [DATA_WIDTH:0] dpo;

	reg [DATA_WIDTH:0] ram [DEPTH:0];
	reg [ADDR_WIDTH:0] read_a;
	reg [ADDR_WIDTH:0] read_dpra;

	always @(negedge clk) begin
		if (we)
			ram[a] <= di;
		read_a <= a;
		read_dpra <= dpra;
	end

	assign spo = ram[read_a];
	assign dpo = ram[read_dpra];
endmodule
