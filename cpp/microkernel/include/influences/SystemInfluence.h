#ifndef SYSTEMINFLUENCE_H
#define SYSTEMINFLUENCE_H

#include "AbstractInfluence.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace influences {

/**
 * Models a system influence that has to be managed at least by the simulation
 * engine of the simulation.
 */
class SystemInfluence : public AbstractInfluence {
public:
  SystemInfluence(const std::string &category,
                  const LevelIdentifier &targetLevel,
                  const SimulationTimeStamp &timeLowerBound,
                  const SimulationTimeStamp &timeUpperBound)
      : AbstractInfluence(category, targetLevel, timeLowerBound,
                          timeUpperBound) {}

  bool isSystem() const override { return true; }
};

} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SYSTEMINFLUENCE_H
