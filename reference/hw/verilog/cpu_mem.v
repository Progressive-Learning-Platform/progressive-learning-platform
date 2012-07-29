/*
    Memory Stage for Progressive Learning Platform
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

module cpu_mem (
  input             rst,
  input             clk,
  input             cpu_stall,
  input             ex_c_rfw,
  input       [1:0] ex_c_wbsource,
  input       [1:0] ex_c_drw,
  input      [31:0] ex_alu_r,
  input      [31:0] ex_rfb,
  input       [4:0] ex_rf_waddr,
  input      [31:0] ex_jalra,
  input       [4:0] ex_rt,
  input      [31:0] wb_wdata,
  input      [31:0] dmem_in,
  output     [31:0] dmem_data,
  output     [31:0] dmem_addr,
  output      [1:0] dmem_drw,
  output reg        p_c_rfw,
  output reg  [1:0] p_c_wbsource,
  output reg [31:0] p_alu_r,
  output reg  [4:0] p_rf_waddr,
  output reg [31:0] p_jalra,
  output reg [31:0] p_dout
  );

  `include "parameters.v"

  assign dmem_addr = ex_alu_r;
  assign dmem_drw  = ex_c_drw;

  /// Forwarding logic
  //
  wire forward = p_c_rfw & (ex_rt == p_rf_waddr) & (p_rf_waddr != 0);
  assign dmem_data = forward ? wb_wdata : ex_rfb;

  always @(posedge clk) begin
    if (rst) begin
      p_c_rfw      <= 0;
      p_c_wbsource <= 0;
      p_alu_r      <= 0;
      p_rf_waddr   <= 0;
      p_jalra      <= 0;
      p_dout       <= 0;
    end else if (!cpu_stall) begin
      p_c_rfw      <= ex_c_rfw;
      p_c_wbsource <= ex_c_wbsource;
      p_alu_r      <= ex_alu_r;
      p_rf_waddr   <= ex_rf_waddr;
      p_jalra      <= ex_jalra;
      p_dout       <= dmem_in;
      //$display("MEM: %x %x %x %x %x %x", ex_rt, p_rf_waddr, wb_wdata, p_c_rfw, forward, dmem_data);
    end
  end

endmodule
