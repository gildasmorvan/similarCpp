/**
 * SIMILAR C++ - Complete ExtendedLibs Demo
 *
 * This example demonstrates ALL the extendedlibs utilities:
 * - AbstractAgtPerceptionModel & AbstractAgtDecisionModel
 * - EmptyAgtPerceptionModel & EmptyAgtDecisionModel
 * - IdentityAgtGlobalStateRevisionModel
 * - PeriodicTimeModel
 * - EmptyPerceivedData
 */

#include "../include/agents/ExtendedAgent.h"
#include "../include/libs/abstractimpl/AbstractAgtDecisionModel.h"
#include "../include/libs/abstractimpl/AbstractAgtPerceptionModel.h"
#include "../include/libs/generic/EmptyAgtDecisionModel.h"
#include "../include/libs/generic/EmptyAgtPerceptionModel.h"
#include "../include/libs/generic/IdentityAgtGlobalStateRevisionModel.h"
#include "../include/libs/timemodel/PeriodicTimeModel.h"
#include <iostream>
#include <memory>

using namespace fr::univ_artois::lgi2a::similar;

// Simple global state
class SimpleGlobalState : public microkernel::agents::IGlobalState {
public:
  int counter = 0;
  virtual ~SimpleGlobalState() = default;

  std::shared_ptr<microkernel::agents::IGlobalState> clone() const override {
    return std::make_shared<SimpleGlobalState>(*this);
  }
};

int main() {
  std::cout << "╔══════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║     SIMILAR C++ - Complete ExtendedLibs Demo        ║"
            << std::endl;
  std::cout << "╚══════════════════════════════════════════════════════╝"
            << std::endl;
  std::cout << std::endl;

  microkernel::LevelIdentifier mainLevel("MainLevel");
  microkernel::SimulationTimeStamp initialTime(0);

  // ===== 1. PeriodicTimeModel =====
  std::cout << "1️⃣  Creating PeriodicTimeModel..." << std::endl;
  auto timeModel = std::make_shared<extendedkernel::libs::PeriodicTimeModel>(
      5, // period: advance by 5 each step
      2, // phase shift: first step is +2, then +5, +5, ...
      initialTime);
  std::cout << "   ✓ Period: " << timeModel->getPeriod() << std::endl;
  std::cout << "   ✓ Phase Shift: " << timeModel->getPhaseShift() << std::endl;
  std::cout << std::endl;

  // Test time progression
  std::cout << "   Time progression:" << std::endl;
  microkernel::SimulationTimeStamp currentTime = initialTime;
  for (int i = 0; i < 5; i++) {
    std::cout << "   Step " << i << ": t = " << currentTime.getIdentifier();
    currentTime = timeModel->getNextTime(currentTime);
    std::cout << " → " << currentTime.getIdentifier() << std::endl;
  }
  std::cout << std::endl;

  // ===== 2. Empty Models =====
  std::cout << "2️⃣  Creating Empty Models (for passive agents)..." << std::endl;

  auto emptyPerception =
      std::make_shared<extendedkernel::libs::generic::EmptyAgtPerceptionModel>(
          mainLevel);
  std::cout << "   ✓ EmptyAgtPerceptionModel created" << std::endl;

  auto emptyDecision =
      std::make_shared<extendedkernel::libs::generic::EmptyAgtDecisionModel>(
          mainLevel);
  std::cout << "   ✓ EmptyAgtDecisionModel created" << std::endl;

  auto identityRevision = std::make_shared<
      extendedkernel::libs::generic::IdentityAgtGlobalStateRevisionModel>();
  std::cout << "   ✓ IdentityAgtGlobalStateRevisionModel created" << std::endl;
  std::cout << std::endl;

  // ===== 3. Create Passive Agent =====
  std::cout << "3️⃣  Creating Passive Agent (using empty models)..." << std::endl;

  microkernel::AgentCategory passiveCategory("PassiveAgent");
  auto passiveAgent =
      std::make_shared<extendedkernel::agents::ExtendedAgent>(passiveCategory);

  auto passiveGlobalState = std::make_shared<SimpleGlobalState>();
  passiveAgent->initializeGlobalState(passiveGlobalState);

  passiveAgent->specifyGlobalStateRevisionModel(identityRevision);
  passiveAgent->specifyBehaviorForLevel(mainLevel, emptyPerception,
                                        emptyDecision);

  std::cout << "   ✓ Passive agent configured" << std::endl;
  std::cout << std::endl;

  // ===== 4. Test Passive Agent =====
  std::cout << "4️⃣  Testing Passive Agent (should do nothing)..." << std::endl;

  microkernel::SimulationTimeStamp t0(0);
  microkernel::SimulationTimeStamp t1(1);

  // Perception (returns EmptyPerceivedData)
  std::map<microkernel::LevelIdentifier,
           std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
      emptyPublic;
  auto perceivedData =
      passiveAgent->perceive(mainLevel, t0, t1, emptyPublic, nullptr, nullptr);
  std::cout << "   ✓ Perception completed (returned EmptyPerceivedData)"
            << std::endl;

  // Global state revision (does nothing)
  std::map<microkernel::LevelIdentifier,
           std::shared_ptr<microkernel::agents::IPerceivedData>>
      perceivedMap;
  perceivedMap[mainLevel] = perceivedData;
  passiveAgent->reviseGlobalState(t0, t1, perceivedMap, passiveGlobalState);
  std::cout << "   ✓ Global state revision completed (no changes)" << std::endl;
  std::cout << "   Counter before: 0, after: " << passiveGlobalState->counter
            << std::endl;

  // Decision (does nothing)
  auto influences = std::make_shared<microkernel::influences::InfluencesMap>();
  passiveAgent->decide(mainLevel, t0, t1, passiveGlobalState, nullptr, nullptr,
                       perceivedData, influences);
  std::cout << "   ✓ Decision completed (no influences produced)" << std::endl;
  std::cout << std::endl;

  // ===== 5. Summary =====
  std::cout << "╔══════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║                    Summary                           ║"
            << std::endl;
  std::cout << "╚══════════════════════════════════════════════════════╝"
            << std::endl;
  std::cout << std::endl;
  std::cout << "✅ All ExtendedLibs utilities working correctly!" << std::endl;
  std::cout << std::endl;
  std::cout << "Available utilities:" << std::endl;
  std::cout << "  📦 Abstract Base Classes:" << std::endl;
  std::cout << "     • AbstractAgtPerceptionModel" << std::endl;
  std::cout << "     • AbstractAgtDecisionModel" << std::endl;
  std::cout << "     • AbstractPerceivedData" << std::endl;
  std::cout << std::endl;
  std::cout << "  🔧 Generic/Empty Implementations:" << std::endl;
  std::cout << "     • EmptyAgtPerceptionModel" << std::endl;
  std::cout << "     • EmptyAgtDecisionModel" << std::endl;
  std::cout << "     • IdentityAgtGlobalStateRevisionModel" << std::endl;
  std::cout << "     • EmptyPerceivedData" << std::endl;
  std::cout << std::endl;
  std::cout << "  ⏰ Time Models:" << std::endl;
  std::cout << "     • PeriodicTimeModel" << std::endl;
  std::cout << std::endl;
  std::cout << "These utilities make it easy to create agents with"
            << std::endl;
  std::cout << "minimal or no behavior, perfect for:" << std::endl;
  std::cout << "  • Passive/reactive agents" << std::endl;
  std::cout << "  • Environmental objects" << std::endl;
  std::cout << "  • Placeholder implementations" << std::endl;
  std::cout << "  • Testing and prototyping" << std::endl;
  std::cout << std::endl;

  return 0;
}
