#ifndef IPUBLICLOCALDYNAMICSTATE_H
#define IPUBLICLOCALDYNAMICSTATE_H

#include <memory>
#include <set>

#include "../LevelIdentifier.h"
#include "../agents/ILocalStateOfAgent.h"
#include "../environment/ILocalStateOfEnvironment.h"
#include "../influences/IInfluence.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace dynamicstate {

/**
 * The parent interface of any public dynamic state of a level of the
 * simulation.
 */
class IPublicLocalDynamicState {
public:
  virtual ~IPublicLocalDynamicState() = default;

  /**
   * Gets the level for which the public local dynamic state is defined.
   * @return The level for which the public local dynamic state is defined.
   */
  virtual LevelIdentifier getLevel() const = 0;

  /**
   * Gets the public local state of the environment contained in this public
   * local dynamic state.
   * @return The public local state of the environment contained in this public
   * local dynamic state.
   */
  virtual std::shared_ptr<environment::ILocalStateOfEnvironment>
  getPublicLocalStateOfEnvironment() const = 0;

  /**
   * Gets the public local state of the agents lying in the level of this
   * dynamic state.
   * @return The public local state of the agents lying in the level of this
   * dynamic state.
   */
  virtual std::set<std::shared_ptr<agents::ILocalStateOfAgent>>
  getPublicLocalStateOfAgents() const = 0;

  /**
   * Gets the state dynamics of this public local dynamic state.
   * @return The state dynamics of this public local dynamic state.
   */
  virtual std::set<std::shared_ptr<influences::IInfluence>>
  getStateDynamics() const = 0;

  /**
   * Gets the system influences contained in the state dynamics.
   * @return The system influences of the value returned by the
   * getStateDynamics() method.
   */
  virtual std::set<std::shared_ptr<influences::IInfluence>>
  getSystemInfluencesOfStateDynamics() const = 0;

  /**
   * Gets the non-system influences contained in the state dynamics.
   * @return The non-system influences of the value returned by the
   * getStateDynamics() method.
   */
  virtual std::set<std::shared_ptr<influences::IInfluence>>
  getRegularInfluencesOfStateDynamics() const = 0;
};

} // namespace dynamicstate
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IPUBLICLOCALDYNAMICSTATE_H
