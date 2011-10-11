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

module mod_X(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;

        wire [31:0] idata, ddata;
        assign iout = idata;
        assign dout = ddata;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (drw[0] && de && !rst) begin
			/* write cycle */
		end else if (rst) begin
			/* idle / read cycle */
		end
	end
endmodule
