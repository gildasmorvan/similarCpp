#ifndef SIMILARWEBCONTROLLER_H
#define SIMILARWEBCONTROLLER_H

#include "../IHtmlControls.h"
#include "../IHtmlRequests.h"
#include "../SimulationExecutionThread.h"
#include "EngineState.h"
#include "IProbe.h"
#include "ISimulationEngine.h"
#include "SimulationTimeStamp.h"
#include "simulationmodel/ISimulationModel.h"
#include <atomic>
#include <memory>
#include <mutex>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {
namespace control {

/**
 * Controller managing synchronization between simulation engine and HTML
 * view. Implements both IProbe (to observe simulation) and IHtmlRequests (to
 * handle web requests).
 */
class SimilarWebController
    : public fr::univ_artois::lgi2a::similar::microkernel::IProbe,
      public fr::univ_artois::lgi2a::similar::extendedkernel::libs::web::
          IHtmlRequests {
private:
  std::shared_ptr<
      fr::univ_artois::lgi2a::similar::microkernel::ISimulationEngine>
      engine;
  std::shared_ptr<fr::univ_artois::lgi2a::similar::extendedkernel::
                      simulationmodel::ISimulationModel>
      model;
  std::unique_ptr<SimulationExecutionThread> simuThread;
  IHtmlControls *viewControls;

  mutable std::mutex stateMutex;
  EngineState engineState;
  std::atomic<bool> togglePause;
  std::atomic<bool> listenToRequests; // Renamed to avoid conflict
  std::atomic<bool> allowShutDown;

  void changeEngineState(EngineState newState);

public:
  /**
   * Creates a controller for the provided engine and model.
   * @param engine The simulation engine
   * @param model The simulation model
   */
  SimilarWebController(
      std::shared_ptr<
          fr::univ_artois::lgi2a::similar::microkernel::ISimulationEngine>
          engine,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::extendedkernel::
                          simulationmodel::ISimulationModel>
          model);

  virtual ~SimilarWebController() = default;

  /**
   * Sets the object forwarding update requests to the view.
   * @param viewControls The view controls interface
   */
  void setViewControls(IHtmlControls *viewControls);

  /**
   * Tells the controller to start listening to view requests.
   */
  void listenToViewRequests();

  /**
   * Sets whether shutdown is allowed.
   */
  void setAllowShutDown(bool allow) { allowShutDown.store(allow); }

  /**
   * Gets whether shutdown is allowed.
   */
  bool isAllowShutDown() const { return allowShutDown.load(); }

  // IHtmlRequests implementation
  std::vector<uint8_t> handleSimulationStateRequest() override;
  void handleNewSimulationRequest() override;
  void handleSimulationAbortionRequest() override;
  void handleSimulationPauseRequest() override;
  void handleShutDownRequest() override;
  std::string getParameter(const std::string &parameter) override;
  void setParameter(const std::string &parameter,
                    const std::string &value) override;

  // IProbe implementation - fix signatures to match interface
  void prepareObservation() override;
  void observeAtInitialTimes(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &initialTimestamp,
      const fr::univ_artois::lgi2a::similar::microkernel::ISimulationEngine
          &simulationEngine) override;
  void observeAtPartialConsistentTime(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timestamp,
      const fr::univ_artois::lgi2a::similar::microkernel::ISimulationEngine
          &simulationEngine) override;
  void observeAtFinalTime(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &finalTimestamp,
      const fr::univ_artois::lgi2a::similar::microkernel::ISimulationEngine
          &simulationEngine) override;
  void reactToError(const std::string &errorMessage,
                    const std::exception &cause) override;
  void reactToAbortion(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timestamp,
      const fr::univ_artois::lgi2a::similar::microkernel::ISimulationEngine
          &simulationEngine) override;
  void endObservation() override;

  std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::IProbe>
  clone() const override {
    return nullptr; // Controller cloning not supported/needed
  }
};

} // namespace control
} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILARWEBCONTROLLER_H
