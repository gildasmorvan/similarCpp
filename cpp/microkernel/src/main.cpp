#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "AgentCategory.h"
#include "ILocalState.h"
#include "IProbe.h"
#include "ISimulationEngine.h"
#include "ISimulationModel.h"
#include "LevelIdentifier.h"
#include "SimulationTimeStamp.h"
#include "agents/IAgent.h"
#include "agents/IAgent4Engine.h"
#include "agents/IGlobalState.h"
#include "agents/ILocalStateOfAgent.h"
#include "agents/ILocalStateOfAgent4Engine.h"
#include "agents/IPerceivedData.h"
#include "dynamicstate/ConsistentPublicLocalDynamicState.h"
#include "dynamicstate/IModifiablePublicLocalDynamicState.h"
#include "dynamicstate/IPublicDynamicStateMap.h"
#include "dynamicstate/IPublicLocalDynamicState.h"
#include "dynamicstate/TransitoryPublicLocalDynamicState.h"
#include "environment/IEnvironment.h"
#include "environment/IEnvironment4Engine.h"
#include "environment/ILocalStateOfEnvironment.h"
#include "influences/AbstractInfluence.h"
#include "influences/IInfluence.h"
#include "influences/InfluencesMap.h"
#include "influences/RegularInfluence.h"
#include "influences/SystemInfluence.h"
#include "influences/system/SystemInfluenceAddAgent.h"
#include "influences/system/SystemInfluenceAddAgentToLevel.h"
#include "influences/system/SystemInfluenceRemoveAgent.h"
#include "influences/system/SystemInfluenceRemoveAgentFromLevel.h"
#include "levels/ILevel.h"
#include "levels/ITimeModel.h"

using namespace fr::univ_artois::lgi2a::similar::microkernel;

// --- Mock Classes for Testing ---

class MockGlobalState : public agents::IGlobalState {
public:
  virtual ~MockGlobalState() = default;

  std::shared_ptr<agents::IGlobalState> clone() const override {
    return std::make_shared<MockGlobalState>(*this);
  }
};

class MockAgent; // Forward declaration

class MockLocalStateOfAgent : public agents::ILocalStateOfAgent4Engine {
private:
  LevelIdentifier level;
  std::shared_ptr<agents::IAgent4Engine> owner;

public:
  MockLocalStateOfAgent(LevelIdentifier level,
                        std::shared_ptr<agents::IAgent4Engine> owner)
      : level(level), owner(owner) {}

  LevelIdentifier getLevel() const override { return level; }
  std::shared_ptr<agents::IAgent4Engine> getOwner() const override {
    return owner;
  }

  AgentCategory getCategoryOfAgent() const override {
    return owner->getCategory();
  }

  bool isOwnedBy(const agents::IAgent &agent) const override {
    return &agent == owner.get();
  }

  std::shared_ptr<ILocalState> clone() const override {
    return std::make_shared<MockLocalStateOfAgent>(*this);
  }
};

class MockAgent : public agents::IAgent4Engine,
                  public std::enable_shared_from_this<MockAgent> {
private:
  AgentCategory category;
  std::map<LevelIdentifier, std::shared_ptr<agents::ILocalStateOfAgent>>
      publicStates;

public:
  MockAgent(const AgentCategory &category) : category(category) {}

  AgentCategory getCategory() const override { return category; }

  std::set<LevelIdentifier> getLevels() const override {
    std::set<LevelIdentifier> levels;
    for (auto const &[key, val] : publicStates) {
      levels.insert(key);
    }
    return levels;
  }

  std::shared_ptr<agents::ILocalStateOfAgent>
  getPublicLocalState(const LevelIdentifier &level) const override {
    auto it = publicStates.find(level);
    if (it != publicStates.end()) {
      return it->second;
    }
    return nullptr;
  }

  std::shared_ptr<agents::ILocalStateOfAgent>
  getPrivateLocalState(const LevelIdentifier &level) const override {
    return nullptr; // Not needed for this test
  }

  std::shared_ptr<agents::IGlobalState> getGlobalState() const override {
    return std::make_shared<MockGlobalState>();
  }

  std::shared_ptr<agents::IPerceivedData>
  perceive(const LevelIdentifier &level,
           const SimulationTimeStamp &timeLowerBound,
           const SimulationTimeStamp &timeUpperBound,
           const std::map<LevelIdentifier,
                          std::shared_ptr<agents::ILocalStateOfAgent>>
               &publicLocalStates,
           std::shared_ptr<agents::ILocalStateOfAgent> privateLocalState,
           std::shared_ptr<dynamicstate::IPublicDynamicStateMap> dynamicStates)
      override {
    return nullptr;
  }

  void reviseGlobalState(
      const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      const std::map<LevelIdentifier, std::shared_ptr<agents::IPerceivedData>>
          &perceivedData,
      std::shared_ptr<agents::IGlobalState> globalState) override {}

  void decide(
      const LevelIdentifier &levelId, const SimulationTimeStamp &timeLowerBound,
      const SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<agents::IGlobalState> globalState,
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<agents::ILocalStateOfAgent> privateLocalState,
      std::shared_ptr<agents::IPerceivedData> perceivedData,
      std::shared_ptr<influences::InfluencesMap> producedInfluences) override {}

  void includeNewLevel(
      const LevelIdentifier &level,
      std::shared_ptr<agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<agents::ILocalStateOfAgent> privateLocalState) override {
    publicStates[level] = publicLocalState;
  }

  void excludeFromLevel(const LevelIdentifier &level) override {
    publicStates.erase(level);
  }

  std::map<LevelIdentifier, std::shared_ptr<agents::IPerceivedData>>
  getPerceivedData() const override {
    return {};
  }

  void setPerceivedData(
      std::shared_ptr<agents::IPerceivedData> perceivedData) override {}

  std::map<LevelIdentifier, std::shared_ptr<agents::ILocalStateOfAgent>>
  getPublicLocalStates() const override {
    return publicStates;
  }

  std::shared_ptr<agents::IAgent> clone() const override {
    return std::make_shared<MockAgent>(*this);
  }
};

