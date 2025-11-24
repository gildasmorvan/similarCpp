#ifndef ABSTRACTSIMULATIONMODEL_H
#define ABSTRACTSIMULATIONMODEL_H

#include "../../ISimulationModel.h"
#include "../../SimulationTimeStamp.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {
namespace abstractimpl {

/**
 * An abstract implementation of the ISimulationModel interface,
 * providing a default behavior to the initial time-related methods.
 */
class AbstractSimulationModel : public ISimulationModel {
private:
  /**
   * The initial time stamp of this simulation model.
   */
  SimulationTimeStamp initialTime;

protected:
  /**
   * Builds an instance of an abstract simulation model, having a specific time
   * stamp as initial time.
   * @param initialTime The initial time stamp of this simulation model.
   */
  explicit AbstractSimulationModel(const SimulationTimeStamp &initialTime)
      : initialTime(initialTime) {}

public:
  virtual ~AbstractSimulationModel() = default;

  /**
   * Gets the initial time stamp of the simulation.
   * @return The initial time stamp.
   */
  SimulationTimeStamp getInitialTime() const override {
    return this->initialTime;
  }
};

} // namespace abstractimpl
} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTSIMULATIONMODEL_H
