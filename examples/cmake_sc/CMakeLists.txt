######################################################################
#
# DESCRIPTION: Verilator Example: Small CMakeLists.txt
#
# This is an example cmake script to build a verilog to systemc project
# using cmake and verilator.
#
# Copyright 2003-2019 by Wilson Snyder. This program is free software; you can
# redistribute it and/or modify it under the terms of either the GNU
# Lesser General Public License Version 3 or the Perl Artistic License
# Version 2.0.
#
######################################################################

# This example builds the tracing_sc example using CMake
# To use it, run the following:

# cd /path/to/verilator/examples/cmake_sc
# rm -rf build && mkdir build && cd build
# cmake ..
# cmake --build .

cmake_minimum_required(VERSION 3.8)
project(cmake_sc_example)

find_package(verilator HINTS $ENV{VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Find SystemC using SystemC's CMake integration
find_package(SystemCLanguage QUIET)

if (NOT SystemCLanguage_FOUND)
    # Find SystemC using include and library paths
    find_path(SYSTEMC_INCLUDE NAMES systemc.h
        HINTS "${SYSTEMC_ROOT}" ENV SYSTEMC_ROOT
        PATH_SUFFIXES include)
    find_library(SYSTEMC_LIBRARY NAMES systemc
        HINTS "${SYSTEMC_ROOT}" ENV SYSTEMC_ROOT
        PATH_SUFFIXES lib)

    # Fail if no include or library path is found
    if(NOT SYSTEMC_INCLUDE OR NOT SYSTEMC_LIBRARY)
        message(FATAL_ERROR "SystemC was not found.")
    endif()

    add_library(SystemC::systemc INTERFACE IMPORTED)
    set_target_properties(SystemC::systemc
        PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${SYSTEMC_INCLUDE}"
            INTERFACE_LINK_LIBRARIES "${SYSTEMC_LIBRARY}")
endif()

# Create a new executable target that will contain all your sources
add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/../tracing_sc/sc_main.cpp)

# Add the Verilated circuit to the target
verilate(example SYSTEMC COVERAGE TRACE
    INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../tracing_sc"
    VERILATOR_ARGS -f ${CMAKE_CURRENT_SOURCE_DIR}/../tracing_sc/input.vc -O2 -x-assign 0
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../tracing_sc/top.v)

target_link_libraries(example PUBLIC SystemC::systemc)
