#ifndef EMPTYLOCALSTATEOFENVIRONMENT_H
#define EMPTYLOCALSTATEOFENVIRONMENT_H

#include "../../LevelIdentifier.h"
#include "../../libs/abstractimpl/AbstractLocalStateOfEnvironment.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace generic {

/**
 * Models an empty public or private local state for the environment in a
 * specific level.
 *
 * Usage:
 * This local state is used when no information about the environment can be
 * perceived for a specific level.
 */
class EmptyLocalStateOfEnvironment
    : public abstractimpl::AbstractLocalStateOfEnvironment {
public:
  /**
   * Builds an empty environment local state for a specific level.
   * @param levelIdentifier The identifier of the level
   * @throws std::invalid_argument If levelIdentifier is invalid
   */
  explicit EmptyLocalStateOfEnvironment(const LevelIdentifier &levelIdentifier)
      : AbstractLocalStateOfEnvironment(levelIdentifier) {}

  virtual ~EmptyLocalStateOfEnvironment() = default;
};

} // namespace generic
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EMPTYLOCALSTATEOFENVIRONMENT_H
