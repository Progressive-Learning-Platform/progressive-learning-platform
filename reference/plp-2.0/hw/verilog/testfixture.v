module test;

        reg clk;
        wire [7:0] leds;
        reg rst,rxd;
        wire txd;
        reg [3:0] switches;
        wire [2:0] rgb;
        wire hs,vs;
        reg [7:0] gpi;
	
	top uut(clk,leds,rst,txd,rxd,switches,rgb,hs,vs,gpi);

	always #1 clk = ~clk;

	initial begin 
		clk = 0;
		rst = 1;
		#10
		rst = 0;
		#30
		$display("done");
	end
endmodule
