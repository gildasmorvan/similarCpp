#ifndef ILOCALSTATEOFAGENT4ENGINE_H
#define ILOCALSTATEOFAGENT4ENGINE_H

#include "IAgent4Engine.h"
#include "ILocalStateOfAgent.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace agents {

/**
 * Models either a public or a private local state of an agent.
 */
class ILocalStateOfAgent4Engine : public ILocalStateOfAgent {
public:
  virtual ~ILocalStateOfAgent4Engine() = default;

  /**
   * Gets the owner of the local state.
   * @return The owner of this state.
   */
  virtual std::shared_ptr<IAgent4Engine> getOwner() const = 0;
};

} // namespace agents
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ILOCALSTATEOFAGENT4ENGINE_H
