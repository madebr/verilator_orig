######################################################################
#
# DESCRIPTION: Verilator CMake Example: Small CMakeLists.txt
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

# This example builds the tracing_c example using CMake
# To use it, run the following:

# cd /path/to/verilator/examples/cmake_c
# rm -rf build && mkdir build && cd build
# cmake ..
# cmake --build .

cmake_minimum_required(VERSION 3.12)
project(cmake_protect_lib)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
  message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()

# Create a shared and static secret library
add_library(verilated_secret_obj  OBJECT)
set_property(TARGET verilated_secret_obj PROPERTY POSITION_INDEPENDENT_CODE 1)
add_library(verilated_secret_static STATIC  $<TARGET_OBJECTS:verilated_secret_obj>)
set_target_properties(verilated_secret_static PROPERTIES OUTPUT_NAME verilated_secret)
add_library(verilated_secret SHARED  $<TARGET_OBJECTS:verilated_secret_obj>)
# Or if you know which you want just use
# add_library(verilated_secret STATIC) # or SHARED
# and change verilated_secret_obj to verilated_secret below

# Create a new executable target that will contain all your sources
add_executable(example ../make_protect_lib/sim_main.cpp)
target_link_libraries(example PRIVATE verilated_secret_static)

# Setup random seed
verilator_generate_key(KEY_INIT)
set(PROTECT_KEY ${KEY_INIT} CACHE STRING "Random seed for protection")
# Add the Verilated modules to the targets
verilate(verilated_secret_obj
  VERILATOR_ARGS --protect-lib verilated_secret
                 --protect-key ${PROTECT_KEY}
  SOURCES ../make_protect_lib/secret_impl.v)

# Include location of verilated_secret.sv wrapper
get_target_property(INCDIRS verilated_secret_obj INCLUDE_DIRECTORIES)
list(TRANSFORM INCDIRS PREPEND -I)
verilate(example
  VERILATOR_ARGS ${INCDIRS}
  SOURCES ../make_protect_lib/top.v)
