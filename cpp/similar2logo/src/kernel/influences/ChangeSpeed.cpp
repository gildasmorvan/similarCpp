#include "kernel/influences/ChangeSpeed.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

ChangeSpeed::ChangeSpeed(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double ds,
    std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : ChangeSpeed(model::levels::LogoSimulationLevelList::LOGO, timeLowerBound,
                  timeUpperBound, ds, target) {}

ChangeSpeed::ChangeSpeed(
    const microkernel::LevelIdentifier &levelIdentifier,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double ds,
    std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      ds(ds), target(target) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
