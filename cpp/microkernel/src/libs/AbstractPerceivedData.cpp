#include "../../include/libs/AbstractPerceivedData.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {

AbstractPerceivedData::AbstractPerceivedData(
    const LevelIdentifier &levelIdentifier,
    const SimulationTimeStamp &transitoryPeriodMin,
    const SimulationTimeStamp &transitoryPeriodMax)
    : levelIdentifier(levelIdentifier),
      transitoryPeriodMin(transitoryPeriodMin),
      transitoryPeriodMax(transitoryPeriodMax) {}

LevelIdentifier AbstractPerceivedData::getLevel() const {
  return levelIdentifier;
}

SimulationTimeStamp AbstractPerceivedData::getTransitoryPeriodMin() const {
  return transitoryPeriodMin;
}

SimulationTimeStamp AbstractPerceivedData::getTransitoryPeriodMax() const {
  return transitoryPeriodMax;
}

} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
