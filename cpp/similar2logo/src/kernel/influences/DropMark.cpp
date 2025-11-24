#include "kernel/influences/DropMark.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

DropMark::DropMark(const microkernel::SimulationTimeStamp &timeLowerBound,
                   const microkernel::SimulationTimeStamp &timeUpperBound,
                   std::shared_ptr<model::environment::SimpleMark> mark)
    : DropMark(model::levels::LogoSimulationLevelList::LOGO, timeLowerBound,
               timeUpperBound, mark) {}

DropMark::DropMark(const microkernel::LevelIdentifier &levelIdentifier,
                   const microkernel::SimulationTimeStamp &timeLowerBound,
                   const microkernel::SimulationTimeStamp &timeUpperBound,
                   std::shared_ptr<model::environment::SimpleMark> mark)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      mark(mark) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
