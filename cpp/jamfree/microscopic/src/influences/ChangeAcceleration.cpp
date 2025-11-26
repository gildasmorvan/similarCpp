#include "../../include/influences/ChangeAcceleration.h"

namespace jamfree {
namespace microscopic {
namespace influences {

const std::string ChangeAcceleration::CATEGORY = "ChangeAcceleration";

ChangeAcceleration::ChangeAcceleration(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound,
    const std::string &ownerId, double acceleration)
    : RegularInfluence(
          CATEGORY, kernel::agents::LevelIdentifier("Microscopic"),
          timeLowerBound, timeUpperBound),
      m_ownerId(ownerId), m_acceleration(acceleration) {}

} // namespace influences
} // namespace microscopic
} // namespace jamfree
