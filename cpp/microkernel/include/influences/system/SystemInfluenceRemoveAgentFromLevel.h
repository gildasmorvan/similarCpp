#ifndef SYSTEMINFLUENCEREMOVEAGENTFROMLEVEL_H
#define SYSTEMINFLUENCEREMOVEAGENTFROMLEVEL_H

#include "../../agents/IAgent4Engine.h"
#include "../../agents/ILocalStateOfAgent4Engine.h"
#include "../SystemInfluence.h"
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
 * remove an agent from the level.
 */
class SystemInfluenceRemoveAgentFromLevel : public SystemInfluence {
public:
  static constexpr const char *CATEGORY =
      "System influence - Remove agent from a level";

private:
  std::shared_ptr<agents::ILocalStateOfAgent4Engine> agent;

  static std::shared_ptr<agents::ILocalStateOfAgent4Engine>
  checkParameterValidity(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState) {
    if (publicLocalState == nullptr) {
      throw std::invalid_argument(
          "The 'publicLocalState' argument cannot be null.");
    }
    auto castedState =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent4Engine>(
            publicLocalState);
    if (castedState == nullptr) {
      throw std::invalid_argument("The 'publicLocalState' argument has to be "
                                  "an instance of ILocalStateOfAgent4Engine.");
    }
    return castedState;
  }

public:
  SystemInfluenceRemoveAgentFromLevel(
      const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<agents::ILocalStateOfAgent4Engine> publicLocalState)
      : SystemInfluence(CATEGORY,
                        checkParameterValidity(publicLocalState)->getLevel(),
                        timeLowerBound, timeUpperBound),
        agent(publicLocalState) {}

  SystemInfluenceRemoveAgentFromLevel(
      const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<agents::IAgent4Engine> agent,
      const LevelIdentifier &levelId)
      : SystemInfluenceRemoveAgentFromLevel(
            timeLowerBound, timeUpperBound,
            std::dynamic_pointer_cast<agents::ILocalStateOfAgent4Engine>(
                agent->getPublicLocalState(levelId))) {}

  std::shared_ptr<agents::ILocalStateOfAgent4Engine>
  getAgentLocalState() const {
    return this->agent;
  }
};

} // namespace system
} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SYSTEMINFLUENCEREMOVEAGENTFROMLEVEL_H