// --- Main Test ---

int main() {
  std::cout << "Running similar-microKernel tests..." << std::endl;

  // 1. Basic Types
  SimulationTimeStamp t1(0);
  SimulationTimeStamp t2(10);
  LevelIdentifier l1("Level1");
  AgentCategory c1("Category1");
  std::cout << "[PASS] Basic types created." << std::endl;

  // 2. Regular Influence
  auto influence =
      std::make_shared<influences::RegularInfluence>("Move", l1, t1, t2);
  if (influence->getCategory() == "Move") {
    std::cout << "[PASS] RegularInfluence created." << std::endl;
  } else {
    std::cerr << "[FAIL] RegularInfluence category mismatch." << std::endl;
  }

  // 3. Dynamic States
  auto consistentState =
      std::make_shared<dynamicstate::ConsistentPublicLocalDynamicState>(t1, l1);
  consistentState->addInfluence(influence);
  std::cout << "[PASS] ConsistentPublicLocalDynamicState created." << std::endl;

  auto transitoryState =
      std::make_shared<dynamicstate::TransitoryPublicLocalDynamicState>(
          consistentState);
  std::cout << "[PASS] TransitoryPublicLocalDynamicState created." << std::endl;

  // 4. System Influences
  auto agent = std::make_shared<MockAgent>(c1);

  // Create mock states. Note: MockLocalStateOfAgent constructor needs an owner.
  // We pass 'agent' but be careful about circular references in real code
  // (weak_ptr usually preferred for owner back-pointer). For this test,
  // shared_ptr is fine as we don't rely on destruction order strictly.
  auto publicState = std::make_shared<MockLocalStateOfAgent>(l1, agent);
  auto privateState = std::make_shared<MockLocalStateOfAgent>(l1, agent);

  // 4a. SystemInfluenceAddAgent
  try {
    auto addAgentInf =
        std::make_shared<influences::system::SystemInfluenceAddAgent>(
            l1, t1, t2, agent);
    std::cout << "[PASS] SystemInfluenceAddAgent created." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[FAIL] SystemInfluenceAddAgent threw: " << e.what()
              << std::endl;
  }

  // 4b. SystemInfluenceAddAgentToLevel
  try {
    auto addAgentToLevelInf =
        std::make_shared<influences::system::SystemInfluenceAddAgentToLevel>(
            t1, t2, publicState, privateState);
    std::cout << "[PASS] SystemInfluenceAddAgentToLevel created." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[FAIL] SystemInfluenceAddAgentToLevel threw: " << e.what()
              << std::endl;
  }

  // 4c. SystemInfluenceRemoveAgent
  try {
    auto removeAgentInf =
        std::make_shared<influences::system::SystemInfluenceRemoveAgent>(
            l1, t1, t2, publicState);
    std::cout << "[PASS] SystemInfluenceRemoveAgent created." << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[FAIL] SystemInfluenceRemoveAgent threw: " << e.what()
              << std::endl;
  }

  // 4d. SystemInfluenceRemoveAgentFromLevel
  try {
    auto removeAgentFromLevelInf = std::make_shared<
        influences::system::SystemInfluenceRemoveAgentFromLevel>(t1, t2,
                                                                 publicState);
    std::cout << "[PASS] SystemInfluenceRemoveAgentFromLevel created."
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "[FAIL] SystemInfluenceRemoveAgentFromLevel threw: "
              << e.what() << std::endl;
  }

  std::cout << "All tests completed." << std::endl;

  return 0;
}
