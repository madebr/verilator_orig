#!/usr/bin/env python3

# Simple python example of using the add.sv verilog module.
# Can be run in python directly, or pytest

import sys
import os
# Search the build directory
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), "build"))
import example


def test_add1():
    """Example using the Verilated module directly"""
    a = example.Vadd1()
    a.clk = 0
    a.rst = 1
    a.eval()
    a.clk = 1
    a.eval()
    a.clk = 0
    a.rst = 0
    a.eval()
    a.value = 5
    assert a.result == 0
    a.clk = 1
    a.eval()
    assert a.result == 6


def verilator_trace(m):
    """Generate a convenience wrapper around the Verilated module"""
    class VTrace(m):
        def __init__(self):
            super(VTrace, self).__init__()
            self._tfp = None
            self._timestep = 0

        def trace_start(self, filename):
            if self._tfp:
                self.trace_stop()
            self._timestep = 0
            example.Verilated.traceEverOn(True)
            self._tfp = example.VerilatedVcdC()
            self.trace(self._tfp, 99)
            self._tfp.open(filename)

        def trace_stop(self):
            if (self._tfp):
                self._tfp.close()
            self._tfp = None

        def cycle(self):
            self.clk = 0
            self.eval()
            if (self._tfp):
                self._tfp.dump(self._timestep)
                self._timestep += 1
            self.clk = 1
            self.eval()

    return VTrace


# Wrap Vadd2 module
Add2 = verilator_trace(example.Vadd2)


def test_add2():
    """Example using the convenience wrapper"""
    a = Add2()
    # Uncomment to produce trace file
    a.trace_start("test_example.vcd")
    a.rst = 1
    a.cycle()
    a.rst = 0
    a.value = 11
    a.cycle()
    assert a.result == 13
    a.cycle()
    a.trace_stop()


if __name__ == "__main__":
    test_add1()
    test_add2()
    print("All tests passed!")
