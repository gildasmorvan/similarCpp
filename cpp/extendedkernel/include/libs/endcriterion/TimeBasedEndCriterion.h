#ifndef TIMEBASEDENDCRITERION_H
#define TIMEBASEDENDCRITERION_H

#include "simulationmodel/IEndCriterionModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace endcriterion {

/**
 * A simulation end criterion based on a final time stamp to reach.
 */
class TimeBasedEndCriterion : public simulationmodel::IEndCriterionModel {
private:
  microkernel::SimulationTimeStamp finalTimeStamp;

public:
  /**
   * Builds a simulation end criterion based on a final time stamp to reach.
   * @param finalTimeStamp The final time stamp of the simulation
   */
  explicit TimeBasedEndCriterion(
      const microkernel::SimulationTimeStamp &finalTimeStamp);

  /**
   * Gets the final time stamp of the simulation.
   * @return The final time stamp
   */
  microkernel::SimulationTimeStamp getFinalTimeStamp() const {
    return finalTimeStamp;
  }

  bool isFinalTimeOrAfter(
      const microkernel::SimulationTimeStamp &currentTime,
      const microkernel::ISimulationEngine *engine) const override;
};

} // namespace endcriterion
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // TIMEBASEDENDCRITERION_H
