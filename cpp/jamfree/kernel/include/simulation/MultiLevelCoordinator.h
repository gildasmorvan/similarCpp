#ifndef JAMFREE_KERNEL_SIMULATION_MULTI_LEVEL_COORDINATOR_H
#define JAMFREE_KERNEL_SIMULATION_MULTI_LEVEL_COORDINATOR_H

#include "../agents/Interfaces.h"
#include "../agents/VehicleAgent.h"
#include "SimulationEngine.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace jamfree {
namespace kernel {
namespace simulation {

/**
 * @brief Multi-level simulation coordinator.
 *
 * Coordinates simulation across multiple levels (microscopic, macroscopic,
 * control) with different time scales and state synchronization.
 *
 * Features:
 * - Level-specific time steps
 * - State synchronization between levels
 * - Level transitions (micro ↔ macro)
 * - Hierarchical control
 */
class MultiLevelCoordinator {
public:
  /**
   * @brief Level configuration.
   */
  struct LevelConfig {
    agents::LevelIdentifier level;
    double time_step;     // Time step for this level (seconds)
    int update_frequency; // How often to update (1 = every step)
    bool active;          // Is this level active?

    LevelConfig(const agents::LevelIdentifier &lvl, double dt = 0.1,
                int freq = 1)
        : level(lvl), time_step(dt), update_frequency(freq), active(true) {}
  };

  /**
   * @brief Constructor.
   */
  MultiLevelCoordinator();

  /**
   * @brief Add a level to the simulation.
   * @param config Level configuration
   */
  void addLevel(const LevelConfig &config);

  /**
   * @brief Set simulation engine.
   * @param engine Simulation engine
   */
  void setSimulationEngine(std::shared_ptr<SimulationEngine> engine) {
    m_engine = engine;
  }

  /**
   * @brief Get simulation engine.
   * @return Simulation engine
   */
  SimulationEngine *getSimulationEngine() { return m_engine.get(); }

  /**
   * @brief Run one coordinated step across all levels.
   *
   * Executes levels according to their update frequencies and
   * synchronizes state between levels.
   */
  void step();

  /**
   * @brief Run multiple coordinated steps.
   * @param numSteps Number of steps to run
   */
  void run(int numSteps);

  /**
   * @brief Transition an agent from one level to another.
   * @param agentId Agent ID
   * @param fromLevel Source level
   * @param toLevel Target level
   */
  void transitionAgent(const std::string &agentId,
                       const agents::LevelIdentifier &fromLevel,
                       const agents::LevelIdentifier &toLevel);

  /**
   * @brief Synchronize state between levels.
   *
   * Updates states across levels to maintain consistency.
   */
  void synchronizeLevels();

  /**
   * @brief Get current time.
   * @return Current simulation time
   */
  double getCurrentTime() const { return m_current_time; }

  /**
   * @brief Get step count.
   * @return Number of steps executed
   */
  int getStepCount() const { return m_step_count; }

  /**
   * @brief Reset coordinator.
   */
  void reset();

private:
  std::shared_ptr<SimulationEngine> m_engine;

  double m_current_time;
  int m_step_count;

  // Level configurations
  std::unordered_map<agents::LevelIdentifier, LevelConfig,
                     agents::LevelIdentifierHash>
      m_levels;

  // Track which agents are in which levels
  std::unordered_map<std::string, std::vector<agents::LevelIdentifier>>
      m_agent_levels;

  /**
   * @brief Check if a level should update this step.
   * @param level Level identifier
   * @return True if level should update
   */
  bool shouldUpdateLevel(const agents::LevelIdentifier &level);

  /**
   * @brief Update a specific level.
   * @param level Level identifier
   */
  void updateLevel(const agents::LevelIdentifier &level);
};

} // namespace simulation
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_SIMULATION_MULTI_LEVEL_COORDINATOR_H
