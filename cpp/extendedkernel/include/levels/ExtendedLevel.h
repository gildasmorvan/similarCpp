#ifndef EXTENDEDLEVEL_H
#define EXTENDEDLEVEL_H

#include "../../microkernel/include/LevelIdentifier.h"
#include "../../microkernel/include/SimulationTimeStamp.h"
#include "../../microkernel/include/levels/ILevel.h"
#include "../../microkernel/include/levels/ITimeModel.h"
#include "../../microkernel/include/libs/abstractimpl/AbstractLevel.h"
#include "ILevelReactionModel.h"
#include <memory>
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace levels {

/**
 * Models a level in the extended kernel.
 *
 * Unlike the micro-kernel where level classes had to be created, this class is
 * self-sufficient. The operational code of the reaction processes and time
 * model is defined in separate classes.
 *
 * Benefits:
 * - The behavior of the level can evolve at runtime
 * - Separation of concerns between structure and behavior
 */
class ExtendedLevel : public microkernel::libs::AbstractLevel {
private:
  std::shared_ptr<microkernel::levels::ITimeModel> timeModel;
  std::shared_ptr<ILevelReactionModel> reactionModel;

public:
  /**
   * Builds an initialized instance of level.
   *
   * @param initialTime The initial time stamp of the level
   * @param identifier The identifier of the level
   * @param timeModel The time model used by this level
   * @param reactionModel The reaction model used by this level
   * @throws std::invalid_argument If timeModel or reactionModel is null
   */
  ExtendedLevel(const microkernel::SimulationTimeStamp &initialTime,
                const microkernel::LevelIdentifier &identifier,
                std::shared_ptr<microkernel::levels::ITimeModel> timeModel,
                std::shared_ptr<ILevelReactionModel> reactionModel)
      : AbstractLevel(initialTime, identifier), timeModel(timeModel),
        reactionModel(reactionModel) {
    if (!timeModel) {
      throw std::invalid_argument("The timeModel of the level '" +
                                  identifier.toString() + "' cannot be null.");
    }
    if (!reactionModel) {
      throw std::invalid_argument("The reactionModel of the level '" +
                                  identifier.toString() + "' cannot be null.");
    }
  }

  virtual ~ExtendedLevel() = default;

  /**
   * Gets the time model used by this level.
   * @return The time model used by this level
   */
  std::shared_ptr<microkernel::levels::ITimeModel> getTimeModel() const {
    return timeModel;
  }

  /**
   * Gets the reaction model used by the level.
   * @return The reaction model used by the level
   * @throws std::runtime_error If no reaction model is defined
   */
  std::shared_ptr<ILevelReactionModel> getReactionModel() const {
    if (!reactionModel) {
      throw std::runtime_error("No reaction model is defined for the level '" +
                               getIdentifier().toString() + "'.");
    }
    return reactionModel;
  }

  /**
   * Sets the reaction model used by the level.
   * @param newReactionModel The new reaction model
   * @throws std::invalid_argument If newReactionModel is null
   */
  void setReactionModel(std::shared_ptr<ILevelReactionModel> newReactionModel) {
    if (!newReactionModel) {
      throw std::invalid_argument("The reaction model of the level '" +
                                  getIdentifier().toString() +
                                  "' cannot be null.");
    }
    reactionModel = newReactionModel;
  }

  // ILevel interface implementation

  void makeRegularReaction(
      const microkernel::SimulationTimeStamp &transitoryTimeMin,
      const microkernel::SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::set<std::shared_ptr<microkernel::influences::IInfluence>>
          &regularInfluencesOftransitoryStateDynamics,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          remainingInfluences) override {
    getReactionModel()->makeRegularReaction(
        transitoryTimeMin, transitoryTimeMax, consistentState,
        regularInfluencesOftransitoryStateDynamics, remainingInfluences);
  }

  void makeSystemReaction(
      const microkernel::SimulationTimeStamp &transitoryTimeMin,
      const microkernel::SimulationTimeStamp &transitoryTimeMax,
      std::shared_ptr<
          microkernel::dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::vector<std::shared_ptr<microkernel::influences::IInfluence>>
          &systemInfluencesToManage,
      bool happensBeforeRegularReaction,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          newInfluencesToProcess) override {
    getReactionModel()->makeSystemReaction(
        transitoryTimeMin, transitoryTimeMax, consistentState,
        systemInfluencesToManage, happensBeforeRegularReaction,
        newInfluencesToProcess);
  }

  microkernel::SimulationTimeStamp
  getNextTime(const microkernel::SimulationTimeStamp &currentTime) override {
    return getTimeModel()->getNextTime(currentTime);
  }
};

} // namespace levels
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EXTENDEDLEVEL_H
