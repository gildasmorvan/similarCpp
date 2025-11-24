#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "LevelIdentifier.h"
#include "SimulationTimeStamp.h"
#include "kernel/tools/Point2D.h"

namespace py = pybind11;

using namespace fr::univ_artois::lgi2a::similar;
using namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel;

PYBIND11_MODULE(_core, m) {
  m.doc() = "SIMILAR2Logo Python Bindings - Minimal Core Module";

  // Point2D - basic 2D point
  py::class_<tools::Point2D>(m, "Point2D")
      .def(py::init<double, double>())
      .def_readwrite("x", &tools::Point2D::x)
      .def_readwrite("y", &tools::Point2D::y)
      .def("__repr__", [](const tools::Point2D &p) {
        return "Point2D(" + std::to_string(p.x) + ", " + std::to_string(p.y) +
               ")";
      });

  // SimulationTimeStamp
  py::class_<microkernel::SimulationTimeStamp>(m, "SimulationTimeStamp")
      .def(py::init<long>())
      .def("get_identifier", &microkernel::SimulationTimeStamp::getIdentifier);

  // LevelIdentifier
  py::class_<microkernel::LevelIdentifier>(m, "LevelIdentifier")
      .def(py::init<const std::string &>())
      .def("__repr__", &microkernel::LevelIdentifier::toString);

  m.attr("__version__") = "0.1.0";
}
