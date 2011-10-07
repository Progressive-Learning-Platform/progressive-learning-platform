/*
    Copyright 2011 David Fritz, Brian Gordon, Wira Mulia

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

performance counter hardware

7.7.2011
*/

module mod_pmc(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, pmc_int, pmc_cache_miss_I, pmc_cache_miss_D, pmc_cache_access_I, pmc_cache_access_D, pmc_uart_recv, pmc_uart_send);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;

	input pmc_int, pmc_cache_miss_I, pmc_cache_miss_D, pmc_cache_access_I, pmc_cache_access_D, pmc_uart_recv, pmc_uart_send;

	/* the counter events */
	reg [31:0] count_int;
	reg [31:0] count_cache_miss_I;
	reg [31:0] count_cache_miss_D;
	reg [31:0] count_cache_access_I;
	reg [31:0] count_cache_access_D;
	reg [31:0] count_uart_recv;
	reg [31:0] count_uart_send;

	assign iout = 0;
	assign dout = 
		daddr == 32'h00000000 ? count_int :
		daddr == 32'h00000004 ? count_cache_miss_I :
		daddr == 32'h00000008 ? count_cache_miss_D :
		daddr == 32'h0000000c ? count_cache_access_I : 
		daddr == 32'h00000010 ? count_cache_access_D :
		daddr == 32'h00000014 ? count_uart_recv : 
		daddr == 32'h00000018 ? count_uart_send : 0;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (rst) begin
			count_int <= 0;
			count_cache_miss_I <= 0;
			count_cache_miss_D <= 0;
			count_cache_access_I <= 0;
			count_cache_access_D <= 0;
			count_uart_recv <= 0;
			count_uart_send <= 0;
		end else begin
			count_int <= count_int + pmc_int;
			count_cache_miss_I <= count_cache_miss_I + pmc_cache_miss_I;
			count_cache_miss_D <= count_cache_miss_D + pmc_cache_miss_D;
			count_cache_access_I <= count_cache_access_I + pmc_cache_access_I;
			count_cache_access_D <= count_cache_access_D + pmc_cache_access_D;
			count_uart_recv <= count_uart_recv + pmc_uart_recv;
			count_uart_send <= count_uart_send + pmc_uart_send;
		end
	end
endmodule
