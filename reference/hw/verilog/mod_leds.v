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

led control module

2.5.2010
*/

/*
There are 8 leds on the Spartan3E board, and they are addressable as the last byte of the returned word
*/
module mod_leds(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, leds);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;
	output reg [7:0] leds;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = idata;
        assign dout = ddata;

	assign idata = 32'h00000000;
	assign ddata = {24'h000000,leds};

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (drw[0] && de && !rst) begin
			leds = din[7:0];
			$display("MOD_LEDS: %x", din[7:0]);
		end else if (rst) begin
			leds = 8'hff;
		end
	end
endmodule
