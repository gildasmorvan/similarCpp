#ifndef JAMFREE_MICROSCOPIC_DECISION_VEHICLE_DECISION_MODEL_MICRO_H
#define JAMFREE_MICROSCOPIC_DECISION_VEHICLE_DECISION_MODEL_MICRO_H

#include "../../../kernel/include/agents/Interfaces.h"
#include "../agents/VehiclePerceivedDataMicro.h"
#include "../agents/VehiclePrivateLocalStateMicro.h"
#include "../agents/VehiclePublicLocalStateMicro.h"
#include "IDecisionMicroSubmodel.h"
#include <memory>

namespace jamfree {
namespace microscopic {
namespace decision {

/**
 * @brief Decision model for vehicles in the microscopic level.
 *
 * Following SIMILAR's architecture, this model orchestrates multiple
 * Decision Micro Sub-models (DMS) to produce vehicle behavior.
 *
 * The decision model delegates to a root DMS, which can be:
 * - A single DMS (e.g., ForwardAccelerationDMS)
 * - A composite DMS (e.g., ConjunctionDMS combining multiple behaviors)
 */
class VehicleDecisionModelMicro : public kernel::agents::IDecisionModel {
public:
  /**
   * @brief Constructor.
   * @param rootDMS Root decision micro sub-model
   */
  explicit VehicleDecisionModelMicro(
      std::shared_ptr<IDecisionMicroSubmodel> rootDMS);

  /**
   * @brief Make decisions and produce influences.
   *
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param globalState Global state of simulation
   * @param publicLocalState Public local state of agent
   * @param privateLocalState Private local state of agent
   * @param perceivedData Perceived data
   * @param producedInfluences Output: produced influences
   */
  kernel::agents::LevelIdentifier getLevel() const override;

  void decide(const kernel::agents::SimulationTimeStamp &timeLowerBound,
              const kernel::agents::SimulationTimeStamp &timeUpperBound,
              std::shared_ptr<kernel::agents::GlobalState> globalState,
              std::shared_ptr<kernel::agents::ILocalState> publicLocalState,
              std::shared_ptr<kernel::agents::ILocalState> privateLocalState,
              std::shared_ptr<kernel::agents::IPerceivedData> perceivedData,
              std::shared_ptr<kernel::agents::InfluencesMap> producedInfluences)
      override;

  /**
   * @brief Get the root DMS.
   * @return Root decision micro sub-model
   */
  IDecisionMicroSubmodel *getRootDMS() const { return m_root_dms.get(); }

private:
  std::shared_ptr<IDecisionMicroSubmodel> m_root_dms;
};

} // namespace decision
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_DECISION_VEHICLE_DECISION_MODEL_MICRO_H
