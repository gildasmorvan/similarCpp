#ifndef JAMFREE_KERNEL_INFLUENCES_ACCELERATION_INFLUENCE_H
#define JAMFREE_KERNEL_INFLUENCES_ACCELERATION_INFLUENCE_H

#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../../../microscopic/include/agents/VehiclePublicLocalStateMicro.h"
#include <memory>

namespace jamfree {
namespace kernel {
namespace influences {

/**
 * @brief Influence representing a request to change acceleration.
 */
class AccelerationInfluence : public fr::univ_artois::lgi2a::similar::
                                  microkernel::influences::RegularInfluence {
public:
  static constexpr const char *CATEGORY = "acceleration";

  /**
   * @brief Constructor.
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param acceleration Requested acceleration (m/s^2)
   * @param target Target vehicle state
   */
  AccelerationInfluence(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeLowerBound,
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeUpperBound,
      double acceleration,
      std::shared_ptr<
          jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
          target)
      : RegularInfluence(CATEGORY, target->getLevel(), timeLowerBound,
                         timeUpperBound),
        m_acceleration(acceleration), m_target(target) {}

  double getAcceleration() const { return m_acceleration; }
  std::shared_ptr<jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
  getTarget() const {
    return m_target;
  }

private:
  double m_acceleration;
  std::shared_ptr<jamfree::microscopic::agents::VehiclePublicLocalStateMicro>
      m_target;
};

} // namespace influences
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_INFLUENCES_ACCELERATION_INFLUENCE_H
