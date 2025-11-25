#ifndef JAMFREE_MICROSCOPIC_DECISION_DMS_SUBSUMPTION_DMS_H
#define JAMFREE_MICROSCOPIC_DECISION_DMS_SUBSUMPTION_DMS_H

#include "../IDecisionMicroSubmodel.h"
#include <memory>
#include <vector>

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

/**
 * @brief Subsumption Decision Micro Sub-model.
 *
 * This composite DMS executes sub-models in priority order.
 * The first sub-model that handles the situation wins, and
 * subsequent sub-models are not executed.
 *
 * This implements a subsumption architecture where higher-priority
 * behaviors can override lower-priority ones.
 */
class SubsumptionDMS : public IDecisionMicroSubmodel {
public:
  /**
   * @brief Constructor.
   */
  SubsumptionDMS() = default;

  /**
   * @brief Add a sub-model to the subsumption hierarchy.
   *
   * Sub-models are executed in the order they are added.
   * Earlier sub-models have higher priority.
   *
   * @param submodel Sub-model to add
   */
  void addSubmodel(std::shared_ptr<IDecisionMicroSubmodel> submodel);

  /**
   * @brief Manage decision by executing sub-models until one handles it.
   *
   * Sub-models are executed in priority order (order added).
   * The first sub-model that returns true stops the execution.
   *
   * @return true if any sub-model handled the situation
   */
  bool
  manageDecision(kernel::agents::SimulationTimeStamp timeLowerBound,
                 kernel::agents::SimulationTimeStamp timeUpperBound,
                 const agents::VehiclePublicLocalStateMicro &publicState,
                 const agents::VehiclePrivateLocalStateMicro &privateState,
                 const agents::VehiclePerceivedDataMicro &perceivedData,
                 const kernel::agents::GlobalState &globalState,
                 kernel::agents::InfluencesMap &producedInfluences) override;

  /**
   * @brief Get number of sub-models.
   * @return Number of sub-models
   */
  size_t getSubmodelCount() const { return m_submodels.size(); }

private:
  std::vector<std::shared_ptr<IDecisionMicroSubmodel>> m_submodels;
};

} // namespace dms
} // namespace decision
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_DECISION_DMS_SUBSUMPTION_DMS_H
