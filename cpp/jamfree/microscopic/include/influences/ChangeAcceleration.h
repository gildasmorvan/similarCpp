#ifndef JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_ACCELERATION_H
#define JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_ACCELERATION_H

#include "../../../kernel/include/agents/Interfaces.h"
#include <string>

namespace jamfree {
namespace microscopic {
namespace influences {

/**
 * @brief Influence to change vehicle acceleration.
 *
 * This influence is emitted by the decision model (typically from
 * the Forward Acceleration DMS using IDM) to request a change in
 * the vehicle's acceleration.
 */
class ChangeAcceleration : public kernel::agents::RegularInfluence {
public:
  /**
   * @brief Category identifier for this influence type.
   */
  static const std::string CATEGORY;

  /**
   * @brief Constructor.
   *
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param acceleration Desired acceleration (m/s²)
   */
  ChangeAcceleration(kernel::agents::SimulationTimeStamp timeLowerBound,
                     kernel::agents::SimulationTimeStamp timeUpperBound,
                     double acceleration);

  /**
   * @brief Get desired acceleration.
   * @return Acceleration in m/s²
   */
  double getAcceleration() const { return m_acceleration; }

private:
  double m_acceleration; // m/s²
};

} // namespace influences
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_ACCELERATION_H
