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
	output sram_clk, sram_adv, sram_cre, sram_ce, sram_oe, sram_we, sram_lb, sram_ub;
	output reg [23:0] sram_addr;
	inout [15:0] sram_data;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
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

	reg [1:0] state = 2'b00;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (drw && de && !rst) begin /* write */
			
		end else if (!drw && de && !rst) begin /* read */
			if (state == 2'b00) begin /* new read cycle */
				cpu_stall = 1;
				state = 2'b01;
				sram_we = 1;
				sram_addr = daddr;
		end
	end
endmodule
