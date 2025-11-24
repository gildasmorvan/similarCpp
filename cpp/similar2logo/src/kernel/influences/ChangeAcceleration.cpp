#include "kernel/influences/ChangeAcceleration.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

ChangeAcceleration::ChangeAcceleration(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double da,
    std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : ChangeAcceleration(model::levels::LogoSimulationLevelList::LOGO,
                         timeLowerBound, timeUpperBound, da, target) {}

ChangeAcceleration::ChangeAcceleration(
    const microkernel::LevelIdentifier &levelIdentifier,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double da,
    std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      da(da), target(target) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
