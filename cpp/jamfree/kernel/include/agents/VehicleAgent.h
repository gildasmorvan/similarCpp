#ifndef JAMFREE_KERNEL_AGENTS_VEHICLE_AGENT_H
#define JAMFREE_KERNEL_AGENTS_VEHICLE_AGENT_H

#include "../../../../extendedkernel/include/agents/ExtendedAgent.h"
#include "Interfaces.h"
#include <memory>
#include <string>

namespace jamfree {
namespace kernel {
namespace agents {

// SIMILAR types are aliased in Interfaces.h

/**
 * @brief Vehicle agent following SIMILAR's agent-based architecture.
 *
 * This class represents a vehicle as an agent with:
 * - Multi-level support (microscopic, macroscopic, control)
 * - Public and private local states
 * - Perception and decision models
 * - Influence emission
 *
 * Inherits from SIMILAR's ExtendedAgent to leverage the core kernel
 * capabilities.
 */
class VehicleAgent : public fr::univ_artois::lgi2a::similar::extendedkernel::
                         agents::ExtendedAgent {
public:
  /**
   * @brief Constructor.
   * @param id Unique identifier for the agent
   */
  explicit VehicleAgent(const std::string &id);

  /**
   * @brief Destructor.
   */
  virtual ~VehicleAgent() = default;

  /**
   * @brief Get agent ID.
   * @return Agent identifier
   */
  const std::string &getId() const { return m_id; }

  /**
   * @brief Add a simulation level to this agent.
   * @param level Level identifier
   */
  void addLevel(const LevelIdentifier &level);

  /**
   * @brief Check if agent participates in a level.
   * @param level Level identifier
   * @return True if agent is in this level
   */
  bool hasLevel(const LevelIdentifier &level) const;

  /**
   * @brief Get public local state for a level.
   * @param level Level identifier
   * @return Public local state or nullptr
   */
  std::shared_ptr<ILocalState>
  getPublicLocalState(const LevelIdentifier &level) const;

  /**
   * @brief Set public local state for a level.
   * @param level Level identifier
   * @param state New public local state
   */
  void setPublicLocalState(const LevelIdentifier &level,
                           std::shared_ptr<ILocalState> state);

  /**
   * @brief Get private local state for a level.
   * @param level Level identifier
   * @return Private local state or nullptr
   */
  std::shared_ptr<ILocalState>
  getPrivateLocalState(const LevelIdentifier &level) const;

  /**
   * @brief Set private local state for a level.
   * @param level Level identifier
   * @param state Private local state
   */
  void setPrivateLocalState(const LevelIdentifier &level,
                            std::shared_ptr<ILocalState> state);

  /**
   * @brief Set perception model for a level.
   * @param level Level identifier
   * @param model Perception model
   */
  void setPerceptionModel(const LevelIdentifier &level,
                          std::shared_ptr<IPerceptionModel> model);

  /**
   * @brief Get perception model for a level.
   * @param level Level identifier
   * @return Perception model
   */
  IPerceptionModel *getPerceptionModel(const LevelIdentifier &level);

  /**
   * @brief Set decision model for a level.
   * @param level Level identifier
   * @param model Decision model
   */
  void setDecisionModel(const LevelIdentifier &level,
                        std::shared_ptr<IDecisionModel> model);

  /**
   * @brief Get decision model for a level.
   * @param level Level identifier
   * @return Decision model
   */
  IDecisionModel *getDecisionModel(const LevelIdentifier &level);

  /**
   * @brief Set both perception and decision models for a level.
   * @param level Level identifier
   * @param perceptionModel Perception model
   * @param decisionModel Decision model
   */
  void setModels(const LevelIdentifier &level,
                 std::shared_ptr<IPerceptionModel> perceptionModel,
                 std::shared_ptr<IDecisionModel> decisionModel);

private:
  std::string m_id;
};

} // namespace agents
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_AGENTS_VEHICLE_AGENT_H
