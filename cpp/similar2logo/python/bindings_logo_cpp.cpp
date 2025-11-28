#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../microkernel/include/LevelIdentifier.h"
#include "../../microkernel/include/SimulationTimeStamp.h"
#include "../../microkernel/include/engine/MultiThreadedSimulationEngine.h"
#include "kernel/agents/LogoAgent.h"
#include "kernel/environment/Environment.h"
#include "kernel/influences/ChangeDirection.h"
#include "kernel/influences/ChangePosition.h"
#include "kernel/influences/ChangeSpeed.h"
#include "kernel/influences/EmitPheromone.h"
#include "kernel/influences/Stop.h"
#include "kernel/model/LogoSimulationModel.h"
#include "kernel/model/environment/LogoEnvPLS.h"
#include "kernel/model/environment/TurtlePLSInLogo.h"
#include "kernel/reaction/Reaction.h"

namespace py = pybind11;

using namespace fr::univ_artois::lgi2a::similar;
using namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel;

namespace mk = fr::univ_artois::lgi2a::similar::microkernel;

PYBIND11_MODULE(_core, m) {
  m.doc() =
      "SIMILAR2Logo C++ Engine - High-performance multithreaded simulation";

  // ========== Microkernel Types ==========
  py::class_<
      ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp>(
      m, "SimulationTimeStamp")
      .def(py::init<double>(), py::arg("time") = 0.0);

  py::class_<::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier>(
      m, "LevelIdentifier")
      .def(py::init<std::string>(), py::arg("id"));

  // ========== Point2D ==========
  py::class_<
      ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D>(
      m, "Point2D")
      .def(py::init<double, double>(), py::arg("x") = 0.0, py::arg("y") = 0.0)
      .def_readwrite("x", &::fr::univ_artois::lgi2a::similar::similar2logo::
                              kernel::tools::Point2D::x)
      .def_readwrite("y", &::fr::univ_artois::lgi2a::similar::similar2logo::
                              kernel::tools::Point2D::y)
      .def("__repr__", [](const ::fr::univ_artois::lgi2a::similar::
                              similar2logo::kernel::tools::Point2D &p) {
        return "Point2D(x=" + std::to_string(p.x) +
               ", y=" + std::to_string(p.y) + ")";
      });

  // ========== Logo Agent ==========
  py::class_<::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
                 LogoAgent,
             std::shared_ptr<::fr::univ_artois::lgi2a::similar::similar2logo::
                                 kernel::agents::LogoAgent>>(m, "LogoAgent")
      .def(py::init<const ::fr::univ_artois::lgi2a::similar::microkernel::
                        AgentCategory &,
                    double, const std::string &>(),
           py::arg("category"), py::arg("initialSpeed") = 1.0,
           py::arg("color") = "blue")
      .def("get_color", &::fr::univ_artois::lgi2a::similar::similar2logo::
                            kernel::agents::LogoAgent::getColor)
      .def("set_color", &::fr::univ_artois::lgi2a::similar::similar2logo::
                            kernel::agents::LogoAgent::setColor);

  // ========== IPerceivedData ==========
  py::class_<mk::agents::IPerceivedData,
             std::shared_ptr<mk::agents::IPerceivedData>>(m, "IPerceivedData");

  // ========== Logo Perceived Data ==========
  py::class_<
      ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
          LogoAgent::LogoPerceivedData,
      std::shared_ptr<::fr::univ_artois::lgi2a::similar::similar2logo::kernel::
                          agents::LogoAgent::LogoPerceivedData>,
      ::fr::univ_artois::lgi2a::similar::microkernel::agents::IPerceivedData>(
      m, "LogoPerceivedData")
      .def("get_position",
           &::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
               LogoAgent::LogoPerceivedData::getPosition)
      .def("get_heading",
           &::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
               LogoAgent::LogoPerceivedData::getHeading)
      .def("get_speed",
           &::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
               LogoAgent::LogoPerceivedData::getSpeed)
      .def("get_nearby_turtles",
           &::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
               LogoAgent::LogoPerceivedData::getNearbyTurtles)
      .def("get_pheromone",
           &::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
               LogoAgent::LogoPerceivedData::getPheromone)
      .def("get_all_pheromones",
           &::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
               LogoAgent::LogoPerceivedData::getAllPheromones);

  // ========== IAgtDecisionModel ==========
  py::class_<::fr::univ_artois::lgi2a::similar::extendedkernel::agents::
                 IAgtDecisionModel,
             std::shared_ptr<::fr::univ_artois::lgi2a::similar::extendedkernel::
                                 agents::IAgtDecisionModel>>(
      m, "IAgtDecisionModel");

  // ========== Python Decision Model ==========
  py::class_<
      ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
          LogoAgent::PythonDecisionModel,
      std::shared_ptr<::fr::univ_artois::lgi2a::similar::similar2logo::kernel::
                          agents::LogoAgent::PythonDecisionModel>,
      ::fr::univ_artois::lgi2a::similar::extendedkernel::agents::
          IAgtDecisionModel>(m, "PythonDecisionModel")
      .def(py::init<
               const mk::LevelIdentifier &,
               ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::agents::
                   LogoAgent::PythonDecisionModel::DecisionCallback>(),
           py::arg("level"), py::arg("callback"));

  // ========== ISimulationModel (Microkernel) ==========
  py::class_<mk::ISimulationModel, std::shared_ptr<mk::ISimulationModel>>(
      m, "ISimulationModel");

  // ========== ISimulationModel (ExtendedKernel) ==========
  py::class_<::fr::univ_artois::lgi2a::similar::extendedkernel::
                 simulationmodel::ISimulationModel,
             mk::ISimulationModel,
             std::shared_ptr<::fr::univ_artois::lgi2a::similar::extendedkernel::
                                 simulationmodel::ISimulationModel>>(
      m, "ExtendedISimulationModel");

  // ========== Logo Simulation Model ==========
  py::class_<model::LogoSimulationModel,
             ::fr::univ_artois::lgi2a::similar::extendedkernel::
                 simulationmodel::ISimulationModel,
             std::shared_ptr<model::LogoSimulationModel>>(m,
                                                          "LogoSimulationModel")
      .def(py::init<int, int, bool, bool, int>(), py::arg("width"),
           py::arg("height"), py::arg("x_torus") = true,
           py::arg("y_torus") = true, py::arg("max_steps") = 1000)
      .def("set_agent_factory", &model::LogoSimulationModel::setAgentFactory)
      .def("add_pheromone", &model::LogoSimulationModel::addPheromone);

  // ========== Multithreaded Engine ==========
  py::class_<mk::engine::MultiThreadedSimulationEngine,
             std::shared_ptr<mk::engine::MultiThreadedSimulationEngine>>(
      m, "MultiThreadedEngine")
      .def(py::init<size_t>(), py::arg("num_threads") = 0)
      .def("run_new_simulation",
           &mk::engine::MultiThreadedSimulationEngine::runNewSimulation)
      .def("run_simulation",
           &mk::engine::MultiThreadedSimulationEngine::runSimulation)
      .def("clone", &mk::engine::MultiThreadedSimulationEngine::clone);

  // ========== Environment (New) ==========
  auto env_module = m.def_submodule("environment", "Environment module");

  py::class_<similar2logo::kernel::environment::Environment>(env_module,
                                                             "Environment")
      .def(py::init<int, int, bool>(), py::arg("width"), py::arg("height"),
           py::arg("toroidal") = false)
      .def("add_pheromone",
           &similar2logo::kernel::environment::Environment::add_pheromone,
           py::arg("identifier"), py::arg("diffusion_coef") = 0.0,
           py::arg("evaporation_coef") = 0.0, py::arg("default_value") = 0.0,
           py::arg("min_value") = 0.0)
      .def("set_pheromone",
           &similar2logo::kernel::environment::Environment::set_pheromone,
           py::arg("x"), py::arg("y"), py::arg("identifier"), py::arg("value"))
      .def("get_pheromone_value",
           &similar2logo::kernel::environment::Environment::get_pheromone_value,
           py::arg("x"), py::arg("y"), py::arg("identifier"))
      .def("random_position",
           &similar2logo::kernel::environment::Environment::random_position)
      .def("random_heading",
           &similar2logo::kernel::environment::Environment::random_heading)
      .def("get_distance",
           &similar2logo::kernel::environment::Environment::get_distance,
           py::arg("a"), py::arg("b"))
      .def("get_direction",
           &similar2logo::kernel::environment::Environment::get_direction,
           py::arg("from"), py::arg("to"));

  // ========== TurtlePLS ==========
  py::class_<model::environment::TurtlePLSInLogo,
             std::shared_ptr<model::environment::TurtlePLSInLogo>>(env_module,
                                                                   "TurtlePLS")
      .def(py::init<const tools::Point2D &, double, double, double, bool,
                    const std::string &>(),
           py::arg("location"), py::arg("heading"), py::arg("speed") = 1.0,
           py::arg("acceleration") = 0.0, py::arg("pen_down") = false,
           py::arg("color") = "blue")
      .def("get_location", &model::environment::TurtlePLSInLogo::getLocation)
      .def("set_location", &model::environment::TurtlePLSInLogo::setLocation)
      .def("get_heading", &model::environment::TurtlePLSInLogo::getHeading)
      .def("set_heading", &model::environment::TurtlePLSInLogo::setHeading)
      .def("get_speed", &model::environment::TurtlePLSInLogo::getSpeed)
      .def("set_speed", &model::environment::TurtlePLSInLogo::setSpeed)
      .def("is_pen_down", &model::environment::TurtlePLSInLogo::isPenDown)
      .def("set_pen_down", &model::environment::TurtlePLSInLogo::setPenDown)
      .def("get_color", &model::environment::TurtlePLSInLogo::getColor)
      .def("set_color", &model::environment::TurtlePLSInLogo::setColor);

  // ========== Influences ==========
  auto influences_module = m.def_submodule("influences", "Influence classes");

  py::class_<mk::influences::IInfluence,
             std::shared_ptr<mk::influences::IInfluence>>(influences_module,
                                                          "IInfluence");

  py::class_<mk::influences::RegularInfluence, mk::influences::IInfluence,
             std::shared_ptr<mk::influences::RegularInfluence>>(
      influences_module, "RegularInfluence");

  py::class_<similar2logo::kernel::influences::ChangePosition,
             mk::influences::RegularInfluence,
             std::shared_ptr<similar2logo::kernel::influences::ChangePosition>>(
      influences_module, "ChangePosition")
      .def(py::init<const mk::SimulationTimeStamp &,
                    const mk::SimulationTimeStamp &, double, double,
                    std::shared_ptr<model::environment::TurtlePLSInLogo>>(),
           py::arg("time_lower"), py::arg("time_upper"), py::arg("dx"),
           py::arg("dy"), py::arg("target"))
      .def("getTarget",
           &similar2logo::kernel::influences::ChangePosition::getTarget);

  py::class_<
      similar2logo::kernel::influences::ChangeDirection,
      mk::influences::RegularInfluence,
      std::shared_ptr<similar2logo::kernel::influences::ChangeDirection>>(
      influences_module, "ChangeDirection")
      .def(py::init<const mk::SimulationTimeStamp &,
                    const mk::SimulationTimeStamp &, double,
                    std::shared_ptr<model::environment::TurtlePLSInLogo>>(),
           py::arg("time_lower"), py::arg("time_upper"), py::arg("dd"),
           py::arg("target"))
      .def("getTarget",
           &similar2logo::kernel::influences::ChangeDirection::getTarget);

  py::class_<similar2logo::kernel::influences::ChangeSpeed,
             mk::influences::RegularInfluence,
             std::shared_ptr<similar2logo::kernel::influences::ChangeSpeed>>(
      influences_module, "ChangeSpeed")
      .def(py::init<const mk::SimulationTimeStamp &,
                    const mk::SimulationTimeStamp &, double,
                    std::shared_ptr<model::environment::TurtlePLSInLogo>>(),
           py::arg("time_lower"), py::arg("time_upper"), py::arg("ds"),
           py::arg("target"))
      .def("getTarget",
           &similar2logo::kernel::influences::ChangeSpeed::getTarget);

  py::class_<similar2logo::kernel::influences::Stop,
             mk::influences::RegularInfluence,
             std::shared_ptr<similar2logo::kernel::influences::Stop>>(
      influences_module, "Stop")
      .def(py::init<const mk::SimulationTimeStamp &,
                    const mk::SimulationTimeStamp &,
                    std::shared_ptr<model::environment::TurtlePLSInLogo>>(),
           py::arg("time_lower"), py::arg("time_upper"), py::arg("target"))
      .def("getTarget", &similar2logo::kernel::influences::Stop::getTarget);

  py::class_<similar2logo::kernel::influences::EmitPheromone,
             mk::influences::RegularInfluence,
             std::shared_ptr<similar2logo::kernel::influences::EmitPheromone>>(
      influences_module, "EmitPheromone")
      .def(py::init<const mk::SimulationTimeStamp &,
                    const mk::SimulationTimeStamp &, const tools::Point2D &,
                    const std::string &, double>(),
           py::arg("time_lower"), py::arg("time_upper"), py::arg("location"),
           py::arg("pheromone_id"), py::arg("value"));

  // ========== Reaction (New) ==========
  auto reaction_module = m.def_submodule("reaction", "Reaction module");
  py::class_<similar2logo::kernel::reaction::Reaction>(reaction_module,
                                                       "Reaction")
      .def(py::init<>())
      .def("apply", &similar2logo::kernel::reaction::Reaction::apply,
           py::arg("influences"), py::arg("env"), py::arg("dt") = 1.0);

  // ========== Helper Functions ==========
  m.def(
      "create_multithreaded_simulation",
      [](int width, int height, int num_agents, int num_threads) {
        auto model =
            std::make_shared<model::LogoSimulationModel>(width, height);
        auto engine =
            std::make_shared<mk::engine::MultiThreadedSimulationEngine>(
                num_threads);
        return std::make_pair(engine, model);
      },
      py::arg("width"), py::arg("height"), py::arg("num_agents") = 100,
      py::arg("num_threads") = 0, "Create a multithreaded Logo simulation");
}
