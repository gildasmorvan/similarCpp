#include "libs/endcriterion/TimeBasedEndCriterion.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace endcriterion {

TimeBasedEndCriterion::TimeBasedEndCriterion(
    const microkernel::SimulationTimeStamp &finalTimeStamp)
    : finalTimeStamp(finalTimeStamp) {}

bool TimeBasedEndCriterion::isFinalTimeOrAfter(
    const microkernel::SimulationTimeStamp &currentTime,
    const microkernel::ISimulationEngine *engine) const {

  return finalTimeStamp.compareToTimeStamp(currentTime) <= 0;
}

} // namespace endcriterion
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
