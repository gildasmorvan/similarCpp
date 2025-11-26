#include "../../include/influences/ChangeLane.h"

namespace jamfree {
namespace microscopic {
namespace influences {

const std::string ChangeLane::CATEGORY = "ChangeLane";

ChangeLane::ChangeLane(kernel::agents::SimulationTimeStamp timeLowerBound,
                       kernel::agents::SimulationTimeStamp timeUpperBound,
                       Direction direction)
    : RegularInfluence(timeLowerBound, timeUpperBound, CATEGORY),
      m_direction(direction) {}

} // namespace influences
} // namespace microscopic
} // namespace jamfree
