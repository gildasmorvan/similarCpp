#include "kernel/influences/RemoveMarks.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

RemoveMarks::RemoveMarks(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    const std::unordered_set<std::shared_ptr<model::environment::SimpleMark>>
        &marks)
    : RemoveMarks(model::levels::LogoSimulationLevelList::LOGO, timeLowerBound,
                  timeUpperBound, marks) {}

RemoveMarks::RemoveMarks(
    const microkernel::LevelIdentifier &levelIdentifier,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    const std::unordered_set<std::shared_ptr<model::environment::SimpleMark>>
        &marks)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      marks(marks) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
