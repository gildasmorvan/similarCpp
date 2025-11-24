#ifndef ISIMULATIONMODEL_H
#define ISIMULATIONMODEL_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "LevelIdentifier.h"
#include "SimulationTimeStamp.h"
#include "agents/IAgent4Engine.h"
#include "environment/IEnvironment4Engine.h"
#include "influences/InfluencesMap.h"
#include "levels/ILevel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

// Forward declaration to avoid circular dependency
class ISimulationEngine;

/**
 * Models a simulation that can be performed using a simulation engine.
 */
class ISimulationModel {
public:
  virtual ~ISimulationModel() = default;

  /**
   * Models the initialization data coming from the generation of the
   * environment of the simulation.
   */
  class EnvironmentInitializationData {
  private:
    /**
     * The environment of the simulation.
     */
    std::shared_ptr<environment::IEnvironment4Engine> environment;
    /**
     * The influences resulting from the generation of the environment.
     */
    std::shared_ptr<influences::InfluencesMap> influences;

  public:
    /**
     * Builds the initialization data coming from the generation of the
     * environment of the simulation.
     * @param environment The environment of the simulation.
     */
    explicit EnvironmentInitializationData(
        std::shared_ptr<environment::IEnvironment4Engine> environment)
        : environment(environment),
          influences(std::make_shared<influences::InfluencesMap>()) {}

    /**
     * Gets the environment of the simulation.
     * @return The environment of the simulation.
     */
    std::shared_ptr<environment::IEnvironment4Engine> getEnvironment() const {
      return this->environment;
    }

    /**
     * Gets the influences resulting from the generation of the environment.
     * @return The influences resulting from the generation of the environment.
     */
    std::shared_ptr<influences::InfluencesMap> getInfluences() const {
      return this->influences;
    }
  };

  /**
   * Models the initialization data coming from the generation of the initial
   * agents of the simulation.
   */
  class AgentInitializationData {
  private:
    /**
     * The agents of the simulation.
     */
    std::set<std::shared_ptr<agents::IAgent4Engine>> agents;
    /**
     * The influences resulting from the generation of the agents.
     */
    std::shared_ptr<influences::InfluencesMap> influences;

  public:
    /**
     * Builds the initialization data coming from the generation of the initial
     * agents of the simulation.
     */
    AgentInitializationData()
        : influences(std::make_shared<influences::InfluencesMap>()) {}

    /**
     * Gets the agents of the simulation.
     * @return The agents of the simulation.
     */
    std::set<std::shared_ptr<agents::IAgent4Engine>> &getAgents() {
      return this->agents;
    }

    /**
     * Gets the influences resulting from the generation of the agents.
     * @return The influences resulting from the generation of the agents.
     */
    std::shared_ptr<influences::InfluencesMap> getInfluences() const {
      return this->influences;
    }
  };

  /**
   * Gets the initial time of this time model.
   * @return The initial time of this model.
   */
  virtual SimulationTimeStamp getInitialTime() const = 0;

  /**
   * Tells if a time stamp is greater or equal to the final time stamp.
   * @param currentTime The last time the dynamic state of the simulation was in
   * a partly consistent state.
   * @param engine The simulation engine containing information about the
   * currently running simulation.
   * @return true if the final time of the level was reached.
   */
  virtual bool isFinalTimeOrAfter(const SimulationTimeStamp &currentTime,
                                  const ISimulationEngine &engine) const = 0;

  /**
   * Generates the bare levels of the simulation. These levels contain no agents
   * and define no environment.
   * @param initialTime The initial time of the simulation.
   * @return The bare levels of the simulation.
   */
  virtual std::vector<std::shared_ptr<levels::ILevel>>
  generateLevels(const SimulationTimeStamp &initialTime) = 0;

  /**
   * Generates the environment of the simulation. At this stage, no agent are
   * generated in the simulation.
   * @param initialTime The initial time of the simulation.
   * @param levels The levels of the simulation.
   * @return The generated environment and the influences.
   */
  virtual EnvironmentInitializationData generateEnvironment(
      const SimulationTimeStamp &initialTime,
      const std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
          &levels) = 0;

  /**
   * Generates the agents of the simulation.
   * @param initialTime The initial time of the simulation
   * @param levels The levels of the simulation.
   * @return The generated agents and the influences.
   */
  virtual AgentInitializationData generateAgents(
      const SimulationTimeStamp &initialTime,
      const std::map<LevelIdentifier, std::shared_ptr<levels::ILevel>>
          &levels) = 0;
};

} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ISIMULATIONMODEL_H
