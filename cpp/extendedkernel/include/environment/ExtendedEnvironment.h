#ifndef EXTENDEDENVIRONMENT_H
#define EXTENDEDENVIRONMENT_H

#include "../../microkernel/include/LevelIdentifier.h"
#include "../../microkernel/include/SimulationTimeStamp.h"
#include "../../microkernel/include/dynamicstate/IPublicDynamicStateMap.h"
#include "../../microkernel/include/environment/ILocalStateOfEnvironment.h"
#include "../../microkernel/include/influences/InfluencesMap.h"
#include "../../microkernel/include/libs/abstractimpl/AbstractEnvironment.h"
#include "IEnvNaturalModel.h"
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace environment {

/**
 * Models an environment in the extended kernel.
 *
 * Unlike the micro-kernel where environment classes had to be created, this
 * class is self-sufficient. The operational code of the natural action
 * processes is defined in separate classes.
 *
 * Benefits:
 * - The behavior of the environment can evolve at runtime
 * - Separation of concerns between structure and behavior
 *
 * Usage:
 * 1. Define initial local states using includeNewLevel()
 * 2. Specify behavior for each level using specifyBehaviorForLevel()
 */
class ExtendedEnvironment : public microkernel::libs::AbstractEnvironment {
private:
  std::unordered_map<microkernel::LevelIdentifier,
                     std::shared_ptr<IEnvNaturalModel>>
      naturalActionModels;

public:
  /**
   * Builds an environment for a simulation containing no levels.
   *
   * The environment must then be initialized using:
   * - includeNewLevel() to define initial local states
   * - specifyBehaviorForLevel() to specify behavior for each level
   */
  ExtendedEnvironment() = default;

  virtual ~ExtendedEnvironment() = default;

  /**
   * Gets the model of the natural action of the environment from that level.
   *
   * @param levelId The identifier of the level
   * @return The model of the natural action
   * @throws std::runtime_error If no natural model is defined for that level
   */
  std::shared_ptr<IEnvNaturalModel>
  getNaturalModelForLevel(const microkernel::LevelIdentifier &levelId) const {
    auto it = naturalActionModels.find(levelId);
    if (it == naturalActionModels.end()) {
      throw std::runtime_error(
          "The natural model of the environment is not specified for the "
          "level '" +
          levelId.toString() + "'.");
    }
    return it->second;
  }

  /**
   * Specifies the behavior of the environment from the specified level.
   *
   * @param levelId The identifier of the level
   * @param naturalMdl The model of the natural action
   * @throws std::invalid_argument If arguments are null or invalid
   */
  void specifyBehaviorForLevel(const microkernel::LevelIdentifier &levelId,
                               std::shared_ptr<IEnvNaturalModel> naturalMdl) {
    if (!naturalMdl) {
      throw std::invalid_argument("The natural model cannot be null.");
    }
    if (!(naturalMdl->getLevel() == levelId)) {
      throw std::invalid_argument(
          "The level of the natural model '" +
          naturalMdl->getLevel().toString() +
          "' has to match the value of the argument 'levelId'.");
    }
    naturalActionModels[levelId] = naturalMdl;
  }

  // IEnvironment4Engine interface implementation

  void natural(
      const microkernel::LevelIdentifier &level,
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
          producedInfluences) override {
    getNaturalModelForLevel(level)->natural(
        timeLowerBound, timeUpperBound, publicLocalStates, privateLocalState,
        dynamicStates, producedInfluences);
  }
};

} // namespace environment
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EXTENDEDENVIRONMENT_H
