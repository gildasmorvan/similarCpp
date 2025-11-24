#ifndef IDENTITYAGTGLOBALSTATEREVISIONMODEL_H
#define IDENTITYAGTGLOBALSTATEREVISIONMODEL_H

#include "../../agents/IAgtGlobalStateRevisionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace generic {

/**
 * Models a global state revision model performing no changes in the global
 * state of the agent.
 *
 * This is the default implementation that simply does nothing, leaving the
 * global state unchanged.
 */
class IdentityAgtGlobalStateRevisionModel
    : public agents::IAgtGlobalStateRevisionModel {
public:
  IdentityAgtGlobalStateRevisionModel() = default;
  virtual ~IdentityAgtGlobalStateRevisionModel() = default;

  // Uses the default implementation from IAgtGlobalStateRevisionModel which
  // does nothing
};

} // namespace generic
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IDENTITYAGTGLOBALSTATEREVISIONMODEL_H
