#include "libs/web/SimulationExecutionThread.h"
#include "ISimulationEngine.h"
#include "simulationmodel/ISimulationModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

SimulationExecutionThread::SimulationExecutionThread(
    std::shared_ptr<microkernel::ISimulationEngine> engine,
    std::shared_ptr<simulationmodel::ISimulationModel> model)
    : engine(engine), model(model), finished(false) {}

SimulationExecutionThread::~SimulationExecutionThread() {
  if (thread && thread->joinable()) {
    thread->join();
  }
}

void SimulationExecutionThread::run() {
  try {
    // Run the simulation using the engine
    engine->runNewSimulation(model);
  } catch (...) {
    // Errors are handled by probes
  }
  finished.store(true);
}

void SimulationExecutionThread::start() {
  finished.store(false);
  thread = std::make_unique<std::thread>(&SimulationExecutionThread::run, this);
}

void SimulationExecutionThread::join() {
  if (thread && thread->joinable()) {
    thread->join();
  }
}

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
