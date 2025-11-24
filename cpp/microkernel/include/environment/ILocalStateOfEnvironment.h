#ifndef ILOCALSTATEOFENVIRONMENT_H
#define ILOCALSTATEOFENVIRONMENT_H

#include "../ILocalState.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace environment {

/**
 * Models either a public or a private local state of the environment.
 */
class ILocalStateOfEnvironment : public virtual ILocalState {
public:
  virtual ~ILocalStateOfEnvironment() = default;
};

} // namespace environment
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ILOCALSTATEOFENVIRONMENT_H
