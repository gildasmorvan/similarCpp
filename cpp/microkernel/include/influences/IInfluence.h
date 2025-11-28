#ifndef IINFLUENCE_H
#define IINFLUENCE_H

#include "../LevelIdentifier.h"
#include "../SimulationTimeStamp.h"
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace influences {

/**
 * Models an influence produced by the agents, the environment or the reaction
 * to modify the dynamic state of the simulation.
 */
class IInfluence {
public:
  virtual ~IInfluence() = default;

  /**
   * Gets the category of the influence.
   * @return The category of the influence.
   */
  virtual ::std::string getCategory() const = 0;

  /**
   * Gets the identifier of the level whose reaction will process this
   * influence.
   * @return The identifier of the level whose reaction will process this
   * influence.
   */
  virtual ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
  getTargetLevel() const = 0;

  /**
   * Checks if this influence is a system influence.
   * @return true if the influence is a system influence.
   */
  virtual bool isSystem() const = 0;

  /**
   * Gets the lower bound of the transitory period during which this influence
   * was created.
   * @return The lower bound of the transitory period during which this
   * influence was created.
   */
  virtual ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
  getTimeLowerBound() const = 0;

  /**
   * Gets the upper bound of the transitory period during which this influence
   * was created.
   * @return The upper bound of the transitory period during which this
   * influence was created.
   */
  virtual ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
  getTimeUpperBound() const = 0;
};

} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IINFLUENCE_H
