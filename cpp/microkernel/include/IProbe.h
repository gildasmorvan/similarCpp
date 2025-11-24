#ifndef IPROBE_H
#define IPROBE_H

#include "ISimulationEngine.h"
#include "SimulationTimeStamp.h"
#include <exception>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

/**
 * Models an observation probe extracting data from the simulation when at least
 * one level becomes consistent. All methods do nothing by default.
 */
class IProbe {
public:
  virtual ~IProbe() = default;

  /**
   * Prepares the observation of a simulation.
   */
  virtual void prepareObservation() {
    // Does nothing.
  }

  /**
   * Observes the state of the simulation when the initial time stamp is
   * reached.
   * @param initialTimestamp The initial time stamp of the simulation.
   * @param simulationEngine The simulation engine embedding the currently
   * running simulation.
   */
  virtual void
  observeAtInitialTimes(const SimulationTimeStamp &initialTimestamp,
                        const ISimulationEngine &simulationEngine) {
    // Does nothing.
  }

  /**
   * Observes the state of the simulation when at least one level is in a
   * consistent state.
   * @param timestamp The time stamp for which at least one level of the
   * simulation is a consistent state.
   * @param simulationEngine The simulation engine embedding the currently
   * running simulation.
   */
  virtual void
  observeAtPartialConsistentTime(const SimulationTimeStamp &timestamp,
                                 const ISimulationEngine &simulationEngine) {
    // Does nothing.
  }

  /**
   * Observes the state of the simulation when the final time stamp of the
   * simulation is reached.
   * @param finalTimestamp The final time stamp of the simulation.
   * @param simulationEngine The simulation engine embedding the currently
   * running simulation.
   */
  virtual void observeAtFinalTime(const SimulationTimeStamp &finalTimestamp,
                                  const ISimulationEngine &simulationEngine) {
    // Does nothing.
  }

  /**
   * Reacts to an error thrown by the simulation engine.
   * @param errorMessage The error message.
   * @param cause The cause of the error.
   */
  virtual void reactToError(const std::string &errorMessage,
                            const std::exception &cause) {
    // Does nothing.
  }

  /**
   * Reacts to the abortion of a currently running simulation.
   * @param timestamp The time stamp reached right after the moment when the
   * abortion was requested.
   * @param simulationEngine The simulation engine embedding the aborted
   * simulation.
   */
  virtual void reactToAbortion(const SimulationTimeStamp &timestamp,
                               const ISimulationEngine &simulationEngine) {
    // Does nothing.
  }

  /**
   * Ends the observation of a simulation.
   */
  virtual void endObservation() {
    // Does nothing.
  }
};

} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IPROBE_H
