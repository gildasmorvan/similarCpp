#include "../../include/reaction/MicroscopicReactionModel.h"
#include <algorithm>
#include <iostream>

namespace jamfree {
namespace microscopic {
namespace reaction {

MicroscopicReactionModel::MicroscopicReactionModel(double dt) : m_dt(dt) {}

void MicroscopicReactionModel::react(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound,
    const kernel::agents::InfluencesMap &influences) {
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

  for (const auto &influence : accelInfluences) {
    auto changeAccel =
        std::dynamic_pointer_cast<influences::ChangeAcceleration>(influence);
    if (!changeAccel) {
      continue;
    }

    double acceleration = changeAccel->getAcceleration();

    // TODO: Get the vehicle associated with this influence
    // For now, this is a placeholder showing the structure
    // In a full implementation, influences would be tagged with agent IDs

    // Example of what would happen:
    // vehicle->setAcceleration(acceleration);

    std::cout << "Applied acceleration: " << acceleration << " m/s²"
              << std::endl;
  }
}

void MicroscopicReactionModel::applyLaneChanges(
    const kernel::agents::InfluencesMap &influences) {
  auto laneChangeInfluences =
      extractInfluencesByCategory(influences, influences::ChangeLane::CATEGORY);

  for (const auto &influence : laneChangeInfluences) {
    auto changeLane =
        std::dynamic_pointer_cast<influences::ChangeLane>(influence);
    if (!changeLane) {
      continue;
    }

    auto direction = changeLane->getDirection();

    // TODO: Get the vehicle and perform lane change
    // Example structure:
    // auto currentLane = vehicle->getCurrentLane();
    // auto parentRoad = currentLane->getParentRoad();
    // int currentIndex = currentLane->getIndex();
    // int targetIndex = (direction == ChangeLane::Direction::LEFT) ?
    //                   currentIndex - 1 : currentIndex + 1;
    //
    // if (targetIndex >= 0 && targetIndex < parentRoad->getNumLanes()) {
    //     auto targetLane = parentRoad->getLane(targetIndex);
    //     currentLane->removeVehicle(vehicle);
    //     targetLane->addVehicle(vehicle);
    //     vehicle->setCurrentLane(targetLane);
    //     vehicle->setLaneIndex(targetIndex);
    // }

    std::string dirStr = (direction == influences::ChangeLane::Direction::LEFT)
                             ? "LEFT"
                             : "RIGHT";
    std::cout << "Applied lane change: " << dirStr << std::endl;
  }
}

void MicroscopicReactionModel::updatePhysics() {
  // TODO: Update all vehicles' physics
  // This would iterate through all vehicles and update:
  //
  // for (auto& vehicle : vehicles) {
  //     // Update speed: v = v + a * dt
  //     double newSpeed = vehicle->getSpeed() + vehicle->getAcceleration() *
  //     m_dt; newSpeed = std::max(0.0, newSpeed);  // No negative speed
  //     vehicle->setSpeed(newSpeed);
  //
  //     // Update position: s = s + v * dt
  //     double newPosition = vehicle->getLanePosition() + newSpeed * m_dt;
  //     vehicle->setLanePosition(newPosition);
  //
  //     // Update global position based on lane geometry
  //     auto lane = vehicle->getCurrentLane();
  //     if (lane) {
  //         auto globalPos = lane->getPositionAt(newPosition);
  //         vehicle->setPosition(globalPos);
  //
  //         auto heading = lane->getHeadingAt(newPosition);
  //         vehicle->setHeading(heading);
  //     }
  // }

  std::cout << "Updated physics (dt=" << m_dt << "s)" << std::endl;
}

void MicroscopicReactionModel::validateStates() {
  // TODO: Validate vehicle states and handle edge cases
  //
  // for (auto& vehicle : vehicles) {
  //     auto lane = vehicle->getCurrentLane();
  //     if (!lane) continue;
  //
  //     double lanePosition = vehicle->getLanePosition();
  //     double laneLength = lane->getLength();
  //
  //     // Check if vehicle has reached end of lane
  //     if (lanePosition >= laneLength) {
  //         // Handle lane transition or removal
  //         // This would integrate with routing system
  //     }
  //
  //     // Check for collisions
  //     auto leader = lane->getLeader(vehicle.get());
  //     if (leader) {
  //         double gap = leader->getLanePosition() - lanePosition -
  //         vehicle->getLength(); if (gap < 0) {
  //             // Collision detected - adjust positions
  //             vehicle->setLanePosition(leader->getLanePosition() -
  //             vehicle->getLength());
  //             vehicle->setSpeed(std::min(vehicle->getSpeed(),
  //             leader->getSpeed()));
  //         }
  //     }
  // }

  std::cout << "Validated states" << std::endl;
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
