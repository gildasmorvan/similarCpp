#include "libs/generic/EmptyAgtPerceptionModel.h"
#include "libs/generic/EmptyPerceivedData.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace generic {

EmptyAgtPerceptionModel::EmptyAgtPerceptionModel(
    const microkernel::LevelIdentifier &levelIdentifier)
    : AbstractAgtPerceptionModel(levelIdentifier) {}

std::shared_ptr<microkernel::agents::IPerceivedData>
EmptyAgtPerceptionModel::perceive(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    const std::map<microkernel::LevelIdentifier,
                   std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
        &publicLocalStates,
    std::shared_ptr<microkernel::agents::ILocalStateOfAgent> privateLocalState,
    std::shared_ptr<microkernel::dynamicstate::IPublicDynamicStateMap>
        dynamicStates) {

  return std::make_shared<microkernel::libs::generic::EmptyPerceivedData>(
      this->getLevel(), timeLowerBound, timeUpperBound);
}

} // namespace generic
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
