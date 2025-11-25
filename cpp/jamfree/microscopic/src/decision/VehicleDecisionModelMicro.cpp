#include "../include/decision/VehicleDecisionModelMicro.h"
#include <stdexcept>

namespace jamfree {
namespace microscopic {
namespace decision {

VehicleDecisionModelMicro::VehicleDecisionModelMicro(
    std::shared_ptr<IDecisionMicroSubmodel> rootDMS)
    : m_root_dms(rootDMS) {
  if (!m_root_dms) {
    throw std::invalid_argument(
        "VehicleDecisionModelMicro: root DMS cannot be null");
  }
}

kernel::agents::LevelIdentifier VehicleDecisionModelMicro::getLevel() const {
  return kernel::agents::LevelIdentifier("Microscopic");
}

void VehicleDecisionModelMicro::decide(
    const kernel::agents::SimulationTimeStamp &timeLowerBound,
    const kernel::agents::SimulationTimeStamp &timeUpperBound,
    std::shared_ptr<kernel::agents::GlobalState> globalState,
    std::shared_ptr<kernel::agents::ILocalState> publicLocalState,
    std::shared_ptr<kernel::agents::ILocalState> privateLocalState,
    std::shared_ptr<kernel::agents::IPerceivedData> perceivedData,
    std::shared_ptr<kernel::agents::InfluencesMap> producedInfluences) {

  // Cast to specific types
  auto publicState =
      std::dynamic_pointer_cast<agents::VehiclePublicLocalStateMicro>(
          publicLocalState);
  auto privateState =
      std::dynamic_pointer_cast<agents::VehiclePrivateLocalStateMicro>(
          privateLocalState);
  auto pData = std::dynamic_pointer_cast<agents::VehiclePerceivedDataMicro>(
      perceivedData);

  if (!publicState || !privateState || !pData) {
    throw std::runtime_error(
        "Invalid state/data types in VehicleDecisionModelMicro");
  }

  // Delegate to root DMS
  if (m_root_dms) {
    m_root_dms->decide(timeLowerBound, timeUpperBound, *publicState,
                       *privateState, *pData, *producedInfluences);
  }
}
} // namespace decision
} // namespace microscopic
} // namespace jamfree
