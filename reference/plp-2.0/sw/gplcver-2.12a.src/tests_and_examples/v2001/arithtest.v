module test;
 reg signed [8:0] a;
 reg signed [8:0] b;
 reg signed [8:0] result;

 reg [7:0] c;
 reg [7:0] d;
 
 initial begin
  a = -35; 
  b = 5;
  c = 35;
  d = 5;

  result = a / b;
 
  $display("%b", a * b);
  $display("%d", a * b);
  $display("%b", a / b);
  $display("%d", a / b);
  $display("%d", result);
 end

endmodule

