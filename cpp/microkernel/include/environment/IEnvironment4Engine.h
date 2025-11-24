#ifndef IENVIRONMENT4ENGINE_H
#define IENVIRONMENT4ENGINE_H

#include "IEnvironment.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace environment {

/**
 * Models the environment of the simulation, which specifications include engine
 * optimization related methods.
 */
class IEnvironment4Engine : public IEnvironment {
public:
  virtual ~IEnvironment4Engine() = default;

  /**
   * Gets the public local states of the environment.
   * @return The public local states of the environment.
   */
  virtual std::map<LevelIdentifier, std::shared_ptr<ILocalStateOfEnvironment>>
  getPublicLocalStates() const = 0;
};

} // namespace environment
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IENVIRONMENT4ENGINE_H
