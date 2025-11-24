#ifndef ILOCALSTATE_H
#define ILOCALSTATE_H

#include "LevelIdentifier.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

/**
 * Models either a public or a private local state of an agent or of the
 * environment.
 */
class ILocalState {
public:
  virtual ~ILocalState() = default;

  /**
   * Gets the level for which this local state was defined.
   * @return The identifier of the level for which this local state was defined.
   */
  virtual LevelIdentifier getLevel() const = 0;
};

} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ILOCALSTATE_H
