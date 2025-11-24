#ifndef SYSTEMINFLUENCEADDAGENT_H
#define SYSTEMINFLUENCEADDAGENT_H

#include "../../agents/IAgent4Engine.h"
#include "../SystemInfluence.h"
#include <stdexcept>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace influences {
namespace system {

/**
 * The system influence sent to a level when the reaction of that level has to
 * insert a new agent into the simulation and make appear it public local state
 * into the public dynamic state of the levels.
 */
class SystemInfluenceAddAgent : public SystemInfluence {
public:
  static constexpr const char *CATEGORY = "System influence - Add agent";

private:
  std::shared_ptr<agents::IAgent4Engine> agent;

public:
  /**
   * Builds an 'Add agent' system influence adding a specific agent to the
   * simulation during the next reaction of a specific level.
   * @param targetLevel The target level.
   * @param timeLowerBound The lower bound of the transitory period.
   * @param timeUpperBound The upper bound of the transitory period.
   * @param agent The agent to add to the simulation.
   */
  SystemInfluenceAddAgent(const LevelIdentifier &targetLevel,
                          const SimulationTimeStamp &timeLowerBound,
                          const SimulationTimeStamp &timeUpperBound,
                          std::shared_ptr<agents::IAgent4Engine> agent)
      : SystemInfluence(CATEGORY, targetLevel, timeLowerBound, timeUpperBound),
        agent(agent) {
    if (agent == nullptr) {
      throw std::invalid_argument("The 'agent' argument cannot be null.");
    }
  }

  /**
   * Gets the agent to add to the simulation.
   * @return The agent to add to the simulation.
   */
  std::shared_ptr<agents::IAgent4Engine> getAgent() const {
    return this->agent;
  }

  // toString not strictly needed as C++ doesn't have a universal toString, but
  // we can add it if needed. For now, skipping toString override unless
  // specifically requested or useful for debugging.
};

} // namespace system
} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SYSTEMINFLUENCEADDAGENT_H
