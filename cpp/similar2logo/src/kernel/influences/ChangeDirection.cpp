#include "kernel/influences/ChangeDirection.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

ChangeDirection::ChangeDirection(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double dd,
    std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : ChangeDirection(model::levels::LogoSimulationLevelList::LOGO,
                      timeLowerBound, timeUpperBound, dd, target) {}

ChangeDirection::ChangeDirection(
    const microkernel::LevelIdentifier &levelIdentifier,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double dd,
    std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      dd(dd), target(target) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
