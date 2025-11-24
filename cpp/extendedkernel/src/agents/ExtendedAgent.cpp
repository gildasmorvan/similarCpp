#include "../../include/agents/ExtendedAgent.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace agents {

ExtendedAgent::ExtendedAgent(const microkernel::AgentCategory &category)
    : AbstractAgent(category) {}

std::shared_ptr<IAgtGlobalStateRevisionModel>
ExtendedAgent::getGlobalStateRevisionModel() const {
  if (globalStateRevisionModel == nullptr) {
    throw std::out_of_range(
        "The global state revision model of the agent is not specified.");
  }
  return globalStateRevisionModel;
}

void ExtendedAgent::specifyGlobalStateRevisionModel(
    std::shared_ptr<IAgtGlobalStateRevisionModel> revisionMdl) {
  if (revisionMdl == nullptr) {
    throw std::invalid_argument("The argument cannot be null.");
  }
  this->globalStateRevisionModel = revisionMdl;
}

std::shared_ptr<IAgtPerceptionModel> ExtendedAgent::getPerceptionModel(
    const microkernel::LevelIdentifier &levelId) const {
  auto it = perceptionModels.find(levelId);
  if (it == perceptionModels.end()) {
    throw std::out_of_range(
        "The perception model of the agent is not specified for the level '" +
        levelId.toString() + "'.");
  }
  return it->second;
}

std::shared_ptr<IAgtDecisionModel> ExtendedAgent::getDecisionModel(
    const microkernel::LevelIdentifier &levelId) const {
  auto it = decisionModels.find(levelId);
  if (it == decisionModels.end()) {
    throw std::out_of_range(
        "The decision model of the agent is not specified for the level '" +
        levelId.toString() + "'.");
  }
  return it->second;
}

void ExtendedAgent::specifyBehaviorForLevel(
    const microkernel::LevelIdentifier &levelId,
    std::shared_ptr<IAgtPerceptionModel> perceptionMdl,
    std::shared_ptr<IAgtDecisionModel> decisionMdl) {

  if (perceptionMdl == nullptr || decisionMdl == nullptr) {
    throw std::invalid_argument("The arguments cannot be null.");
  }
  if (!(perceptionMdl->getLevel() == levelId)) {
    throw std::invalid_argument("The level of the perception model has to "
                                "match the value of the argument 'levelId'.");
  }
  if (!(decisionMdl->getLevel() == levelId)) {
    throw std::invalid_argument("The level of the decision model has to match "
                                "the value of the argument 'levelId'.");
  }

  perceptionModels[levelId] = perceptionMdl;
  decisionModels[levelId] = decisionMdl;
}

void ExtendedAgent::removeBehaviorForLevel(
    const microkernel::LevelIdentifier &levelId) {
  perceptionModels.erase(levelId);
  decisionModels.erase(levelId);
}

std::shared_ptr<microkernel::agents::IPerceivedData> ExtendedAgent::perceive(
    const microkernel::LevelIdentifier &level,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    const std::map<microkernel::LevelIdentifier,
                   std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
        &publicLocalStates,
    std::shared_ptr<microkernel::agents::ILocalStateOfAgent> privateLocalState,
    std::shared_ptr<microkernel::dynamicstate::IPublicDynamicStateMap>
        dynamicStates) {

  return getPerceptionModel(level)->perceive(timeLowerBound, timeUpperBound,
                                             publicLocalStates,
                                             privateLocalState, dynamicStates);
}

void ExtendedAgent::reviseGlobalState(
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    const std::map<microkernel::LevelIdentifier,
                   std::shared_ptr<microkernel::agents::IPerceivedData>>
        &perceivedData,
    std::shared_ptr<microkernel::agents::IGlobalState> globalState) {

  getGlobalStateRevisionModel()->reviseGlobalState(
      timeLowerBound, timeUpperBound, perceivedData, globalState);
}

void ExtendedAgent::decide(
    const microkernel::LevelIdentifier &levelId,
    const microkernel::SimulationTimeStamp &timeLowerBound,
    const microkernel::SimulationTimeStamp &timeUpperBound,
    std::shared_ptr<microkernel::agents::IGlobalState> globalState,
    std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
    std::shared_ptr<microkernel::agents::ILocalStateOfAgent> privateLocalState,
    std::shared_ptr<microkernel::agents::IPerceivedData> perceivedData,
    std::shared_ptr<microkernel::influences::InfluencesMap>
        producedInfluences) {

  getDecisionModel(levelId)->decide(timeLowerBound, timeUpperBound, globalState,
                                    publicLocalState, privateLocalState,
                                    perceivedData, producedInfluences);
}

} // namespace agents
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
