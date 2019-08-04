// DESCRIPTION: Verilator: Verilog parameterized test module
//
// This file ONLY is placed into the Public Domain, for any use,
// without warranty, 2019 by Wilson Snyder.
// ======================================================================

// Adds a parameterised constant to an int

module add #(
    parameter integer N = 5
)(
    input wire rst,
    input wire clk,
    input integer value,
    output integer result,
    output wire [15:0] out
);

always @(posedge clk) begin
    if (rst) begin
        result <= 0;
    end else begin
        result <= value + N;
    end
end

counter ctr(.clk(clk), .rst(rst), .out(out));

endmodule

