#include "../../include/reaction/MicroscopicReactionModel.h"
#include "../../../kernel/include/simulation/SimulationEngine.h"
#include "../../../kernel/include/agents/VehicleAgent.h"
#include "../../../kernel/include/model/Lane.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>

namespace jamfree {
namespace microscopic {
namespace reaction {

using jamfree::kernel::agents::LevelIdentifier;
using jamfree::kernel::agents::VehicleAgent;
using jamfree::microscopic::agents::VehiclePublicLocalStateMicro;
using jamfree::kernel::model::Lane;

MicroscopicReactionModel::MicroscopicReactionModel(double dt) : m_dt(dt) {}

void MicroscopicReactionModel::setSimulationEngine(
    kernel::simulation::SimulationEngine *engine) {
  m_engine = engine;
}

void MicroscopicReactionModel::react(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound,
    const kernel::agents::InfluencesMap &influences) {
  if (!m_engine) {
    std::cerr << "[MicroscopicReactionModel] No SimulationEngine set; "
                 "skipping reaction."
              << std::endl;
    return;
  }

  // Apply influences in order:
  // 1. Lane changes (must happen before acceleration)
  // 2. Acceleration changes
  // 3. Physics updates
  // 4. State validation

  applyLaneChanges(influences);
  applyAccelerationChanges(influences);
  updatePhysics();
  validateStates();
}

void MicroscopicReactionModel::applyAccelerationChanges(
    const kernel::agents::InfluencesMap &influences) {
  auto accelInfluences = extractInfluencesByCategory(
      influences, influences::ChangeAcceleration::CATEGORY);

  LevelIdentifier microLevel("Microscopic");

  for (const auto &influence : accelInfluences) {
    auto changeAccel =
        std::dynamic_pointer_cast<influences::ChangeAcceleration>(influence);
    if (!changeAccel) {
      continue;
    }

    const std::string &ownerId = changeAccel->getOwnerId();
    auto agent = m_engine->getAgent(ownerId);
    if (!agent) {
      std::cerr << "[MicroscopicReactionModel] No agent found for ownerId="
                << ownerId << std::endl;
      continue;
    }

    auto publicStateBase = agent->getPublicLocalState(microLevel);
    auto publicState =
        std::dynamic_pointer_cast<VehiclePublicLocalStateMicro>(
            publicStateBase);
    if (!publicState) {
      std::cerr
          << "[MicroscopicReactionModel] Invalid public state type for agent "
          << ownerId << " in microscopic level." << std::endl;
      continue;
    }

    double acceleration = changeAccel->getAcceleration();
    publicState->setAcceleration(acceleration);
  }
}

void MicroscopicReactionModel::applyLaneChanges(
    const kernel::agents::InfluencesMap &influences) {
  auto laneChangeInfluences =
      extractInfluencesByCategory(influences, influences::ChangeLane::CATEGORY);

  LevelIdentifier microLevel("Microscopic");

  for (const auto &influence : laneChangeInfluences) {
    auto changeLane =
        std::dynamic_pointer_cast<influences::ChangeLane>(influence);
    if (!changeLane) {
      continue;
    }

    const std::string &ownerId = changeLane->getOwnerId();
    auto agent = m_engine->getAgent(ownerId);
    if (!agent) {
      std::cerr << "[MicroscopicReactionModel] No agent found for ownerId="
                << ownerId << " (lane change ignored)." << std::endl;
      continue;
    }

    auto publicStateBase = agent->getPublicLocalState(microLevel);
    auto publicState =
        std::dynamic_pointer_cast<VehiclePublicLocalStateMicro>(
            publicStateBase);
    if (!publicState) {
      std::cerr
          << "[MicroscopicReactionModel] Invalid public state type for agent "
          << ownerId << " in microscopic level (lane change ignored)."
          << std::endl;
      continue;
    }

    auto direction = changeLane->getDirection();

    Lane *currentLane = publicState->getCurrentLane();
    if (!currentLane) {
      continue;
    }

    auto parentRoad = currentLane->getParentRoad();
    if (!parentRoad) {
      continue;
    }

    int currentIndex = currentLane->getIndex();
    int targetIndex = (direction == influences::ChangeLane::Direction::LEFT)
                          ? currentIndex - 1
                          : currentIndex + 1;

    if (targetIndex < 0 || targetIndex >= parentRoad->getNumLanes()) {
      continue;
    }

    auto targetLane = parentRoad->getLane(targetIndex);
    if (!targetLane) {
      continue;
    }

    publicState->setCurrentLane(targetLane.get());
    publicState->setLaneIndex(targetIndex);
  }
}

void MicroscopicReactionModel::updatePhysics() {
  if (!m_engine) {
    return;
  }

  LevelIdentifier microLevel("Microscopic");

  for (const auto &agentPtr : m_engine->getAgents()) {
    if (!agentPtr) {
      continue;
    }

    auto publicStateBase = agentPtr->getPublicLocalState(microLevel);
    auto publicState =
        std::dynamic_pointer_cast<VehiclePublicLocalStateMicro>(
            publicStateBase);
    if (!publicState || !publicState->isActive()) {
      continue;
    }

    double speed = publicState->getSpeed();
    double accel = publicState->getAcceleration();

    double newSpeed = speed + accel * m_dt;
    if (newSpeed < 0.0) {
      newSpeed = 0.0;
    }

    double lanePos = publicState->getLanePosition();
    double newLanePos = lanePos + newSpeed * m_dt;

    publicState->setSpeed(newSpeed);
    publicState->setLanePosition(newLanePos);

    Lane *lane = publicState->getCurrentLane();
    if (lane) {
      auto globalPos = lane->getPositionAt(newLanePos);
      publicState->setPosition(globalPos);

      double heading = lane->getHeadingAt(newLanePos);
      publicState->setHeading(heading);
    }
  }
}

void MicroscopicReactionModel::validateStates() {
  if (!m_engine) {
    return;
  }

  LevelIdentifier microLevel("Microscopic");

  std::unordered_map<Lane *,
                     std::vector<std::shared_ptr<VehiclePublicLocalStateMicro>>>
      laneToVehicles;

  for (const auto &agentPtr : m_engine->getAgents()) {
    if (!agentPtr) {
      continue;
    }

    auto publicStateBase = agentPtr->getPublicLocalState(microLevel);
    auto publicState =
        std::dynamic_pointer_cast<VehiclePublicLocalStateMicro>(
            publicStateBase);
    if (!publicState || !publicState->isActive()) {
      continue;
    }

    Lane *lane = publicState->getCurrentLane();
    if (!lane) {
      continue;
    }

    double lanePos = publicState->getLanePosition();
    double laneLength = lane->getLength();
    if (lanePos < 0.0) {
      publicState->setLanePosition(0.0);
    } else if (lanePos > laneLength) {
      publicState->setLanePosition(laneLength);
    }

    laneToVehicles[lane].push_back(publicState);
  }

  for (auto &entry : laneToVehicles) {
    auto &vehicles = entry.second;

    std::sort(vehicles.begin(), vehicles.end(),
              [](const std::shared_ptr<VehiclePublicLocalStateMicro> &a,
                 const std::shared_ptr<VehiclePublicLocalStateMicro> &b) {
                return a->getLanePosition() < b->getLanePosition();
              });

    for (std::size_t i = 1; i < vehicles.size(); ++i) {
      auto &rear = vehicles[i - 1];
      auto &front = vehicles[i];

      double rearFrontPos = rear->getLanePosition() + rear->getLength();
      double gap = front->getLanePosition() - rearFrontPos;

      if (gap < 0.0) {
        front->setLanePosition(rearFrontPos);
        double minSpeed = std::min(front->getSpeed(), rear->getSpeed());
        front->setSpeed(minSpeed);
      }
    }
  }
}

std::vector<std::shared_ptr<kernel::agents::IInfluence>>
MicroscopicReactionModel::extractInfluencesByCategory(
    const kernel::agents::InfluencesMap &influences,
    const std::string &category) {
  std::vector<std::shared_ptr<kernel::agents::IInfluence>> result;

  // In SIMILAR, InfluencesMap organizes influences by level.
  // We need to iterate over all levels or a specific level.
  // Since this is the Microscopic reaction model, we should look for influences
  // targeting the "Microscopic" level.

  // Note: InfluencesMap is not const-correct in getInfluencesForLevel in the
  // provided header view but usually it should be. Let's assume we need to cast
  // away const or the header view was incomplete/I should check if there is a
  // const version. Looking at InfluencesMap.h:
  // std::list<std::shared_ptr<IInfluence>> getInfluencesForLevel(const
  // LevelIdentifier &targetLevel) It is NOT const. This is a potential issue if
  // we are passed a const reference. However, we can iterate over the map
  // directly if we had access, but it's private. We can use getDefinedKeys()
  // which is const, then getInfluencesForLevel. But getInfluencesForLevel is
  // not const. We might need to const_cast, or better, fix InfluencesMap if we
  // could (we can't easily). Let's try to use const_cast for now as a
  // workaround, or check if there's a const iterator.

  // Actually, let's look at the header again.
  // std::map<LevelIdentifier, std::list<std::shared_ptr<IInfluence>>>
  // influences; is private. getInfluencesForLevel is not const.

  auto &nonConstInfluences =
      const_cast<kernel::agents::InfluencesMap &>(influences);
  kernel::agents::LevelIdentifier microLevel("Microscopic");

  if (!nonConstInfluences.isEmpty(microLevel)) {
    auto localInfluences = nonConstInfluences.getInfluencesForLevel(microLevel);
    for (const auto &influence : localInfluences) {
      if (influence->getCategory() == category) {
        result.push_back(influence);
      }
    }
  }

  return result;
}

} // namespace reaction
} // namespace microscopic
} // namespace jamfree
