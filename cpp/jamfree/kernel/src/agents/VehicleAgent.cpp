#include "../../include/agents/VehicleAgent.h"
#include "../../../../microkernel/include/AgentCategory.h"
#include <stdexcept>

namespace jamfree {
namespace kernel {
namespace agents {

VehicleAgent::VehicleAgent(const std::string &id)
    : fr::univ_artois::lgi2a::similar::extendedkernel::agents::ExtendedAgent(
          fr::univ_artois::lgi2a::similar::microkernel::AgentCategory(
              "Vehicle")),
      m_id(id) {}

void VehicleAgent::addLevel(const LevelIdentifier &level) {
  // In SIMILAR, levels are added via includeNewLevel with states.
  // This method is kept for compatibility but does nothing if states are not
  // provided. Users should use includeNewLevel or
  // setPublicLocalState/setPrivateLocalState carefully.
}

bool VehicleAgent::hasLevel(const LevelIdentifier &level) const {
  auto levels = getLevels();
  return levels.find(level) != levels.end();
}

std::shared_ptr<ILocalState>
VehicleAgent::getPublicLocalState(const LevelIdentifier &level) const {
  return ExtendedAgent::getPublicLocalState(level);
}

void VehicleAgent::setPublicLocalState(const LevelIdentifier &level,
                                       std::shared_ptr<ILocalState> state) {
  if (hasLevel(level)) {
    auto privateState = fr::univ_artois::lgi2a::similar::extendedkernel::
        agents::ExtendedAgent::getPrivateLocalState(level);
    excludeFromLevel(level);
    includeNewLevel(level, state, privateState);
  } else {
    // If level doesn't exist, we can't add it without private state.
    // We throw an exception to enforce correct usage.
    throw std::runtime_error(
        "Cannot set public state for new level '" + level.toString() +
        "' without providing both public and private states");
  }
}

std::shared_ptr<ILocalState>
VehicleAgent::getPrivateLocalState(const LevelIdentifier &level) const {
  return ExtendedAgent::getPrivateLocalState(level);
}

void VehicleAgent::setPrivateLocalState(const LevelIdentifier &level,
                                        std::shared_ptr<ILocalState> state) {
  if (hasLevel(level)) {
    auto publicState = fr::univ_artois::lgi2a::similar::extendedkernel::agents::
        ExtendedAgent::getPublicLocalState(level);
    excludeFromLevel(level);
    includeNewLevel(level, publicState, state);
  } else {
    throw std::runtime_error(
        "Cannot set private state for new level '" + level.toString() +
        "' without public state. Use includeNewLevel(level, public, private).");
  }
}

void VehicleAgent::setPerceptionModel(const LevelIdentifier &level,
                                      std::shared_ptr<IPerceptionModel> model) {
  std::shared_ptr<IDecisionModel> decisionModelShared = nullptr;
  try {
    decisionModelShared = fr::univ_artois::lgi2a::similar::extendedkernel::
        agents::ExtendedAgent::getDecisionModel(level);
  } catch (...) {
    // Decision model might not be set yet
  }

  specifyBehaviorForLevel(level, model, decisionModelShared);
}

IPerceptionModel *
VehicleAgent::getPerceptionModel(const LevelIdentifier &level) {
  return fr::univ_artois::lgi2a::similar::extendedkernel::agents::
      ExtendedAgent::getPerceptionModel(level)
          .get();
}

void VehicleAgent::setDecisionModel(const LevelIdentifier &level,
                                    std::shared_ptr<IDecisionModel> model) {
  std::shared_ptr<IPerceptionModel> perceptionModelShared = nullptr;
  try {
    perceptionModelShared = fr::univ_artois::lgi2a::similar::extendedkernel::
        agents::ExtendedAgent::getPerceptionModel(level);
  } catch (...) {
    // Perception model might not be set yet
  }

  specifyBehaviorForLevel(level, perceptionModelShared, model);
}

void VehicleAgent::setModels(const LevelIdentifier &level,
                             std::shared_ptr<IPerceptionModel> perceptionModel,
                             std::shared_ptr<IDecisionModel> decisionModel) {
  specifyBehaviorForLevel(level, perceptionModel, decisionModel);
}

void VehicleAgent::setStates(const LevelIdentifier &level,
                             std::shared_ptr<ILocalState> publicState,
                             std::shared_ptr<ILocalState> privateState) {
  if (hasLevel(level)) {
    excludeFromLevel(level);
  }
  includeNewLevel(level, publicState, privateState);
}

IDecisionModel *VehicleAgent::getDecisionModel(const LevelIdentifier &level) {
  return fr::univ_artois::lgi2a::similar::extendedkernel::agents::
      ExtendedAgent::getDecisionModel(level)
          .get();
}

} // namespace agents
} // namespace kernel
} // namespace jamfree
