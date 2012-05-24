module top;

 reg [7:0] a;
 reg signed [7:0] b;

 initial begin
  a = $unsigned(-4);
  b = $signed(4'b1100);

  $display("a=%b", a);
  $display("b=%b (%d)", b, b);
 end

endmodule
