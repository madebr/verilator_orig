######################################################################
#
# DESCRIPTION: Verilator CMake example usage
#
# This file shows usage of the cmake script.
# This makefile is here for testing the examples and should
# generally not be added to a cmake project.
#
# Copyright 2003-2019 by Wilson Snyder. This program is free software; you can
# redistribute it and/or modify it under the terms of either the GNU
# Lesser General Public License Version 3 or the Perl Artistic License
# Version 2.0.
#
######################################################################

######################################################################
# Set up variables

# If $VERILATOR_ROOT isn't in the environment, we assume it is part of a
# package install, and verilator is in your path. Otherwise find the
# binary relative to $VERILATOR_ROOT (such as when inside the git sources).

ifeq ($(VERILATOR_ROOT),)
VERILATOR_COVERAGE = verilator_coverage
else
export VERILATOR_ROOT
VERILATOR_COVERAGE = $(VERILATOR_ROOT)/bin/verilator_coverage
endif
######################################################################

# Check if cmake is installed and of correct version
ifeq ($(shell which cmake),)
	TARGET := nocmake
else
CMAKE_VERSION := $(shell cmake --version | grep -Po '(\d[\.\d]+)')
CMAKE_MAJOR := $(shell echo $(CMAKE_VERSION) | cut -f1 -d.)
CMAKE_MINOR := $(shell echo $(CMAKE_VERSION) | cut -f2 -d.)
CMAKE_GT_3_8 := $(shell [ $(CMAKE_MAJOR) -gt 3 -o \( $(CMAKE_MAJOR) -eq 3 -a $(CMAKE_MINOR) -ge 8 \) ] && echo true)
ifeq ($(CMAKE_GT_3_8),true)
FINDSC := $(shell cmake --find-package -DNAME=SystemCLanguage -DCMAKE_USE_PTHREADS_INIT=ON -DCOMPILER_ID=GNU -DLANGUAGE=CXX -DMODE=EXIST -DThreads_FOUND=ON)
ifneq (,$(findstring SystemCLanguage found,$(FINDSC)))
TARGET := run
else
TARGET := nosc
endif
else
TARGET := oldcmake
endif
endif

default: $(TARGET)

run:
	@echo
	@echo "-- Verilator cmake systemc tracing example"

	@echo
	@echo "-- CMake ----------------"
	cd build && cmake ..

	@echo
	@echo "-- COMPILE -----------------"
	cmake --build build

	@echo
	@echo "-- RUN ---------------------"
	@mkdir -p logs
	build/example +trace

	@echo
	@echo "-- COVERAGE ----------------"
	$(VERILATOR_COVERAGE) --annotate logs/annotated logs/coverage.dat

	@echo
	@echo "-- DONE --------------------"
	@echo "To see waveforms, open vlt_dump.vcd in a waveform viewer"
	@echo

clean:
	@rm -rf build && mkdir build

nocmake:
	@echo
	@echo "%Skip: cmake has not been found"
	@echo

oldcmake:
	@echo
	@echo "%Skip: cmake version is too old (need at least 3.8)"
	@echo

nosc:
	@echo
	@echo "%Skip: cmake could not find systemc. Please compile and install systemc with cmake support."
	@echo
