#ifndef SYSTEMINFLUENCEADDAGENTTOLEVEL_H
#define SYSTEMINFLUENCEADDAGENTTOLEVEL_H

#include "../../agents/ILocalStateOfAgent4Engine.h"
#include "../SystemInfluence.h"
#include <memory>
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace influences {
namespace system {

/**
 * The system influence sent to a level when the reaction of that level has to
 * include an agent to the level.
 */
class SystemInfluenceAddAgentToLevel : public SystemInfluence {
public:
  static constexpr const char *CATEGORY =
      "System influence - Add agent to a level";

private:
  std::shared_ptr<agents::ILocalStateOfAgent4Engine> publicLocalState;
  std::shared_ptr<agents::ILocalStateOfAgent4Engine> privateLocalState;

  static std::shared_ptr<agents::ILocalStateOfAgent4Engine>
  checkParameterValidity(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<agents::ILocalStateOfAgent> privateLocalState) {
    if (publicLocalState == nullptr) {
      throw std::invalid_argument(
          "The 'publicLocalState' argument cannot be null.");
    }
    auto castedPublic =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent4Engine>(
            publicLocalState);
    if (castedPublic == nullptr) {
      throw std::invalid_argument("The 'publicLocalState' argument has to be "
                                  "an instance of ILocalStateOfAgent4Engine.");
    }
    if (privateLocalState == nullptr) {
      throw std::invalid_argument(
          "The 'privateLocalState' argument cannot be null.");
    }
    auto castedPrivate =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent4Engine>(
            privateLocalState);
    if (castedPrivate == nullptr) {
      throw std::invalid_argument("The 'privateLocalState' argument has to be "
                                  "an instance of ILocalStateOfAgent4Engine.");
    }
    return castedPublic;
  }

public:
  SystemInfluenceAddAgentToLevel(
      const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<agents::ILocalStateOfAgent> privateLocalState)
      : SystemInfluence(
            CATEGORY,
            checkParameterValidity(publicLocalState, privateLocalState)
                ->getLevel(),
            timeLowerBound, timeUpperBound) {
    this->publicLocalState =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent4Engine>(
            publicLocalState);
    this->privateLocalState =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent4Engine>(
            privateLocalState);
  }

  std::shared_ptr<agents::ILocalStateOfAgent4Engine>
  getPublicLocalState() const {
    return this->publicLocalState;
  }

  std::shared_ptr<agents::ILocalStateOfAgent4Engine>
  getPrivateLocalState() const {
    return this->privateLocalState;
  }
};

} // namespace system
} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SYSTEMINFLUENCEADDAGENTTOLEVEL_H
