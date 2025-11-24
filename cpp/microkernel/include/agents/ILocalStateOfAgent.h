#ifndef ILOCALSTATEOFAGENT_H
#define ILOCALSTATEOFAGENT_H

#include "../AgentCategory.h"
#include "../ILocalState.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

// Forward declaration to avoid circular dependency
namespace agents {
class IAgent;
}

namespace agents {

/**
 * Models either a public or a private local state of an agent.
 */
class ILocalStateOfAgent : public virtual ILocalState {
public:
  virtual ~ILocalStateOfAgent() = default;

  /**
   * Gets the category of the agent owning this local state.
   * @return The category of the agent owning this local state.
   */
  virtual AgentCategory getCategoryOfAgent() const = 0;

  /**
   * Checks if an agent is the owner of a local state.
   * @param agent The agent which ownership is checked.
   * @return true if an agent is the owner of a local state.
   */
  virtual bool isOwnedBy(const IAgent &agent) const = 0;
};

} // namespace agents
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ILOCALSTATEOFAGENT_H
