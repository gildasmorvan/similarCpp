#ifndef ISIMULATIONENGINE_H
#define ISIMULATIONENGINE_H

#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

#include "ISimulationModel.h"
#include "LevelIdentifier.h"
#include "agents/IAgent4Engine.h"
#include "dynamicstate/ConsistentPublicLocalDynamicState.h"
#include "dynamicstate/IPublicDynamicStateMap.h"
#include "dynamicstate/TransitoryPublicLocalDynamicState.h"
#include "environment/IEnvironment4Engine.h"
#include "levels/ILevel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

// Forward declaration
class IProbe;

/**
 * Exception thrown when the simulation is aborted.
 */
class ExceptionSimulationAborted : public std::runtime_error {
public:
  ExceptionSimulationAborted(const std::string &message)
      : std::runtime_error(message) {}
};

/**
 * Models a simulation engine, i.e. the object moving the simulation through
 * time.
 */
class ISimulationEngine {
public:
  virtual ~ISimulationEngine() = default;

  /**
   * Adds a probe to this simulation engine.
   * @param identifier An unique identifier for the probe.
   * @param probe The probe to add to this simulation engine.
   * @throws std::invalid_argument If the arguments are null/empty, or if a
   * probe is already defined for this identifier.
   */
  virtual void addProbe(const std::string &identifier,
                        std::shared_ptr<IProbe> probe) = 0;

  /**
   * Removes a probe from the simulation engine.
   * @param identifier The identifier of the probe to remove.
   * @return The removed probe, nullptr if no probe having the provided
   * identifier was registered to this engine.
   */
  virtual std::shared_ptr<IProbe>
  removeProbe(const std::string &identifier) = 0;

  /**
   * Lists the identifier of all the probes that are registered to this engine.
   * @return The identifier of all the probes that are registered to this
   * engine.
   */
  virtual std::set<std::string> getProbesIdentifiers() const = 0;

  /**
   * Requests the abortion of the simulation currently running with this
   * simulation engine.
   */
  virtual void requestSimulationAbortion() = 0;

  /**
   * Initializes and then runs completely a simulation.
   * @param simulationModel The simulation model running the simulation.
   * @throws std::invalid_argument If the arguments are null.
   * @throws ExceptionSimulationAborted if the simulation has ended because it
   * was aborted by the user.
   */
  virtual void
  runNewSimulation(std::shared_ptr<ISimulationModel> simulationModel) = 0;

  /**
   * Runs the simulation until the specified final time.
   * This method assumes the simulation has been initialized.
   * @param finalTime The time until which the simulation should run.
   */
  virtual void runSimulation(const SimulationTimeStamp &finalTime) = 0;

  /**
   * Gets the current dynamic states of the simulation.
   * @return The dynamic state of the simulation.
   */
  virtual std::shared_ptr<dynamicstate::IPublicDynamicStateMap>
  getSimulationDynamicStates() const = 0;

  /**
   * Gets the set of all the agents lying in the simulation.
   * @return The set of all the agents lying in the simulation.
   */
  virtual std::set<std::shared_ptr<agents::IAgent4Engine>>
  getAgents() const = 0;

  /**
   * Gets the set of level identifiers contained in the simulation.
   * @return The list of levels contained in the simulation.
   */
  virtual std::set<LevelIdentifier> getLevelIdentifiers() const = 0;

  /**
   * Gets the list of levels contained in the simulation.
   * @return The list of levels contained in the simulation.
   */
  virtual std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
  getLevels() const = 0;

  /**
   * Gets the set of all the agents lying in a specific level of the simulation.
   * @param level The levels where to get the agents.
   * @return The set of all the agents lying in a specific level of in the
   * simulation.
   * @throws std::out_of_range If no such level was defined for the simulation.
   */
  virtual std::set<std::shared_ptr<agents::IAgent4Engine>>
  getAgents(const LevelIdentifier &level) const = 0;

  /**
   * Gets the environment the simulation.
   * @return The environment of the simulation.
   */
  virtual std::shared_ptr<environment::IEnvironment4Engine>
  getEnvironment() const = 0;

  /**
   * Disambiguates a public local dynamic state.
   * @param transitoryDynamicState The transitory state for which a
   * disambiguation is computed.
   * @return the observable dynamic state corresponding to the disambiguation of
   * the transitory dynamic state.
   */
  virtual std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
  disambiguation(
      std::shared_ptr<dynamicstate::TransitoryPublicLocalDynamicState>
          transitoryDynamicState) const = 0;

  /**
   * Clones the simulation engine, creating a deep copy of the simulation state.
   * @return A deep copy of the simulation engine.
   */
  virtual std::shared_ptr<ISimulationEngine> clone() const = 0;
};

} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ISIMULATIONENGINE_H
