#ifndef EXTENDEDAGENT_H
#define EXTENDEDAGENT_H

#include "IAgtDecisionModel.h"
#include "IAgtGlobalStateRevisionModel.h"
#include "IAgtPerceptionModel.h"
#include "libs/AbstractAgent.h"
#include <map>
#include <memory>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace microkernel = similar::microkernel;
namespace agents {

/**
 * Models an agent in the extended kernel.
 * The behavior is defined in separate model classes for modularity.
 */
class ExtendedAgent : public similar::microkernel::libs::AbstractAgent {
private:
  std::map<microkernel::LevelIdentifier, std::shared_ptr<IAgtPerceptionModel>>
      perceptionModels;
  std::shared_ptr<IAgtGlobalStateRevisionModel> globalStateRevisionModel;
  std::map<microkernel::LevelIdentifier, std::shared_ptr<IAgtDecisionModel>>
      decisionModels;

public:
  explicit ExtendedAgent(const microkernel::AgentCategory &category);
  virtual ~ExtendedAgent() = default;

  // Expose base class methods
  using similar::microkernel::libs::AbstractAgent::includeNewLevel;
  using similar::microkernel::libs::AbstractAgent::initializeGlobalState;

  // Extended agent methods
  std::shared_ptr<IAgtGlobalStateRevisionModel>
  getGlobalStateRevisionModel() const;
  void specifyGlobalStateRevisionModel(
      std::shared_ptr<IAgtGlobalStateRevisionModel> revisionMdl);

  std::shared_ptr<IAgtPerceptionModel>
  getPerceptionModel(const microkernel::LevelIdentifier &levelId) const;
  std::shared_ptr<IAgtDecisionModel>
  getDecisionModel(const microkernel::LevelIdentifier &levelId) const;

  void
  specifyBehaviorForLevel(const microkernel::LevelIdentifier &levelId,
                          std::shared_ptr<IAgtPerceptionModel> perceptionMdl,
                          std::shared_ptr<IAgtDecisionModel> decisionMdl);

  void removeBehaviorForLevel(const microkernel::LevelIdentifier &levelId);

  // Microkernel agent interface implementations
  std::shared_ptr<microkernel::agents::IPerceivedData> perceive(
      const microkernel::LevelIdentifier &level,
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
          &publicLocalStates,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::dynamicstate::IPublicDynamicStateMap>
          dynamicStates) override;

  void reviseGlobalState(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::IPerceivedData>>
          &perceivedData,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState) override;

  void decide(
      const microkernel::LevelIdentifier &levelId,
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::agents::IPerceivedData> perceivedData,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          producedInfluences) override;
};

} // namespace agents
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EXTENDEDAGENT_H
