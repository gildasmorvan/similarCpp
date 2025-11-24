#ifndef ABSTRACTLOCALSTATE_H
#define ABSTRACTLOCALSTATE_H

#include "../../ILocalState.h"
#include "../../LevelIdentifier.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace abstractimpl {

/**
 * An abstract implementation of the ILocalState interface,
 * providing a default behavior to the method of the interface.
 */
class AbstractLocalState : public virtual ILocalState {
private:
  LevelIdentifier level;

protected:
  /**
   * Builds an initialized instance of this class.
   * @param level The level for which this local state was defined
   * @throws std::invalid_argument If level is invalid
   */
  explicit AbstractLocalState(const LevelIdentifier &level) : level(level) {
    if (level.toString().empty()) {
      throw std::invalid_argument("The 'level' argument cannot be null.");
    }
  }

public:
  virtual ~AbstractLocalState() = default;

  /**
   * Gets the level for which this local state was defined.
   * @return The level identifier
   */
  LevelIdentifier getLevel() const override { return level; }
};

} // namespace abstractimpl
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTLOCALSTATE_H
