#include "kernel/influences/PheromoneFieldUpdate.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

PheromoneFieldUpdate::PheromoneFieldUpdate(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound)
    : PheromoneFieldUpdate(model::levels::LogoSimulationLevelList::LOGO,
                           timeLowerBound, timeUpperBound) {}

PheromoneFieldUpdate::PheromoneFieldUpdate(
    const microkernel::LevelIdentifier &levelIdentifier,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
