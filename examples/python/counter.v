// DESCRIPTION: Verilator: Verilog parameterized test module
//
// This file ONLY is placed into the Public Domain, for any use,
// without warranty, 2019 by Wilson Snyder.
// ======================================================================

// Increments on each posedge of clk

module counter(
    input wire clk,
    input wire rst,
    output reg [15:0] out
);

always @(posedge clk) begin
    if (rst) begin
        out <= 0;
    end else begin
        out <= out + 1;
    end
end

endmodule
