#ifndef JAMFREE_MICROSCOPIC_REACTION_MICROSCOPIC_REACTION_MODEL_H
#define JAMFREE_MICROSCOPIC_REACTION_MICROSCOPIC_REACTION_MODEL_H

#include "../../kernel/include/agents/Interfaces.h"
#include "../include/agents/VehiclePublicLocalStateMicro.h"
#include "../include/influences/ChangeAcceleration.h"
#include "../include/influences/ChangeLane.h"
#include <memory>
#include <vector>

namespace jamfree {
namespace microscopic {
namespace reaction {

/**
 * @brief Reaction model for the microscopic level.
 *
 * This model applies influences produced by vehicle agents and updates
 * their states accordingly. It handles:
 * - Acceleration changes (from IDM)
 * - Lane changes (from MOBIL)
 * - Physics updates (position, speed)
 * - Collision detection
 * - State validation
 */
class MicroscopicReactionModel : public kernel::agents::IReactionModel {
public:
  /**
   * @brief Constructor.
   * @param dt Time step for physics integration (seconds)
   */
  explicit MicroscopicReactionModel(double dt = 0.1);

  /**
   * @brief React to influences and update states.
   *
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param influences Influences to react to
   */
  void react(kernel::agents::SimulationTimeStamp timeLowerBound,
             kernel::agents::SimulationTimeStamp timeUpperBound,
             const kernel::agents::InfluencesMap &influences) override;

  /**
   * @brief Set time step for physics integration.
   * @param dt Time step (seconds)
   */
  void setTimeStep(double dt) { m_dt = dt; }

  /**
   * @brief Get time step.
   * @return Time step (seconds)
   */
  double getTimeStep() const { return m_dt; }

private:
  double m_dt; // Time step for physics integration

  /**
   * @brief Apply acceleration changes.
   * @param influences All influences
   */
  void
  applyAccelerationChanges(const kernel::agents::InfluencesMap &influences);

  /**
   * @brief Apply lane changes.
   * @param influences All influences
   */
  void applyLaneChanges(const kernel::agents::InfluencesMap &influences);

  /**
   * @brief Update vehicle physics (position, speed).
   */
  void updatePhysics();

  /**
   * @brief Validate states and handle collisions.
   */
  void validateStates();

  /**
   * @brief Extract influences of a specific type.
   * @param influences All influences
   * @param category Category to extract
   * @return Influences of the specified category
   */
  std::vector<std::shared_ptr<kernel::agents::IInfluence>>
  extractInfluencesByCategory(const kernel::agents::InfluencesMap &influences,
                              const std::string &category);
};

} // namespace reaction
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_REACTION_MICROSCOPIC_REACTION_MODEL_H
