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
project(cmake_verilator_example)

find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()

find_package(pybind11)
if (NOT pybind11_FOUND)
    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/pybind11)
        add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pybind11)
    else()
        message(FATAL_ERROR "pybind11 not found. Either install it using your system package manager (e.g. conda install pybind11) or download it locally (git clone https://github.com/pybind/pybind11.git).")
    endif()
endif()

pybind11_add_module(example )

# Build 2 versions of the verilog with different parameters
verilate(example PREFIX Vadd1 THREADS 4 TOP_MODULE add PYTHON SOURCES add.v VERILATOR_ARGS -GN=1)
verilate(example PREFIX Vadd2 TOP_MODULE add PYTHON TRACE SOURCES add.v VERILATOR_ARGS -GN=2)


target_sources(example PRIVATE pyexample.cpp)
