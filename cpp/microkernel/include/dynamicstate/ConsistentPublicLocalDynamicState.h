#ifndef CONSISTENTPUBLICLOCALDYNAMICSTATE_H
#define CONSISTENTPUBLICLOCALDYNAMICSTATE_H

#include "../SimulationTimeStamp.h"
#include "IModifiablePublicLocalDynamicState.h"
#include <algorithm>
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace dynamicstate {

/**
 * Models a consistent public local dynamic state for a level l at a time t.
 */
class ConsistentPublicLocalDynamicState
    : public IModifiablePublicLocalDynamicState {
private:
  LevelIdentifier level;
  SimulationTimeStamp time;
  std::shared_ptr<environment::ILocalStateOfEnvironment>
      publicLocalstateOfEnvironment;
  std::set<std::shared_ptr<agents::ILocalStateOfAgent>>
      publicLocalStateOfAgents;
  std::set<std::shared_ptr<influences::IInfluence>>
      stateDynamicsSystemInfluences;
  std::set<std::shared_ptr<influences::IInfluence>>
      stateDynamicsRegularInfluences;

public:
  ConsistentPublicLocalDynamicState(const SimulationTimeStamp &time,
                                    const LevelIdentifier &level)
      : level(level), time(time) {
    // In Java it checked for null level.
  }

  LevelIdentifier getLevel() const override { return this->level; }

  SimulationTimeStamp getTime() const { return this->time; }

  void setTime(const SimulationTimeStamp &time) { this->time = time; }

  std::shared_ptr<environment::ILocalStateOfEnvironment>
  getPublicLocalStateOfEnvironment() const override {
    return this->publicLocalstateOfEnvironment;
  }

  std::set<std::shared_ptr<agents::ILocalStateOfAgent>>
  getPublicLocalStateOfAgents() const override {
    return this->publicLocalStateOfAgents;
  }

  std::set<std::shared_ptr<influences::IInfluence>>
  getStateDynamics() const override {
    std::set<std::shared_ptr<influences::IInfluence>> allInfluences;
    allInfluences.insert(stateDynamicsSystemInfluences.begin(),
                         stateDynamicsSystemInfluences.end());
    allInfluences.insert(stateDynamicsRegularInfluences.begin(),
                         stateDynamicsRegularInfluences.end());
    return allInfluences;
  }

  std::set<std::shared_ptr<influences::IInfluence>>
  getSystemInfluencesOfStateDynamics() const override {
    return this->stateDynamicsSystemInfluences;
  }

  std::set<std::shared_ptr<influences::IInfluence>>
  getRegularInfluencesOfStateDynamics() const override {
    return this->stateDynamicsRegularInfluences;
  }

  void setPublicLocalStateOfEnvironment(
      std::shared_ptr<environment::ILocalStateOfEnvironment> publicLocalState)
      override {
    if (publicLocalState == nullptr) {
      throw std::invalid_argument(
          "The 'publicLocalState' argument cannot be null.");
    }
    this->publicLocalstateOfEnvironment = publicLocalState;
  }

  void addPublicLocalStateOfAgent(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState) override {
    if (publicLocalState == nullptr) {
      throw std::invalid_argument(
          "The 'publicLocalState' argument cannot be null.");
    }
    this->publicLocalStateOfAgents.insert(publicLocalState);
  }

  void removePublicLocalStateOfAgent(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState) override {
    if (publicLocalState == nullptr) {
      throw std::invalid_argument(
          "The 'publicLocalState' argument cannot be null.");
    }
    this->publicLocalStateOfAgents.erase(publicLocalState);
  }

  void
  addInfluence(std::shared_ptr<influences::IInfluence> influence) override {
    if (influence == nullptr) {
      throw std::invalid_argument("The 'influence' argument cannot be null.");
    }
    if (influence->isSystem()) {
      this->stateDynamicsSystemInfluences.insert(influence);
    } else {
      this->stateDynamicsRegularInfluences.insert(influence);
    }
  }

  void setStateDynamicsAsCopyOf(
      const std::vector<std::shared_ptr<influences::IInfluence>> &toCopy)
      override {
    this->stateDynamicsRegularInfluences.clear();
    this->stateDynamicsSystemInfluences.clear();
    for (const auto &influence : toCopy) {
      this->addInfluence(influence);
    }
  }

  void clearSystemInfluences() override {
    this->stateDynamicsSystemInfluences.clear();
  }

  void clearRegularInfluences() override {
    this->stateDynamicsRegularInfluences.clear();
  }

  std::shared_ptr<IPublicLocalDynamicState> clone() const override {
    auto clonedState = std::make_shared<ConsistentPublicLocalDynamicState>(
        this->time, this->level);
    if (this->publicLocalstateOfEnvironment) {
      clonedState->setPublicLocalStateOfEnvironment(
          std::dynamic_pointer_cast<environment::ILocalStateOfEnvironment>(
              this->publicLocalstateOfEnvironment->clone()));
    }
    for (const auto &agentState : this->publicLocalStateOfAgents) {
      clonedState->addPublicLocalStateOfAgent(
          std::dynamic_pointer_cast<agents::ILocalStateOfAgent>(
              agentState->clone()));
    }
    // Shallow copy of influences as they are typically immutable
    for (const auto &influence : this->stateDynamicsSystemInfluences) {
      clonedState->addInfluence(influence);
    }
    for (const auto &influence : this->stateDynamicsRegularInfluences) {
      clonedState->addInfluence(influence);
    }
    return clonedState;
  }
};

} // namespace dynamicstate
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // CONSISTENTPUBLICLOCALDYNAMICSTATE_H
