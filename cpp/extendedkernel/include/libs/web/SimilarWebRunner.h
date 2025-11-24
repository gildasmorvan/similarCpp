#ifndef SIMILARWEBRUNNER_H
#define SIMILARWEBRUNNER_H

#include "IHtmlInitializationData.h"
#include "ISimulationEngine.h"
#include "SimilarWebConfig.h"
#include "control/SimilarWebController.h"
#include "simulationmodel/ISimulationModel.h"
#include "simulationmodel/ISimulationParameters.h"
#include "view/SimilarHttpServer.h"
#include <memory>

// Forward declarations
namespace fr::univ_artois::lgi2a::similar::microkernel {
class ISimulationEngine;
class IProbe;
} // namespace fr::univ_artois::lgi2a::similar::microkernel

namespace fr::univ_artois::lgi2a::similar::extendedkernel::simulationmodel {
class ISimulationModel;
class ISimulationParameters;
} // namespace fr::univ_artois::lgi2a::similar::extendedkernel::simulationmodel

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

/**
 * Facilitates the execution of SIMILAR simulations using the HTML web
 * interface.
 */
class SimilarWebRunner : public IHtmlInitializationData {
private:
  SimilarWebConfig config;
  std::shared_ptr<microkernel::ISimulationEngine> engine;
  std::shared_ptr<simulationmodel::ISimulationModel> model;
  simulationmodel::ISimulationParameters *simulationParameters;
  std::unique_ptr<control::SimilarWebController> controller;
  std::unique_ptr<view::SimilarHttpServer> httpServer;

public:
  /**
   * Creates a new runner with default configuration.
   */
  SimilarWebRunner();

  virtual ~SimilarWebRunner() = default;

  /**
   * Initializes the runner with a specific simulation model.
   * This operation can only be performed once.
   * @param engine The simulation engine
   * @param model The simulation model
   * @throws std::runtime_error if already initialized
   */
  void
  initializeRunner(std::shared_ptr<microkernel::ISimulationEngine> engine,
                   std::shared_ptr<simulationmodel::ISimulationModel> model);

  /**
   * Opens the view on the simulation.
   * @throws std::runtime_error if not initialized
   */
  void showView();

  /**
   * Gets the configuration.
   * @return The configuration
   */
  SimilarWebConfig *getConfig() override { return &config; }

  /**
   * Gets the simulation parameters.
   * @return The simulation parameters
   */
  simulationmodel::ISimulationParameters *getSimulationParameters() override {
    return simulationParameters;
  }

  /**
   * Adds a probe to the simulation engine.
   * @param name The name of the probe
   * @param probe The probe to add
   * @throws std::runtime_error if not initialized
   */
  void addProbe(const std::string &name,
                std::shared_ptr<microkernel::IProbe> probe);

  /**
   * Gets the simulation engine.
   * @return The engine
   */
  std::shared_ptr<microkernel::ISimulationEngine> getEngine() const {
    return engine;
  }

  /**
   * Gets the controller.
   * @return The controller
   */
  control::SimilarWebController *getController() const {
    return controller.get();
  }
};

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILARWEBRUNNER_H
