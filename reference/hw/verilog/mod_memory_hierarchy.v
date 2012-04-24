/*
    Copyright 2011 David Fritz and Wira Mulia

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

2.5.2011
*/

/* 
this is as basic as it gets:
	direct mapped cache
	write allocate
	write through
	unified, single level
*/

module mod_memory_hierarchy(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, cpu_stall, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr, mod_vga_sram_data, mod_vga_sram_addr, mod_vga_sram_read, mod_vga_sram_rdy, pmc_cache_miss_I, pmc_cache_miss_D, pmc_cache_access_I, pmc_cache_access_D);
	input rst;
	input clk;
	input ie, de;
	input [31:0] iaddr, daddr;
	input [1:0] drw;
	input [31:0] din;
	output [31:0] iout, dout;
	output cpu_stall;
 
	/* sram signals */
	output [31:0] mod_vga_sram_data;
	input  [31:0] mod_vga_sram_addr;
	input	      mod_vga_sram_read;
	output        mod_vga_sram_rdy;
	output 	      sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub;
	output [23:1] sram_addr;
	inout  [15:0] sram_data;

	output pmc_cache_miss_I, pmc_cache_miss_D, pmc_cache_access_I, pmc_cache_access_D;

	wire          cache_iwrite, cache_dwrite;
	wire   [10:0] cache_iaddr, cache_daddr;
	wire   [31:0] cache_iin, cache_din, cache_iout, cache_dout;
	wire	      sram_nrdy, sram_ie, sram_de;
	wire   [1:0]  sram_drw;
	wire   [31:0] sram_iaddr, sram_daddr, sram_din, sram_iout, sram_dout;
	wire   [31:0] tag_iin, tag_din, tag_iout, tag_dout;

	/* cache */
	cache_memory #(11, 32) data_array(clk, cache_iwrite, cache_dwrite, cache_iaddr, cache_daddr, cache_iin, cache_din, cache_iout, cache_dout);
	cache_memory #(11, 32) tag_array (clk, cache_iwrite, cache_dwrite, cache_iaddr, cache_daddr, tag_iin,   tag_din,   tag_iout,   tag_dout);
	
	/* sram */
	mod_sram sram_t(rst, clk, sram_ie, sram_de, sram_iaddr, sram_daddr, sram_drw, sram_din, sram_iout, sram_dout, sram_nrdy, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr, mod_vga_sram_data, mod_vga_sram_addr, mod_vga_sram_read, mod_vga_sram_rdy);

	/* state :
	 * 	000 - idle / reading cache / cache hit
	 *	001 - servicing data miss
	 *	010 - servicing inst miss
	 *	011 - servicing data and instruction miss
	 *	101 - servicing data write
	 *	111 - servicing data write and instruction miss
	*/
	reg 	[3:0]  state = 4'b0000;
	wire 	[3:0]  next_state;
	wire	       ihit, dhit;
	wire	       conflict;

	/* performance counter logic */
	assign pmc_cache_miss_I   = state[3] & state[1];
	assign pmc_cache_miss_D   = state[3] & state[0];
	assign pmc_cache_access_I = ie && next_state == 4'b0000;
	assign pmc_cache_access_D = de && drw != 2'b00 && next_state == 4'b0000;

	assign cpu_stall    = next_state != 4'b0000;
	assign cache_iwrite = state[1] && !conflict;
	assign cache_dwrite = state[0];
	assign cache_iaddr  = iaddr[12:2];
	assign cache_daddr  = daddr[12:2];
	assign cache_iin    = sram_iout;
	assign cache_din    = state[2] ? din : sram_dout;
	assign tag_iin 	    = {13'b0000000000001, iaddr[31:13]};
	assign tag_din	    = {13'b0000000000001, daddr[31:13]};
	assign iout	    = state[1] ? sram_iout : cache_iout;
	assign dout	    = state[0] ? sram_dout : cache_dout;
	assign sram_ie	    = state[1] && !state[3];
	assign sram_de	    = state[0] && !state[3];
	assign sram_drw	    = drw;
	assign sram_iaddr   = iaddr;
	assign sram_daddr   = daddr;
	assign sram_din	    = din;
	assign ihit	    = tag_iout == tag_iin && !conflict;
	assign dhit	    = tag_dout == tag_din;
	assign conflict	    = (cache_iaddr == cache_daddr) && ie && de && drw != 2'b00;
	assign next_state   =
		state == 4'b0000 && ((ihit && ie) || !ie) && !dhit && drw == 2'b10 && de ? 4'b0001 : /* data miss */
		state == 4'b0000 && ((dhit && de && drw == 2'b10) || drw == 2'b00 || !de) && !ihit && ie ? 4'b0010 : /* instruction miss */
		state == 4'b0000 && !ihit && !dhit && drw == 2'b10 && ie && de           ? 4'b0011 : /* instruction and data miss */
		state == 4'b0000 && ((ihit && ie) || !ie) && drw == 2'b01 && de          ? 4'b0101 : /* data write */
		state == 4'b0000 && !ihit && drw == 2'b01 && de && ie                    ? 4'b0111 : /* instruction miss and data write */
		state != 4'b0000 && !state[3] && !sram_nrdy		  	         ? {1'b1,state[2:0]} : 
		state[3]        							 ? 4'b0000 : state; /* returning from sram */

	always @(posedge clk) begin
		if (rst) begin
			state <= 4'b0000;
		end else begin
			state <= next_state;
		end
	end
endmodule

/* inferred dual port ram as indicated by the xilinx xst guide */
module cache_memory(clk, wea, web, addra, addrb, dia, dib, doa, dob);
	parameter ADDR_WIDTH = 0;
	parameter DATA_WIDTH = 0;
	parameter DEPTH = 1 << ADDR_WIDTH;

	input clk;
	input wea, web;
	input [ADDR_WIDTH-1:0] addra, addrb;
	input [DATA_WIDTH-1:0] dia, dib;
	output reg [DATA_WIDTH-1:0] doa, dob;

	reg [DATA_WIDTH-1:0] RAM [DEPTH-1:0];

	always @(negedge clk) begin
		if (wea)
			RAM[addra] <= dia;
		doa <= RAM[addra];
	end

	always @(negedge clk) begin
		if (web)
			RAM[addrb] <= dib;
		dob <= RAM[addrb];
	end
endmodule
