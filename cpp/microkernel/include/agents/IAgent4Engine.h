#ifndef IAGENT4ENGINE_H
#define IAGENT4ENGINE_H

#include "IAgent.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace agents {

/**
 * Models an agent contained in the simulation, which specifications include
 * engine optimization related methods.
 */
class IAgent4Engine : public IAgent {
public:
  virtual ~IAgent4Engine() = default;

  /**
   * Include a new level in the specification of this agent.
   * @param levelIdentifier The identifier of the specified level.
   * @param publicLocalState The public local state of the agent in that level.
   * @param privateLocalState The private local state of the agent in that
   * level.
   */
  virtual void
  includeNewLevel(const LevelIdentifier &levelIdentifier,
                  std::shared_ptr<ILocalStateOfAgent> publicLocalState,
                  std::shared_ptr<ILocalStateOfAgent> privateLocalState) = 0;

  /**
   * Removes a level from the specification of this agent.
   * @param levelIdentifier The identifier of the level from which the agent has
   * to be removed.
   */
  virtual void excludeFromLevel(const LevelIdentifier &levelIdentifier) = 0;

  /**
   * Gets the data that were lastly perceived by the agent, for all the levels
   * where it lies.
   * @return The data that were lastly perceived by the agent.
   */
  virtual std::map<LevelIdentifier, std::shared_ptr<IPerceivedData>>
  getPerceivedData() const = 0;

  /**
   * Sets the data that were lastly perceived by the agent from a specific level
   * where it lies.
   * @param perceivedData The data that were lastly perceived by the agent.
   */
  virtual void
  setPerceivedData(std::shared_ptr<IPerceivedData> perceivedData) = 0;

  /**
   * Gets the public local states of the agent.
   * @return The public local states of the agent.
   */
  virtual std::map<LevelIdentifier, std::shared_ptr<ILocalStateOfAgent>>
  getPublicLocalStates() const = 0;
};

} // namespace agents
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IAGENT4ENGINE_H
