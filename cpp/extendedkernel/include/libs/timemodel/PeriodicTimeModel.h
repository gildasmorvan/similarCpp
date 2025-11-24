#ifndef PERIODICTIMEMODEL_H
#define PERIODICTIMEMODEL_H

#include "../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../microkernel/include/levels/ITimeModel.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {

/**
 * A time model using a period and a phase shift to determine the next time
 * stamp.
 */
class PeriodicTimeModel : public microkernel::levels::ITimeModel {
private:
  long period;
  long phaseShift;
  microkernel::SimulationTimeStamp initialTime;

  static void checkConstructorArgumentsValidity(
      long period, long phaseShift,
      const microkernel::SimulationTimeStamp &initialTime);

public:
  /**
   * Builds a periodic time model having a specific period and phase shift.
   * @param period The period of the time model. This value has to be strictly
   * positive.
   * @param phaseShift The phase shift of the time model.
   * @param initialTime The initial time stamp of the simulation.
   * @throws std::invalid_argument If an argument is invalid.
   */
  PeriodicTimeModel(long period, long phaseShift,
                    const microkernel::SimulationTimeStamp &initialTime);
  virtual ~PeriodicTimeModel() = default;

  /**
   * Gets the period of the time model.
   * @return The period of the time model.
   */
  long getPeriod() const { return period; }

  /**
   * Gets the phase shift of the time model.
   * @return The phase shift of the time model.
   */
  long getPhaseShift() const { return phaseShift; }

  microkernel::SimulationTimeStamp
  getNextTime(const microkernel::SimulationTimeStamp &currentTime) override;
};

} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // PERIODICTIMEMODEL_H
