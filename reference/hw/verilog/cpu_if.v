/*
    Instruction Fetch Stage for Progressive Learning Platform
    Copyright 2012 David Fritz, Brian Gordon, Wira Mulia, Matthew Gaalswyk

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

module cpu_if (
  input             rst,
  input             clk,
  input             cpu_stall,
  input             stall,
  input      [31:0] iin,
  output     [31:0] imem_addr, // basically PC
  input      [31:0] b_addr,
  input      [31:0] j_addr,
  input             pc_b,
  input             pc_j,
  output reg [31:0] p_pc,
  output reg [31:0] p_inst,
  input             int,
  input      [31:0] int_pc,
  output            int_ack,
  output            int_flush
);

`include "parameters.v"

reg  [31:0] pc;
wire [31:0] next_pc;

/// Flush logic (for branches and jumps)
wire flush = pc_b | pc_j;

/// Interrupt logic
// we're guaranteed not to get an interrupt when
// handling one already, as interrupts disable
// in the interrupt controller automatically.
//
// state machine:
// 00 - idle, waiting for interrupt
// 01 - wait 1 cycle for branch delay slot
// 10 - injecting interrupt
// 11 - injecting nop (branch delay slot)

localparam S_INT_IDLE = 2'b00;
localparam S_INT_WAIT = 2'b01;
localparam S_INT_IINS = 2'b10;
localparam S_INT_INOP = 2'b11;

reg  [1:0] int_state;
wire cycle_wait = (pc - int_pc) != 8;
wire [1:0] next_int_state =
  int_state == S_INT_IDLE && int &&  cycle_wait ? S_INT_WAIT : // wait 1
  int_state == S_INT_IDLE && int && !cycle_wait ? S_INT_IINS : // go!
  int_state == S_INT_WAIT &&        !cycle_wait ? S_INT_IINS : // ...go
  int_state == S_INT_IINS                       ? S_INT_INOP :
  int_state == S_INT_INOP                       ? S_INT_IDLE :
                                                  int_state;   // invalid

localparam IV_ADDR = `W_ADDR'h0340d809; // JALR $i1, $i0
localparam IV_NOP  = `W_ADDR'h0;
wire [31:0] next_inst =
  int_state == S_INT_IINS ? IV_ADDR :
  int_state == S_INT_INOP ? IV_NOP  :
                            iin;

assign int_ack = int_state != S_INT_IDLE;
assign int_flush = next_int_state == S_INT_IINS;

assign next_pc =
  (        next_int_state == S_INT_IINS) ? int_pc - 8 :
  (pc_b && next_int_state != S_INT_IINS) ? b_addr :
  (pc_j && next_int_state != S_INT_IINS) ? j_addr :
                                           pc + 4;

// our instruction memory pointer value is held by PC
assign imem_addr = pc;

// sync reset
always @(posedge clk) begin
  if (rst) begin
    pc        <= 0;
    p_pc      <= 0;
    p_inst    <= 0;
    int_state <= S_INT_IDLE;
  end else if (!cpu_stall) begin
    int_state <= next_int_state;

    if (stall && !flush) begin
      // stall signal from ID stage
      // hold up a minute and keep things the same
      pc     <= pc;
      p_pc   <= p_pc;
      p_inst <= p_inst;
    end else if ((flush || int_flush) && !stall) begin
      // we branched or jumped or got an interrupt flush signal
      // so we gotta drop what we were going to do
      pc     <= next_pc;
      p_pc   <= 0;
      p_inst <= 0;
    end else begin
      // no stalling or flushing, proceed to next step
      pc     <= next_pc;
      p_pc   <= pc;
      p_inst <= next_inst;
    end

    //$display("IF: PC: %x INST: %x", p_pc, p_inst);
  end
end

endmodule
