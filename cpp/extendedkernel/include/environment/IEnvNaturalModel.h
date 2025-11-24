#ifndef IENVNATURALMODEL_H
#define IENVNATURALMODEL_H

#include "../../microkernel/include/LevelIdentifier.h"
#include "../../microkernel/include/SimulationTimeStamp.h"
#include "../../microkernel/include/dynamicstate/IPublicDynamicStateMap.h"
#include "../../microkernel/include/environment/ILocalStateOfEnvironment.h"
#include "../../microkernel/include/influences/InfluencesMap.h"
#include <map>
#include <memory>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace environment {

/**
 * Models the natural action process used by an environment to produce
 * influences from a specific level.
 */
class IEnvNaturalModel {
public:
  virtual ~IEnvNaturalModel() = default;

  /**
   * Gets the level from which the natural action is made.
   * @return The level from which the natural action is made.
   */
  virtual microkernel::LevelIdentifier getLevel() const = 0;

  /**
   * Models the natural action of the environment on the simulation.
   *
   * This method models the natural dynamics of the environment (e.g.,
   * pheromone evaporation, diffusion, weather changes, etc.)
   *
   * @param timeLowerBound Lower bound of the transitory period
   * @param timeUpperBound Upper bound of the transitory period
   * @param publicLocalStates All public local states of the environment
   * @param privateLocalState Private local state of the environment
   * @param dynamicStates Dynamic state of the various levels
   * @param producedInfluences Map where resulting influences are stored
   */
  virtual void natural(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<
          microkernel::LevelIdentifier,
          std::shared_ptr<microkernel::environment::ILocalStateOfEnvironment>>
          &publicLocalStates,
      std::shared_ptr<microkernel::environment::ILocalStateOfEnvironment>
          privateLocalState,
      std::shared_ptr<microkernel::dynamicstate::IPublicDynamicStateMap>
          dynamicStates,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          producedInfluences) = 0;
};

} // namespace environment
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IENVNATURALMODEL_H
