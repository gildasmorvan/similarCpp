/**
 * @file bindings.cpp
 * @brief Python bindings for JamFree C++ traffic simulation library.
 *
 * This file uses pybind11 to expose JamFree C++ classes and functions to
 * Python.
 */

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../hybrid/include/AdaptiveSimulator.h"
#include "../../kernel/include/model/Lane.h"
#include "../../kernel/include/model/Point2D.h"
#include "../../kernel/include/model/Road.h"
#include "../../kernel/include/model/SpatialIndex.h"
#include "../../kernel/include/model/Vehicle.h"
#include "../../kernel/include/tools/FastMath.h"
#include "../../macroscopic/include/CTM.h"
#include "../../macroscopic/include/LWR.h"
#include "../../macroscopic/include/MicroMacroBridge.h"
#include "../../microscopic/include/IDM.h"
#include "../../microscopic/include/IDMLookup.h"
#include "../../microscopic/include/MOBIL.h"
#include "../../realdata/include/OSMParser.h"

namespace py = pybind11;
using namespace jamfree;
using namespace jamfree::kernel::model;
using namespace jamfree::kernel::tools;
using namespace jamfree::microscopic::models;
using namespace jamfree::macroscopic::models;
using namespace jamfree::realdata::osm;
using namespace jamfree::hybrid;

#include "../../../microkernel/include/engine/MultiThreadedSimulationEngine.h"
#include "../../../microkernel/include/engine/SequentialSimulationEngine.h"
#include "../../kernel/include/agents/VehicleAgent.h"
#include "../../kernel/include/simulation/TrafficSimulationModel.h"
#include "../../microscopic/include/agents/VehiclePrivateLocalStateMicro.h"
#include "../../microscopic/include/agents/VehiclePublicLocalStateMicro.h"
#include "../../microscopic/include/decision/VehicleDecisionModelMicro.h"
#include "../../microscopic/include/decision/dms/ConjunctionDMS.h"
#include "../../microscopic/include/decision/dms/ForwardAccelerationDMS.h"
#include "../../microscopic/include/decision/dms/LaneChangeDMS.h"
#include "../../microscopic/include/decision/dms/SubsumptionDMS.h"
#include "../../microscopic/include/perception/VehiclePerceptionModelMicro.h"

using namespace jamfree::kernel::agents;

