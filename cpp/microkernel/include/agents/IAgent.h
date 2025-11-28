#ifndef IAGENT_H
#define IAGENT_H

#include <map>
#include <memory>
#include <set>

#include "../AgentCategory.h"
#include "../LevelIdentifier.h"
#include "../SimulationTimeStamp.h"
#include "../dynamicstate/IPublicDynamicStateMap.h"
#include "../influences/InfluencesMap.h"
#include "IGlobalState.h"
#include "ILocalStateOfAgent.h"
#include "IPerceivedData.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace agents {

/**
 * Models an agent contained in the simulation.
 */
class IAgent {
public:
  virtual ~IAgent() = default;

  /**
   * Gets the 'category' of agents to which this instance belongs to.
   * @return The 'category' of the agent.
   */
  virtual AgentCategory getCategory() const = 0;

  /**
   * Gets the collection of the levels where this agent lies.
   * @return A collection containing the identifier of the levels where this
   * agent lies.
   */
  virtual std::set<LevelIdentifier> getLevels() const = 0;

  /**
   * Gets the global state of the agent.
   * @return The global state of the agent.
   */
  virtual std::shared_ptr<IGlobalState> getGlobalState() const = 0;

  /**
   * Gets the public local state of the agent located in a specific level of the
   * simulation.
   * @param levelIdentifier The identifier of the level of the public local
   * state;
   * @return The public local state of the agent in the specified level.
   * @throws std::out_of_range If the agent does not define a public local state
   * for the provided level identifier.
   */
  virtual std::shared_ptr<ILocalStateOfAgent>
  getPublicLocalState(const LevelIdentifier &levelIdentifier) const = 0;

  /**
   * Gets the private local state of the agent located in a specific level of
   * the simulation.
   * @param levelIdentifier The identifier of the level of the private local
   * state;
   * @return The private local state of the agent in the specified level.
   * @throws std::out_of_range If the agent does not define a private local
   * state for the provided level identifier.
   */
  virtual std::shared_ptr<ILocalStateOfAgent>
  getPrivateLocalState(const LevelIdentifier &levelIdentifier) const = 0;

  /**
   * Creates the data perceived by an agent located in a specific level.
   * @param level The level from which perception is made.
   * @param timeLowerBound Is the lower bound of the transitory period.
   * @param timeUpperBound Is the upper bound of the transitory period.
   * @param publicLocalStates All the public local states of the agent.
   * @param privateLocalState The private local state of the agent in the level
   * from which perception is made.
   * @param dynamicStates The dynamic state of the various levels.
   * @return The data being perceived by the agent.
   */
  virtual std::shared_ptr<IPerceivedData> perceive(
      const LevelIdentifier &level, const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      const std::map<LevelIdentifier, std::shared_ptr<ILocalStateOfAgent>>
          &publicLocalStates,
      std::shared_ptr<ILocalStateOfAgent> privateLocalState,
      std::shared_ptr<dynamicstate::IPublicDynamicStateMap> dynamicStates) = 0;

  /**
   * Revises the content of the global state of the agent.
   * @param timeLowerBound Is the lower bound of the transitory period.
   * @param timeUpperBound Is the upper bound of the transitory period.
   * @param perceivedData The map containing the data that were lastly
   * perceived.
   * @param globalState The previous value of the global state of the agent.
   */
  virtual void reviseGlobalState(
      const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      const std::map<LevelIdentifier, std::shared_ptr<IPerceivedData>>
          &perceivedData,
      std::shared_ptr<IGlobalState> globalState) = 0;

  /**
   * Produces the influences resulting from the decisions of an agent.
   * @param levelId The identifier of the level from which the decision is made.
   * @param timeLowerBound Is the lower bound of the transitory period.
   * @param timeUpperBound Is the upper bound of the transitory period.
   * @param globalState The revised global state of the agent.
   * @param publicLocalState The public local state of the agent.
   * @param privateLocalState The private local state of the agent.
   * @param perceivedData The data that were perceived.
   * @param producedInfluences The map where the influences resulting from the
   * decisions are stored.
   */
  virtual void
  decide(const LevelIdentifier &levelId,
         const SimulationTimeStamp &timeLowerBound,
         const SimulationTimeStamp &timeUpperBound,
         std::shared_ptr<IGlobalState> globalState,
         std::shared_ptr<ILocalStateOfAgent> publicLocalState,
         std::shared_ptr<ILocalStateOfAgent> privateLocalState,
         std::shared_ptr<IPerceivedData> perceivedData,
         std::shared_ptr<influences::InfluencesMap> producedInfluences) = 0;

  /**
   * Clones the agent, creating a deep copy of its state.
   * @return A deep copy of the agent.
   */
  virtual std::shared_ptr<IAgent> clone() const = 0;
};

} // namespace agents
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IAGENT_H
