#ifndef JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_LANE_H
#define JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_LANE_H

#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../../../kernel/include/agents/Interfaces.h"
#include <string>

namespace jamfree {
namespace microscopic {
namespace influences {

/**
 * @brief Influence to change lane.
 *
 * This influence is emitted by the decision model (typically from
 * the Lane Change DMS using MOBIL) to request a lane change for a
 * specific vehicle.
 */
class ChangeLane : public fr::univ_artois::lgi2a::similar::microkernel::
                       influences::RegularInfluence {
public:
  /**
   * @brief Lane change direction.
   */
  enum class Direction {
    LEFT, ///< Change to left lane
    RIGHT ///< Change to right lane
  };

  /**
   * @brief Category identifier for this influence type.
   */
  static const std::string CATEGORY;

  /**
   * @brief Constructor.
   *
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param ownerId Identifier of the vehicle that should change lane
   * @param direction Lane change direction
   */
  ChangeLane(kernel::agents::SimulationTimeStamp timeLowerBound,
             kernel::agents::SimulationTimeStamp timeUpperBound,
             const std::string &ownerId, Direction direction);

  /**
   * @brief Get lane change direction.
   * @return Direction (LEFT or RIGHT)
   */
  Direction getDirection() const { return m_direction; }

  /**
   * @brief Get the identifier of the target vehicle.
   * @return Vehicle owner ID
   */
  const std::string &getOwnerId() const { return m_ownerId; }

private:
  std::string m_ownerId;
  Direction m_direction;
};

} // namespace influences
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_LANE_H
