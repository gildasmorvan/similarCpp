#ifndef ITIMEMODEL_H
#define ITIMEMODEL_H

#include "../SimulationTimeStamp.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace levels {

/**
 * Models how the time stamps of the simulation or of a level evolve through
 * time.
 */
class ITimeModel {
public:
  virtual ~ITimeModel() = default;

  /**
   * Gets the time stamp following a specific time stamp in this model.
   * @param currentTime The time stamp for which this method computes a
   * successor.
   * @return The time stamp following the currentTime time stamp.
   */
  virtual SimulationTimeStamp
  getNextTime(const SimulationTimeStamp &currentTime) = 0;
};

} // namespace levels
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ITIMEMODEL_H
