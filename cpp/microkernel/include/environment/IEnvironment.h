#ifndef IENVIRONMENT_H
#define IENVIRONMENT_H

#include <map>
#include <memory>

#include "../LevelIdentifier.h"
#include "../SimulationTimeStamp.h"
#include "../dynamicstate/IPublicDynamicStateMap.h"
#include "../influences/InfluencesMap.h"
#include "ILocalStateOfEnvironment.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace environment {

/**
 * Models the environment of the simulation.
 */
class IEnvironment {
public:
  virtual ~IEnvironment() = default;

  /**
   * Gets the public local state of the environment for a specific level.
   * @param level The level of the public local state of the environment.
   * @return The public local state of the environment for a specific level.
   * @throws std::out_of_range If no public local state was defined for the
   * specified level.
   */
  virtual std::shared_ptr<ILocalStateOfEnvironment>
  getPublicLocalState(const LevelIdentifier &level) const = 0;

  /**
   * Gets the private local state of the environment for a specific level.
   * @param level The level of the private local state of the environment.
   * @return The private local state of the environment for a specific level.
   * @throws std::out_of_range If no private local state was defined for the
   * specified level.
   */
  virtual std::shared_ptr<ILocalStateOfEnvironment>
  getPrivateLocalState(const LevelIdentifier &level) const = 0;

  /**
   * Models the natural action of the environment on the simulation, from a
   * specific level.
   * @param level The level from which the natural action of the environment is
   * made.
   * @param timeLowerBound Is the lower bound of the transitory period.
   * @param timeUpperBound Is the upper bound of the transitory period.
   * @param publicLocalStates All the public local states of the environment.
   * @param privateLocalState The private local state of the environment.
   * @param dynamicStates The dynamic state of the various levels.
   * @param producedInfluences The map where the influences resulting from the
   * natural action are stored.
   */
  virtual void natural(
      const LevelIdentifier &level, const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      const std::map<LevelIdentifier, std::shared_ptr<ILocalStateOfEnvironment>>
          &publicLocalStates,
      std::shared_ptr<ILocalStateOfEnvironment> privateLocalState,
      std::shared_ptr<dynamicstate::IPublicDynamicStateMap> dynamicStates,
      std::shared_ptr<influences::InfluencesMap> producedInfluences) = 0;

  /**
   * Clones the environment, creating a deep copy of its state.
   * @return A deep copy of the environment.
   */
  virtual std::shared_ptr<IEnvironment> clone() const = 0;
};

} // namespace environment
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IENVIRONMENT_H
