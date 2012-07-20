/*
  Instruction decode stage for Progressive Learning Platform
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

module cpu_id (
  input             rst,
  input             clk,
  input             cpu_stall,
  input      [31:0] if_pc,
  input      [31:0] if_inst,
  input             wb_rfw,
  input       [4:0] wb_rf_waddr,
  input      [31:0] wb_rf_wdata,
  input             int_flush,
  output reg [31:0] p_rfa,
  output reg [31:0] p_rfb,
  output reg [31:0] p_imm32,
  output reg  [4:0] p_shamt,
  output reg  [5:0] p_func,
  output reg  [4:0] p_rf_waddr,
  output reg        p_c_rfw,
  output reg  [1:0] p_c_wbsource,
  output reg  [1:0] p_c_drw,
  output reg  [5:0] p_c_alucontrol,
  output reg        p_c_j,
  output reg        p_c_b,
  output reg        p_c_jjr,
  output reg [25:0] p_jaddr,
  output reg [31:0] p_pc,
  output reg        p_c_rfbse,
  output reg  [4:0] p_rs,
  output reg  [4:0] p_rt,
  output            c_stall
);

`include "parameters.v"

reg [31:0] rf [31:1];

// opcode | rs | rt | rd | shamt | func
// opcode | rs | rt | imm
// opcode | jaddr
// Split up the instruction into its component fields
wire  [5:0] opcode = if_inst[31:26];
wire  [4:0] rf_rs  = if_inst[25:21];
wire  [4:0] rf_rt  = if_inst[20:16];
wire  [4:0] rf_rd  = if_inst[15:11];
wire  [4:0] shamt  = if_inst[10: 6];
wire  [5:0] func   = if_inst[ 5: 0];
wire [15:0] imm    = if_inst[15: 0];
wire [25:0] jaddr  = if_inst[25: 0];

wire [31:0] rfa    = rf_rs == 0 ? 0 : rf[rf_rs];
wire [31:0] rfb    = rf_rt == 0 ? 0 : rf[rf_rt];

/// HAZARD LOGIC
assign c_stall =
  p_c_rfw &&                          // writing to register,
  p_c_alucontrol == LW &&             // from a LW operation,
  (p_rt == rf_rs || p_rt == rf_rt) && // and target is same as incoming rs or rt,
  p_rt != 0 &&                        // (and is not zero register),
  opcode != SW;                       // and incoming instruction is not SW

/// CONTROL LOGIC
wire c_rfw =
  opcode != BEQ &&
  opcode != BNE &&
  opcode != SW  &&
  opcode != J   &&
  !c_stall; // secret bug, jump register asserts write enable but the assembler sets rd = 0

wire [1:0] c_wbsource =
  (opcode == LW ) ? MUX_DWORD :
  (opcode == JAL) || (opcode == R_TYPE && func == JALR)
                  ? MUX_JALRA :
                    MUX_ALU_R ;

wire [1:0] c_drw =
  (opcode == SW && !c_stall) ? DMEM_WRITE :
  (opcode == LW && !c_stall) ? DMEM_READ  :
                               DMEM_NOP   ;

wire [5:0] c_alucontrol = opcode; // just pass-through the opcode

// zero/sign extension
// TODO: rename?
wire c_se = (opcode == ANDI || opcode == ORI) ? MUX_ZE : MUX_SE;

// ALU Y source (rfb or immediate extended value)
// TODO: I think I want to rename this
wire c_rfbse =
  opcode == R_TYPE || opcode == BEQ || opcode == BNE ? MUX_RFB : MUX_IMM;

wire c_jjr = (opcode == J || opcode == JAL ) ? MUX_JADDR : MUX_JRA;

wire [1:0] c_rd_rt_31 =
  (opcode == JAL   ) ? MUX_RA :
  (opcode == R_TYPE) ? MUX_RD :
                       MUX_RT ;

/// INTERNAL LOGIC

// immediate field zero/sign extension
wire [15:0] imm_extension = c_se ? {16{imm[15]}} : 16'b0;
wire [31:0] imm32 = {imm_extension, imm};


wire [4:0] rd_rt_31 =
  (c_rd_rt_31 == MUX_RD) ? rf_rd :
  (c_rd_rt_31 == MUX_RT) ? rf_rt :
  (c_rd_rt_31 == MUX_RA) ? SPR_JRA :
                           rf_rd;

// assert jump signal if not stalled and instruction is one of the 4 Jump inst
wire c_j =
  !c_stall &&
  ( (opcode == J  ) ||
    (opcode == JAL) ||
    (func   == JR   && opcode == R_TYPE ) ||
    (func   == JALR && opcode == R_TYPE ) );

wire c_b = (opcode == BEQ || opcode == BNE) && !c_stall;

always @(posedge clk) begin
  // TODO: reorganize to proper reset block
  if (!cpu_stall) begin
    if (rst || int_flush) begin
      // TODO: do these all need to be reset to zero?
      p_rfa          <= 0;
      p_rfb          <= 0;
      p_shamt        <= 0;
      p_func         <= 0;
      p_rf_waddr     <= 0;
      p_c_rfw        <= 0;
      p_c_wbsource   <= 0;
      p_c_drw        <= 0;
      p_c_alucontrol <= 0;
      p_c_j          <= 0;
      p_c_b          <= 0;
      p_c_jjr        <= 0;
      p_jaddr        <= 0;
      p_pc           <= 0;
      p_c_rfbse      <= 0;
      p_rs           <= 0;
      p_rt           <= 0;
      p_imm32        <= 0;
    end else begin
      p_rfa          <= rfa;
      p_rfb          <= rfb;
      p_shamt        <= shamt;
      p_func         <= func;
      p_rf_waddr     <= rd_rt_31;
      p_c_rfw        <= c_rfw;
      p_c_wbsource   <= c_wbsource;
      p_c_drw        <= c_drw;
      p_c_alucontrol <= c_alucontrol;
      p_c_j          <= c_j;
      p_c_b          <= c_b;
      p_c_jjr        <= c_jjr;
      p_jaddr        <= jaddr;
      p_pc           <= if_pc;
      p_c_rfbse      <= c_rfbse;
      p_rs           <= rf_rs;
      p_rt           <= rf_rt;
      p_imm32        <= imm32;
    end
  end
end

always @(negedge clk) begin
  // writes to register file on negative clock edge
  if (wb_rfw && wb_rf_waddr != 5'd0) begin
    rf[wb_rf_waddr] <= wb_rf_wdata;
  end
end

endmodule
