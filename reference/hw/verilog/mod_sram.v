/*
    Copyright 2010, 2011 David Fritz, Brian Gordon, Wira Mulia

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

SRAM interface

31.12.2010
*/

module mod_sram(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, cpu_stall, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr, mod_vga_sram_data, mod_vga_sram_addr, mod_vga_sram_read, mod_vga_sram_rdy);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;

	/* vga bypass signals */
	output [31:0] mod_vga_sram_data;
	input [31:0] mod_vga_sram_addr;
	input mod_vga_sram_read;
	output mod_vga_sram_rdy;

	/* sram related signals */
	output cpu_stall, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub;
	output [23:1] sram_addr;
	inout [15:0] sram_data;
	
	wire [31:0] eff_addr;
	wire eff_drw;
	wire [31:0] sram_dout;
	wire rdy;
	wire eff_rst;
	sram_interface sram(eff_rst, clk, eff_addr, eff_drw, din, sram_dout, rdy, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr);

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        reg [31:0] idata, ddata;
        assign iout = idata;
        assign dout = ddata;

	/* 
	 * there are six possible scenarios
	 * nop
	 * instruction read and no data
	 * data read and no instruction
	 * instruction and data read
	 * data write and no instruction
	 * instruction read and data write
	 *
	 * state = 00 = idle
	 * state = 10 = instruction
	 * state = 11 = data
	 *
	 * bypass_state = 0 = no vga bypass active
	 * bypass_state = 1 = vga bypass in flight
	 *
	 */
	reg [1:0] state = 2'b00;
	reg bypass_state = 1'b0;
	reg mod_vga_sram_rdy = 1'b0;
	assign eff_addr = bypass_state ? mod_vga_sram_addr :
			  (state[0] && !bypass_state) ? daddr : iaddr;
	assign eff_drw  = state[0] && de && drw[0] && !rst && !bypass_state;
	assign cpu_stall = (state != 2'b00);
	assign eff_rst = state == 2'b00 && !bypass_state;
	wire [1:0] next_state = (state == 2'b00 && ie)         ? 2'b10 : /* idle to instruction read */
				(state == 2'b00 && !ie && de && drw != 2'b00)  ? 2'b11 : /* idle to data r/w */
				(state == 2'b10 && de && drw != 2'b00 && rdy && !bypass_state) ? 2'b11 : /* instruction read to data r/w */
				(state == 2'b10 && (drw == 2'b00 || !de) && rdy && !bypass_state) ? 2'b00 : /* instruction read to idle */
				(state == 2'b11 && rdy && !bypass_state) ? 2'b00 : /* data r/w to idle */
				state;					 /* otherwise stay put */

	wire vga_bypass_next_state = (state == 2'b00 && mod_vga_sram_read && !bypass_state) ? 1'b1 : /* a new vga bypass request */
				     (bypass_state && rdy && !mod_vga_sram_read)	    ? 1'b0 : /* vga bypass request done */
				     bypass_state;						     /* otherwise stay put */

	/* vga bypass */
	assign mod_vga_sram_data = ddata;
//	assign mod_vga_sram_rdy  = bypass_state && rdy;

        /* all data bus activity is negative edge triggered */
        always @(negedge clk) begin
                if (state == 2'b10 && ie && rdy && !rst && !bypass_state)
			idata <= sram_dout;
		else if (state == 2'b11 && de && rdy && !rst && !bypass_state)
			ddata <= sram_dout;	/* if it's a write cycle, we'll just read garbage, which is fine */
		
		/* vga bypass */
		if (bypass_state && !rst && rdy) begin
			ddata <= sram_dout;
			mod_vga_sram_rdy <= 1'b1;
		end else begin
			mod_vga_sram_rdy <= 1'b0;
		end

		/* handle the state */
		if (rst) begin 
			state <= 2'b00;
			bypass_state <= 1'b0;
		end else begin
			state <= next_state;
			bypass_state <= vga_bypass_next_state;
		end
        end
endmodule

module sram_interface(rst, clk, addr, drw, din, dout, rdy, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr);
        input clk, rst;
        input [31:0] addr;
        input drw;
        input [31:0] din;
        output reg [31:0] dout;
	output rdy;
	output sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_lb, sram_ub;
	output [23:1] sram_addr;
	output sram_we;
	inout [15:0] sram_data;

	/* some sram signals are static */
	assign sram_clk = 0;
	assign sram_adv = 0;
	assign sram_cre = 0;
	assign sram_ce  = 0;
	assign sram_oe  = 0; /* sram_we overrides this signal */
	assign sram_ub  = 0;
	assign sram_lb  = 0;

	reg [2:0] state = 3'b000;
	wire UL = (state == 3'b000 || state == 3'b001 || state == 3'b010) ? 0 : 1;

	assign sram_data = (!drw) ? 16'hzzzz : 
			   (state == 3'b000 || state == 3'b001 || state == 3'b010) ? din[31:16] : din[15:0];
	assign sram_addr = {addr[23:2],UL};
	assign sram_we   = !(drw && state != 3'b010 && state != 3'b100);
	assign rdy = (state == 3'b000);

	always @(posedge clk) begin
		if (!rst) begin
			if (state == 3'b010) dout[31:16] <= sram_data;
			if (state == 3'b100) dout[15:0]  <= sram_data;
			if ((state == 3'b101 && drw) || (state == 3'b100 && !drw))
				state <= 3'b000;
			else
				state <= state + 1;
		end else begin
			state <= 3'b000;
		end
	end
endmodule
