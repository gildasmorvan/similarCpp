#include "libs/web/SimilarWebRunner.h"
#include "IProbe.h"
#include "ISimulationEngine.h"
#include "simulationmodel/ISimulationModel.h"
#include "simulationmodel/ISimulationParameters.h"
#include <iostream>
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

// Bring IProbe into scope for easier use
using microkernel::IProbe;

SimilarWebRunner::SimilarWebRunner() : simulationParameters(nullptr) {}

void SimilarWebRunner::initializeRunner(
    std::shared_ptr<microkernel::ISimulationEngine> engine,
    std::shared_ptr<simulationmodel::ISimulationModel> model) {

  if (!model) {
    throw std::invalid_argument("The model cannot be null");
  }

  if (config.isAlreadyInitialized()) {
    throw std::runtime_error("The runner is already initialized");
  }

  // Set default simulation name if not provided
  if (config.getSimulationName() == "SIMILAR Simulation") {
    // Could use typeid or a model name method here
    config.setSimulationName("SIMILAR C++ Simulation");
  }

  // Finalize configuration
  config.finalizeConfiguration();

  // Store engine and model
  this->engine = engine;
  this->model = model;

  // Get simulation parameters
  this->simulationParameters = model->getSimulationParameters();

  // Create controller
  controller = std::make_unique<control::SimilarWebController>(engine, model);

  // Add controller as a probe to the engine (non-owning shared_ptr)
  std::shared_ptr<IProbe> probePtr(controller.get(), [](IProbe *) {});
  engine->addProbe("Web Controller", probePtr);

  // Create HTTP server
  httpServer =
      std::make_unique<view::SimilarHttpServer>(controller.get(), this);

  // Initialize server
  httpServer->initServer();

  // Bind view and controller
  controller->setViewControls(httpServer.get());

  std::cout << "✅ Web interface initialized successfully!" << std::endl;
  std::cout << "   Simulation: " << config.getSimulationName() << std::endl;
  std::cout << "   Port: " << config.getPort() << std::endl;
}

void SimilarWebRunner::showView() {
  if (!config.isAlreadyInitialized()) {
    throw std::runtime_error("The runner is not initialized");
  }

  // Show the view (starts server and opens browser)
  httpServer->showView();

  // Tell controller to start listening
  controller->listenToViewRequests();

  std::cout << "🌐 Web interface running!" << std::endl;
  std::cout << "   Browse to: http://localhost:" << config.getPort()
            << std::endl;
  std::cout << "   Press Ctrl+C to stop" << std::endl;
}

void SimilarWebRunner::addProbe(const std::string &name,
                                std::shared_ptr<microkernel::IProbe> probe) {

  if (!config.isAlreadyInitialized()) {
    throw std::runtime_error(
        "The runner must be initialized before adding probes");
  }

  engine->addProbe(name, probe);
}

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
