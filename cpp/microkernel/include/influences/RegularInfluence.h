#ifndef REGULARINFLUENCE_H
#define REGULARINFLUENCE_H

#include "AbstractInfluence.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace influences {

/**
 * Models a non-system and user-defined influence.
 */
class RegularInfluence : public AbstractInfluence {
public:
  RegularInfluence(
      const std::string &category,
      const ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &targetLevel,
      const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeLowerBound,
      const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeUpperBound)
      : AbstractInfluence(category, targetLevel, timeLowerBound,
                          timeUpperBound) {}

  bool isSystem() const override { return false; }
};

} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // REGULARINFLUENCE_H
