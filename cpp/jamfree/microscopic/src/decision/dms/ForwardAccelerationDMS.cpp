#include "../include/decision/dms/ForwardAccelerationDMS.h"
#include "../include/influences/ChangeAcceleration.h"
#include <algorithm>
#include <limits>

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

ForwardAccelerationDMS::ForwardAccelerationDMS(std::shared_ptr<models::IDM> idm)
    : m_idm(idm) {
  if (!m_idm) {
    throw std::invalid_argument(
        "ForwardAccelerationDMS: IDM model cannot be null");
  }
}

bool ForwardAccelerationDMS::manageDecision(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound,
    const agents::VehiclePublicLocalStateMicro &publicState,
    const agents::VehiclePrivateLocalStateMicro &privateState,
    const agents::VehiclePerceivedDataMicro &perceivedData,
    const kernel::agents::GlobalState &globalState,
    kernel::agents::InfluencesMap &producedInfluences) {
  // Get current state
  double currentSpeed = publicState.getSpeed();
  double desiredSpeed = privateState.getDesiredSpeed();

  // Get leader information
  double gapToLeader = perceivedData.getGapToLeader();
  double leaderSpeed = perceivedData.getLeaderSpeed();

  // If no leader, use infinite gap (free flow)
  if (!perceivedData.getLeader()) {
    gapToLeader = std::numeric_limits<double>::infinity();
    leaderSpeed = desiredSpeed;
  }

  // Calculate speed difference
  double deltaV = currentSpeed - leaderSpeed;

  // Update IDM parameters from private state
  m_idm->setDesiredSpeed(desiredSpeed);
  m_idm->setTimeHeadway(privateState.getTimeHeadway());
  m_idm->setMinGap(privateState.getMinGap());
  m_idm->setMaxAcceleration(privateState.getMaxAcceleration());
  m_idm->setComfortableDeceleration(privateState.getComfortableDeceleration());
  m_idm->setAccelerationExponent(privateState.getAccelerationExponent());

  // Calculate IDM acceleration
  double acceleration =
      m_idm->calculateAcceleration(currentSpeed, gapToLeader, deltaV);

  // Respect speed limit
  double speedLimit = perceivedData.getCurrentSpeedLimit();
  if (currentSpeed >= speedLimit && acceleration > 0) {
    // Don't accelerate if already at or above speed limit
    acceleration = std::min(0.0, acceleration);
  }

  // Create and emit ChangeAcceleration influence
  auto influence = std::make_shared<influences::ChangeAcceleration>(
      timeLowerBound, timeUpperBound, acceleration);
  producedInfluences.add(influence);

  return true; // Always handles the situation
}

} // namespace dms
} // namespace decision
} // namespace microscopic
} // namespace jamfree