PYBIND11_MODULE(_jamfree, m) {
  m.doc() = "JamFree: Traffic simulation library with microscopic models";

  // ========================================================================
  // Point2D
  // ========================================================================
  py::class_<Point2D>(m, "Point2D")
      .def(py::init<double, double>(), py::arg("x") = 0.0, py::arg("y") = 0.0,
           "Create a 2D point")
      .def_readwrite("x", &Point2D::x, "X coordinate")
      .def_readwrite("y", &Point2D::y, "Y coordinate")
      .def("distance_to", &Point2D::distanceTo, py::arg("other"),
           "Calculate distance to another point")
      .def("dot", &Point2D::dot, py::arg("other"),
           "Dot product with another point")
      .def("magnitude", &Point2D::magnitude, "Magnitude (length) of the vector")
      .def("normalized", &Point2D::normalized, "Return normalized vector")
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self * double())
      .def("__repr__", [](const Point2D &p) {
        return "Point2D(" + std::to_string(p.x) + ", " + std::to_string(p.y) +
               ")";
      });

  // ========================================================================
  // Lane
  // ========================================================================
  py::class_<Lane, std::shared_ptr<Lane>>(m, "Lane")
      .def(py::init<const std::string &, int, double, double>(), py::arg("id"),
           py::arg("index"), py::arg("width"), py::arg("length"),
           "Create a lane")
      .def("get_id", &Lane::getId, "Get lane ID")
      .def("get_index", &Lane::getIndex, "Get lane index")
      .def("get_width", &Lane::getWidth, "Get lane width")
      .def("get_length", &Lane::getLength, "Get lane length")
      .def("get_speed_limit", &Lane::getSpeedLimit, "Get speed limit")
      .def("set_speed_limit", &Lane::setSpeedLimit, py::arg("speed_limit"),
           "Set speed limit")
      .def("get_position_at", &Lane::getPositionAt, py::arg("distance"),
           "Get position at distance along lane")
      .def("get_heading_at", &Lane::getHeadingAt, py::arg("distance"),
           "Get heading at distance along lane")
      .def("add_vehicle", &Lane::addVehicle, py::arg("vehicle"),
           "Add vehicle to lane")
      .def("remove_vehicle", &Lane::removeVehicle, py::arg("vehicle"),
           "Remove vehicle from lane")
      .def("get_vehicles", &Lane::getVehicles, "Get all vehicles in lane")
      .def("get_leader", &Lane::getLeader, py::arg("vehicle"),
           "Get leader vehicle", py::return_value_policy::reference)
      .def("get_follower", &Lane::getFollower, py::arg("vehicle"),
           "Get follower vehicle", py::return_value_policy::reference)
      .def("get_parent_road", &Lane::getParentRoad, "Get parent road",
           py::return_value_policy::reference)
      .def("__repr__", [](const Lane &l) {
        return "Lane(id='" + l.getId() +
               "', index=" + std::to_string(l.getIndex()) + ")";
      });

  // ========================================================================
  // Road
  // ========================================================================
  py::class_<Road, std::shared_ptr<Road>>(m, "Road")
      .def(py::init<const std::string &, const Point2D &, const Point2D &, int,
                    double>(),
           py::arg("id"), py::arg("start"), py::arg("end"),
           py::arg("num_lanes"), py::arg("lane_width"), "Create a road")
      .def("get_id", &Road::getId, "Get road ID")
      .def("get_start", &Road::getStart, "Get start point")
      .def("get_end", &Road::getEnd, "Get end point")
      .def("get_length", &Road::getLength, "Get road length")
      .def("get_num_lanes", &Road::getNumLanes, "Get number of lanes")
      .def("get_lane", &Road::getLane, py::arg("index"), "Get lane by index")
      .def("get_position_at", &Road::getPositionAt, py::arg("distance"),
           "Get position at distance along road")
      .def("get_heading_at", &Road::getHeadingAt, py::arg("distance"),
           "Get heading at distance along road")
      .def("__repr__", [](const Road &r) {
        return "Road(id='" + r.getId() +
               "', length=" + std::to_string(r.getLength()) +
               "m, lanes=" + std::to_string(r.getNumLanes()) + ")";
      });

  // ========================================================================
  // Vehicle
  // ========================================================================
  py::class_<Vehicle, std::shared_ptr<Vehicle>>(m, "Vehicle")
      .def(py::init<const std::string &, double, double, double, double>(),
           py::arg("id"), py::arg("length") = 5.0, py::arg("max_speed") = 55.0,
           py::arg("max_accel") = 3.0, py::arg("max_decel") = 6.0,
           "Create a vehicle")
      .def("get_id", &Vehicle::getId, "Get vehicle ID")
      .def("get_length", &Vehicle::getLength, "Get vehicle length")
      .def("get_width", &Vehicle::getWidth, "Get vehicle width")
      .def("get_position", &Vehicle::getPosition, "Get 2D position")
      .def("get_speed", &Vehicle::getSpeed, "Get current speed")
      .def("get_acceleration", &Vehicle::getAcceleration,
           "Get current acceleration")
      .def("get_heading", &Vehicle::getHeading, "Get heading (radians)")
      .def("get_lane_position", &Vehicle::getLanePosition,
           "Get position along lane")
      .def("get_current_lane", &Vehicle::getCurrentLane, "Get current lane")
      .def("set_speed", &Vehicle::setSpeed, py::arg("speed"), "Set speed")
      .def("set_lane_position", &Vehicle::setLanePosition, py::arg("position"),
           "Set position along lane")
      .def("set_current_lane", &Vehicle::setCurrentLane, py::arg("lane"),
           "Set current lane")
      .def("update", &Vehicle::update, py::arg("dt"), py::arg("acceleration"),
           "Update vehicle state for one time step")
      .def("get_gap_to", &Vehicle::getGapTo, py::arg("leader"),
           "Get gap to vehicle ahead")
      .def("get_relative_speed_to", &Vehicle::getRelativeSpeedTo,
           py::arg("leader"), "Get relative speed to vehicle ahead")
      .def("is_stopped", &Vehicle::isStopped, py::arg("threshold") = 0.1,
           "Check if vehicle is stopped")
      .def("__repr__", [](const Vehicle &v) {
        return "Vehicle(id='" + v.getId() +
               "', speed=" + std::to_string(v.getSpeed() * 3.6) + " km/h)";
      });

  // ========================================================================
  // IDM (Intelligent Driver Model)
  // ========================================================================
  py::class_<IDM, std::shared_ptr<IDM>>(m, "IDM")
      .def(py::init<double, double, double, double, double, double>(),
           py::arg("desired_speed") = 33.3, py::arg("time_headway") = 1.5,
           py::arg("min_gap") = 2.0, py::arg("max_accel") = 1.0,
           py::arg("comfortable_decel") = 1.5, py::arg("accel_exponent") = 4.0,
           "Create IDM car-following model")
      .def(
          "calculate_acceleration",
          [](const IDM &idm, const Vehicle &vehicle, const Vehicle *leader) {
            return idm.calculateAcceleration(vehicle, leader);
          },
          py::arg("vehicle"), py::arg("leader") = nullptr,
          "Calculate acceleration for a vehicle")
      .def("calculate_desired_gap", &IDM::calculateDesiredGap, py::arg("speed"),
           py::arg("speed_diff"), "Calculate desired gap")
      .def("get_desired_speed", &IDM::getDesiredSpeed, "Get desired speed")
      .def("get_time_headway", &IDM::getTimeHeadway, "Get time headway")
      .def("get_min_gap", &IDM::getMinGap, "Get minimum gap")
      .def("get_max_accel", &IDM::getMaxAccel, "Get maximum acceleration")
      .def("get_comfortable_decel", &IDM::getComfortableDecel,
           "Get comfortable deceleration")
      .def("set_desired_speed", &IDM::setDesiredSpeed, py::arg("v0"),
           "Set desired speed")
      .def("set_time_headway", &IDM::setTimeHeadway, py::arg("T"),
           "Set time headway")
      .def("__repr__", [](const IDM &idm) {
        return "IDM(v0=" + std::to_string(idm.getDesiredSpeed() * 3.6) +
               " km/h, T=" + std::to_string(idm.getTimeHeadway()) + "s)";
      });

  // ========================================================================
  // IDMPlus (Enhanced IDM)
  // ========================================================================
  py::class_<IDMPlus, IDM, std::shared_ptr<IDMPlus>>(m, "IDMPlus")
      .def(py::init<double, double, double, double, double, double>(),
           py::arg("desired_speed") = 33.3, py::arg("time_headway") = 1.5,
           py::arg("min_gap") = 2.0, py::arg("max_accel") = 1.0,
           py::arg("comfortable_decel") = 1.5, py::arg("accel_exponent") = 4.0,
           "Create IDM+ car-following model with improved emergency braking")
      .def("__repr__", [](const IDMPlus &idm) {
        return "IDMPlus(v0=" + std::to_string(idm.getDesiredSpeed() * 3.6) +
               " km/h, T=" + std::to_string(idm.getTimeHeadway()) + "s)";
      });

  // ========================================================================
  // MOBIL (Lane-Changing Model)
  // ========================================================================
  py::enum_<MOBIL::Direction>(m, "LaneChangeDirection")
      .value("NONE", MOBIL::Direction::NONE, "No lane change")
      .value("LEFT", MOBIL::Direction::LEFT, "Change to left lane")
      .value("RIGHT", MOBIL::Direction::RIGHT, "Change to right lane")
      .export_values();

  py::class_<MOBIL, std::shared_ptr<MOBIL>>(m, "MOBIL")
      .def(py::init<double, double, double, double>(),
           py::arg("politeness") = 0.5, py::arg("threshold") = 0.1,
           py::arg("max_safe_decel") = 4.0, py::arg("bias_right") = 0.3,
           "Create MOBIL lane-changing model")
      .def(
          "decide_lane_change",
          [](const MOBIL &mobil, const Vehicle &vehicle,
             const Lane &current_lane, Lane *left_lane, Lane *right_lane,
             const IDM &cf_model) {
            return mobil.decideLaneChange(vehicle, current_lane, left_lane,
                                          right_lane, cf_model);
          },
          py::arg("vehicle"), py::arg("current_lane"), py::arg("left_lane"),
          py::arg("right_lane"), py::arg("car_following_model"),
          "Decide whether to change lanes")
      .def("__repr__", [](const MOBIL &mobil) {
        return "MOBIL(politeness=0.5, threshold=0.1)";
      });

  // ========================================================================
  // OSM (OpenStreetMap) Support
  // ========================================================================
  py::class_<RoadNetwork>(m, "RoadNetwork")
      .def(py::init<>(), "Create empty road network")
      .def_readonly("roads", &RoadNetwork::roads, "List of roads")
      .def_readonly("min_lat", &RoadNetwork::min_lat, "Minimum latitude")
      .def_readonly("max_lat", &RoadNetwork::max_lat, "Maximum latitude")
      .def_readonly("min_lon", &RoadNetwork::min_lon, "Minimum longitude")
      .def_readonly("max_lon", &RoadNetwork::max_lon, "Maximum longitude")
      .def("__repr__", [](const RoadNetwork &net) {
        return "RoadNetwork(roads=" + std::to_string(net.roads.size()) + ")";
      });

  py::class_<OSMParser>(m, "OSMParser")
      .def_static("parse_file", &OSMParser::parseFile, py::arg("filename"),
                  "Parse OSM XML file and return road network")
      .def_static("parse_string", &OSMParser::parseString,
                  py::arg("xml_content"),
                  "Parse OSM XML string and return road network")
      .def_static("lat_lon_to_meters", &OSMParser::latLonToMeters,
                  py::arg("lat"), py::arg("lon"), py::arg("center_lat"),
                  py::arg("center_lon"), "Convert lat/lon to local meters")
      .def_static("meters_to_lat_lon", &OSMParser::metersToLatLon, py::arg("x"),
                  py::arg("y"), py::arg("center_lat"), py::arg("center_lon"),
                  "Convert local meters to lat/lon")
      .def_static("get_default_lanes", &OSMParser::getDefaultLanes,
                  py::arg("highway_type"),
                  "Get default lane count for highway type")
      .def_static("get_default_speed_limit", &OSMParser::getDefaultSpeedLimit,
                  py::arg("highway_type"), py::arg("country") = "FR",
                  "Get default speed limit for highway type");

  // ========================================================================
  // Macroscopic Models
  // ========================================================================

  // LWR Model
  py::class_<LWR>(m, "LWR")
      .def(py::init<double, double, double, int>(),
           py::arg("free_flow_speed") = 33.3, py::arg("jam_density") = 0.15,
           py::arg("road_length") = 1000.0, py::arg("num_cells") = 100,
           "Create LWR macroscopic model")
      .def("update", &LWR::update, py::arg("dt"), "Update traffic state")
      .def("set_density", &LWR::setDensity, py::arg("cell_index"),
           py::arg("density"), "Set density at cell")
      .def("get_density", &LWR::getDensity, py::arg("cell_index"),
           "Get density at cell")
      .def("get_flow", &LWR::getFlow, py::arg("cell_index"), "Get flow at cell")
      .def("get_speed", &LWR::getSpeed, py::arg("cell_index"),
           "Get speed at cell")
      .def("speed_from_density", &LWR::speedFromDensity, py::arg("density"),
           "Calculate speed from density")
      .def("flow_from_density", &LWR::flowFromDensity, py::arg("density"),
           "Calculate flow from density")
      .def("get_critical_density", &LWR::getCriticalDensity,
           "Get critical density")
      .def("get_max_flow", &LWR::getMaxFlow, "Get maximum flow")
      .def("get_num_cells", &LWR::getNumCells, "Get number of cells")
      .def("get_road_length", &LWR::getRoadLength, "Get road length")
      .def("get_cell_length", &LWR::getCellLength, "Get cell length")
      .def("get_free_flow_speed", &LWR::getFreeFlowSpeed, "Get free-flow speed")
      .def("get_jam_density", &LWR::getJamDensity, "Get jam density")
      .def("get_densities", &LWR::getDensities, "Get all densities")
      .def("__repr__", [](const LWR &lwr) {
        return "LWR(cells=" + std::to_string(lwr.getNumCells()) +
               ", length=" + std::to_string(lwr.getRoadLength()) + "m)";
      });

  // CTM Model
  py::class_<CTM>(m, "CTM")
      .def(py::init<double, double, double, double, int>(),
           py::arg("free_flow_speed") = 33.3, py::arg("wave_speed") = 5.56,
           py::arg("jam_density") = 0.15, py::arg("road_length") = 1000.0,
           py::arg("num_cells") = 100, "Create CTM macroscopic model")
      .def("update", &CTM::update, py::arg("dt"), "Update traffic state")
      .def("set_num_vehicles", &CTM::setNumVehicles, py::arg("cell_index"),
           py::arg("num_vehicles"), "Set number of vehicles in cell")
      .def("get_num_vehicles", &CTM::getNumVehicles, py::arg("cell_index"),
           "Get number of vehicles in cell")
      .def("get_density", &CTM::getDensity, py::arg("cell_index"),
           "Get density at cell")
      .def("get_flow", &CTM::getFlow, py::arg("cell_index"), "Get flow at cell")
      .def("get_speed", &CTM::getSpeed, py::arg("cell_index"),
           "Get speed at cell")
      .def("sending_flow", &CTM::sendingFlow, py::arg("num_vehicles"),
           py::arg("dt"), "Calculate sending flow")
      .def("receiving_flow", &CTM::receivingFlow, py::arg("num_vehicles"),
           py::arg("dt"), "Calculate receiving flow")
      .def("get_num_cells", &CTM::getNumCells, "Get number of cells")
      .def("get_road_length", &CTM::getRoadLength, "Get road length")
      .def("get_cell_length", &CTM::getCellLength, "Get cell length")
      .def("get_free_flow_speed", &CTM::getFreeFlowSpeed, "Get free-flow speed")
      .def("get_wave_speed", &CTM::getWaveSpeed, "Get wave speed")
      .def("get_jam_density", &CTM::getJamDensity, "Get jam density")
      .def("get_critical_density", &CTM::getCriticalDensity,
           "Get critical density")
      .def("get_max_flow", &CTM::getMaxFlow, "Get maximum flow")
      .def("get_num_vehicles_array", &CTM::getNumVehiclesArray,
           "Get all vehicle counts")
      .def("__repr__", [](const CTM &ctm) {
        return "CTM(cells=" + std::to_string(ctm.getNumCells()) +
               ", length=" + std::to_string(ctm.getRoadLength()) + "m)";
      });

  // Micro-Macro Bridge
  py::class_<MicroMacroBridge::AggregateStats>(m, "AggregateStats")
      .def_readonly("avg_density",
                    &MicroMacroBridge::AggregateStats::avg_density)
      .def_readonly("avg_flow", &MicroMacroBridge::AggregateStats::avg_flow)
      .def_readonly("avg_speed", &MicroMacroBridge::AggregateStats::avg_speed)
      .def_readonly("num_vehicles",
                    &MicroMacroBridge::AggregateStats::num_vehicles);

  py::class_<MicroMacroBridge>(m, "MicroMacroBridge")
      .def_static("extract_density_profile",
                  &MicroMacroBridge::extractDensityProfile, py::arg("lane"),
                  py::arg("num_cells"), "Extract density profile from lane")
      .def_static("extract_flow_profile", &MicroMacroBridge::extractFlowProfile,
                  py::arg("lane"), py::arg("num_cells"),
                  "Extract flow profile from lane")
      .def_static("extract_speed_profile",
                  &MicroMacroBridge::extractSpeedProfile, py::arg("lane"),
                  py::arg("num_cells"), "Extract speed profile from lane")
      .def_static("initialize_lwr_from_lane",
                  &MicroMacroBridge::initializeLWRFromLane, py::arg("lane"),
                  py::arg("lwr"), "Initialize LWR from microscopic lane")
      .def_static("initialize_ctm_from_lane",
                  &MicroMacroBridge::initializeCTMFromLane, py::arg("lane"),
                  py::arg("ctm"), "Initialize CTM from microscopic lane")
      .def_static("calibrate_fundamental_diagram",
                  &MicroMacroBridge::calibrateFundamentalDiagram,
                  py::arg("lane"), py::arg("num_samples") = 50,
                  "Calibrate fundamental diagram from microscopic data")
      .def_static("estimate_free_flow_speed",
                  &MicroMacroBridge::estimateFreeFlowSpeed, py::arg("lane"),
                  "Estimate free-flow speed from microscopic data")
      .def_static("estimate_jam_density", &MicroMacroBridge::estimateJamDensity,
                  py::arg("lane"), py::arg("vehicle_length") = 5.0,
                  "Estimate jam density from microscopic data");

  // ========================================================================
  // Performance Optimizations
  // ========================================================================

  // IDMLookup - IDM with lookup tables (30-40% faster)
  py::class_<IDMLookup, IDM, std::shared_ptr<IDMLookup>>(m, "IDMLookup")
      .def(py::init<double, double, double, double, double, double>(),
           py::arg("desired_speed") = 33.3, py::arg("time_headway") = 1.5,
           py::arg("min_gap") = 2.0, py::arg("max_accel") = 1.0,
           py::arg("comfortable_decel") = 1.5, py::arg("accel_exponent") = 4.0,
           "Create IDM with lookup tables (30-40% faster)")
      .def("calculate_acceleration", &IDMLookup::calculateAcceleration,
           py::arg("vehicle"), py::arg("leader") = nullptr,
           "Calculate acceleration using lookup tables")
      .def("__repr__", [](const IDMLookup &idm) {
        return "IDMLookup(v0=" + std::to_string(idm.getDesiredSpeed() * 3.6) +
               " km/h, T=" + std::to_string(idm.getTimeHeadway()) + "s)";
      });

  // FastMath utilities
  py::class_<FastMath>(m, "FastMath")
      .def_static("fast_sqrt", &FastMath::fastSqrt, py::arg("x"),
                  "Fast square root approximation")
      .def_static("fast_inv_sqrt", &FastMath::fastInvSqrt, py::arg("x"),
                  "Fast inverse square root")
      .def_static("fast_pow", &FastMath::fastPow, py::arg("base"),
                  py::arg("exp"), "Fast power for small integer exponents")
      .def_static("fast_abs", &FastMath::fastAbs, py::arg("x"),
                  "Fast absolute value")
      .def_static("fast_min", &FastMath::fastMin, py::arg("a"), py::arg("b"),
                  "Fast minimum")
      .def_static("fast_max", &FastMath::fastMax, py::arg("a"), py::arg("b"),
                  "Fast maximum")
      .def_static("fast_clamp", &FastMath::fastClamp, py::arg("value"),
                  py::arg("min_val"), py::arg("max_val"), "Fast clamp")
      .def_static("fast_exp", &FastMath::fastExp, py::arg("x"),
                  "Fast exponential approximation")
      .def_static("fast_log", &FastMath::fastLog, py::arg("x"),
                  "Fast natural logarithm");

  // Spatial Index
  py::class_<SpatialIndex>(m, "SpatialIndex")
      .def(py::init<>(), "Create spatial index for efficient queries")
      .def("add_vehicle", &SpatialIndex::addVehicle, py::arg("vehicle"),
           "Add vehicle to index")
      .def("remove_vehicle", &SpatialIndex::removeVehicle, py::arg("vehicle"),
           "Remove vehicle from index")
      .def("update", &SpatialIndex::update, "Update index (resort if needed)")
      .def("find_leader", &SpatialIndex::findLeader, py::arg("vehicle"),
           "Find leader (O(log N))", py::return_value_policy::reference)
      .def("find_follower", &SpatialIndex::findFollower, py::arg("vehicle"),
           "Find follower (O(log N))", py::return_value_policy::reference)
      .def("find_in_range", &SpatialIndex::findInRange, py::arg("min_pos"),
           py::arg("max_pos"), "Find vehicles in range",
           py::return_value_policy::reference)
      .def("size", &SpatialIndex::size, "Get number of vehicles")
      .def("clear", &SpatialIndex::clear, "Clear index")
      .def("get_vehicles", &SpatialIndex::getVehicles, "Get all vehicles");

  // ========================================================================
  // Adaptive Hybrid Simulation
  // ========================================================================

  // SimulationMode enum
  py::enum_<SimulationMode>(m, "SimulationMode")
      .value("MICROSCOPIC", SimulationMode::MICROSCOPIC,
             "Individual vehicle tracking")
      .value("MACROSCOPIC", SimulationMode::MACROSCOPIC, "Continuum flow model")
      .value("TRANSITIONING", SimulationMode::TRANSITIONING,
             "Switching between modes")
      .export_values();

  // AdaptiveSimulator::Config
  py::class_<AdaptiveSimulator::Config>(m, "AdaptiveSimulatorConfig")
      .def(py::init<>(), "Create default configuration")
      .def_readwrite("micro_to_macro_density",
                     &AdaptiveSimulator::Config::micro_to_macro_density,
                     "Density threshold to switch to macro (veh/m)")
      .def_readwrite("macro_to_micro_density",
                     &AdaptiveSimulator::Config::macro_to_micro_density,
                     "Density threshold to switch to micro (veh/m)")
      .def_readwrite("micro_to_macro_count",
                     &AdaptiveSimulator::Config::micro_to_macro_count,
                     "Vehicle count threshold to switch to macro")
      .def_readwrite("macro_to_micro_count",
                     &AdaptiveSimulator::Config::macro_to_micro_count,
                     "Vehicle count threshold to switch to micro")
      .def_readwrite("max_micro_time_ms",
                     &AdaptiveSimulator::Config::max_micro_time_ms,
                     "Max time for micro update (ms)")
      .def_readwrite("macro_num_cells",
                     &AdaptiveSimulator::Config::macro_num_cells,
                     "Number of cells for macroscopic model")
      .def_readwrite("hysteresis_factor",
                     &AdaptiveSimulator::Config::hysteresis_factor,
                     "Hysteresis factor to prevent rapid switching")
      .def_readwrite("force_micro_intersections",
                     &AdaptiveSimulator::Config::force_micro_intersections,
                     "Force microscopic mode at intersections")
      .def_readwrite("force_micro_ramps",
                     &AdaptiveSimulator::Config::force_micro_ramps,
                     "Force microscopic mode at ramps")
      .def("__repr__", [](const AdaptiveSimulator::Config &c) {
        return "AdaptiveSimulatorConfig(micro_to_macro_density=" +
               std::to_string(c.micro_to_macro_density) +
               ", macro_to_micro_density=" +
               std::to_string(c.macro_to_micro_density) + ")";
      });

  // AdaptiveSimulator::Statistics
  py::class_<AdaptiveSimulator::Statistics>(m, "AdaptiveSimulatorStatistics")
      .def_readonly("total_lanes", &AdaptiveSimulator::Statistics::total_lanes,
                    "Total number of lanes")
      .def_readonly("micro_lanes", &AdaptiveSimulator::Statistics::micro_lanes,
                    "Number of lanes in microscopic mode")
      .def_readonly("macro_lanes", &AdaptiveSimulator::Statistics::macro_lanes,
                    "Number of lanes in macroscopic mode")
      .def_readonly("transitioning_lanes",
                    &AdaptiveSimulator::Statistics::transitioning_lanes,
                    "Number of lanes transitioning")
      .def_readonly("total_vehicles",
                    &AdaptiveSimulator::Statistics::total_vehicles,
                    "Total number of vehicles")
      .def_readonly("avg_density", &AdaptiveSimulator::Statistics::avg_density,
                    "Average density across all lanes")
      .def_readonly("total_update_time_ms",
                    &AdaptiveSimulator::Statistics::total_update_time_ms,
                    "Total update time (ms)")
      .def_readonly("speedup_factor",
                    &AdaptiveSimulator::Statistics::speedup_factor,
                    "Speedup factor vs pure microscopic")
      .def("__repr__", [](const AdaptiveSimulator::Statistics &s) {
        return "AdaptiveSimulatorStatistics(micro=" +
               std::to_string(s.micro_lanes) +
               ", macro=" + std::to_string(s.macro_lanes) +
               ", speedup=" + std::to_string(s.speedup_factor) + "x)";
      });

  // AdaptiveSimulator::LaneState
  py::class_<AdaptiveSimulator::LaneState>(m, "AdaptiveSimulatorLaneState")
      .def_readonly("current_density",
                    &AdaptiveSimulator::LaneState::current_density,
                    "Current density (vehicles/m)")
      .def_readonly("avg_speed", &AdaptiveSimulator::LaneState::avg_speed,
                    "Average speed (m/s)")
      .def_readonly("flow", &AdaptiveSimulator::LaneState::flow,
                    "Flow (vehicles/s)")
      .def_readonly("vehicle_count",
                    &AdaptiveSimulator::LaneState::vehicle_count,
                    "Number of vehicles")
      .def("__repr__", [](const AdaptiveSimulator::LaneState &s) {
        return "LaneState(density=" + std::to_string(s.current_density) +
               ", speed=" + std::to_string(s.avg_speed) +
               ", vehicles=" + std::to_string(s.vehicle_count) + ")";
      });

  // AdaptiveSimulator
  py::class_<AdaptiveSimulator>(m, "AdaptiveSimulator")
      .def(py::init<>(), "Create adaptive hybrid simulator with default config")
      .def(py::init<const AdaptiveSimulator::Config &>(), py::arg("config"),
           "Create adaptive hybrid simulator with custom config")
      .def("register_lane", &AdaptiveSimulator::registerLane, py::arg("lane"),
           py::arg("is_critical") = false,
           "Register a lane for adaptive simulation")
      .def("update", &AdaptiveSimulator::update, py::arg("dt"), py::arg("idm"),
           "Update all lanes for one time step")
      .def("get_mode", &AdaptiveSimulator::getMode, py::arg("lane_id"),
           "Get current simulation mode for a lane")
      .def("get_lane_state", &AdaptiveSimulator::getLaneState,
           py::arg("lane_id"), "Get lane state with metrics",
           py::return_value_policy::reference)
      .def("get_statistics", &AdaptiveSimulator::getStatistics,
           "Get overall statistics")
      .def("force_microscopic", &AdaptiveSimulator::forceMicroscopic,
           py::arg("lane_id"), "Force a lane to microscopic mode")
      .def("force_macroscopic", &AdaptiveSimulator::forceMacroscopic,
           py::arg("lane_id"), "Force a lane to macroscopic mode")
      .def("allow_automatic", &AdaptiveSimulator::allowAutomatic,
           py::arg("lane_id"), "Allow automatic mode switching")
      .def("__repr__", [](const AdaptiveSimulator &sim) {
        auto stats = sim.getStatistics();
        return "AdaptiveSimulator(lanes=" + std::to_string(stats.total_lanes) +
               ", micro=" + std::to_string(stats.micro_lanes) +
               ", macro=" + std::to_string(stats.macro_lanes) + ")";
      });

  // ========================================================================
  // Utility functions
  // ========================================================================
  m.def(
      "kmh_to_ms", [](double kmh) { return kmh / 3.6; }, py::arg("kmh"),
      "Convert km/h to m/s");

  m.def(
      "ms_to_kmh", [](double ms) { return ms * 3.6; }, py::arg("ms"),
      "Convert m/s to km/h");

  // ========================================================================
  // Multithreaded Simulation Engine
  // ========================================================================

  using namespace fr::univ_artois::lgi2a::similar::microkernel;
  using namespace fr::univ_artois::lgi2a::similar::microkernel::engine;
  using namespace jamfree::kernel::simulation;

  py::class_<ISimulationEngine>(m, "ISimulationEngine");

  py::class_<MultiThreadedSimulationEngine, ISimulationEngine>(
      m, "MultithreadedSimulationEngine")
      .def(py::init<int>(), py::arg("num_threads") = 0,
           "Create multithreaded simulation engine")
      .def("run_new_simulation",
           &MultiThreadedSimulationEngine::runNewSimulation,
           py::call_guard<py::gil_scoped_release>(), py::arg("final_time"),
           "Run simulation schedule")
      .def("add_probe", &MultiThreadedSimulationEngine::addProbe,
           py::arg("probe_name"), py::arg("probe"),
           "Add a probe to the engine");

  // ========================================================================
  // Traffic Simulation Model
  // ========================================================================

  py::class_<ISimulationModel, std::shared_ptr<ISimulationModel>>(
      m, "ISimulationModel");

  py::class_<TrafficSimulationModel, ISimulationModel,
             std::shared_ptr<TrafficSimulationModel>>(m,
                                                      "TrafficSimulationModel")
      .def(py::init<const SimulationTimeStamp &, std::shared_ptr<RoadNetwork>,
                    const std::vector<std::shared_ptr<
                        jamfree::kernel::agents::VehicleAgent>> &>(),
           py::arg("initial_time"), py::arg("network"), py::arg("vehicles"),
           "Create traffic simulation model");

  // ========================================================================
  // Simulation Time Stamp
  // ========================================================================
  py::class_<SimulationTimeStamp>(m, "SimulationTimeStamp")
      .def(py::init<long>(), py::arg("identifier"), "Create time stamp")
      .def("get_identifier", &SimulationTimeStamp::getIdentifier,
           "Get identifier")
      .def("__repr__", [](const SimulationTimeStamp &t) {
        return "SimulationTimeStamp(" + std::to_string(t.getIdentifier()) + ")";
      });

  // ========================================================================
  // Vehicle Agent and Local States
  // ========================================================================

  using namespace jamfree::kernel::agents;
  using namespace jamfree::microscopic::agents;
  using namespace jamfree::microscopic::perception;
  using namespace jamfree::microscopic::decision;

  // VehiclePublicLocalStateMicro
  py::class_<VehiclePublicLocalStateMicro,
             std::shared_ptr<VehiclePublicLocalStateMicro>>(
      m, "VehiclePublicLocalStateMicro")
      .def(py::init<const std::string &>(), py::arg("owner_id"),
           "Create public local state")
      .def("get_position", &VehiclePublicLocalStateMicro::getPosition,
           "Get vehicle position")
      .def("set_position", &VehiclePublicLocalStateMicro::setPosition,
           py::arg("position"), "Set vehicle position")
      .def("get_speed", &VehiclePublicLocalStateMicro::getSpeed,
           "Get vehicle speed (m/s)")
      .def("set_speed", &VehiclePublicLocalStateMicro::setSpeed,
           py::arg("speed"), "Set vehicle speed (m/s)")
      .def("get_acceleration", &VehiclePublicLocalStateMicro::getAcceleration,
           "Get vehicle acceleration (m/s²)")
      .def("set_acceleration", &VehiclePublicLocalStateMicro::setAcceleration,
           py::arg("acceleration"), "Set vehicle acceleration (m/s²)")
      .def("get_heading", &VehiclePublicLocalStateMicro::getHeading,
           "Get vehicle heading (radians)")
      .def("set_heading", &VehiclePublicLocalStateMicro::setHeading,
           py::arg("heading"), "Set vehicle heading (radians)")
      .def("get_lane_position", &VehiclePublicLocalStateMicro::getLanePosition,
           "Get position along lane (m)")
      .def("set_lane_position", &VehiclePublicLocalStateMicro::setLanePosition,
           py::arg("position"), "Set position along lane (m)")
      .def("get_lane_index", &VehiclePublicLocalStateMicro::getLaneIndex,
           "Get lane index")
      .def("set_lane_index", &VehiclePublicLocalStateMicro::setLaneIndex,
           py::arg("index"), "Set lane index")
      .def("get_length", &VehiclePublicLocalStateMicro::getLength,
           "Get vehicle length (m)")
      .def("set_length", &VehiclePublicLocalStateMicro::setLength,
           py::arg("length"), "Set vehicle length (m)")
      .def("is_active", &VehiclePublicLocalStateMicro::isActive,
           "Check if vehicle is active")
      .def("set_active", &VehiclePublicLocalStateMicro::setActive,
           py::arg("active"), "Set vehicle active status");

  // VehiclePrivateLocalStateMicro
  py::class_<VehiclePrivateLocalStateMicro,
             std::shared_ptr<VehiclePrivateLocalStateMicro>>(
      m, "VehiclePrivateLocalStateMicro")
      .def(py::init<const std::string &>(), py::arg("owner_id"),
           "Create private local state")
      .def("get_desired_speed", &VehiclePrivateLocalStateMicro::getDesiredSpeed,
           "Get desired speed (m/s)")
      .def("set_desired_speed", &VehiclePrivateLocalStateMicro::setDesiredSpeed,
           py::arg("speed"), "Set desired speed (m/s)")
      .def("get_time_headway", &VehiclePrivateLocalStateMicro::getTimeHeadway,
           "Get time headway (s)")
      .def("set_time_headway", &VehiclePrivateLocalStateMicro::setTimeHeadway,
           py::arg("headway"), "Set time headway (s)")
      .def("get_politeness", &VehiclePrivateLocalStateMicro::getPoliteness,
           "Get politeness factor (0-1)")
      .def("set_politeness", &VehiclePrivateLocalStateMicro::setPoliteness,
           py::arg("politeness"), "Set politeness factor (0-1)")
      .def("get_max_acceleration",
           &VehiclePrivateLocalStateMicro::getMaxAcceleration,
           "Get max acceleration (m/s²)")
      .def("set_max_acceleration",
           &VehiclePrivateLocalStateMicro::setMaxAcceleration, py::arg("accel"),
           "Set max acceleration (m/s²)")
      .def("get_comfortable_deceleration",
           &VehiclePrivateLocalStateMicro::getComfortableDeceleration,
           "Get comfortable deceleration (m/s²)")
      .def("set_comfortable_deceleration",
           &VehiclePrivateLocalStateMicro::setComfortableDeceleration,
           py::arg("decel"), "Set comfortable deceleration (m/s²)");

  // VehicleAgent
  py::class_<VehicleAgent, std::shared_ptr<VehicleAgent>>(m, "VehicleAgent")
      .def(py::init<const std::string &>(), py::arg("id"),
           "Create vehicle agent")
      .def(
          "set_perception_model",
          [](VehicleAgent &agent, const std::string &level,
             std::shared_ptr<IPerceptionModel> model) {
            agent.setPerceptionModel(kernel::agents::LevelIdentifier(level),
                                     model);
          },
          py::arg("level"), py::arg("model"), "Set perception model for level")
      .def(
          "set_decision_model",
          [](VehicleAgent &agent, const std::string &level,
             std::shared_ptr<IDecisionModel> model) {
            agent.setDecisionModel(kernel::agents::LevelIdentifier(level),
                                   model);
          },
          py::arg("level"), py::arg("model"), "Set decision model for level");

  // ========================================================================
  // Interfaces
  // ========================================================================
  py::class_<IPerceptionModel, std::shared_ptr<IPerceptionModel>>(
      m, "IPerceptionModel");
  py::class_<IDecisionModel, std::shared_ptr<IDecisionModel>>(m,
                                                              "IDecisionModel");

  // ========================================================================
  // Perception Model
  // ========================================================================
  py::class_<
      microscopic::perception::VehiclePerceptionModelMicro, IPerceptionModel,
      std::shared_ptr<microscopic::perception::VehiclePerceptionModelMicro>>(
      m, "VehiclePerceptionModelMicro")
      .def(py::init<double>(), py::arg("perception_range") = 150.0,
           "Create vehicle perception model with perception range (meters)");

  // ========================================================================
  // Decision Micro Submodels (DMS)
  // ========================================================================

  // Base interface
  py::class_<microscopic::decision::IDecisionMicroSubmodel,
             std::shared_ptr<microscopic::decision::IDecisionMicroSubmodel>>(
      m, "IDecisionMicroSubmodel");

  // ForwardAccelerationDMS - basic car-following
  py::class_<
      microscopic::decision::dms::ForwardAccelerationDMS,
      microscopic::decision::IDecisionMicroSubmodel,
      std::shared_ptr<microscopic::decision::dms::ForwardAccelerationDMS>>(
      m, "ForwardAccelerationDMS")
      .def(py::init<std::shared_ptr<IDM>>(), py::arg("idm"),
           "Create forward acceleration DMS (IDM car-following)");

  // LaneChangeDMS - lane changing behavior
  py::class_<microscopic::decision::dms::LaneChangeDMS,
             microscopic::decision::IDecisionMicroSubmodel,
             std::shared_ptr<microscopic::decision::dms::LaneChangeDMS>>(
      m, "LaneChangeDMS")
      .def(py::init<std::shared_ptr<MOBIL>, std::shared_ptr<IDM>>(),
           py::arg("mobil"), py::arg("idm"),
           "Create lane change DMS (MOBIL lane-changing)");

  // ConjunctionDMS - combines multiple DMS
  py::class_<microscopic::decision::dms::ConjunctionDMS,
             microscopic::decision::IDecisionMicroSubmodel,
             std::shared_ptr<microscopic::decision::dms::ConjunctionDMS>>(
      m, "ConjunctionDMS")
      .def(py::init<>(), "Create conjunction DMS")
      .def("add_submodel",
           &microscopic::decision::dms::ConjunctionDMS::addSubmodel,
           py::arg("submodel"), "Add a sub-model to the conjunction");

  // SubsumptionDMS - priority-based DMS selection
  py::class_<microscopic::decision::dms::SubsumptionDMS,
             microscopic::decision::IDecisionMicroSubmodel,
             std::shared_ptr<microscopic::decision::dms::SubsumptionDMS>>(
      m, "SubsumptionDMS")
      .def(py::init<>(), "Create subsumption DMS")
      .def("add_submodel",
           &microscopic::decision::dms::SubsumptionDMS::addSubmodel,
           py::arg("submodel"), "Add a sub-model to the subsumption hierarchy");

  // ========================================================================
  // Decision Model
  // ========================================================================
  py::class_<microscopic::decision::VehicleDecisionModelMicro, IDecisionModel,
             std::shared_ptr<microscopic::decision::VehicleDecisionModelMicro>>(
      m, "VehicleDecisionModelMicro")
      .def(
          py::init<
              std::shared_ptr<microscopic::decision::IDecisionMicroSubmodel>>(),
          py::arg("root_dms"), "Create vehicle decision model with root DMS");
}
