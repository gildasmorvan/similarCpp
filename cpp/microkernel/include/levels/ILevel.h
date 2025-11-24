#ifndef ILEVEL_H
#define ILEVEL_H

#include "../LevelIdentifier.h"
#include "../SimulationTimeStamp.h"
#include "../dynamicstate/ConsistentPublicLocalDynamicState.h"
#include "../dynamicstate/TransitoryPublicLocalDynamicState.h"
#include "../influences/IInfluence.h"
#include "../influences/InfluencesMap.h"
#include "ITimeModel.h"
#include <memory>
#include <set>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace levels {

/**
 * Models a level of the simulation.
 */
class ILevel : public ITimeModel {
public:
  virtual ~ILevel() = default;

  /**
   * Gets the identifier of this level.
   * @return The identifier of this level.
   */
  virtual LevelIdentifier getIdentifier() const = 0;

  /**
   * Gets the levels that can be perceived by agents located in this level.
   * @return The levels that can be perceived by agents located in this level.
   */
  virtual std::set<LevelIdentifier> getPerceptibleLevels() const = 0;

  /**
   * Gets the levels that can be influenced by agents located in this level.
   * @return The levels that can be influenced by agents located in this level.
   */
  virtual std::set<LevelIdentifier> getInfluenceableLevels() const = 0;

  /**
   * Gets the last consistent dynamic state of the level.
   * @return The last consistent dynamic state of the level.
   */
  virtual std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
  getLastConsistentState() const = 0;

  /**
   * Gets the last transitory dynamic state of the level.
   * @return The last transitory dynamic state of the level.
   */
  virtual std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
  getLastTransitoryState() const = 0;

  /**
   * Performs a user-defined reaction to the regular influences.
   * @param transitoryTimeMin The lower bound of the transitory period.
   * @param transitoryTimeMax The upper bound of the transitory period.
   * @param consistentState The consistent state being updated by this user
   * reaction.
   * @param regularInfluencesOftransitoryStateDynamics The regular influences
   * that have to be managed.
   * @param remainingInfluences The data structure that will contain the
   * influences that were produced or persist.
   */
  virtual void makeRegularReaction(
      const SimulationTimeStamp &transitoryTimeMin,
      const SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::set<std::shared_ptr<influences::IInfluence>>
          &regularInfluencesOftransitoryStateDynamics,
      std::shared_ptr<influences::InfluencesMap> remainingInfluences) = 0;

  /**
   * Performs a user-defined reaction to the system influences.
   * @param transitoryTimeMin The lower bound of the transitory period.
   * @param transitoryTimeMax The upper bound of the transitory period.
   * @param consistentState The consistent state of the level being
   * progressively updated.
   * @param systemInfluencesToManage The system influences that have to be
   * managed.
   * @param happensBeforeRegularReaction true if this user-defined system
   * reaction is performed before the regular reaction.
   * @param newInfluencesToProcess The data structure where the influences
   * resulting from this user reaction have to be added.
   */
  virtual void makeSystemReaction(
      const SimulationTimeStamp &transitoryTimeMin,
      const SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::vector<std::shared_ptr<influences::IInfluence>>
          &systemInfluencesToManage,
      bool happensBeforeRegularReaction,
      std::shared_ptr<influences::InfluencesMap> newInfluencesToProcess) = 0;
};

} // namespace levels
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ILEVEL_H
