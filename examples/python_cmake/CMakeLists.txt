######################################################################
#
# DESCRIPTION: Verilator Python Example: CMakeLists.txt
#
# This is an example cmake script to build a verilog to Python project
# using CMake, Verilator and pybind11.
#
# Copyright 2003-2019 by Wilson Snyder. This program is free software; you can
# redistribute it and/or modify it under the terms of either the GNU
# Lesser General Public License Version 3 or the Perl Artistic License
# Version 2.0.
#
######################################################################

cmake_minimum_required(VERSION 3.8)
project(python_cmake_example)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()

verilator_add_python_module(example MAIN DOC "An example Python module")
verilate(example PREFIX Vadd THREADS 4 TOP_MODULE add PYTHON SOURCES add.v VERILATOR_ARGS -GN=1)

verilator_add_python_module(example2 MAIN DOC "An example Python module2")
verilate(example2 PREFIX Vadd TOP_MODULE add PYTHON TRACE SOURCES add.v VERILATOR_ARGS -GN=2)

verilator_add_python_module(example3 MAIN DOC "An example Python module3")
verilate(example3 PREFIX Vadd TOP_MODULE add PYTHON TRACE_FST SOURCES add.v VERILATOR_ARGS -GN=3)
verilate(example3 PREFIX Vadd2 TOP_MODULE add PYTHON TRACE SOURCES add.v VERILATOR_ARGS -GN=4
    OPT_SLOW -O0 -DSLOW OPT_FAST -O3 -DFAST)
