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
		pc_b, b_addr, j_addr, iin, p_inst, stall, int);
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

	reg [31:0] pc;
	wire [31:0] next_pc;

	/* interrupt logic */
	/* we're guaranteed not to get an interrupt when 
	 * handling one already, as interrupts disable
	 * in the interrupt controller automatically.
	 *
	 * state machine:
	 * 00 - idle, waiting for interrupt
	 * 01 - stalling for previous branch
	 * 10 - injecting interrupt
	 * 11 - injecting nop (branch delay slot)
	 */
	reg  [1:0] int_state = 2'b00;
	wire prev_branch = iin[31:26] == 6'h04 || /* beq */
			   iin[31:26] == 6'h05 || /* bne */
			   iin[31:26] == 6'h02 || /* j */
			   iin[31:26] == 6'h03 || /* jal */
			   ((iin[31:26] == 6'h00) && (iin[5:0] == 6'h08)) || /* jr */ 
			   ((iin[31:26] == 6'h00) && (iin[5:0] == 6'h09)); /* jalr */
	wire [1:0] next_int_state = 
		int_state == 2'b00 && int && prev_branch ? 				2'b01 : /* stall for one cycle */
		int_state == 2'b00 && int && (flush || stall) ?				2'b01 : /* stall for flush or stalls, interrupt will happen later */
		int_state == 2'b00 && int && !prev_branch && !stall && !flush ? 	2'b10 : /* go for it! */
		int_state == 2'b01 ? 							2'b10 :
		int_state == 2'b10 ?							2'b11 :
		int_state == 2'b11 ? 							2'b00 : 2'b00; /* default case is invalid */
	wire [31:0] next_inst = 
		int_state == 2'b00 ? iin : /* no interrupt */
		int_state == 2'b01 ? iin : /* stalling, allow iin to drive still */
		int_state == 2'b10 ? 32'h0340d809 : 32'h0; /*jalr $k1, $k0 - 0000_0011_0100_0000_1101_1000_0000_1001*/

	/* flush logic (for branches and jumps) */
	wire flush = pc_b | pc_j;

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
			int_state <= int_state;
		end else if (flush && !rst && !stall) begin
			p_pc <= 0;
			p_inst <= 0;
			pc <= next_pc;
			int_state <= int_state;
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
		(pc_b) ? b_addr :
		(pc_j & !pc_b) ? j_addr :
		pc + 4;

	assign imem_addr = pc;
endmodule
