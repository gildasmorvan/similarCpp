#ifndef ILEVELREACTIONMODEL_H
#define ILEVELREACTIONMODEL_H

#include "../../microkernel/include/SimulationTimeStamp.h"
#include "../../microkernel/include/dynamicstate/ConsistentPublicLocalDynamicState.h"
#include "../../microkernel/include/influences/IInfluence.h"
#include "../../microkernel/include/influences/InfluencesMap.h"
#include <memory>
#include <set>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace levels {

/**
 * Models the reaction process used by a level to react to the influences it
 * received.
 */
class ILevelReactionModel {
public:
  virtual ~ILevelReactionModel() = default;

  /**
   * Performs a user-defined reaction to the regular influences.
   *
   * This method must:
   * - Update the local state of the environment and agents in consistentState
   * - Add persisting influences to remainingInfluences
   * - Add new influences produced by this reaction to remainingInfluences
   *
   * @param transitoryTimeMin Lower bound of the transitory period
   * @param transitoryTimeMax Upper bound of the transitory period
   * @param consistentState The consistent state being updated
   * @param regularInfluencesOftransitoryStateDynamics Regular influences to
   * manage
   * @param remainingInfluences Influences that persist or are produced
   */
  virtual void makeRegularReaction(
      const microkernel::SimulationTimeStamp &transitoryTimeMin,
      const microkernel::SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::set<std::shared_ptr<microkernel::influences::IInfluence>>
          &regularInfluencesOftransitoryStateDynamics,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          remainingInfluences) = 0;

  /**
   * Performs a user-defined reaction to the system influences.
   *
   * This method is called twice during the reaction phase:
   * 1. After system reaction to influences at the beginning
   * 2. After user-defined reaction to regular influences
   *
   * @param transitoryTimeMin Lower bound of the transitory period
   * @param transitoryTimeMax Upper bound of the transitory period
   * @param consistentState The consistent state being updated
   * @param systemInfluencesToManage System influences to manage
   * @param happensBeforeRegularReaction True if called before regular reaction
   * @param newInfluencesToProcess Influences resulting from this reaction
   */
  virtual void makeSystemReaction(
      const microkernel::SimulationTimeStamp &transitoryTimeMin,
      const microkernel::SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::vector<std::shared_ptr<microkernel::influences::IInfluence>>
          &systemInfluencesToManage,
      bool happensBeforeRegularReaction,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          newInfluencesToProcess) = 0;
};

} // namespace levels
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ILEVELREACTIONMODEL_H
