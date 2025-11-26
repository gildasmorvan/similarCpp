#include "../../../include/decision/dms/LaneChangeDMS.h"
#include "../../../include/influences/ChangeLane.h"
#include <limits>

namespace jamfree {
namespace microscopic {
namespace decision {
namespace dms {

LaneChangeDMS::LaneChangeDMS(std::shared_ptr<models::MOBIL> mobil,
                             std::shared_ptr<models::IDM> idm)
    : m_mobil(mobil), m_idm(idm) {
  if (!m_mobil) {
    throw std::invalid_argument("LaneChangeDMS: MOBIL model cannot be null");
  }
  if (!m_idm) {
    throw std::invalid_argument("LaneChangeDMS: IDM model cannot be null");
  }
}

bool LaneChangeDMS::manageDecision(
    kernel::agents::SimulationTimeStamp timeLowerBound,
    kernel::agents::SimulationTimeStamp timeUpperBound,
    const agents::VehiclePublicLocalStateMicro &publicState,
    const agents::VehiclePrivateLocalStateMicro &privateState,
    const agents::VehiclePerceivedDataMicro &perceivedData,
    const kernel::agents::GlobalState &globalState,
    kernel::agents::InfluencesMap &producedInfluences) {
  // Check if there are adjacent lanes
  bool hasLeftLane = perceivedData.getLeftLane() != nullptr;
  bool hasRightLane = perceivedData.getRightLane() != nullptr;

  if (!hasLeftLane && !hasRightLane) {
    return false; // Cannot handle - no lanes to change to
  }

  // Update MOBIL parameters from private state
  m_mobil->setPoliteness(privateState.getPoliteness());
  m_mobil->setThreshold(privateState.getLaneChangeThreshold());
  m_mobil->setMaxSafeDeceleration(privateState.getMaxSafeDeceleration());
  m_mobil->setBiasRight(privateState.getRightLaneBias());

  // Update IDM parameters
  m_idm->setDesiredSpeed(privateState.getDesiredSpeed());
  m_idm->setTimeHeadway(privateState.getTimeHeadway());
  m_idm->setMinGap(privateState.getMinGap());
  m_idm->setMaxAcceleration(privateState.getMaxAcceleration());
  m_idm->setComfortableDeceleration(privateState.getComfortableDeceleration());
  m_idm->setAccelerationExponent(privateState.getAccelerationExponent());

  // Evaluate lane changes
  double leftIncentive = -std::numeric_limits<double>::infinity();
  double rightIncentive = -std::numeric_limits<double>::infinity();

  if (hasLeftLane) {
    leftIncentive =
        evaluateLaneChange(models::MOBIL::Direction::LEFT, publicState,
                           privateState, perceivedData);
  }

  if (hasRightLane) {
    rightIncentive =
        evaluateLaneChange(models::MOBIL::Direction::RIGHT, publicState,
                           privateState, perceivedData);
  }

  // Decide which lane change to make (if any)
  models::MOBIL::Direction chosenDirection = models::MOBIL::Direction::NONE;

  if (rightIncentive > leftIncentive &&
      rightIncentive > privateState.getLaneChangeThreshold()) {
    chosenDirection = models::MOBIL::Direction::RIGHT;
  } else if (leftIncentive > privateState.getLaneChangeThreshold()) {
    chosenDirection = models::MOBIL::Direction::LEFT;
  }

  // Emit influence if lane change is chosen
  if (chosenDirection != models::MOBIL::Direction::NONE) {
    influences::ChangeLane::Direction direction;
    if (chosenDirection == models::MOBIL::Direction::LEFT) {
      direction = influences::ChangeLane::Direction::LEFT;
    } else {
      direction = influences::ChangeLane::Direction::RIGHT;
    }

    auto influence = std::make_shared<influences::ChangeLane>(
        timeLowerBound, timeUpperBound, direction);
    producedInfluences.add(influence);
  }

  return true; // Handled the situation
}

double LaneChangeDMS::evaluateLaneChange(
    models::MOBIL::Direction direction,
    const agents::VehiclePublicLocalStateMicro &publicState,
    const agents::VehiclePrivateLocalStateMicro &privateState,
    const agents::VehiclePerceivedDataMicro &perceivedData) {
  // Get current lane information
  double currentSpeed = publicState.getSpeed();
  double currentGap = perceivedData.getGapToLeader();
  double currentLeaderSpeed = perceivedData.getLeaderSpeed();

  // Get target lane information
  double targetGapToLeader, targetGapToFollower;
  double targetLeaderSpeed, targetFollowerSpeed;

  if (direction == models::MOBIL::Direction::LEFT) {
    targetGapToLeader = perceivedData.getGapToLeftLeader();
    targetGapToFollower = perceivedData.getGapToLeftFollower();
    targetLeaderSpeed = perceivedData.getLeftLeader()
                            ? perceivedData.getLeftLeader()->getSpeed()
                            : currentSpeed;
    targetFollowerSpeed = perceivedData.getLeftFollower()
                              ? perceivedData.getLeftFollower()->getSpeed()
                              : currentSpeed;
  } else {
    targetGapToLeader = perceivedData.getGapToRightLeader();
    targetGapToFollower = perceivedData.getGapToRightFollower();
    targetLeaderSpeed = perceivedData.getRightLeader()
                            ? perceivedData.getRightLeader()->getSpeed()
                            : currentSpeed;
    targetFollowerSpeed = perceivedData.getRightFollower()
                              ? perceivedData.getRightFollower()->getSpeed()
                              : currentSpeed;
  }

  // Safety check: would the follower in target lane have to brake too hard?
  if (targetGapToFollower < std::numeric_limits<double>::infinity()) {
    double followerDecel =
        m_idm->calculateAcceleration(targetFollowerSpeed, targetGapToFollower,
                                     targetFollowerSpeed - currentSpeed);

    if (followerDecel < -privateState.getMaxSafeDeceleration()) {
      return -std::numeric_limits<double>::infinity(); // Unsafe
    }
  }

  // Calculate acceleration advantage
  double currentAccel = m_idm->calculateAcceleration(
      currentSpeed, currentGap, currentSpeed - currentLeaderSpeed);

  double targetAccel = m_idm->calculateAcceleration(
      currentSpeed, targetGapToLeader, currentSpeed - targetLeaderSpeed);

  double incentive = targetAccel - currentAccel;

  // Apply politeness factor (consider impact on others)
  double politeness = privateState.getPoliteness();
  if (politeness > 0 &&
      targetGapToFollower < std::numeric_limits<double>::infinity()) {
    // Calculate how much the follower would be affected
    double followerOldAccel = m_idm->calculateAcceleration(
        targetFollowerSpeed, targetGapToFollower + publicState.getLength(),
        targetFollowerSpeed - targetLeaderSpeed);

    double followerNewAccel =
        m_idm->calculateAcceleration(targetFollowerSpeed, targetGapToFollower,
                                     targetFollowerSpeed - currentSpeed);

    incentive -= politeness * (followerOldAccel - followerNewAccel);
  }

  // Apply right-lane bias
  if (direction == models::MOBIL::Direction::RIGHT) {
    incentive += privateState.getRightLaneBias();
  }

  return incentive;
}

} // namespace dms
} // namespace decision
} // namespace microscopic
} // namespace jamfree
