#ifndef ABSTRACTENVIRONMENT_H
#define ABSTRACTENVIRONMENT_H

#include "../../LevelIdentifier.h"
#include "../../environment/IEnvironment4Engine.h"
#include "../../environment/ILocalStateOfEnvironment.h"
#include <map>
#include <memory>
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace abstractimpl {

/**
 * An abstract implementation of the IEnvironment and IEnvironment4Engine
 * interfaces, providing a default behavior to the generic methods.
 *
 * This class also provides a method to set the level-related information of the
 * environment.
 */
class AbstractEnvironment : public environment::IEnvironment4Engine {
private:
  /**
   * The public local states of the environment in the various levels of the
   * simulation.
   */
  std::map<LevelIdentifier,
           std::shared_ptr<environment::ILocalStateOfEnvironment>>
      publicLocalStates;

  /**
   * The private local states of the environment in the various levels of the
   * simulation.
   */
  std::map<LevelIdentifier,
           std::shared_ptr<environment::ILocalStateOfEnvironment>>
      privateLocalStates;

public:
  /**
   * Builds an environment for a simulation containing no levels.
   * Levels are then added using the includeNewLevel method.
   */
  AbstractEnvironment() = default;

  virtual ~AbstractEnvironment() = default;

  /**
   * Gets the public local state of the environment for a specific level.
   * @param level The identifier of the level.
   * @return The public local state.
   */
  std::shared_ptr<environment::ILocalStateOfEnvironment>
  getPublicLocalState(const LevelIdentifier &level) const override {
    auto it = publicLocalStates.find(level);
    if (it == publicLocalStates.end()) {
      throw std::logic_error("No public local state is defined in the "
                             "environment for the level '" +
                             level.toString() + "'.");
    }
    return it->second;
  }

  /**
   * Gets the public local states of the environment.
   * @return The map of public local states.
   */
  std::map<LevelIdentifier,
           std::shared_ptr<environment::ILocalStateOfEnvironment>>
  getPublicLocalStates() const override {
    return publicLocalStates;
  }

  /**
   * Gets the private local state of the environment for a specific level.
   * @param level The identifier of the level.
   * @return The private local state.
   */
  std::shared_ptr<environment::ILocalStateOfEnvironment>
  getPrivateLocalState(const LevelIdentifier &level) const override {
    auto it = privateLocalStates.find(level);
    if (it == privateLocalStates.end()) {
      throw std::logic_error("No private local state is defined in the "
                             "environment for the level '" +
                             level.toString() + "'.");
    }
    return it->second;
  }

  /**
   * Introduces the level-related data of the environment for a new level.
   * @param level The level for which data are added.
   * @param publicLocalState The public local state of the environment for that
   * level.
   * @param privateLocalState The private local state of the environment for
   * that level.
   */
  void includeNewLevel(
      const LevelIdentifier &level,
      std::shared_ptr<environment::ILocalStateOfEnvironment> publicLocalState,
      std::shared_ptr<environment::ILocalStateOfEnvironment>
          privateLocalState) {
    if (!publicLocalState || !privateLocalState) {
      throw std::invalid_argument("The arguments cannot be null.");
    }

    if (!(level == publicLocalState->getLevel()) ||
        !(level == privateLocalState->getLevel())) {
      throw std::invalid_argument(
          "The 'getLevel()' method of the 'publicLocalState' and "
          "'privateLocalState' arguments have to be equal to the 'level' "
          "argument.");
    }

    if (publicLocalStates.find(level) != publicLocalStates.end()) {
      throw std::invalid_argument("The level '" + level.toString() +
                                  "' is already defined for this environment.");
    }

    publicLocalStates[level] = publicLocalState;
    privateLocalStates[level] = privateLocalState;
  }
};

} // namespace abstractimpl
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTENVIRONMENT_H
