#include "../../../include/libs/timemodel/PeriodicTimeModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {

void PeriodicTimeModel::checkConstructorArgumentsValidity(
    long period, long phaseShift,
    const microkernel::SimulationTimeStamp &initialTime) {

  if (period <= 0) {
    throw std::invalid_argument(
        "The period has to be a strictly positive value (was '" +
        std::to_string(period) + "').");
  }
  if (phaseShift < 0) {
    throw std::invalid_argument(
        "The phase shift has to be a positive value (was '" +
        std::to_string(phaseShift) + "').");
  }
}

PeriodicTimeModel::PeriodicTimeModel(
    long period, long phaseShift,
    const microkernel::SimulationTimeStamp &initialTime)
    : period(period), phaseShift(phaseShift), initialTime(initialTime) {

  checkConstructorArgumentsValidity(period, phaseShift, initialTime);
}

microkernel::SimulationTimeStamp PeriodicTimeModel::getNextTime(
    const microkernel::SimulationTimeStamp &currentTime) {

  // If the simulation engine works appropriately, this method will be called
  // either if currentTime is the initial time, or if the identifier of the
  // currentTime has the form initialTimeId + phaseShift + N * period.
  if (currentTime == this->initialTime && this->phaseShift != 0) {
    return microkernel::SimulationTimeStamp(currentTime.getIdentifier() +
                                            this->phaseShift);
  } else {
    // Case where the currentTime has the form "initialTimeId + phaseShift + N *
    // period"
    return microkernel::SimulationTimeStamp(currentTime.getIdentifier() +
                                            this->period);
  }
}

} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
