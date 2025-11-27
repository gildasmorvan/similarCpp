#include "../../include/perception/VehiclePerceptionModelMicro.h"
#include "../../../kernel/include/model/Road.h"
#include <algorithm>
#include <limits>

namespace jamfree {
namespace microscopic {
namespace perception {

VehiclePerceptionModelMicro::VehiclePerceptionModelMicro(double perceptionRange)
    : m_perception_range(perceptionRange) {}

std::shared_ptr<kernel::agents::IPerceivedData>
VehiclePerceptionModelMicro::perceive(
    const kernel::agents::SimulationTimeStamp &timeLowerBound,
    const kernel::agents::SimulationTimeStamp &timeUpperBound,
    const std::map<kernel::agents::LevelIdentifier,
                   std::shared_ptr<kernel::agents::ILocalState>>
        &publicLocalStates,
    std::shared_ptr<kernel::agents::ILocalState> privateLocalState,
    std::shared_ptr<kernel::agents::IPublicDynamicStateMap> dynamicStates) {

  // Create perceived data
  auto perceivedData = std::make_shared<agents::VehiclePerceivedDataMicro>();
  perceivedData->setTransitoryPeriodMin(timeLowerBound);
  perceivedData->setTransitoryPeriodMax(timeUpperBound);

  // Get public state for the microscopic level
  kernel::agents::LevelIdentifier microLevel("Microscopic");
  auto it = publicLocalStates.find(microLevel);
  if (it == publicLocalStates.end()) {
    throw std::runtime_error("No public state for level " +
                             microLevel.toString());
  }
  auto publicState =
      std::dynamic_pointer_cast<agents::VehiclePublicLocalStateMicro>(
          it->second);
  auto privateState =
      std::dynamic_pointer_cast<agents::VehiclePrivateLocalStateMicro>(
          privateLocalState);

  if (!publicState || !privateState) {
    throw std::runtime_error(
        "Invalid state types in VehiclePerceptionModelMicro");
  }

  // Call internal perception methods
  perceiveCurrentLane(*publicState, *perceivedData);
  perceiveAdjacentLanes(*publicState, *perceivedData);
  perceiveLaneEnd(*publicState, *privateState, *perceivedData);
  perceiveSpeedLimit(*publicState, *perceivedData);

  return perceivedData;
}

void VehiclePerceptionModelMicro::perceiveCurrentLane(
    const agents::VehiclePublicLocalStateMicro &publicState,
    agents::VehiclePerceivedDataMicro &perceivedData) {
  auto currentLane = publicState.getCurrentLane();
  if (!currentLane) {
    return;
  }

  double position = publicState.getLanePosition();

  // Find leader
  auto [leader, gapToLeader] =
      findLeader(currentLane, position, m_perception_range);
  if (leader) {
    perceivedData.setLeader(leader);
    perceivedData.setGapToLeader(gapToLeader);
    perceivedData.setLeaderSpeed(leader->getSpeed());
  }

  // Find follower
  auto [follower, gapToFollower] =
      findFollower(currentLane, position, m_perception_range);
  if (follower) {
    perceivedData.setFollower(follower);
    perceivedData.setGapToFollower(gapToFollower);
    perceivedData.setFollowerSpeed(follower->getSpeed());
  }
}

void VehiclePerceptionModelMicro::perceiveAdjacentLanes(
    const agents::VehiclePublicLocalStateMicro &publicState,
    agents::VehiclePerceivedDataMicro &perceivedData) {
  auto currentLane = publicState.getCurrentLane();
  if (!currentLane) {
    return;
  }

  auto parentRoad = currentLane->getParentRoad();
  if (!parentRoad) {
    return;
  }

  int currentLaneIndex = publicState.getLaneIndex();
  double position = publicState.getLanePosition();

  // Left lane (index - 1)
  if (currentLaneIndex > 0) {
    auto leftLane = parentRoad->getLane(currentLaneIndex - 1);
    perceivedData.setLeftLane(leftLane.get());

    // Find leader and follower in left lane
    auto [leftLeader, gapToLeftLeader] =
        findLeader(leftLane.get(), position, m_perception_range);
    if (leftLeader) {
      perceivedData.setLeftLeader(leftLeader);
      perceivedData.setGapToLeftLeader(gapToLeftLeader);
    }

    auto [leftFollower, gapToLeftFollower] =
        findFollower(leftLane.get(), position, m_perception_range);
    if (leftFollower) {
      perceivedData.setLeftFollower(leftFollower);
      perceivedData.setGapToLeftFollower(gapToLeftFollower);
    }
  }

  // Right lane (index + 1)
  if (currentLaneIndex < parentRoad->getNumLanes() - 1) {
    auto rightLane = parentRoad->getLane(currentLaneIndex + 1);
    perceivedData.setRightLane(rightLane.get());

    // Find leader and follower in right lane
    auto [rightLeader, gapToRightLeader] =
        findLeader(rightLane.get(), position, m_perception_range);
    if (rightLeader) {
      perceivedData.setRightLeader(rightLeader);
      perceivedData.setGapToRightLeader(gapToRightLeader);
    }

    auto [rightFollower, gapToRightFollower] =
        findFollower(rightLane.get(), position, m_perception_range);
    if (rightFollower) {
      perceivedData.setRightFollower(rightFollower);
      perceivedData.setGapToRightFollower(gapToRightFollower);
    }
  }
}

void VehiclePerceptionModelMicro::perceiveLaneEnd(
    const agents::VehiclePublicLocalStateMicro &publicState,
    const agents::VehiclePrivateLocalStateMicro &privateState,
    agents::VehiclePerceivedDataMicro &perceivedData) {
  auto currentLane = publicState.getCurrentLane();
  if (!currentLane) {
    return;
  }

  double position = publicState.getLanePosition();
  double laneLength = currentLane->getLength();
  double distanceToEnd = laneLength - position;

  perceivedData.setDistanceToLaneEnd(distanceToEnd);

  // Consider approaching if within perception range
  if (distanceToEnd < m_perception_range) {
    perceivedData.setApproachingLaneEnd(true);

    // Get next road from route
    const auto &route = privateState.getRoute();
    int routeIndex = privateState.getCurrentRouteIndex();

    if (routeIndex >= 0 && routeIndex < static_cast<int>(route.size())) {
      perceivedData.setNextRoadId(route[routeIndex]);
      // Target lane index would be determined by routing logic
      // For now, default to same lane index
      perceivedData.setTargetLaneIndex(publicState.getLaneIndex());
    }
  }
}

void VehiclePerceptionModelMicro::perceiveSpeedLimit(
    const agents::VehiclePublicLocalStateMicro &publicState,
    agents::VehiclePerceivedDataMicro &perceivedData) {
  auto currentLane = publicState.getCurrentLane();
  if (currentLane) {
    perceivedData.setCurrentSpeedLimit(currentLane->getSpeedLimit());
  }
}

std::pair<kernel::model::Vehicle *, double>
VehiclePerceptionModelMicro::findLeader(kernel::model::Lane *lane,
                                        double position, double maxRange) {
  if (!lane) {
    return {nullptr, std::numeric_limits<double>::infinity()};
  }

  const auto &vehicles = lane->getVehicles();

  kernel::model::Vehicle *closestLeader = nullptr;
  double minGap = std::numeric_limits<double>::infinity();

  for (const auto &vehicle : vehicles) {
    double vehiclePos = vehicle->getLanePosition();

    // Vehicle is ahead
    if (vehiclePos > position) {
      double gap = vehiclePos - position - vehicle->getLength();

      if (gap < minGap && gap < maxRange) {
        minGap = gap;
        closestLeader = vehicle.get();
      }
    }
  }

  return {closestLeader, minGap};
}

std::pair<kernel::model::Vehicle *, double>
VehiclePerceptionModelMicro::findFollower(kernel::model::Lane *lane,
                                          double position, double maxRange) {
  if (!lane) {
    return {nullptr, std::numeric_limits<double>::infinity()};
  }

  const auto &vehicles = lane->getVehicles();

  kernel::model::Vehicle *closestFollower = nullptr;
  double minGap = std::numeric_limits<double>::infinity();

  for (const auto &vehicle : vehicles) {
    double vehiclePos = vehicle->getLanePosition();

    // Vehicle is behind
    if (vehiclePos < position) {
      double gap = position - vehiclePos - vehicle->getLength();

      if (gap < minGap && gap < maxRange) {
        minGap = gap;
        closestFollower = vehicle.get();
      }
    }
  }

  return {closestFollower, minGap};
}

// ... (existing code)

kernel::agents::LevelIdentifier VehiclePerceptionModelMicro::getLevel() const {
  return kernel::agents::LevelIdentifier("Microscopic");
}

} // namespace perception
} // namespace microscopic
} // namespace jamfree
