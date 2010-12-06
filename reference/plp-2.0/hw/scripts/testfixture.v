module test;

        reg clk;
        wire [7:0] leds;
        reg rst,rxd;
        wire txd;
        reg [7:0] switches;

	top uut(clk,leds,rst,txd,rxd,switches);

	always #1 clk = ~clk;

//	always @(posedge clk)
//		$display("-----------------------------------------");

	initial begin 
		clk = 0;
		rst = 1;
		#10
		rst = 0;
		#1000
		$display("done");
		$stop;
	end
endmodule
