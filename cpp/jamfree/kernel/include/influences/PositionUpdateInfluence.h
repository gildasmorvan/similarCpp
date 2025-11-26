#ifndef JAMFREE_KERNEL_INFLUENCES_POSITION_UPDATE_INFLUENCE_H
#define JAMFREE_KERNEL_INFLUENCES_POSITION_UPDATE_INFLUENCE_H

#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../../microscopic/include/agents/VehiclePublicLocalStateMicro.h"
#include <memory>

namespace jamfree {
namespace kernel {
namespace influences {

/**
 * @brief Influence representing a direct position update (e.g. for
 * initialization or teleport).
 */
class PositionUpdateInfluence : public fr::univ_artois::lgi2a::similar::
                                    microkernel::influences::RegularInfluence {
public:
  static constexpr const char *CATEGORY = "position_update";

  /**
   * @brief Constructor.
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param x New X coordinate
   * @param y New Y coordinate
   * @param heading New heading (radians)
   * @param target Target vehicle state
   */
  PositionUpdateInfluence(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeLowerBound,
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeUpperBound,
      double x, double y, double heading,
      std::shared_ptr<
          jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
          target)
      : RegularInfluence(CATEGORY, CATEGORY, timeLowerBound, timeUpperBound),
        m_x(x), m_y(y), m_heading(heading), m_target(target) {}

  double getX() const { return m_x; }
  double getY() const { return m_y; }
  double getHeading() const { return m_heading; }
  std::shared_ptr<jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
  getTarget() const {
    return m_target;
  }

private:
  double m_x;
  double m_y;
  double m_heading;
  std::shared_ptr<jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
      m_target;
};

} // namespace influences
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_INFLUENCES_POSITION_UPDATE_INFLUENCE_H
