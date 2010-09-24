/*
Behavioral MIPS-like register file

David Fritz

1.14.2009
*/

module registerfile(raddr1,raddr2,waddr,wdata,w,rdata1,rdata2,clk);
	input [4:0] raddr1,raddr2,waddr; /* address lines for reading/writing */
	input [31:0] wdata; /* write data bus */
	input w; /* write control: 0=no-write 1=write */
	input clk; /* clock */
	output [31:0] rdata1,rdata2; /* read data buses */

	/* the registers of the regfile */
	reg [31:0] rfile [31:1]; /* the zero row is the "zero register" */

	/* we want the outputs to not be registered, ie mealy, so we use assign statements for them */
	assign rdata1 = raddr1 == 0 ? 32'd0 : rfile[raddr1];
	assign rdata2 = raddr2 == 0 ? 32'd0 : rfile[raddr2];

	always @(posedge clk) begin
		if (w && waddr != 5'd0) begin
			rfile[waddr] <= wdata;
		end
	end
endmodule
