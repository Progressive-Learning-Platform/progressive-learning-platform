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

timer module

a simple 32-bit timer. Timer increments on every edge. 

*/
module mod_timer(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, int);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;
	output int;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = idata;
        assign dout = ddata;

	reg [31:0] timer;
	
	assign int = timer == 32'hffffffff;

	assign ddata = timer;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		timer = timer + 1;

		if (drw[0] && de && !rst) timer = din;
		else if (rst) timer = 0;
	end
endmodule
