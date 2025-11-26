#include "../../../include/decision/dms/SubsumptionDMS.h"

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

void SubsumptionDMS::addSubmodel(
    std::shared_ptr<IDecisionMicroSubmodel> submodel) {
  if (submodel) {
    m_submodels.push_back(submodel);
  }
}

bool SubsumptionDMS::manageDecision(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound,
    const agents::VehiclePublicLocalStateMicro &publicState,
    const agents::VehiclePrivateLocalStateMicro &privateState,
    const agents::VehiclePerceivedDataMicro &perceivedData,
    const kernel::agents::GlobalState &globalState,
    kernel::agents::InfluencesMap &producedInfluences) {
  // Execute sub-models in priority order
  // First one that handles the situation wins
  for (auto &submodel : m_submodels) {
    bool handled = submodel->manageDecision(
        timeLowerBound, timeUpperBound, publicState, privateState,
        perceivedData, globalState, producedInfluences);

    if (handled) {
      return true; // Stop here, this sub-model handled it
    }
  }

  return false; // No sub-model handled the situation
}

} // namespace dms
} // namespace decision
} // namespace microscopic
} // namespace jamfree
