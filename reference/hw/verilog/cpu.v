/*
    Pipelined CPU for Progressive Learning Platform
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

module cpu(
  input         rst,
  input         clk,
  input         cpu_stall,

  input  [31:0] iin,
  input  [31:0] din,

  output [31:0] imem_addr,
  output [31:0] dmem_addr,
  output [31:0] dout,

  output  [1:0] drw,

  input         int,
  output        int_ack
);

wire [31:0] if_pc;
wire [31:0] if_inst;

wire        id_stall; // not a register

wire [31:0] id_rfa;
wire [31:0] id_rfb;
wire [31:0] id_imm32;
wire  [4:0] id_shamt;
wire  [5:0] id_func;
wire  [4:0] id_rf_waddr;
wire        id_c_rfw;
wire  [1:0] id_c_wbsource;
wire  [1:0] id_c_drw;
wire  [5:0] id_c_alucontrol;
wire        id_c_j;
wire        id_c_b;
wire        id_c_jjr;
wire [25:0] id_jaddr;
wire [31:0] id_pc;
wire        id_c_rfbse;
wire  [4:0] id_rs;
wire  [4:0] id_rt;

wire        ex_c_rfw;
wire  [1:0] ex_c_wbsource;
wire  [1:0] ex_c_drw;
wire [31:0] ex_alu_r;
wire [31:0] ex_rfb;
wire  [4:0] ex_rf_waddr;
wire [31:0] ex_jalra;
wire  [4:0] ex_rt;
wire [31:0] ex_baddr;
wire [31:0] ex_jaddr;
wire        ex_b;
wire        ex_j;

wire        mem_c_rfw;
wire  [1:0] mem_c_wbsource;
wire [31:0] mem_alu_r;
wire  [4:0] mem_rf_waddr;
wire [31:0] mem_jalra;
wire [31:0] mem_dout;

wire        wb_rfw;
wire [31:0] wb_wdata;
wire  [4:0] wb_waddr;

wire [31:0] int_pc;
wire        int_flush;

cpu_if  stage_if (
  .rst       (rst      ),
  .clk       (clk      ),
  .cpu_stall (cpu_stall),
  .stall     (id_stall ),

  .iin       (iin      ),
  .imem_addr (imem_addr), // output

  // pipeline inputs
  .pc_b      (ex_b     ),
  .pc_j      (ex_j     ),
  .b_addr    (ex_baddr ),
  .j_addr    (ex_jaddr ),

  // pipeline outputs
  .p_pc      (if_pc    ),
  .p_inst    (if_inst  ),

  // interrupt signals
  .int       (int      ),
  .int_pc    (int_pc   ),
  .int_ack   (int_ack  ), // output
  .int_flush (int_flush)  // output
);

cpu_id  stage_id (
  .rst            (rst            ),
  .clk            (clk            ),
  .cpu_stall      (cpu_stall      ),

  // pipeline inputs
  .if_pc          (if_pc          ),
  .if_inst        (if_inst        ),
  .wb_rfw         (wb_rfw         ),
  .wb_rf_waddr    (wb_waddr       ),
  .wb_rf_wdata    (wb_wdata       ),

  // interrupt signal
  .int_flush      (int_flush      ),

  // pipeline outputs
  .p_rfa          (id_rfa         ),
  .p_rfb          (id_rfb         ),
  .p_imm32        (id_imm32       ),
  .p_shamt        (id_shamt       ),
  .p_func         (id_func        ),
  .p_rf_waddr     (id_rf_waddr    ),
  .p_c_rfw        (id_c_rfw       ),
  .p_c_wbsource   (id_c_wbsource  ),
  .p_c_drw        (id_c_drw       ),
  .p_c_alucontrol (id_c_alucontrol),
  .p_c_j          (id_c_j         ),
  .p_c_b          (id_c_b         ),
  .p_c_jjr        (id_c_jjr       ),
  .p_jaddr        (id_jaddr       ),
  .p_pc           (id_pc          ),
  .p_c_rfbse      (id_c_rfbse     ),
  .p_rs           (id_rs          ),
  .p_rt           (id_rt          ),

  // stall output (not a reg)
  .c_stall        (id_stall       )
);

cpu_ex  stage_ex (
  .rst             (rst            ),
  .clk             (clk            ),
  .cpu_stall       (cpu_stall      ),

  // pipeline inputs
  .id_c_rfw        (id_c_rfw       ),
  .id_c_wbsource   (id_c_wbsource  ),
  .id_c_drw        (id_c_drw       ),
  .id_c_alucontrol (id_c_alucontrol),
  .id_c_j          (id_c_j         ),
  .id_c_b          (id_c_b         ),
  .id_c_jjr        (id_c_jjr       ),
  .id_rfa          (id_rfa         ),
  .id_rfb          (id_rfb         ),
  .id_imm32        (id_imm32       ),
  .id_shamt        (id_shamt       ),
  .id_func         (id_func        ),
  .id_rf_waddr     (id_rf_waddr    ),
  .id_pc           (id_pc          ),
  .id_jaddr        (id_jaddr       ),
  .id_c_rfbse      (id_c_rfbse     ),
  .id_rs           (id_rs          ),
  .id_rt           (id_rt          ),

  .wb_wdata        (wb_wdata       ),
  .wb_rfw          (wb_rfw         ),
  .wb_waddr        (wb_waddr       ),

  // interrupt signals
  .int_flush       (int_flush      ),
  .int_pc          (int_pc         ), // output

  // pipeline outputs
  .p_c_rfw         (ex_c_rfw       ),
  .p_c_wbsource    (ex_c_wbsource  ),
  .p_c_drw         (ex_c_drw       ),
  .p_alu_r         (ex_alu_r       ),
  .p_rfb           (ex_rfb         ),
  .p_rf_waddr      (ex_rf_waddr    ),
  .p_jalra         (ex_jalra       ),
  .p_rt            (ex_rt          ),

  // outputs
  .baddr           (ex_baddr       ),
  .jaddr           (ex_jaddr       ),
  .c_b             (ex_b           ),
  .c_j             (ex_j           )
);

cpu_mem stage_mem (
  .rst           (rst           ),
  .clk           (clk           ),
  .cpu_stall     (cpu_stall     ),

  // pipeline inputs
  .ex_c_rfw      (ex_c_rfw      ),
  .ex_c_wbsource (ex_c_wbsource ),
  .ex_c_drw      (ex_c_drw      ),
  .ex_alu_r      (ex_alu_r      ),
  .ex_rfb        (ex_rfb        ),
  .ex_rf_waddr   (ex_rf_waddr   ),
  .ex_jalra      (ex_jalra      ),
  .ex_rt         (ex_rt         ),
  .wb_wdata      (wb_wdata      ), // TODO: check the nomenclature on this

  // memory i/o
  .dmem_in       (din           ), // input
  .dmem_data     (dout          ), // output
  .dmem_addr     (dmem_addr     ), // output
  .dmem_drw      (drw           ), // output

  // pipeline outputs
  .p_c_rfw       (mem_c_rfw     ),
  .p_c_wbsource  (mem_c_wbsource),
  .p_alu_r       (mem_alu_r     ),
  .p_rf_waddr    (mem_rf_waddr  ),
  .p_jalra       (mem_jalra     ),
  .p_dout        (mem_dout      )
);

cpu_wb  stage_wb (
  // pipeline inputs
  mem_c_rfw,
  mem_c_wbsource,
  mem_alu_r,
  mem_dout,
  mem_rf_waddr,
  mem_jalra,

  // pipeline outputs
  wb_rfw,
  wb_wdata,
  wb_waddr
);
endmodule
