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

seven segment display

*/

module mod_sseg(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, sseg_an, sseg_display);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;
	output reg [3:0] sseg_an;
	output [7:0] sseg_display;
	

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = idata;
        assign dout = ddata;

	/* the seven segment register */
	reg [31:0] sseg;

	assign idata = 32'h00000000;
	assign ddata = sseg;

	parameter CLOCK_FREQ = 25000000;
	parameter TICKS = CLOCK_FREQ/240; /* 60Hz (by 4) tick clock */
	reg [31:0] counter;

	/* segment logic */
	assign sseg_display = sseg_an == 4'b1110 ? sseg[7:0] : 
			      sseg_an == 4'b1101 ? sseg[15:8] :
			      sseg_an == 4'b1011 ? sseg[23:16] :
			      sseg_an == 4'b0111 ? sseg[31:24] : sseg[7:0];

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (drw[0] && de && !rst) begin
			sseg = din;
		end else if (rst) begin
			sseg = 32'h00000000;
			counter = 0;
		end
		counter = counter + 1;
		if (counter == TICKS) begin
			counter = 0;
			sseg_an = sseg_an == 4'b1110 ? 4'b1101 :
				  sseg_an == 4'b1101 ? 4'b1011 :
				  sseg_an == 4'b1011 ? 4'b0111 :
				  sseg_an == 4'b0111 ? 4'b1110 : 4'b1110;
		end
	end
endmodule
