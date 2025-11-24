#ifndef ABSTRACTLEVEL_H
#define ABSTRACTLEVEL_H

#include "../../LevelIdentifier.h"
#include "../../SimulationTimeStamp.h"
#include "../../dynamicstate/ConsistentPublicLocalDynamicState.h"
#include "../../dynamicstate/TransitoryPublicLocalDynamicState.h"
#include "../../levels/ILevel.h"
#include <memory>
#include <set>
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace abstractimpl {

/**
 * An abstract implementation of the ILevel interface,
 * providing a default behavior to most methods.
 *
 * It also provides setters for the perception and the influence relation graph.
 */
class AbstractLevel : public levels::ILevel {
private:
  /**
   * The identifier of the level.
   */
  LevelIdentifier identifier;

  /**
   * The last consistent public local dynamic state of this level.
   */
  std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
      lastConsistentPublicLocalDynamicState;

  /**
   * The last transitory public local dynamic state of this level.
   */
  std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
      lastTransitoryPublicLocalDynamicState;

  /**
   * The perceptible levels of this level.
   */
  std::set<LevelIdentifier> perceptibleLevels;

  /**
   * The influenceable levels of this level.
   */
  std::set<LevelIdentifier> influenceableLevels;

protected:
  /**
   * Builds an initialized instance of level.
   * @param initialTime The initial time stamp of the level.
   * @param identifier The identifier of the level.
   * @throws std::invalid_argument if identifier is invalid.
   */
  AbstractLevel(const SimulationTimeStamp &initialTime,
                const LevelIdentifier &identifier)
      : identifier(identifier) {
    if (identifier.toString().empty()) {
      throw std::invalid_argument("The 'identifier' argument cannot be empty.");
    }

    // Create the out neighborhood of the perception relation graph for this
    // level.
    this->perceptibleLevels.insert(identifier);

    // Create the out neighborhood of the influence relation graph for this
    // level.
    this->influenceableLevels.insert(identifier);

    // Create a partially initialized instance of the dynamic state of the
    // level.
    initializeStates(
        std::make_shared<dynamicstate::ConsistentPublicLocalDynamicState>(
            initialTime, identifier));
  }

public:
  virtual ~AbstractLevel() = default;

  /**
   * Gets the identifier of this level.
   * @return The identifier of this level.
   */
  LevelIdentifier getIdentifier() const override { return this->identifier; }

  /**
   * Gets the last consistent public local dynamic state of this level.
   * @return The last consistent state.
   */
  std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
  getLastConsistentState() const override {
    if (!this->lastConsistentPublicLocalDynamicState) {
      throw std::logic_error("The state of the level " +
                             this->getIdentifier().toString() +
                             " is not initialized.");
    }
    return this->lastConsistentPublicLocalDynamicState;
  }

  /**
   * Gets the last transitory public local dynamic state of this level.
   * @return The last transitory state.
   */
  std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
  getLastTransitoryState() const override {
    if (!this->lastConsistentPublicLocalDynamicState) {
      throw std::logic_error("The state of the level " +
                             this->getIdentifier().toString() +
                             " is not initialized.");
    }
    return this->lastTransitoryPublicLocalDynamicState;
  }

  /**
   * Sets the initial value of the last consistent and the last transitory
   * public state of the level.
   * @param lastConsistentState The initial value of the last consistent state
   * of this level.
   */
  void initializeStates(
      std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
          lastConsistentState) {
    if (!lastConsistentState) {
      throw std::invalid_argument(
          "The 'lastConsistentState' argument cannot be null.");
    }

    this->lastConsistentPublicLocalDynamicState = lastConsistentState;
    this->lastTransitoryPublicLocalDynamicState =
        std::make_shared<dynamicstate::TransitoryPublicLocalDynamicState>(
            this->lastConsistentPublicLocalDynamicState,
            SimulationTimeStamp(std::numeric_limits<long long>::max()));
  }

  /**
   * Gets the levels that are perceptible from this level.
   * @return The set of perceptible levels.
   */
  std::set<LevelIdentifier> getPerceptibleLevels() const override {
    return this->perceptibleLevels;
  }

  /**
   * Adds a perceptible level by this level.
   * @param perceptibleLevel The level becoming perceptible by this level.
   */
  void addPerceptibleLevel(const LevelIdentifier &perceptibleLevel) {
    this->perceptibleLevels.insert(perceptibleLevel);
  }

  /**
   * Gets the levels that can be influenced by this level.
   * @return The set of influenceable levels.
   */
  std::set<LevelIdentifier> getInfluenceableLevels() const override {
    return this->influenceableLevels;
  }

  /**
   * Adds a level to the levels that can be influenced by this level.
   * @param influenceableLevel The level becoming influenceable by this level.
   */
  void addInfluenceableLevel(const LevelIdentifier &influenceableLevel) {
    this->influenceableLevels.insert(influenceableLevel);
  }
};

} // namespace abstractimpl
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTLEVEL_H
