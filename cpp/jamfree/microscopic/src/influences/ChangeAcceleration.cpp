#include "../include/influences/ChangeAcceleration.h"

namespace jamfree {
namespace microscopic {
namespace influences {

const std::string ChangeAcceleration::CATEGORY = "ChangeAcceleration";

ChangeAcceleration::ChangeAcceleration(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound, double acceleration)
    : RegularInfluence(timeLowerBound, timeUpperBound, CATEGORY),
      m_acceleration(acceleration) {}

} // namespace influences
} // namespace microscopic
} // namespace jamfree
