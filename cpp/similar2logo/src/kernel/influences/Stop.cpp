#include "kernel/influences/Stop.h"
#include "kernel/model/levels/LogoSimulationLevelList.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

Stop::Stop(const microkernel::SimulationTimeStamp &timeLowerBound,
           const microkernel::SimulationTimeStamp &timeUpperBound,
           std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : Stop(model::levels::LogoSimulationLevelList::LOGO, timeLowerBound,
           timeUpperBound, target) {}

Stop::Stop(const microkernel::LevelIdentifier &levelIdentifier,
           const microkernel::SimulationTimeStamp &timeLowerBound,
           const microkernel::SimulationTimeStamp &timeUpperBound,
           std::shared_ptr<model::environment::TurtlePLSInLogo> target)
    : RegularInfluence(CATEGORY, levelIdentifier, timeLowerBound,
                       timeUpperBound),
      target(target) {}

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
