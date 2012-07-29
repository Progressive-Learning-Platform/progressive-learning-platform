/*
    Execution Stage for Progressive Learning Platform
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

module cpu_ex (
  input             rst,
  input             clk,
  input             cpu_stall,

  input             id_c_rfw,
  input       [1:0] id_c_wbsource,
  input       [1:0] id_c_drw,
  input       [5:0] id_c_alucontrol,
  input             id_c_j,
  input             id_c_b,
  input             id_c_jjr,
  input      [31:0] id_rfa,
  input      [31:0] id_rfb,
  input      [31:0] id_imm32,
  input       [4:0] id_shamt,
  input       [5:0] id_func,
  input       [4:0] id_rf_waddr,
  input      [31:0] id_pc,
  input      [25:0] id_jaddr,
  input             id_c_rfbse,
  input       [4:0] id_rs,
  input       [4:0] id_rt,

  input      [31:0] wb_wdata,
  input             wb_rfw,
  input       [4:0] wb_waddr,

  input             int_flush,
  output     [31:0] int_pc,

  output reg        p_c_rfw,
  output reg  [1:0] p_c_wbsource,
  output reg  [1:0] p_c_drw,
  output reg [31:0] p_alu_r,
  output reg [31:0] p_rfb,
  output reg  [4:0] p_rf_waddr,
  output reg [31:0] p_jalra,
  output reg  [4:0] p_rt,

  output     [31:0] baddr,
  output     [31:0] jaddr,
  output            c_b,
  output            c_j
);

`include "parameters.v"

/// Forward Logic
// TODO: mnemonics for the bitmasks here
wire [1:0] forwardX =
  p_c_rfw && (p_rf_waddr == id_rs) && (p_rf_waddr != 0) ? 2'b01 :
  wb_rfw && (wb_waddr == id_rs) && (wb_waddr != 0)      ? 2'b10 :
                                                          0;
wire [1:0] forwardY =
  p_c_rfw && (p_rf_waddr == id_rt) && (p_rf_waddr != 0) ? 2'b01 :
  wb_rfw && (wb_waddr == id_rt) && (wb_waddr != 0)      ? 2'b10 :
                                                          0;

wire [31:0] x =
  (forwardX == 2'b00) ? id_rfa :
  (forwardX == 2'b01) ? p_alu_r :
  (forwardX == 2'b10) ? wb_wdata : 0;
wire [31:0] eff_y =
  (forwardY == 2'b00) ? id_rfb :
  (forwardY == 2'b01) ? p_alu_r :
  (forwardY == 2'b10) ? wb_wdata : 0;

wire [31:0] y = (id_c_rfbse == MUX_IMM) ? id_imm32 : eff_y;
wire cmp_signed = $signed(x) < $signed(y);
wire cmp_unsigned = x < y;

/// Multiplication
wire [63:0] r_mul = $signed(x) * $signed(y);

/// ALU Control
//
wire [5:0] alu_func =
  (id_c_alucontrol == R_TYPE) ? id_func  :
  (id_c_alucontrol == JR    ) ? F_ADD    :
  (id_c_alucontrol == JALR  ) ? F_ADD    :
  (id_c_alucontrol == ANDI  ) ? F_AND    :
  (id_c_alucontrol == ORI   ) ? F_OR     :
  (id_c_alucontrol == XORI  ) ? F_XOR    :
  (id_c_alucontrol == SLTI  ) ? F_CMP_S  :
  (id_c_alucontrol == SLTIU ) ? F_CMP_U  :
  (id_c_alucontrol == LW    ) ? F_ADD    :
  (id_c_alucontrol == SW    ) ? F_ADD    :
  (id_c_alucontrol == LUI   ) ? F_LSHIFT :
  (id_c_alucontrol == BEQ   ) ? F_NEQ    :
  (id_c_alucontrol == BNE   ) ? F_EQ     :
                                0;

wire [4:0] shamt =
  (id_c_alucontrol == LUI) ? 5'd16 :
  (alu_func == F_LSHIFTV ||
   alu_func == F_RSHIFTV ) ? id_rfa[4:0] : // we only use the lower 5 bits
                             id_shamt;

/// The ALU
// This is a major portion of FPGA area
wire [31:0] alu_r =
  (alu_func == F_ADD    ) ? x + y        :
  (alu_func == F_AND    ) ? x & y        :
  (alu_func == F_NOR    ) ? ~(x|y)       :
  (alu_func == F_OR     ) ? x | y        :
  (alu_func == F_XOR    ) ? x ^ y        :
  (alu_func == F_CMP_S  ) ? cmp_signed   :
  (alu_func == F_CMP_U  ) ? cmp_unsigned :
  (alu_func == F_LSHIFT ||
   alu_func == F_LSHIFTV) ? y << shamt   :
  (alu_func == F_RSHIFT ||
   alu_func == F_RSHIFTV) ? y >> shamt   :
  (alu_func == F_SUB    ) ? x - y        :
  (alu_func == F_NEQ    ) ? x != y       :
  (alu_func == F_EQ     ) ? x == y       :
  (alu_func == F_MULLO  ) ? r_mul[31:0]  :
  (alu_func == F_MULHI  ) ? r_mul[63:32] :
                           0;


/// Branch and Jump Logic
wire [31:0] jalra = id_pc + 8;
wire [31:0] pc_4  = id_pc + 4;
wire [31:0] jjal_jaddr = {pc_4[31:28], id_jaddr, 2'b0};
assign c_j = id_c_j;
assign c_b = id_c_b && (alu_r == 0);
assign jaddr = id_c_jjr ? x : jjal_jaddr;
assign baddr = {id_imm32[29:0],2'b0} + pc_4;

assign int_pc = id_pc;

always @(posedge clk) begin
  if (rst) begin
    // do all these need to be reset?
    p_c_rfw      <= 0;
    p_c_wbsource <= 0;
    p_c_drw      <= 0;
    p_alu_r      <= 0;
    p_rfb        <= 0;
    p_rf_waddr   <= 0;
    p_jalra      <= 0;
    p_rt         <= 0;
  end else if (!cpu_stall) begin
    if (int_flush) begin
      // TODO: can we move this in with the rst signal?
      // is this even necessary?
      p_c_rfw      <= 0;
      p_c_wbsource <= 0;
      p_c_drw      <= 0;
      p_alu_r      <= 0;
      p_rfb        <= 0;
      p_rf_waddr   <= 0;
      p_jalra      <= 0;
      p_rt         <= 0;
    end else begin
      p_c_rfw      <= id_c_rfw;
      p_c_wbsource <= id_c_wbsource;
      p_c_drw      <= id_c_drw;
      p_alu_r      <= alu_r;
      p_rfb        <= eff_y;
      p_rf_waddr   <= id_rf_waddr;
      p_jalra      <= jalra;
      p_rt         <= id_rt;
    end
  end
end
endmodule
