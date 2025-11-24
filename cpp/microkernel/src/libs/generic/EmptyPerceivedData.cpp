#include "../../../include/libs/generic/EmptyPerceivedData.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace generic {

EmptyPerceivedData::EmptyPerceivedData(
    const LevelIdentifier &levelIdentifier,
    const SimulationTimeStamp &transitoryPeriodMin,
    const SimulationTimeStamp &transitoryPeriodMax)
    : AbstractPerceivedData(levelIdentifier, transitoryPeriodMin,
                            transitoryPeriodMax) {}

} // namespace generic
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
