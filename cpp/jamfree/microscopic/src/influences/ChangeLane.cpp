#include "../../include/influences/ChangeLane.h"

namespace jamfree {
namespace microscopic {
namespace influences {

const std::string ChangeLane::CATEGORY = "ChangeLane";

ChangeLane::ChangeLane(kernel::agents::SimulationTimeStamp timeLowerBound,
                       kernel::agents::SimulationTimeStamp timeUpperBound,
                       const std::string &ownerId, Direction direction)
    : RegularInfluence(
          CATEGORY, kernel::agents::LevelIdentifier("Microscopic"),
          timeLowerBound, timeUpperBound),
      m_ownerId(ownerId), m_direction(direction) {}

} // namespace influences
} // namespace microscopic
} // namespace jamfree
