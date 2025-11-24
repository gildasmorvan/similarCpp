#include "kernel/influences/EmitPheromone.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

EmitPheromone::EmitPheromone(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    const tools::Point2D &location, const std::string &pheromoneIdentifier,
    double value)
    : EmitPheromone(model::levels::LogoSimulationLevelList::LOGO,
                    timeLowerBound, timeUpperBound, location,
                    pheromoneIdentifier, value) {}

EmitPheromone::EmitPheromone(
    const microkernel::LevelIdentifier &levelIdentifier,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    const tools::Point2D &location, const std::string &pheromoneIdentifier,
    double value)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      pheromoneIdentifier(pheromoneIdentifier), location(location),
      value(value) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
