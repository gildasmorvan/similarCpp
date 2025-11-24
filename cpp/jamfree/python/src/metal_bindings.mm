/**
 * @file metal_bindings.cpp
 * @brief Python bindings for JamFree Metal GPU acceleration.
 *
 * This file uses pybind11 to expose Metal GPU acceleration to Python.
 * Compiled separately as an Objective-C++ extension.
 */

#ifdef __APPLE__

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../gpu/metal/MetalCompute.h"
#include "../../kernel/include/model/Vehicle.h"

namespace py = pybind11;
using namespace jamfree::gpu::metal;
using namespace jamfree::kernel::model;

PYBIND11_MODULE(_metal, m) {
  m.doc() = "JamFree Metal GPU Acceleration Module";

  // ========================================================================
  // GPUVehicleState
  // ========================================================================
  py::class_<GPUVehicleState>(m, "GPUVehicleState")
      .def(py::init<>(), "Create GPU vehicle state")
      .def_readwrite("position", &GPUVehicleState::position,
                     "Position along lane")
      .def_readwrite("speed", &GPUVehicleState::speed, "Current speed")
      .def_readwrite("acceleration", &GPUVehicleState::acceleration,
                     "Current acceleration")
      .def_readwrite("leader_index", &GPUVehicleState::leader_index,
                     "Index of leader vehicle")
      .def_readwrite("gap", &GPUVehicleState::gap, "Gap to leader")
      .def_readwrite("relative_speed", &GPUVehicleState::relative_speed,
                     "Relative speed to leader");

  // ========================================================================
  // GPUIDMParams
  // ========================================================================
  py::class_<GPUIDMParams>(m, "GPUIDMParams")
      .def(py::init<>(), "Create GPU IDM parameters")
      .def_readwrite("desired_speed", &GPUIDMParams::desired_speed,
                     "Desired speed (m/s)")
      .def_readwrite("time_headway", &GPUIDMParams::time_headway,
                     "Time headway (s)")
      .def_readwrite("min_gap", &GPUIDMParams::min_gap, "Minimum gap (m)")
      .def_readwrite("max_accel", &GPUIDMParams::max_accel,
                     "Maximum acceleration (m/s²)")
      .def_readwrite("comfortable_decel", &GPUIDMParams::comfortable_decel,
                     "Comfortable deceleration (m/s²)")
      .def_readwrite("accel_exponent", &GPUIDMParams::accel_exponent,
                     "Acceleration exponent");

  // ========================================================================
  // MetalCompute
  // ========================================================================
  py::class_<MetalCompute>(m, "MetalCompute")
      .def(py::init<>(), "Create Metal compute engine")
      .def_static("is_available", &MetalCompute::isAvailable,
                  "Check if Metal GPU acceleration is available")
      .def("initialize", &MetalCompute::initialize, py::arg("shader_path") = "",
           "Initialize Metal compute pipeline with optional shader path")
      .def("upload_vehicles", &MetalCompute::uploadVehicles,
           py::arg("vehicles"), "Upload vehicle states to GPU")
      .def("download_vehicles", &MetalCompute::downloadVehicles,
           py::arg("vehicles"), "Download vehicle states from GPU")
      .def("set_idm_params", &MetalCompute::setIDMParams,
           py::arg("desired_speed"), py::arg("time_headway"),
           py::arg("min_gap"), py::arg("max_accel"),
           py::arg("comfortable_decel"), py::arg("accel_exponent"),
           "Set IDM parameters for GPU computation")
      .def("compute_idm_accelerations", &MetalCompute::computeIDMAccelerations,
           py::arg("num_vehicles"), "Compute IDM accelerations on GPU")
      .def("update_positions", &MetalCompute::updatePositions,
           py::arg("num_vehicles"), py::arg("dt"),
           "Update vehicle positions on GPU")
      .def("calculate_gaps", &MetalCompute::calculateGaps,
           py::arg("num_vehicles"), "Calculate gaps and relative speeds on GPU")
      .def("simulation_step", &MetalCompute::simulationStep,
           py::arg("num_vehicles"), py::arg("dt"),
           "Run complete simulation step on GPU (gaps + IDM + update)")
      .def("update_lwr", &MetalCompute::updateLWR, py::arg("density"),
           py::arg("density_new"), py::arg("num_cells"), py::arg("dt"),
           py::arg("dx"), py::arg("free_flow_speed"), py::arg("jam_density"),
           "Update LWR macroscopic model on GPU")
      .def("get_device_name", &MetalCompute::getDeviceName,
           "Get GPU device name")
      .def("get_max_threads_per_threadgroup",
           &MetalCompute::getMaxThreadsPerThreadgroup,
           "Get maximum threads per threadgroup")
      .def("__repr__", [](const MetalCompute &mc) {
        return "MetalCompute(device='" + mc.getDeviceName() + "')";
      });

  // Module-level functions
  m.def("is_metal_available", &MetalCompute::isAvailable,
        "Check if Metal GPU acceleration is available on this system");
}

#else

// Stub module for non-Apple platforms
#include <pybind11/pybind11.h>

namespace py = pybind11;

PYBIND11_MODULE(_metal, m) {
  m.doc() =
      "JamFree Metal GPU Acceleration Module (not available on this platform)";

  m.def(
      "is_metal_available", []() { return false; },
      "Check if Metal GPU acceleration is available (always false on non-Apple "
      "platforms)");
}

#endif // __APPLE__
