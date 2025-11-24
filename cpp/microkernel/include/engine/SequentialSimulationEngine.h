#ifndef SEQUENTIALSIMULATIONENGINE_H
#define SEQUENTIALSIMULATIONENGINE_H

#include "../IProbe.h"
#include "../ISimulationEngine.h"
#include "../ISimulationModel.h"
#include <atomic>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace engine {

/**
 * A sequential implementation of the simulation engine.
 * This engine executes the simulation loop in a single thread.
 */
class SequentialSimulationEngine : public ISimulationEngine {
private:
  // Probes
  std::map<std::string, std::shared_ptr<IProbe>> probes;
  std::mutex probesMutex;

  // Simulation state
  std::atomic<bool> abortionRequested;
  std::shared_ptr<ISimulationModel> currentModel;

  // Dynamic state
  std::shared_ptr<dynamicstate::IPublicDynamicStateMap> dynamicStates;

  // Cache for simulation components (retrieved from model)
  std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>> levels;
  std::shared_ptr<environment::IEnvironment4Engine> environment;
  std::set<std::shared_ptr<agents::IAgent4Engine>> agents;
  std::map<LevelIdentifier, std::set<std::shared_ptr<agents::IAgent4Engine>>>
      agentsByLevel;

  // Helper methods
  void initializeSimulation(std::shared_ptr<ISimulationModel> model);
  void runSimulationLoop();
  void notifyProbesOfPreparation(const SimulationTimeStamp &initialTime);
  void notifyProbesOfStart(const SimulationTimeStamp &initialTime);
  void notifyProbesOfEnd(const SimulationTimeStamp &finalTime);
  void notifyProbesOfUpdate(const SimulationTimeStamp &time);

public:
  SequentialSimulationEngine();
  virtual ~SequentialSimulationEngine() = default;

  // ISimulationEngine implementation
  void addProbe(const std::string &identifier,
                std::shared_ptr<IProbe> probe) override;
  std::shared_ptr<IProbe> removeProbe(const std::string &identifier) override;
  std::set<std::string> getProbesIdentifiers() const override;
  void requestSimulationAbortion() override;
  void
  runNewSimulation(std::shared_ptr<ISimulationModel> simulationModel) override;

  std::shared_ptr<dynamicstate::IPublicDynamicStateMap>
  getSimulationDynamicStates() const override;
  std::set<std::shared_ptr<agents::IAgent4Engine>> getAgents() const override;
  std::set<LevelIdentifier> getLevelIdentifiers() const override;
  std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
  getLevels() const override;
  std::set<std::shared_ptr<agents::IAgent4Engine>>
  getAgents(const LevelIdentifier &level) const override;
  std::shared_ptr<environment::IEnvironment4Engine>
  getEnvironment() const override;

  std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
  disambiguation(
      std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
          transitoryDynamicState) const override;
};

} // namespace engine
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SEQUENTIALSIMULATIONENGINE_H
