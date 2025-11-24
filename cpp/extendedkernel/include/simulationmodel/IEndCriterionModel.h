#ifndef IENDCRITERIONMODEL_H
#define IENDCRITERIONMODEL_H

#include "../../microkernel/include/SimulationTimeStamp.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {

// Forward declaration
namespace microkernel {
class ISimulationEngine;
}

namespace extendedkernel {
namespace simulationmodel {

/**
 * Models the ending criterion used in the simulation.
 */
class IEndCriterionModel {
public:
  virtual ~IEndCriterionModel() = default;

  /**
   * Tells if a time stamp is greater or equal to the final time stamp.
   * @param currentTime The last time the dynamic state of the simulation was in
   * a partly consistent state
   * @param engine The simulation engine containing information about the
   * currently running simulation
   * @return true if the final time of the level was reached
   */
  virtual bool
  isFinalTimeOrAfter(const microkernel::SimulationTimeStamp &currentTime,
                     const microkernel::ISimulationEngine *engine) const = 0;
};

} // namespace simulationmodel
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IENDCRITERIONMODEL_H
