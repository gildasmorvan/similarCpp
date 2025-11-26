#ifndef JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_ACCELERATION_H
#define JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_ACCELERATION_H

#include "../../../../microkernel/include/influences/RegularInfluence.h"
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
 * the vehicle's acceleration for a specific vehicle.
 */
class ChangeAcceleration : public fr::univ_artois::lgi2a::similar::microkernel::
                               influences::RegularInfluence {
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
   * @param ownerId Identifier of the vehicle whose acceleration changes
   * @param acceleration Desired acceleration (m/s²)
   */
  ChangeAcceleration(kernel::agents::SimulationTimeStamp timeLowerBound,
                     kernel::agents::SimulationTimeStamp timeUpperBound,
                     const std::string &ownerId, double acceleration);

  /**
   * @brief Get desired acceleration.
   * @return Acceleration in m/s²
   */
  double getAcceleration() const { return m_acceleration; }

  /**
   * @brief Get the identifier of the target vehicle.
   * @return Vehicle owner ID
   */
  const std::string &getOwnerId() const { return m_ownerId; }

private:
  std::string m_ownerId;
  double m_acceleration; // m/s²
};

} // namespace influences
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_INFLUENCES_CHANGE_ACCELERATION_H
