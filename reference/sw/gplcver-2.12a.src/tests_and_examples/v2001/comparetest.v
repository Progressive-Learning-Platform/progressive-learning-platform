module test;
 reg signed [7:0] a;
 reg signed [7:0] b;
 reg signed [7:0] result;

 reg [7:0] c;
 reg [7:0] d;
 
 initial begin
  a = -2; 
  b = -50;

  c = 50;

  if (a < b) 
   $display("%d < %d", a, b);
  if (a > b) 
   $display("%d > %d", a, b);
  if (b < a) 
   $display("%d < %d", b, a);
  if (b > a) 
   $display("%d > %d", b, a);
 
  if (a <= b) 
   $display("%d <= %d", a, b);
  if (a >= b) 
   $display("%d >= %d", a, b);
  if (b <= a) 
   $display("%d <= %d", b, a);
  if (b >= a) 
   $display("%d >= %d", b, a);
 
  if (a < c) 
   $display("%d < %d", a, c);
  if (a > c) 
   $display("%d > %d", a, c);
  if (c < a) 
   $display("%d < %d", c, a);
  if (c > a) 
   $display("%d > %d", c, a);
 end

endmodule

