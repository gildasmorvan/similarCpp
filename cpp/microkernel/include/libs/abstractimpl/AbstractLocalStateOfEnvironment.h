#ifndef ABSTRACTLOCALSTATEOFENVIRONMENT_H
#define ABSTRACTLOCALSTATEOFENVIRONMENT_H

#include "../../LevelIdentifier.h"
#include "../../environment/ILocalStateOfEnvironment.h"
#include "AbstractLocalState.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace abstractimpl {

/**
 * An abstract implementation of the ILocalStateOfEnvironment interface,
 * providing a default behavior to their methods.
 */
class AbstractLocalStateOfEnvironment
    : public AbstractLocalState,
      public ::fr::univ_artois::lgi2a::similar::microkernel::environment::
          ILocalStateOfEnvironment {
public:
  /**
   * Builds an initialized instance of this class.
   * @param level The level for which this local state was defined
   */
  explicit AbstractLocalStateOfEnvironment(
      const ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &level)
      : AbstractLocalState(level) {}

  virtual ~AbstractLocalStateOfEnvironment() = default;

  // Inherit getLevel() from AbstractLocalState
};

} // namespace abstractimpl
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTLOCALSTATEOFENVIRONMENT_H
