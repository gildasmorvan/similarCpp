#include "kernel/influences/AgentPositionUpdate.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

AgentPositionUpdate::AgentPositionUpdate(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound)
    : AgentPositionUpdate(model::levels::LogoSimulationLevelList::LOGO,
                          timeLowerBound, timeUpperBound) {}

AgentPositionUpdate::AgentPositionUpdate(
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
