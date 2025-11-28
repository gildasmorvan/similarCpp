#include "../../include/libs/AbstractAgent.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace libs {

AbstractAgent::AbstractAgent(const AgentCategory &category)
    : category(category) {
  // Maps are default initialized as empty
}

AbstractAgent::AbstractAgent(const AbstractAgent &other)
    : category(other.category) {
  if (other.globalState) {
    this->globalState = std::dynamic_pointer_cast<agents::IGlobalState>(
        other.globalState->clone());
  }
  for (const auto &pair : other.publicLocalStates) {
    this->publicLocalStates[pair.first] =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent>(
            pair.second->clone());
  }
  for (const auto &pair : other.privateLocalStates) {
    this->privateLocalStates[pair.first] =
        std::dynamic_pointer_cast<agents::ILocalStateOfAgent>(
            pair.second->clone());
  }
  for (const auto &pair : other.lastPerceivedData) {
    this->lastPerceivedData[pair.first] =
        std::dynamic_pointer_cast<agents::IPerceivedData>(pair.second->clone());
  }
}

AgentCategory AbstractAgent::getCategory() const { return this->category; }

std::shared_ptr<agents::IGlobalState> AbstractAgent::getGlobalState() const {
  return this->globalState;
}

void AbstractAgent::initializeGlobalState(
    std::shared_ptr<agents::IGlobalState> initialGlobalState) {
  if (initialGlobalState == nullptr) {
    throw std::invalid_argument(
        buildNullArgumentExceptionText("initialGlobalState"));
  }
  this->globalState = initialGlobalState;
}

std::set<LevelIdentifier> AbstractAgent::getLevels() const {
  std::set<LevelIdentifier> levels;
  for (const auto &pair : publicLocalStates) {
    levels.insert(pair.first);
  }
  return levels;
}

std::shared_ptr<agents::ILocalStateOfAgent>
AbstractAgent::getPublicLocalState(const LevelIdentifier &levelId) const {
  auto it = publicLocalStates.find(levelId);
  if (it == publicLocalStates.end()) {
    throw std::out_of_range(
        "The agent does not define a public local state for the level '" +
        levelId.toString() + "'.");
  }
  return it->second;
}

std::map<LevelIdentifier, std::shared_ptr<agents::ILocalStateOfAgent>>
AbstractAgent::getPublicLocalStates() const {
  return this->publicLocalStates;
}

std::shared_ptr<agents::ILocalStateOfAgent>
AbstractAgent::getPrivateLocalState(const LevelIdentifier &levelId) const {
  auto it = privateLocalStates.find(levelId);
  if (it == privateLocalStates.end()) {
    throw std::out_of_range(
        "The agent does not define a private local state for the level '" +
        levelId.toString() + "'.");
  }
  return it->second;
}

void AbstractAgent::includeNewLevel(
    const LevelIdentifier &levelIdentifier,
    std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState,
    std::shared_ptr<agents::ILocalStateOfAgent> privateLocalState) {

  if (publicLocalState == nullptr) {
    throw std::invalid_argument(
        buildNullArgumentExceptionText("publicLocalState"));
  }
  if (privateLocalState == nullptr) {
    throw std::invalid_argument(
        buildNullArgumentExceptionText("privateLocalState"));
  }

  if (publicLocalStates.find(levelIdentifier) == publicLocalStates.end()) {
    publicLocalStates[levelIdentifier] = publicLocalState;
    privateLocalStates[levelIdentifier] = privateLocalState;
  }
}

void AbstractAgent::excludeFromLevel(const LevelIdentifier &levelIdentifier) {
  publicLocalStates.erase(levelIdentifier);
  privateLocalStates.erase(levelIdentifier);
}

std::map<LevelIdentifier, std::shared_ptr<agents::IPerceivedData>>
AbstractAgent::getPerceivedData() const {
  return this->lastPerceivedData;
}

void AbstractAgent::setPerceivedData(
    std::shared_ptr<agents::IPerceivedData> perceivedData) {
  if (perceivedData == nullptr) {
    throw std::invalid_argument(
        buildNullArgumentExceptionText("perceivedData"));
  }
  this->lastPerceivedData[perceivedData->getLevel()] = perceivedData;
}

} // namespace libs
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
