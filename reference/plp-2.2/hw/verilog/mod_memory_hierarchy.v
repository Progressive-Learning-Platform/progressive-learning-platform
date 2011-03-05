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

module mod_memory_hierarchy(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, cpu_stall, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr, mod_vga_sram_data, mod_vga_sram_addr, mod_vga_sram_read, mod_vga_sram_rdy);
	input rst;
	input clk;
	input ie, de;
	input [31:0] iaddr, daddr;
	input drw;
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

	wire          cache_iwrite, cache_dwrite;
	wire   [10:0] cache_iaddr, cache_daddr;
	wire   [31:0] cache_iin, cache_din, cache_iout, cache_dout, tag_iin, tag_din, tag_iout, tag_dout;
	wire	      sram_rdy, sram_ie, sram_de, sram_drw;
	wire   [31:0] sram_iaddr, sram_daddr, sram_din, sram_iout, sram_dout;
	

	/* cache */
	cache_memory #(11, 32) data_array(clk, clk, cache_iwrite, cache_dwrite, cache_iaddr, cache_daddr, cache_iin, cache_din, cache_iout, cache_dout);
	cache_memory #(11, 22) tag_array(clk, clk, cache_iwrite, cache_dwrite, cache_iaddr, cache_daddr, tag_iin, tag_din, tag_iout, tag_dout);
	
	/* sram */
	mod_sram sram_t(rst, clk, sram_ie, sram_de, sram_iaddr, sram_daddr, sram_drw, sram_din, sram_iout, sram_dout, sram_rdy, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr, mod_vga_sram_data, mod_vga_sram_addr, mod_vga_sram_read, mod_vga_sram_rdy);

	/* state :
	 * 	000 - idle / reading cache / cache hit
	 *	001 - servicing data miss
	 *	010 - servicing inst miss
	 *	011 - servicing data and instruction miss
	 *	101 - servicing data write
	 *	111 - servicing data write and instruction miss
	*/
	reg 	[2:0] state = 3'b000;
	wire 	[2:0] next_state;
	wire	      ihit, dhit;

	assign cpu_stall    = next_state != 3'b000;
	assign cache_iwrite = (state & 3'b010);
	assign cache_dwrite = (state & 3'b001);
	assign cache_iaddr  = iaddr[10:0];
	assign cache_daddr  = daddr[10:0];
	assign cache_iin    = sram_iout;
	assign cache_din    = (state & 3'b100) ? din : sram_dout;
	assign tag_iin 	    = {1'b1, iaddr[31:11]};
	assign tag_din	    = {1'b1, daddr[31:11]};
	assign iout	    = cache_iout;
	assign dout	    = cache_dout;
	assign sram_ie	    = (state & 3'b010);
	assign sram_de	    = (state & 3'b001);
	assign sram_drw	    = drw;
	assign sram_iaddr   = iaddr;
	assign sram_daddr   = daddr;
	assign sram_din	    = din;
	assign ihit	    = tag_iout == iaddr[31:11];
	assign dhit	    = tag_dout == daddr[31:11];
	assign next_state   =
		state == 3'b000 && ihit && !dhit && !drw && ie        ? 3'b001 : /* data miss */
		state == 3'b000 && !ihit && dhit && !drw && de        ? 3'b010 : /* instruction miss */
		state == 3'b000 && !ihit && !dhit && !drw && ie && de ? 3'b011 : /* instruction and data miss */
		state == 3'b000 && ihit && drw && de                  ? 3'b101 : /* data write */
		state == 3'b000 && !ihit && drw && de && ie           ? 3'b111 : /* instruction miss and data write */
		state != 3'b000 && sram_rdy		  	      ? 3'b000 : 0; /* returning from sram */

	always @(posedge clk) begin
		if (rst)
			state <= 3'b000;
		else
			state <= next_state;	
	end
endmodule

/* inferred dual port ram as indicated by the xilinx xst guide */
module cache_memory(clka, clkb, wea, web, addra, addrb, dia, dib, doa, dob);
	parameter ADDR_WIDTH = 0;
	parameter DATA_WIDTH = 0;
	parameter DEPTH = 1 << ADDR_WIDTH;

	input clka, clkb;
	input wea, web;
	input [ADDR_WIDTH-1:0] addra, addrb;
	input [DATA_WIDTH-1:0] dia, dib;
	output reg [DATA_WIDTH-1:0] doa, dob;

	reg [DATA_WIDTH-1:0] RAM [DEPTH-1:0];

	always @(negedge clka) begin
		if (wea)
			RAM[addra] <= dia;
		doa <= RAM[addra];
	end

	always @(negedge clkb) begin
		if (web)
			RAM[addrb] <= dib;
		dob <= RAM[addrb];
	end
endmodule
