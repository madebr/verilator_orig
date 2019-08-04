// -*- mode: C++; c-file-style: "cc-mode" -*-
//*************************************************************************
//
// Copyright 2003-2019 by Wilson Snyder. This program is free software; you can
// redistribute it and/or modify it under the terms of either the GNU
// Lesser General Public License Version 3 or the Perl Artistic License.
// Version 2.0.
//
// Verilator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//*************************************************************************
///
/// \file
/// \brief Verilator: Common include for verilator python wrappers
///
/// Code available from: http://www.veripool.org/verilator
///
//*************************************************************************


#ifndef _VERILATED_PY_H_
#define _VERILATED_PY_H_ 1 ///< Header Guard

#include "verilated.h"
#include <pybind11/pybind11.h>
#ifdef VM_TRACE
#ifdef VM_TRACE_FST
#include "verilated_fst_c.h"
#else
#include "verilated_vcd_c.h"
#endif
#endif

namespace vl_py {

void declare_globals(pybind11::module& m);

} // namespace vl_py

#ifndef VL_PY_MODULE
#define VL_PY_MODULE(m, t) pybind11::class_<t, VerilatedModule>(m, #t) \
                            .def(pybind11::init<const char*>(), pybind11::arg("name") = "TOP")
#endif
#ifndef VL_PY_PORT
#define VL_PY_PORT(n, p) .def_readwrite(#p, &n::p)
#endif
#ifndef VL_PY_PARAM
#define VL_PY_PARAM(n, p) .def_readonly(#p, &n::p)
#endif
#ifndef VL_PY_FUNC
#define VL_PY_FUNC(n, f) .def(#f, &n::f)
#endif
#ifndef VL_PY_FUNC_STATIC
#define VL_PY_FUNC_STATIC(n, f) .def_static(#f, &n::f)
#endif
#ifndef VL_PY_FUNC_TRACE
#define VL_PY_FUNC_TRACE(n) .def("trace", &n::trace, pybind11::arg("tfp"), pybind11::arg("levels"), pybind11::arg("options") = 0)
#endif
#ifndef VL_PY_VAR
#define VL_PY_VAR(n, v) .def_readwrite(#v, &n::v)
#endif

#endif /*_VERILATED_PY_H_*/
