/*
    Button Debounce module for Progressive Learning Platform
    Copyright 2012 Matthew Gaalswyk

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

module debounce(
  input  clk,
  input  i_button,
  output o_signal
  );

parameter MIN_DELAY = 50; // minimum number of cycles to count
parameter W_COUNTER = 3;
reg [W_COUNTER:0] counter; // one bit wider than min_delay

assign o_signal = counter[W_COUNTER];

always @(posedge clk) begin
  if (!i_button) begin
    // if button sensor is showing 'off' state, reset the counter
    counter <= 0;
  end else if (!counter[W_COUNTER]) begin
    counter <= counter + 1'b1;
  end
end

endmodule
