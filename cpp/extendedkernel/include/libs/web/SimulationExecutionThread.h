#ifndef SIMULATIONEXECUTIONTHREAD_H
#define SIMULATIONEXECUTIONTHREAD_H

#include <atomic>
#include <memory>
#include <thread>

// Forward declarations
namespace fr::univ_artois::lgi2a::similar::microkernel {
class ISimulationEngine;
}

// Include full definition
#include "simulationmodel/ISimulationModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

/**
 * Thread that executes a simulation in the background.
 */
class SimulationExecutionThread {
private:
  std::shared_ptr<microkernel::ISimulationEngine> engine;
  std::shared_ptr<simulationmodel::ISimulationModel> model;
  std::unique_ptr<std::thread> thread;
  std::atomic<bool> finished;

  void run();

public:
  /**
   * Creates a new simulation execution thread.
   * @param engine The simulation engine
   * @param model The simulation model
   */
  SimulationExecutionThread(
      std::shared_ptr<microkernel::ISimulationEngine> engine,
      std::shared_ptr<simulationmodel::ISimulationModel> model);

  ~SimulationExecutionThread();

  /**
   * Starts the simulation thread.
   */
  void start();

  /**
   * Checks if the simulation has finished.
   * @return true if finished, false otherwise
   */
  bool hasFinished() const { return finished.load(); }

  /**
   * Waits for the simulation thread to complete.
   */
  void join();
};

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMULATIONEXECUTIONTHREAD_H
