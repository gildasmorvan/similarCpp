#ifndef JAMFREE_MICROSCOPIC_DECISION_I_DECISION_MICRO_SUBMODEL_H
#define JAMFREE_MICROSCOPIC_DECISION_I_DECISION_MICRO_SUBMODEL_H

#include "../../../kernel/include/agents/Interfaces.h"
#include "../agents/VehiclePerceivedDataMicro.h"
#include "../agents/VehiclePrivateLocalStateMicro.h"
#include "../agents/VehiclePublicLocalStateMicro.h"
#include <memory>

namespace jamfree {
namespace microscopic {
namespace decision {

/**
 * @brief Interface for Decision Micro Sub-models (DMS).
 *
 * Following SIMILAR's architecture, decision-making is decomposed into
 * elementary bricks called Decision Micro Sub-models (DMS). Each DMS
 * handles a specific aspect of behavior (e.g., acceleration, lane change).
 *
 * A DMS can either:
 * 1. Handle the situation and produce influences
 * 2. Indicate it cannot handle the situation (return false)
 */
class IDecisionMicroSubmodel {
public:
  virtual ~IDecisionMicroSubmodel() = default;

  /**
   * @brief Manage decision for this sub-model.
   *
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param publicState Public local state of vehicle
   * @param privateState Private local state of vehicle
   * @param perceivedData Perceived data from environment
   * @param globalState Global simulation state
   * @param producedInfluences Output: influences to emit
   * @return true if this DMS handled the situation, false otherwise
   */
  virtual bool
  manageDecision(kernel::agents::SimulationTimeStamp timeLowerBound,
                 kernel::agents::SimulationTimeStamp timeUpperBound,
                 const agents::VehiclePublicLocalStateMicro &publicState,
                 const agents::VehiclePrivateLocalStateMicro &privateState,
                 const agents::VehiclePerceivedDataMicro &perceivedData,
                 const kernel::agents::GlobalState &globalState,
                 kernel::agents::InfluencesMap &producedInfluences) = 0;
};

} // namespace decision
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_DECISION_I_DECISION_MICRO_SUBMODEL_H
