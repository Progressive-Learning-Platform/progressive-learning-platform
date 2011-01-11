/* 

David Fritz and Brian Gordon 

VGA module, framebuffer, and signal generator

Memory map:

0xf0400000 - control register
0xf0400004 - frame buffer pointer

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
	wire [10:0] hcount, vcount;
	wire blank, fb_write;

	reg enable;
	reg [31:0] fb_pointer;
	
	/* the vga controller */
	vga_controller vga(clk, rst, hs, vs, blank, hcount, vcount);
	vga_sram_bypass (clk, fb_pointer, hcount, vcount, eff_rgb);

	always @(negedge clk) begin
		if (drw && de) begin
			if (daddr == 32'h00000000)
				enable <= din[0];
			else if (daddr = 32'h00000004)
				fb_pointer <= din;
		end
	end			

	assign rgb = (blank) ? 0 : 
		     (enable) ? eff_rgb: 0;
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

module vga_sram_bypass (clk, fb_addr, hcount, vcount, rgb);
	input clk;
	input [31:0] fb_addr;
	input [10:0] hcount, vcount;
	output [7:0] rgb;

	reg [7:0] buffer [639:0]; /* our buffer */
	
	/* we use hcount to index into the buffer */
	assign rgb = buffer[hcount];

	
endmodule
