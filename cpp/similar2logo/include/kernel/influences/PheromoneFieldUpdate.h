#ifndef SIMILAR2LOGO_PHEROMONEFIELDUPDATE_H
#define SIMILAR2LOGO_PHEROMONEFIELDUPDATE_H

#include "../../../../microkernel/include/LevelIdentifier.h"
#include "../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../microkernel/include/influences/RegularInfluence.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

/**
 * Models an influence that aims at updating the pheromone field.
 * Matches the Java implementation exactly.
 *
 * @author Gildas Morvan
 * @author Yoann Kubera
 */
class PheromoneFieldUpdate : public microkernel::influences::RegularInfluence {
public:
  /**
   * The category of the influence, used as a unique identifier in
   * the reaction of the target level to determine the nature of the influence.
   */
  static constexpr const char *CATEGORY = "pheromone field update";

public:
  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in the LOGO level.
   *
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   */
  PheromoneFieldUpdate(const microkernel::SimulationTimeStamp &timeLowerBound,
                       const microkernel::SimulationTimeStamp &timeUpperBound);

  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in a given level.
   *
   * @param levelIdentifier The level in which the influence is emitted
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   */
  PheromoneFieldUpdate(const microkernel::LevelIdentifier &levelIdentifier,
                       const microkernel::SimulationTimeStamp &timeLowerBound,
                       const microkernel::SimulationTimeStamp &timeUpperBound);
};

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_PHEROMONEFIELDUPDATE_H
