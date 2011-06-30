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

gpio module

12.1.2011
*/

module mod_gpio(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, gpio);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;
	inout [15:0] gpio;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = idata;
        assign dout = ddata;

	/* direction bits = 0 = input */
	reg [15:0] direction = 16'h0000;
	reg [7:0] gpio_a, gpio_b;

	assign idata = 32'h00000000;
	assign ddata = (daddr == 32'h00000000) ? {16'h0000, direction} : /* tristate register */
		       (daddr == 32'h00000004) ? {24'h000000, gpio[7:0]} :  /* block a */
		       (daddr == 32'h00000008) ? {24'h000000, gpio[15:8]} : 0;

	assign gpio[0]  = (direction[0])  ? gpio_a[0] : 1'bz;
	assign gpio[1]  = (direction[1])  ? gpio_a[1] : 1'bz;
	assign gpio[2]  = (direction[2])  ? gpio_a[2] : 1'bz;
	assign gpio[3]  = (direction[3])  ? gpio_a[3] : 1'bz;
	assign gpio[4]  = (direction[4])  ? gpio_a[4] : 1'bz;
	assign gpio[5]  = (direction[5])  ? gpio_a[5] : 1'bz;
	assign gpio[6]  = (direction[6])  ? gpio_a[6] : 1'bz;
	assign gpio[7]  = (direction[7])  ? gpio_a[7] : 1'bz;
	assign gpio[8]  = (direction[8])  ? gpio_b[0] : 1'bz;
	assign gpio[9]  = (direction[9])  ? gpio_b[1] : 1'bz;
	assign gpio[10] = (direction[10]) ? gpio_b[2] : 1'bz;
	assign gpio[11] = (direction[11]) ? gpio_b[3] : 1'bz;
	assign gpio[12] = (direction[12]) ? gpio_b[4] : 1'bz;
	assign gpio[13] = (direction[13]) ? gpio_b[5] : 1'bz;
	assign gpio[14] = (direction[14]) ? gpio_b[6] : 1'bz;
	assign gpio[15] = (direction[15]) ? gpio_b[7] : 1'bz;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (drw[0] && de && !rst) begin
			if (daddr == 32'h00000000)
				direction <= din[15:0];
			else if (daddr == 32'h00000004)
				gpio_a <= din[7:0];
			else if (daddr == 32'h00000008)
				gpio_b <= din[7:0];
		end else if (rst) begin
			gpio_a <= 8'h00;
			gpio_b <= 8'h00;
			direction <= 16'h0000;
		end
	end
endmodule
