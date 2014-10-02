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

fritz

instruction fetch phase

*/

module cpu_if(rst, clk, cpu_stall, imem_addr, p_pc, pc_j,
		pc_b, b_addr, j_addr, iin, p_inst, stall, int, int_ack, int_flush, int_pc);
	input 		rst, clk, cpu_stall, int;
	input 		pc_j;
	input 		pc_b;
	input 	[31:0] 	b_addr;
	input 	[31:0] 	j_addr;
	input 		stall;
	output reg [31:0] p_pc;
	output 	[31:0] 	imem_addr;
	input	[31:0]	iin;
	output reg [31:0] p_inst;
	output int_ack, int_flush;
	input [31:0] int_pc;

	reg [31:0] pc;
	wire [31:0] next_pc;

	/* flush logic (for branches and jumps) */
	wire flush = pc_b | pc_j;

	/* interrupt logic */
	/* we're guaranteed not to get an interrupt when 
	 * handling one already, as interrupts disable
	 * in the interrupt controller automatically.
	 *
	 * state machine:
	 * 00 - idle, waiting for interrupt
	 * 01 - wait 1 cycle for branch delay slot
	 * 10 - injecting interrupt
	 * 11 - injecting nop (branch delay slot)
	 */
	reg  [1:0] int_state = 2'b00;
	wire cycle_wait = pc - int_pc != 8;
	wire [1:0] next_int_state = 
		int_state == 2'b00 && int &&  cycle_wait ? 2'b01 : /* wait 1 cycle */
		int_state == 2'b00 && int && !cycle_wait ? 2'b10 : /* go! */
		int_state == 2'b01 && !cycle_wait	 ? 2'b10 : /* ...go */
		int_state == 2'b10 			 ? 2'b11 :
		int_state == 2'b11 			 ? 2'b00 : int_state; /* default case is invalid */
	wire [31:0] next_inst = 
		int_state == 2'b10 ? 32'h0380f009 : /* jalr $ir, $iv - 0000_0011_1000_0000_1111_0000_0000_1001 */
		int_state == 2'b11 ? 32'h00000000 : /* nop */
				     iin;
	assign int_ack = int_state != 2'b00;
	assign int_flush = next_int_state == 2'b10;

	always @(posedge clk) begin
		if (!cpu_stall) begin
		if (rst) begin
			p_pc <= 0;
			pc <= 0;
			p_inst <= 0;
			int_state <= 0;
		end else if (stall && !flush && !rst) begin
			p_pc <= p_pc;
			pc <= pc;
			p_inst <= p_inst;
			int_state <= next_int_state;
		end else if ((flush || int_flush) && !rst && !stall) begin
			p_pc <= 0;
			p_inst <= 0;
			pc <= next_pc;
			int_state <= next_int_state;
		end else begin
			p_pc <= pc;
			pc   <= next_pc;
			p_inst <= next_inst;
			int_state <= next_int_state;
		end
		
		/* debug code, not synthesized by Xilinx */
		$display("IF: PC: %x INST: %x", p_pc, p_inst);
		end
	end		

	assign next_pc =
		(next_int_state == 2'b10) ? int_pc - 8: 
		(pc_b & next_int_state != 2'b10) ? b_addr :
		(pc_j & !pc_b & next_int_state != 2'b10) ? j_addr :
		pc + 4;

	assign imem_addr = pc;
endmodule
