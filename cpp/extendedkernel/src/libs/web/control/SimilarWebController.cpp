#include "libs/web/control/SimilarWebController.h"
#include "ISimulationEngine.h"
#include "libs/web/SimulationExecutionThread.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {
namespace control {

SimilarWebController::SimilarWebController(
    std::shared_ptr<microkernel::ISimulationEngine> engine,
    std::shared_ptr<simulationmodel::ISimulationModel> model)
    : engine(engine), model(model), viewControls(nullptr),
      engineState(EngineState::IDLE), togglePause(false),
      listenToRequests(false), allowShutDown(true) {}

void SimilarWebController::setViewControls(IHtmlControls *viewControls) {
  this->viewControls = viewControls;
  // Force update of view buttons
  std::lock_guard<std::mutex> lock(stateMutex);
  if (EngineStateUtil::allowsNewRun(engineState)) {
    changeEngineState(EngineState::IDLE);
  }
}

void SimilarWebController::listenToViewRequests() {
  listenToRequests.store(true);
}

void SimilarWebController::changeEngineState(EngineState newState) {
  engineState = newState;
  if (viewControls) {
    viewControls->setStartButtonState(EngineStateUtil::allowsNewRun(newState));
    viewControls->setPauseButtonState(EngineStateUtil::allowsPause(newState));
    viewControls->setAbortButtonState(EngineStateUtil::allowsAbort(newState));
  }
}

// IHtmlRequests implementation

std::vector<uint8_t> SimilarWebController::handleSimulationStateRequest() {
  std::lock_guard<std::mutex> lock(stateMutex);
  std::string stateStr = EngineStateUtil::toString(engineState);
  return std::vector<uint8_t>(stateStr.begin(), stateStr.end());
}

void SimilarWebController::handleNewSimulationRequest() {
  if (!listenToRequests.load()) {
    return;
  }

  std::lock_guard<std::mutex> lock(stateMutex);

  // Check if we can start a new simulation
  if (!EngineStateUtil::allowsNewRun(engineState) ||
      (simuThread && !simuThread->hasFinished())) {
    std::cout << "Ignored simulation start request (current state: "
              << EngineStateUtil::toString(engineState) << ")" << std::endl;
    return;
  }

  // Start new simulation
  changeEngineState(EngineState::RUN_PLANNED);
  togglePause.store(false);
  simuThread = std::make_unique<SimulationExecutionThread>(engine, model);
  simuThread->start();
}

void SimilarWebController::handleSimulationAbortionRequest() {
  if (!listenToRequests.load()) {
    return;
  }

  std::lock_guard<std::mutex> lock(stateMutex);

  if (!EngineStateUtil::allowsAbort(engineState)) {
    std::cout << "Ignored simulation abortion request (current state: "
              << EngineStateUtil::toString(engineState) << ")" << std::endl;
    return;
  }

  changeEngineState(EngineState::ABORT_REQUESTED);
  engine->requestSimulationAbortion();
}

void SimilarWebController::handleSimulationPauseRequest() {
  if (!listenToRequests.load()) {
    return;
  }

  std::lock_guard<std::mutex> lock(stateMutex);

  if (!EngineStateUtil::allowsPause(engineState)) {
    std::cout << "Ignored simulation pause request (current state: "
              << EngineStateUtil::toString(engineState) << ")" << std::endl;
    return;
  }

  togglePause.store(true);
}

void SimilarWebController::handleShutDownRequest() {
  if (!listenToRequests.load() || !allowShutDown.load()) {
    return;
  }

  std::lock_guard<std::mutex> lock(stateMutex);

  if (!EngineStateUtil::allowsEject(engineState)) {
    std::cout << "Ignored server shutdown request (current state: "
              << EngineStateUtil::toString(engineState) << ")" << std::endl;
    return;
  }

  changeEngineState(EngineState::SHUTDOWN_REQUESTED);
  engine->requestSimulationAbortion();
}

std::string SimilarWebController::getParameter(const std::string &parameter) {
  if (!listenToRequests.load()) {
    return "";
  }
  // Parameter reflection would go here
  // For now, return placeholder
  return "Parameter reflection not yet implemented";
}

void SimilarWebController::setParameter(const std::string &parameter,
                                        const std::string &value) {
  if (!listenToRequests.load()) {
    return;
  }
  // Parameter reflection would go here
  std::cout << "Set parameter: " << parameter << " = " << value << std::endl;
}

// IProbe implementation

void SimilarWebController::prepareObservation() {
  std::lock_guard<std::mutex> lock(stateMutex);
  if (!EngineStateUtil::isAborting(engineState) &&
      !EngineStateUtil::isShuttingDown(engineState)) {
    changeEngineState(EngineState::INITIALIZING);
  }
}

void SimilarWebController::observeAtInitialTimes(
    const microkernel::SimulationTimeStamp &initialTimestamp,
    const microkernel::ISimulationEngine &simulationEngine) {

  std::lock_guard<std::mutex> lock(stateMutex);
  if (!EngineStateUtil::isAborting(engineState) &&
      !EngineStateUtil::isShuttingDown(engineState)) {
    changeEngineState(EngineState::RUN);
  }
}

void SimilarWebController::observeAtPartialConsistentTime(
    const microkernel::SimulationTimeStamp &timestamp,
    const microkernel::ISimulationEngine &simulationEngine) {

  // Handle pause/resume
  bool paused = false;
  {
    std::lock_guard<std::mutex> lock(stateMutex);
    if (EngineStateUtil::allowsPause(engineState)) {
      paused = (engineState == EngineState::RUN) && togglePause.load();
      togglePause.store(false);
      if (paused) {
        changeEngineState(EngineState::PAUSED);
      }
    }
  }

  // While paused, wait
  while (paused) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::lock_guard<std::mutex> lock(stateMutex);
    paused = !togglePause.load() && !EngineStateUtil::isAborting(engineState) &&
             !EngineStateUtil::isShuttingDown(engineState);
  }

  // Resume
  {
    std::lock_guard<std::mutex> lock(stateMutex);
    togglePause.store(false);
    if (!EngineStateUtil::isAborting(engineState) &&
        !EngineStateUtil::isShuttingDown(engineState) &&
        engineState != EngineState::RUN) {
      changeEngineState(EngineState::RUN);
    }
  }
}

void SimilarWebController::observeAtFinalTime(
    const microkernel::SimulationTimeStamp &finalTimestamp,
    const microkernel::ISimulationEngine &simulationEngine) {
  // Nothing to do - handled in endObservation
}

void SimilarWebController::reactToError(const std::string &errorMessage,
                                        const std::exception &cause) {

  std::lock_guard<std::mutex> lock(stateMutex);
  if (viewControls) {
    viewControls->shutDownView();
  }
  changeEngineState(EngineState::INACTIVE);
}

void SimilarWebController::reactToAbortion(
    const microkernel::SimulationTimeStamp &timestamp,
    const microkernel::ISimulationEngine &simulationEngine) {

  std::lock_guard<std::mutex> lock(stateMutex);
  if (!EngineStateUtil::isShuttingDown(engineState)) {
    changeEngineState(EngineState::ABORTING);
  }
}

void SimilarWebController::endObservation() {
  std::lock_guard<std::mutex> lock(stateMutex);
  if (EngineStateUtil::isShuttingDown(engineState)) {
    changeEngineState(EngineState::INACTIVE);
  } else {
    changeEngineState(EngineState::IDLE);
  }
}

} // namespace control
} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
