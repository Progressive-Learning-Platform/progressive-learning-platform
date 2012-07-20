module debounce(
  input  clk,
  input  button_i,
  output button_debounced_o
  );

parameter MIN_DELAY = 50; // minimum number of cycles to count
parameter W_COUNTER = 3;
reg [W_COUNTER:0] counter; // one bit wider than min_delay

assign button_debounced_o = counter[W_COUNTER];

always @(posedge clk) begin
  if (!button_i) begin
    // if button sensor is showing 'off' state, reset the counter
    counter <= 0;
  end else if (!counter[W_COUNTER]) begin
    counter <= counter + 1'b1;
  end
end

endmodule
