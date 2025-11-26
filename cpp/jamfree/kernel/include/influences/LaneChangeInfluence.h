#ifndef JAMFREE_KERNEL_INFLUENCES_LANE_CHANGE_INFLUENCE_H
#define JAMFREE_KERNEL_INFLUENCES_LANE_CHANGE_INFLUENCE_H

#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../../../microscopic/include/agents/VehiclePublicLocalStateMicro.h"
#include <memory>

namespace jamfree {
namespace kernel {
namespace influences {

/**
 * @brief Influence representing a request to change lanes.
 */
class LaneChangeInfluence : public fr::univ_artois::lgi2a::similar::
                                microkernel::influences::RegularInfluence {
public:
  static constexpr const char *CATEGORY = "lane_change";

  enum class Direction { NONE = 0, LEFT = 1, RIGHT = -1 };

  /**
   * @brief Constructor.
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param direction Requested lane change direction
   * @param target Target vehicle state
   */
  LaneChangeInfluence(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeLowerBound,
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeUpperBound,
      Direction direction,
      std::shared_ptr<
          jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
          target)
      : RegularInfluence(CATEGORY, target->getLevel(), timeLowerBound,
                         timeUpperBound),
        m_direction(direction), m_target(target) {}

  Direction getDirection() const { return m_direction; }
  std::shared_ptr<jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
  getTarget() const {
    return m_target;
  }

private:
  Direction m_direction;
  std::shared_ptr<jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
      m_target;
};

} // namespace influences
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_INFLUENCES_LANE_CHANGE_INFLUENCE_H
