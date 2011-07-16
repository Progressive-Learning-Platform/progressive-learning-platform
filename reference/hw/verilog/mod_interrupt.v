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

interrupt controller module

27.6.2011
*/

module mod_interrupt(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, int, int_ack, i_timer, i_uart, i_button);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input [1:0] drw;
        input [31:0] din;
        output [31:0] iout, dout;
	
	output int;
	input int_ack;

	input i_timer;
	input i_uart;
	input i_button;

	reg [31:1] status; /* zeroth bit is defined as 1 */
	reg [31:0] mask;
	reg	   state; /* 0 idle - 1 interrupting and waiting for ack */

        assign iout = 32'b0; /* never allow executing from this module */
        assign dout = daddr == 32'h00000000 ? mask : {status,1'b1};

	wire [31:1] external_interrupts =
		{28'b0, i_button, i_uart, i_timer};

	assign int = state;
	wire next_state = !state && ((mask[31:1] & status) != 0) && mask[0] ? 1 :
			  state && int_ack ? 0 : state;
	wire [31:0] mask_v = drw[0] && de && daddr == 32'h00000000 ? din : mask;
	wire [31:0] next_mask = state ? {mask_v[31:1],1'b0} : mask_v; /* clear the gie on interrupts */
	wire [31:1] status_v = drw[0] && de && daddr == 32'h00000004 ? din[31:1] : status;
	wire [31:1] next_status = external_interrupts | status_v;

	/* all data bus activity is negative edge triggered */
	always @(negedge clk) begin
		if (!rst) begin
			status <= next_status;
			mask   <= next_mask;
			state  <= next_state;
		end else if (rst) begin
			status <= 0;
			mask   <= 0;
			state  <= 0;
		end
	end
endmodule
