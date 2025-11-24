#ifndef IMODIFIABLEPUBLICLOCALDYNAMICSTATE_H
#define IMODIFIABLEPUBLICLOCALDYNAMICSTATE_H

#include "IPublicLocalDynamicState.h"
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace dynamicstate {

/**
 * Models a modifiable public local dynamic state.
 */
class IModifiablePublicLocalDynamicState
    : public virtual IPublicLocalDynamicState {
public:
  virtual ~IModifiablePublicLocalDynamicState() = default;

  /**
   * Sets the value of the public local state of the environment in this dynamic
   * state.
   * @param publicLocalState The public local state of the environment.
   */
  virtual void setPublicLocalStateOfEnvironment(
      std::shared_ptr<environment::ILocalStateOfEnvironment>
          publicLocalState) = 0;

  /**
   * Adds the public local state of an agent to this public local dynamic state.
   * @param publicLocalState The added public local state of the agent.
   */
  virtual void addPublicLocalStateOfAgent(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState) = 0;

  /**
   * Removes the public local state of an agent from this public local dynamic
   * state.
   * @param publicLocalState The removed public local state of the agent.
   */
  virtual void removePublicLocalStateOfAgent(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState) = 0;

  /**
   * Adds an influence to this public local dynamic state.
   * @param influence The influence to add to this public local dynamic state.
   */
  virtual void
  addInfluence(std::shared_ptr<influences::IInfluence> influence) = 0;

  /**
   * Sets the state dynamics of this public local dynamic state as a copy of the
   * specified value.
   * @param toCopy The set of influences that have to be contained in the state
   * dynamics of this local dynamic state.
   */
  virtual void setStateDynamicsAsCopyOf(
      const std::vector<std::shared_ptr<influences::IInfluence>> &toCopy) = 0;

  /**
   * Remove all the system influences of this dynamic state.
   */
  virtual void clearSystemInfluences() = 0;

  /**
   * Remove all the regular influences of this dynamic state.
   */
  virtual void clearRegularInfluences() = 0;
};

} // namespace dynamicstate
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IMODIFIABLEPUBLICLOCALDYNAMICSTATE_H
