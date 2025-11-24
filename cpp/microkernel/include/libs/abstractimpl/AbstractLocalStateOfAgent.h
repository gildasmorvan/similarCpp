#ifndef ABSTRACTLOCALSTATEOFAGENT_H
#define ABSTRACTLOCALSTATEOFAGENT_H

#include "../../agents/IAgent4Engine.h"
#include "../../agents/ILocalStateOfAgent4Engine.h"
#include "AbstractLocalState.h"
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
 * An abstract implementation of the ILocalStateOfAgent and
 * ILocalStateOfAgent4Engine interfaces, providing a default behavior to their
 * methods.
 */
class AbstractLocalStateOfAgent : public AbstractLocalState,
                                  public agents::ILocalStateOfAgent4Engine {
private:
  /**
   * The agent owning this local state.
   */
  std::shared_ptr<agents::IAgent4Engine> owner;

protected:
  /**
   * Builds an initialized instance of this class.
   * @param level The level for which this local state was defined.
   * @param owner The agent owning this local state.
   * @throws std::invalid_argument If owner is null.
   */
  AbstractLocalStateOfAgent(const LevelIdentifier &level,
                            std::shared_ptr<agents::IAgent4Engine> owner)
      : AbstractLocalState(level), owner(owner) {
    if (!owner) {
      throw new std::invalid_argument("The 'owner' argument cannot be null.");
    }
  }

public:
  virtual ~AbstractLocalStateOfAgent() = default;

  /**
   * Gets the agent owning this local state.
   * @return The owner agent.
   */
  std::shared_ptr<agents::IAgent4Engine> getOwner() const override {
    return this->owner;
  }

  /**
   * Checks if this local state is owned by a specific agent.
   * @param agent The agent to check.
   * @return True if owned by the agent.
   */
  bool isOwnedBy(std::shared_ptr<agents::IAgent> agent) const override {
    return this->owner == agent;
  }

  /**
   * Gets the category of the agent owning this local state.
   * @return The category.
   */
  AgentCategory getCategoryOfAgent() const override {
    return this->owner->getCategory();
  }

  // Inherit getLevel() from AbstractLocalState
  LevelIdentifier getLevel() const override {
    return AbstractLocalState::getLevel();
  }
};

} // namespace abstractimpl
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTLOCALSTATEOFAGENT_H
