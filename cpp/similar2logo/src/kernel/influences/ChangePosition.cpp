#include "kernel/influences/ChangePosition.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

ChangePosition::ChangePosition(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double dx,
    double dy, std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : ChangePosition(model::levels::LogoSimulationLevelList::LOGO,
                     timeLowerBound, timeUpperBound, dx, dy, target) {}

ChangePosition::ChangePosition(
    const microkernel::LevelIdentifier &levelIdentifier,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound, double dx,
    double dy, std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      dx(dx), dy(dy), target(target) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
