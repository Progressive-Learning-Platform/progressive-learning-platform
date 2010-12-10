/* 

David Fritz and Brian Gordon 

VGA module, framebuffer, and signal generator

Memory map:

0xf0400000 - data field (read/write to row and column address specified in row/column registers)
0xf0400004 - row address (used for specifying row instead of flat memory access)
0xf0400008 - column address (used for specifying column instead of flat memory access)
0xf040000c - flat memory access of the vga framebuffer (varying size)

*/

module mod_vga(rst, clk, ie, de, iaddr, daddr, drw, din, iout, dout, rgb, hs, vs);
        input rst;
        input clk;
        input ie,de;
        input [31:0] iaddr, daddr;
        input drw;
        input [31:0] din;
        output [31:0] iout, dout;
        output [7:0] rgb;
	output hs, vs;

        /* by spec, the iout and dout signals must go hiZ when we're not using them */
        wire [31:0] idata, ddata;
        assign iout = ie ? idata : 32'hzzzzzzzz;
        assign dout = de ? ddata : 32'hzzzzzzzz;

        assign idata = 32'h00000000;

	wire [7:0] eff_rgb;
	wire [7:0] eff_dout_fb;
	wire [16:0] screen_addr, eff_daddr;
	wire [10:0] hcount, vcount;
	wire blank, fb_write;
	
	reg [16:0] row, col;
	
	/* the vga controller */
	vga_controller vga(clk, rst, hs, vs, blank, hcount, vcount);

	/* block ram for the vga controller */
	/* first port is for cpu access, second is for vga controller */
	vga_bram vga_bram_t (~clk,~clk,1'b1,1'b1,fb_write,eff_daddr,screen_addr,din[7:0],eff_dout_fb,eff_rgb);

	always @(negedge clk) begin
		if (drw && de) begin
			if (daddr == 32'h00000004) //row
				row = din;
			else if (daddr == 32'h00000008) //col
				col = din;
		end
	end			
	
	assign fb_write = (de && drw && (daddr == 32'h00000000 || daddr => 32'h0000000c) ? 1 : 0;
	assign eff_daddr = (daddr == 32'h00000000) ? (col + (320 * row)) : daddr[16:0];
	assign ddata = (daddr == 32'h00000004) ? {22'b0, row} :
			  (daddr == 32'h00000008) ? {22'b0, col} : {24'b0, eff_dout_fb};
	assign screen_addr = (hcount >> 1) + (320 * (vcount >> 1));
	assign rgb = (blank) ? 0 : eff_rgb;
endmodule

module vga_controller (clk, rst, hs, vs, blank, hcount, vcount);

  input             clk, rst;
  output 	        hs, vs;
  output reg 		  blank;
  output reg [10:0] hcount, vcount;
  reg        [10:0] hcounter, vcounter;
  
  parameter H_FRONTPORCH = 16;
  parameter H_BACKPORCH  = 48;
  parameter H_PULSEWIDTH = 96;
  parameter H_PERIOD     = 800;
  
  parameter V_FRONTPORCH = 10;
  parameter V_BACKPORCH  = 29;
  parameter V_PULSEWIDTH = 2;
  parameter V_PERIOD     = 521;
    
  assign hs = (hcounter < H_PULSEWIDTH) ? 0 : 1;
  assign vs = (vcounter < V_PULSEWIDTH) ? 0 : 1;
   
  always @(negedge clk) begin
	if (rst) begin
		hcount   = 0;
		vcount   = 0;
		hcounter = 0;
		vcounter = 0;
		blank    = 1;
	end
  
    // blank signal
    blank = (hcounter >= H_PULSEWIDTH + H_BACKPORCH && 
             hcounter <  H_PERIOD - H_FRONTPORCH &&
             vcounter >= V_PULSEWIDTH + V_BACKPORCH &&
             vcounter <  V_PERIOD - V_FRONTPORCH)
             ? 0 : 1;
             
    //hcount = (blank) ? 0 : hcount + 1;
    //Vcount = (blank) ? 0 : Vcount + 1;
             
    hcounter = hcounter + 1;
    if (hcounter == H_PERIOD) begin
      hcounter = 0;
      vcounter = (vcounter == V_PERIOD) ? 0 : vcounter + 1;
    end

    hcount = ((hcounter >= H_PULSEWIDTH + H_BACKPORCH) && 
              (hcounter <  H_PERIOD - H_FRONTPORCH))
              ? (hcounter - H_PULSEWIDTH) - H_BACKPORCH : 0;
              
    vcount = ((vcounter >= V_PULSEWIDTH + V_BACKPORCH) &&
              (vcounter <  V_PERIOD - V_FRONTPORCH))
              ? (vcounter - V_PULSEWIDTH) - V_BACKPORCH : 0;
    
  end
endmodule

module vga_bram(clka, clkb, ena, enb, wea, addra, addrb, dia, doa, dob);
        input clka, clkb;
        input wea;
        input ena, enb;
        input [10:0] addra, addrb;
        input [7:0] dia;
        output reg [7:0] doa, dob;
        reg [7:0] RAM [76799:0];

        always @(posedge clka) begin
                if (ena) begin
                        if (wea) begin
                                RAM[addra] <= dia;
                                $display("RAM: %x written to %x", dia, addra);
                        end
                        doa <= RAM[addra];
                end
        end

        always @(posedge clkb) begin
                if (enb) begin
                        dob <= RAM[addrb];
                end
        end
endmodule

