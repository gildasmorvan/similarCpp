#include "libs/generic/EmptyAgtDecisionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace generic {

EmptyAgtDecisionModel::EmptyAgtDecisionModel(
    const microkernel::LevelIdentifier &levelIdentifier)
    : AbstractAgtDecisionModel(levelIdentifier) {}

void EmptyAgtDecisionModel::decide(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    std::shared_ptr<microkernel::agents::IGlobalState> globalState,
    std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
    std::shared_ptr<microkernel::agents::ILocalStateOfAgent> privateLocalState,
    std::shared_ptr<microkernel::agents::IPerceivedData> perceivedData,
    std::shared_ptr<microkernel::influences::InfluencesMap>
        producedInfluences) {
  // Do nothing
}

} // namespace generic
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
