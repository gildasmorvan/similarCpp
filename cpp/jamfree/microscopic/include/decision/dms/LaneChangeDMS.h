#ifndef JAMFREE_MICROSCOPIC_DECISION_DMS_LANE_CHANGE_DMS_H
#define JAMFREE_MICROSCOPIC_DECISION_DMS_LANE_CHANGE_DMS_H

#include "../../../include/IDM.h"
#include "../../../include/MOBIL.h"
#include "../IDecisionMicroSubmodel.h"
#include <memory>

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

/**
 * @brief Lane Change Decision Micro Sub-model.
 *
 * This DMS uses the MOBIL (Minimizing Overall Braking Induced by Lane changes)
 * model to decide whether to change lanes.
 *
 * It emits a ChangeLane influence if a lane change is beneficial and safe.
 */
class LaneChangeDMS : public IDecisionMicroSubmodel {
public:
  /**
   * @brief Constructor.
   * @param mobil MOBIL model for lane change decisions
   * @param idm IDM model for acceleration calculations
   */
  LaneChangeDMS(std::shared_ptr<models::MOBIL> mobil,
                std::shared_ptr<models::IDM> idm);

  /**
   * @brief Manage decision for lane changing.
   *
   * Uses MOBIL to evaluate lane change opportunities:
   * - Checks if adjacent lanes exist
   * - Evaluates safety criterion
   * - Evaluates incentive criterion
   * - Considers politeness factor
   * - Applies right-lane bias
   *
   * @return true if handled (lanes available), false if no lanes to change to
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
  std::shared_ptr<models::MOBIL> m_mobil;
  std::shared_ptr<models::IDM> m_idm;

  /**
   * @brief Evaluate lane change to a specific direction.
   *
   * @param direction LEFT or RIGHT
   * @param publicState Vehicle public state
   * @param privateState Vehicle private state
   * @param perceivedData Perceived data
   * @return Incentive value (positive = beneficial)
   */
  double
  evaluateLaneChange(models::MOBIL::Direction direction,
                     const agents::VehiclePublicLocalStateMicro &publicState,
                     const agents::VehiclePrivateLocalStateMicro &privateState,
                     const agents::VehiclePerceivedDataMicro &perceivedData);
};

} // namespace dms
} // namespace decision
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_DECISION_DMS_LANE_CHANGE_DMS_H
