#ifndef JAMFREE_MICROSCOPIC_DECISION_DMS_CONJUNCTION_DMS_H
#define JAMFREE_MICROSCOPIC_DECISION_DMS_CONJUNCTION_DMS_H

#include "../IDecisionMicroSubmodel.h"
#include <memory>
#include <vector>

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

/**
 * @brief Conjunction Decision Micro Sub-model.
 *
 * This composite DMS executes multiple sub-models in sequence.
 * All sub-models are executed regardless of their return values.
 *
 * This allows combining multiple behaviors (e.g., acceleration + lane change).
 */
class ConjunctionDMS : public IDecisionMicroSubmodel {
public:
  /**
   * @brief Constructor.
   */
  ConjunctionDMS() = default;

  /**
   * @brief Add a sub-model to the conjunction.
   * @param submodel Sub-model to add
   */
  void addSubmodel(std::shared_ptr<IDecisionMicroSubmodel> submodel);

  /**
   * @brief Manage decision by executing all sub-models.
   *
   * All sub-models are executed in the order they were added.
   *
   * @return true if at least one sub-model handled the situation
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

#endif // JAMFREE_MICROSCOPIC_DECISION_DMS_CONJUNCTION_DMS_H
