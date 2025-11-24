#ifndef SIMILAR2LOGO_LOGOSIMULATIONMODEL_H
#define SIMILAR2LOGO_LOGOSIMULATIONMODEL_H

#include "kernel/agents/LogoAgent.h"
#include "kernel/model/environment/LogoEnvPLS.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"
#include <ISimulationEngine.h>
#include <functional>
#include <memory>
#include <simulationmodel/ISimulationModel.h>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {

namespace mk = fr::univ_artois::lgi2a::similar::microkernel;
namespace ek = fr::univ_artois::lgi2a::similar::extendedkernel;

/**
 * Logo Simulation Model - C++ implementation.
 *
 * This provides a high-performance Logo simulation that can be
 * controlled from Python while using the C++ multithreaded engine.
 */
class LogoSimulationModel : public ek::simulationmodel::ISimulationModel {
private:
  int width, height;
  bool xTorus, yTorus;
  int maxSteps;

  // Agent factory callback (can be set from Python)
  using AgentFactory =
      std::function<std::vector<std::shared_ptr<agents::LogoAgent>>()>;
  AgentFactory agentFactory;

  // Environment configuration
  std::unordered_set<environment::Pheromone> pheromones;

public:
  LogoSimulationModel(int width, int height, bool xTorus = true,
                      bool yTorus = true, int maxSteps = 1000);

  virtual ~LogoSimulationModel() = default;

  // Configuration
  void setAgentFactory(AgentFactory factory) { agentFactory = factory; }
  void addPheromone(const environment::Pheromone &pheromone) {
    pheromones.insert(pheromone);
  }

  // ISimulationModel interface
  ek::simulationmodel::ISimulationParameters *
  getSimulationParameters() override {
    return nullptr;
  }

  mk::SimulationTimeStamp getInitialTime() const override {
    return mk::SimulationTimeStamp(0);
  }

  bool isFinalTimeOrAfter(const mk::SimulationTimeStamp &currentTime,
                          const mk::ISimulationEngine &) const override {
    return currentTime.getIdentifier() >= maxSteps;
  }

  std::vector<std::shared_ptr<mk::levels::ILevel>>
  generateLevels(const mk::SimulationTimeStamp &initialTime) override;

  mk::ISimulationModel::EnvironmentInitializationData generateEnvironment(
      const mk::SimulationTimeStamp &initialTime,
      const std::map<mk::LevelIdentifier, std::shared_ptr<mk::levels::ILevel>>
          &levels) override;

  mk::ISimulationModel::AgentInitializationData generateAgents(
      const mk::SimulationTimeStamp &initialTime,
      const std::map<mk::LevelIdentifier, std::shared_ptr<mk::levels::ILevel>>
          &levels) override;
};

} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_LOGOSIMULATIONMODEL_H
