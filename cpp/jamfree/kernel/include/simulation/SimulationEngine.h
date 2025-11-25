#ifndef JAMFREE_KERNEL_SIMULATION_SIMULATION_ENGINE_H
#define JAMFREE_KERNEL_SIMULATION_SIMULATION_ENGINE_H

#include "../agents/Interfaces.h"
#include "../agents/VehicleAgent.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace jamfree {
namespace kernel {
namespace simulation {

/**
 * @brief Simulation engine for SIMILAR-based multi-agent simulation.
 *
 * This engine orchestrates the complete simulation cycle:
 * 1. Perception phase - All agents perceive their environment
 * 2. Decision phase - All agents make decisions
 * 3. Reaction phase - Apply influences and update states
 *
 * Supports multi-level simulation with different levels running
 * at different time scales.
 */
class SimulationEngine {
public:
  /**
   * @brief Constructor.
   * @param dt Default time step (seconds)
   */
  explicit SimulationEngine(double dt = 0.1);

  /**
   * @brief Add an agent to the simulation.
   * @param agent Agent to add
   */
  void addAgent(std::shared_ptr<agents::VehicleAgent> agent);

  /**
   * @brief Remove an agent from the simulation.
   * @param agentId ID of agent to remove
   */
  void removeAgent(const std::string &agentId);

  /**
   * @brief Get an agent by ID.
   * @param agentId Agent ID
   * @return Agent or nullptr if not found
   */
  std::shared_ptr<agents::VehicleAgent> getAgent(const std::string &agentId);

  /**
   * @brief Get all agents.
   * @return Vector of all agents
   */
  const std::vector<std::shared_ptr<agents::VehicleAgent>> &getAgents() const {
    return m_agents;
  }

  /**
   * @brief Set reaction model for a level.
   * @param level Level identifier
   * @param reactionModel Reaction model
   */
  void setReactionModel(const agents::LevelIdentifier &level,
                        std::shared_ptr<agents::IReactionModel> reactionModel);

  /**
   * @brief Get reaction model for a level.
   * @param level Level identifier
   * @return Reaction model or nullptr
   */
  agents::IReactionModel *
  getReactionModel(const agents::LevelIdentifier &level);

  /**
   * @brief Run one simulation step.
   *
   * Executes the complete perception-decision-reaction cycle
   * for all agents at all levels.
   */
  void step();

  /**
   * @brief Run multiple simulation steps.
   * @param numSteps Number of steps to run
   */
  void run(int numSteps);

  /**
   * @brief Get current simulation time.
   * @return Current time (seconds)
   */
  double getCurrentTime() const { return m_current_time; }

  /**
   * @brief Get time step.
   * @return Time step (seconds)
   */
  double getTimeStep() const { return m_dt; }

  /**
   * @brief Set time step.
   * @param dt Time step (seconds)
   */
  void setTimeStep(double dt) { m_dt = dt; }

  /**
   * @brief Get step count.
   * @return Number of steps executed
   */
  int getStepCount() const { return m_step_count; }

  /**
   * @brief Reset simulation.
   */
  void reset();

private:
  double m_dt;           // Time step
  double m_current_time; // Current simulation time
  int m_step_count;      // Number of steps executed

  // Agents
  std::vector<std::shared_ptr<agents::VehicleAgent>> m_agents;
  std::unordered_map<std::string, std::shared_ptr<agents::VehicleAgent>>
      m_agents_by_id;

  // Reaction models per level
  std::unordered_map<agents::LevelIdentifier,
                     std::shared_ptr<agents::IReactionModel>>
      m_reaction_models;

  // Global state
  class SimulationGlobalState : public agents::GlobalState {
  public:
    SimulationGlobalState(double time) : m_time(time) {}
    agents::SimulationTimeStamp getCurrentTime() const {
      return agents::SimulationTimeStamp(m_time);
    }
    void setTime(double time) { m_time = time; }

  private:
    double m_time;
  };

  std::shared_ptr<SimulationGlobalState> m_global_state;

  /**
   * @brief Execute perception phase for all agents.
   */
  void perceptionPhase();

  /**
   * @brief Execute decision phase for all agents.
   * @return All influences produced
   */
  agents::InfluencesMap decisionPhase();

  /**
   * @brief Execute reaction phase.
   * @param influences Influences to apply
   */
  void reactionPhase(const agents::InfluencesMap &influences);
};

} // namespace simulation
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_SIMULATION_SIMULATION_ENGINE_H
