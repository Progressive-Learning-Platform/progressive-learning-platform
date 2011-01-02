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

SRAM interface

31.12.2010
*/

module mod_sram(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, cpu_stall, sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub, sram_data, sram_addr);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input drw;
        input [31:0] din;
        output [31:0] iout, dout;
	
	output cpu_stall;
	output sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_lb, sram_ub;
	output [23:1] sram_addr;
	output sram_we;
	inout [15:0] sram_data;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        reg [31:0] idata, ddata;
        assign iout = ie ? idata : 32'hzzzzzzzz;
        assign dout = de ? ddata : 32'hzzzzzzzz;

	/* some sram signals are static */
	assign sram_clk = 0;
	assign sram_adv = 0;
	assign sram_cre = 0;
	assign sram_ce  = 0;
	assign sram_oe  = 0; /* sram_we overrides this signal */
	assign sram_ub  = 0;
	assign sram_lb  = 0;

	reg [2:0] state = 3'b000;
	wire UL = (state == 3'b000 || state == 3'b001 || state == 3'b010 || state == 3'b011) ? 0 : 1;

	assign sram_data = (!drw && de && !rst) ? 16'hzzzz : 
			   (state == 3'b000 || state == 3'b001 || state == 3'b010 || state == 3'b011) ? din[31:16] : din[15:0];
	assign sram_addr = {daddr[23:2],UL};
	assign sram_we   = !(drw && de && !rst && state != 3'b011 && state != 3'b110);
	assign cpu_stall = (state != 3'b000);

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (de && !rst) begin
			if (state == 3'b011) ddata[31:16] <= sram_data;
			if (state == 3'b110) ddata[15:0]  <= sram_data;
			if (state == 3'b110)
				state <= 3'b000;
			else
				state <= state + 1;
		end else if (rst) begin
			state <= 3'b000;
		end
	end
endmodule
