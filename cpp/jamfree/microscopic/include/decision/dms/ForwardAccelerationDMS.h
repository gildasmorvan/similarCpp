#ifndef JAMFREE_MICROSCOPIC_DECISION_DMS_FORWARD_ACCELERATION_DMS_H
#define JAMFREE_MICROSCOPIC_DECISION_DMS_FORWARD_ACCELERATION_DMS_H

#include "../../../include/IDM.h"
#include "../IDecisionMicroSubmodel.h"
#include <memory>

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

/**
 * @brief Forward Acceleration Decision Micro Sub-model.
 *
 * This DMS uses the Intelligent Driver Model (IDM) to calculate
 * the desired acceleration for car-following behavior.
 *
 * It emits a ChangeAcceleration influence.
 */
class ForwardAccelerationDMS : public IDecisionMicroSubmodel {
public:
  /**
   * @brief Constructor.
   * @param idm IDM model for acceleration calculation
   */
  explicit ForwardAccelerationDMS(std::shared_ptr<models::IDM> idm);

  /**
   * @brief Manage decision for forward acceleration.
   *
   * Uses IDM to calculate acceleration based on:
   * - Current speed
   * - Desired speed
   * - Gap to leader
   * - Leader speed
   *
   * @return Always true (this DMS always handles the situation)
   */
  bool
  manageDecision(kernel::agents::SimulationTimeStamp timeLowerBound,
                 kernel::agents::SimulationTimeStamp timeUpperBound,
                 const agents::VehiclePublicLocalStateMicro &publicState,
                 const agents::VehiclePrivateLocalStateMicro &privateState,
                 const agents::VehiclePerceivedDataMicro &perceivedData,
                 const kernel::agents::GlobalState &globalState,
                 kernel::agents::InfluencesMap &producedInfluences) override;

private:
  std::shared_ptr<models::IDM> m_idm;
};

} // namespace dms
} // namespace decision
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_DECISION_DMS_FORWARD_ACCELERATION_DMS_H
