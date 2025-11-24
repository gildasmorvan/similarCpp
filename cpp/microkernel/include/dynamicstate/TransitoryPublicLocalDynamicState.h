#ifndef TRANSITORYPUBLICLOCALDYNAMICSTATE_H
#define TRANSITORYPUBLICLOCALDYNAMICSTATE_H

#include "ConsistentPublicLocalDynamicState.h"
#include "IModifiablePublicLocalDynamicState.h"
#include <limits>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace dynamicstate {

/**
 * Models the public dynamic state of a level when the level is in a transitory
 * phase.
 */
class TransitoryPublicLocalDynamicState
    : public IModifiablePublicLocalDynamicState {
private:
  std::shared_ptr<ConsistentPublicLocalDynamicState> lastConsistentDynamicState;
  SimulationTimeStamp timeUpperBound;
  std::set<std::shared_ptr<influences::IInfluence>>
      stateTransitoryDynamicsSystemInfluences;
  std::set<std::shared_ptr<influences::IInfluence>>
      stateTransitoryDynamicsRegularInfluences;

  static constexpr const char *FORBIDDEN_OPERATION_TEXT =
      "Transitory states do not allow this operation.";

public:
  TransitoryPublicLocalDynamicState(
      std::shared_ptr<ConsistentPublicLocalDynamicState>
          lastConsistentDynamicState,
      const SimulationTimeStamp &timeUpperBound)
      : lastConsistentDynamicState(lastConsistentDynamicState),
        timeUpperBound(timeUpperBound) {
    if (lastConsistentDynamicState == nullptr) {
      throw std::invalid_argument(
          "The 'lastConsistentDynamicState' argument cannot be null.");
    }
  }

  TransitoryPublicLocalDynamicState(
      std::shared_ptr<ConsistentPublicLocalDynamicState>
          lastConsistentDynamicState)
      : TransitoryPublicLocalDynamicState(
            lastConsistentDynamicState,
            SimulationTimeStamp(std::numeric_limits<long>::max())) {}

  LevelIdentifier getLevel() const override {
    return this->lastConsistentDynamicState->getLevel();
  }

  SimulationTimeStamp getTransitoryPeriodMin() const {
    return this->lastConsistentDynamicState->getTime();
  }

  SimulationTimeStamp getTransitoryPeriodMax() const {
    return this->timeUpperBound;
  }

  void setTransitoryPeriodMax(const SimulationTimeStamp &time) {
    if (time.compareToTimeStamp(this->getTransitoryPeriodMin()) <= 0) {
      throw std::invalid_argument("The time following the value has to be "
                                  "strictly greater than the min.");
    }
    this->timeUpperBound = time;
  }

  std::shared_ptr<environment::ILocalStateOfEnvironment>
  getPublicLocalStateOfEnvironment() const override {
    return this->lastConsistentDynamicState->getPublicLocalStateOfEnvironment();
  }

  std::set<std::shared_ptr<agents::ILocalStateOfAgent>>
  getPublicLocalStateOfAgents() const override {
    return this->lastConsistentDynamicState->getPublicLocalStateOfAgents();
  }

  std::set<std::shared_ptr<influences::IInfluence>>
  getStateDynamics() const override {
    std::set<std::shared_ptr<influences::IInfluence>> allInfluences =
        this->lastConsistentDynamicState->getStateDynamics();
    allInfluences.insert(stateTransitoryDynamicsSystemInfluences.begin(),
                         stateTransitoryDynamicsSystemInfluences.end());
    allInfluences.insert(stateTransitoryDynamicsRegularInfluences.begin(),
                         stateTransitoryDynamicsRegularInfluences.end());
    return allInfluences;
  }

  std::set<std::shared_ptr<influences::IInfluence>>
  getSystemInfluencesOfStateDynamics() const override {
    std::set<std::shared_ptr<influences::IInfluence>> systemInfluences =
        this->lastConsistentDynamicState->getSystemInfluencesOfStateDynamics();
    systemInfluences.insert(stateTransitoryDynamicsSystemInfluences.begin(),
                            stateTransitoryDynamicsSystemInfluences.end());
    return systemInfluences;
  }

  std::set<std::shared_ptr<influences::IInfluence>>
  getRegularInfluencesOfStateDynamics() const override {
    std::set<std::shared_ptr<influences::IInfluence>> regularInfluences =
        this->lastConsistentDynamicState->getRegularInfluencesOfStateDynamics();
    regularInfluences.insert(stateTransitoryDynamicsRegularInfluences.begin(),
                             stateTransitoryDynamicsRegularInfluences.end());
    return regularInfluences;
  }

  void setPublicLocalStateOfEnvironment(
      std::shared_ptr<environment::ILocalStateOfEnvironment> publicLocalState)
      override {
    throw std::logic_error(FORBIDDEN_OPERATION_TEXT);
  }

  void addPublicLocalStateOfAgent(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState) override {
    throw std::logic_error(FORBIDDEN_OPERATION_TEXT);
  }

  void removePublicLocalStateOfAgent(
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState) override {
    throw std::logic_error(FORBIDDEN_OPERATION_TEXT);
  }

  void
  addInfluence(std::shared_ptr<influences::IInfluence> influence) override {
    if (influence == nullptr) {
      throw std::invalid_argument("The 'influence' argument cannot be null.");
    }
    if (influence->isSystem()) {
      this->stateTransitoryDynamicsSystemInfluences.insert(influence);
    } else {
      this->stateTransitoryDynamicsRegularInfluences.insert(influence);
    }
  }

  void setStateDynamicsAsCopyOf(
      const std::vector<std::shared_ptr<influences::IInfluence>> &toCopy)
      override {
    throw std::logic_error(FORBIDDEN_OPERATION_TEXT);
  }

  std::shared_ptr<ConsistentPublicLocalDynamicState>
  getLastConsistentDynamicState() const {
    return this->lastConsistentDynamicState;
  }

  void clearSystemInfluences() override {
    this->stateTransitoryDynamicsSystemInfluences.clear();
  }

  void clearRegularInfluences() override {
    this->stateTransitoryDynamicsRegularInfluences.clear();
  }

  void moveInfluencesToConsistentState() {
    for (const auto &influence :
         this->stateTransitoryDynamicsSystemInfluences) {
      this->lastConsistentDynamicState->addInfluence(influence);
    }
    this->stateTransitoryDynamicsSystemInfluences.clear();
    for (const auto &influence :
         this->stateTransitoryDynamicsRegularInfluences) {
      this->lastConsistentDynamicState->addInfluence(influence);
    }
    this->stateTransitoryDynamicsRegularInfluences.clear();
  }
};

} // namespace dynamicstate
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // TRANSITORYPUBLICLOCALDYNAMICSTATE_H
