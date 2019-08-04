#include "verilated_py.h"

namespace vl_py {

void declare_globals(pybind11::module& m) {
    m.def("VL_THREAD_ID", &VL_THREAD_ID);
    pybind11::class_<VerilatedMutex>(m, "VerilatedMutex")
        .def("lock", &VerilatedMutex::lock)
        .def("unlock", &VerilatedMutex::unlock);
    pybind11::class_<VerilatedModule>(m, "VerilatedModule")
        .def_property_readonly("name", &VerilatedModule::name);
    pybind11::class_<Verilated>(m, "Verilated")
        .def_property_static("randReset", pybind11::overload_cast<>(&Verilated::randReset), pybind11::overload_cast<int>(&Verilated::randReset))
        .def_property_static("debug", pybind11::overload_cast<>(&Verilated::debug), pybind11::overload_cast<int>(&Verilated::debug))
        .def_property_static("calcUnusedSigs", pybind11::overload_cast<>(&Verilated::calcUnusedSigs), pybind11::overload_cast<bool>(&Verilated::calcUnusedSigs))
        .def_property_static("finished", pybind11::overload_cast<>(&Verilated::gotFinish), pybind11::overload_cast<bool>(&Verilated::gotFinish))
        .def_static("traceEverOn", &Verilated::traceEverOn)
        .def_property_static("assertions", pybind11::overload_cast<>(&Verilated::assertOn), pybind11::overload_cast<bool>(&Verilated::assertOn))
        .def_property_static("vpi_error_fatal", pybind11::overload_cast<>(&Verilated::fatalOnVpiError), pybind11::overload_cast<bool>(&Verilated::fatalOnVpiError))
        .def_property_static("profThreadsStart", pybind11::overload_cast<>(&Verilated::profThreadsStart), pybind11::overload_cast<vluint64_t>(&Verilated::profThreadsStart))
        .def_property_static("profThreadsWindow", pybind11::overload_cast<>(&Verilated::profThreadsWindow), pybind11::overload_cast<vluint64_t>(&Verilated::profThreadsWindow))
        .def_property_static("profThreadsFilenamep", pybind11::overload_cast<>(&Verilated::profThreadsFilenamep), pybind11::overload_cast<const char*>(&Verilated::profThreadsFilenamep))
        .def_static("flushCall", &Verilated::flushCall)
        .def_property_readonly_static("productName", &Verilated::productName)
        .def_property_readonly_static("productVersion", &Verilated::productVersion)
        .def_static("quiesce", &Verilated::quiesce)
        .def_static("internalsDump", &Verilated::internalsDump)
        .def_static("scopesDump", &Verilated::scopesDump);
#ifdef VM_TRACE
#ifdef VM_TRACE_FST
    pybind11::class_<VerilatedFstC>(m, "VerilatedFstC")
        .def(pybind11::init<>())
        .def_property_readonly("isOpen", &VerilatedFstC::isOpen)
        .def("open", &VerilatedFstC::open)
        .def("close", &VerilatedFstC::close)
        .def("flush", &VerilatedFstC::flush)
        .def("dump", pybind11::overload_cast<vluint64_t>(&VerilatedFstC::dump))
        .def("dump", pybind11::overload_cast<double>(&VerilatedFstC::dump))
        .def("set_time_unit", pybind11::overload_cast<const std::string&>(&VerilatedVcdC::set_time_unit))
        .def("set_time_resolution", pybind11::overload_cast<const std::string&>(&VerilatedVcdC::set_time_resolution));
#else
    pybind11::class_<VerilatedVcdC>(m, "VerilatedVcdC")
        .def(pybind11::init<>())
        .def_property_readonly("isOpen", &VerilatedVcdC::isOpen)
        .def("open", &VerilatedVcdC::open)
        .def("openNext", &VerilatedVcdC::openNext)
        .def("rolloverMB", &VerilatedVcdC::rolloverMB)
        .def("close", &VerilatedVcdC::close)
        .def("flush", &VerilatedVcdC::flush)
        .def("dump", pybind11::overload_cast<vluint64_t>(&VerilatedVcdC::dump))
        .def("dump", pybind11::overload_cast<double>(&VerilatedVcdC::dump))
        .def("set_time_unit", pybind11::overload_cast<const std::string&>(&VerilatedVcdC::set_time_unit))
        .def("set_time_resolution", pybind11::overload_cast<const std::string&>(&VerilatedVcdC::set_time_resolution));
#endif
#endif
}

} // namespace vl_py
