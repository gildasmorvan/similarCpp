#ifndef SYSTEMINFLUENCEREMOVEAGENT_H
#define SYSTEMINFLUENCEREMOVEAGENT_H

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
 * remove the agent from the simulation.
 */
class SystemInfluenceRemoveAgent : public SystemInfluence {
public:
  static constexpr const char *CATEGORY = "System influence - Remove agent";

private:
  std::shared_ptr<agents::IAgent4Engine> agent;

public:
  SystemInfluenceRemoveAgent(
      const LevelIdentifier &targetLevel,
      const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalStateOfAgent)
      : SystemInfluence(CATEGORY, targetLevel, timeLowerBound, timeUpperBound) {
    if (publicLocalStateOfAgent == nullptr) {
      throw std::invalid_argument(
          "The 'publicLocalStateOfAgent' argument cannot be null.");
    }
    auto castedState =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent4Engine>(
            publicLocalStateOfAgent);
    if (castedState == nullptr) {
      throw std::invalid_argument(
          "The 'publicLocalStateOfAgent' argument has to be an instance of "
          "ILocalStateOfAgent4Engine.");
    }
    this->agent = castedState->getOwner();
  }

  std::shared_ptr<agents::IAgent4Engine> getAgent() const {
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

#endif // SYSTEMINFLUENCEREMOVEAGENT_H
