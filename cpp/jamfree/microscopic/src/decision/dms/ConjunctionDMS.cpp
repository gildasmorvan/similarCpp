#include "../../../include/decision/dms/ConjunctionDMS.h"

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

void ConjunctionDMS::addSubmodel(
    std::shared_ptr<IDecisionMicroSubmodel> submodel) {
  if (submodel) {
    m_submodels.push_back(submodel);
  }
}

bool ConjunctionDMS::manageDecision(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound,
    const agents::VehiclePublicLocalStateMicro &publicState,
    const agents::VehiclePrivateLocalStateMicro &privateState,
    const agents::VehiclePerceivedDataMicro &perceivedData,
    const kernel::agents::GlobalState &globalState,
    kernel::agents::InfluencesMap &producedInfluences) {
  bool anyHandled = false;

  // Execute all sub-models
  for (auto &submodel : m_submodels) {
    bool handled = submodel->manageDecision(
        timeLowerBound, timeUpperBound, publicState, privateState,
        perceivedData, globalState, producedInfluences);

    if (handled) {
      anyHandled = true;
    }
  }

  return anyHandled;
}

} // namespace dms
} // namespace decision
} // namespace microscopic
} // namespace jamfree
