#ifndef MULTITHREADEDSIMULATIONENGINE_H
#define MULTITHREADEDSIMULATIONENGINE_H

#include "../ISimulationEngine.h"
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace engine {

/**
 * A multithreaded simulation engine that parallelizes agent perception and
 * decision phases.
 *
 * This engine divides agents across multiple threads for concurrent processing
 * during:
 * - Perception building
 * - Decision making
 *
 * The reaction phase remains sequential to ensure consistency.
 */
class MultiThreadedSimulationEngine : public ISimulationEngine {
private:
  /** Map of probes observing this simulation */
  std::map<std::string, std::shared_ptr<IProbe>> probes;

  /** Number of worker threads */
  size_t numThreads;

  /** Flag to abort simulation */
  std::atomic<bool> abortRequested{false};

  /** The current simulation model */
  std::shared_ptr<ISimulationModel> currentModel;

  // Cache for simulation components (retrieved from model)
  std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>> levels;
  std::shared_ptr<environment::IEnvironment4Engine> environment;
  std::set<std::shared_ptr<agents::IAgent4Engine>> agents;
  std::map<LevelIdentifier, std::set<std::shared_ptr<agents::IAgent4Engine>>>
      agentsByLevel;

  // Dynamic state
  std::shared_ptr<dynamicstate::IPublicDynamicStateMap> dynamicStates;

  // Simulation state
  SimulationTimeStamp currentTime;

public:
  /**
   * Creates a multithreaded simulation engine.
   * @param numThreads Number of worker threads (0 = auto-detect from hardware)
   */
  explicit MultiThreadedSimulationEngine(size_t numThreads = 0);

  virtual ~MultiThreadedSimulationEngine() = default;

  /**
   * {@inheritDoc}
   */
  void addProbe(const std::string &probeName,
                std::shared_ptr<IProbe> probe) override;

  /**
   * {@inheritDoc}
   */
  std::shared_ptr<IProbe> removeProbe(const std::string &identifier) override;

  /**
   * {@inheritDoc}
   */
  std::set<std::string> getProbesIdentifiers() const override;

  /**
   * {@inheritDoc}
   */
  void requestSimulationAbortion() override;

  /**
   * {@inheritDoc}
   */
  void runNewSimulation(std::shared_ptr<ISimulationModel> model) override;

  /**
   * {@inheritDoc}
   */
  void runSimulation(const SimulationTimeStamp &finalTime) override;

  // ISimulationEngine getters implementation
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
  makeRegularReaction(
      SimulationTimeStamp transitoryPeriodMin,
      SimulationTimeStamp transitoryPeriodMax,
      std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
          consistentDynamicState,
      std::set<std::shared_ptr<influences::IInfluence>>
          regularInfluencesOftransitoryStateDynamics,
      std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
          transitoryDynamicState) const;

  std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
  disambiguation(
      std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
          transitoryDynamicState) const override;

  std::shared_ptr<ISimulationEngine> clone() const override;

private:
  /**
   * Helper class to manage public dynamic state during simulation.
   */
  class PublicDynamicStateMap : public dynamicstate::IPublicDynamicStateMap {
  private:
    std::map<LevelIdentifier,
             std::shared_ptr<dynamicstate::IPublicLocalDynamicState>>
        states;

  public:
    void set(const LevelIdentifier &level,
             std::shared_ptr<dynamicstate::IPublicLocalDynamicState> state) {
      states[level] = state;
    }

    void put(std::shared_ptr<dynamicstate::IPublicLocalDynamicState> state)
        override {
      // Assuming state has getLevel() or similar?
      // IPublicLocalDynamicState has getLevel().
      states[state->getLevel()] = state;
    }

    std::shared_ptr<dynamicstate::IPublicLocalDynamicState>
    get(const LevelIdentifier &level) const override {
      auto it = states.find(level);
      if (it != states.end()) {
        return it->second;
      }
      return nullptr;
    }

    std::set<LevelIdentifier> keySet() const override {
      std::set<LevelIdentifier> keys;
      for (const auto &pair : states) {
        keys.insert(pair.first);
      }
      return keys;
    }
  };

  /**
   * Process agents in parallel for perception phase.
   */
  void parallelPerception(
      const std::vector<std::shared_ptr<agents::IAgent4Engine>> &agents,
      SimulationTimeStamp timeLowerBound, SimulationTimeStamp timeUpperBound,
      std::shared_ptr<dynamicstate::IPublicDynamicStateMap> consistentState);

  /**
   * Process agents in parallel for decision phase.
   */
  void parallelDecision(
      const std::vector<std::shared_ptr<agents::IAgent4Engine>> &agents,
      SimulationTimeStamp timeLowerBound, SimulationTimeStamp timeUpperBound,
      std::shared_ptr<dynamicstate::IPublicDynamicStateMap> transitoryState);

  /**
   * Worker function for parallel processing.
   */
  template <typename Func>
  void parallelProcess(
      const std::vector<std::shared_ptr<agents::IAgent4Engine>> &agents,
      Func processFunc);
};

} // namespace engine
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // MULTITHREADEDSIMULATIONENGINE_H
