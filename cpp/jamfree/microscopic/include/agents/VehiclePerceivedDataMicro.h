#ifndef JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PERCEIVED_DATA_MICRO_H
#define JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PERCEIVED_DATA_MICRO_H

#include "../../../kernel/include/agents/Interfaces.h"
#include "../../../kernel/include/model/Lane.h"
#include "../../../kernel/include/model/Vehicle.h"
#include <memory>
#include <vector>

namespace jamfree {
namespace microscopic {
namespace agents {

/**
 * @brief Perceived data for a vehicle in the microscopic level.
 *
 * Contains all information that the vehicle has perceived from
 * its environment (leaders, followers, adjacent lanes, etc.).
 */
class VehiclePerceivedDataMicro : public kernel::agents::IPerceivedData {
public:
  /**
   * @brief Constructor.
   */
  VehiclePerceivedDataMicro();

  /**
   * @brief Clone this perceived data.
   * @return Cloned data
   */
  std::shared_ptr<kernel::agents::IPerceivedData> clone() const;

  // Implement IPerceivedData
  kernel::agents::LevelIdentifier getLevel() const override;
  kernel::agents::SimulationTimeStamp getTransitoryPeriodMin() const override;
  kernel::agents::SimulationTimeStamp getTransitoryPeriodMax() const override;

  void setTransitoryPeriodMin(kernel::agents::SimulationTimeStamp t) {
    m_transitory_min = t;
  }
  void setTransitoryPeriodMax(kernel::agents::SimulationTimeStamp t) {
    m_transitory_max = t;
  }

  // Leader information (vehicle ahead in same lane)
  kernel::model::Vehicle *getLeader() const { return m_leader; }
  void setLeader(kernel::model::Vehicle *leader) { m_leader = leader; }

  double getGapToLeader() const { return m_gap_to_leader; }
  void setGapToLeader(double gap) { m_gap_to_leader = gap; }

  double getLeaderSpeed() const { return m_leader_speed; }
  void setLeaderSpeed(double speed) { m_leader_speed = speed; }

  // Follower information (vehicle behind in same lane)
  kernel::model::Vehicle *getFollower() const { return m_follower; }
  void setFollower(kernel::model::Vehicle *follower) { m_follower = follower; }

  double getGapToFollower() const { return m_gap_to_follower; }
  void setGapToFollower(double gap) { m_gap_to_follower = gap; }

  double getFollowerSpeed() const { return m_follower_speed; }
  void setFollowerSpeed(double speed) { m_follower_speed = speed; }

  // Left lane information
  kernel::model::Lane *getLeftLane() const { return m_left_lane; }
  void setLeftLane(kernel::model::Lane *lane) { m_left_lane = lane; }

  kernel::model::Vehicle *getLeftLeader() const { return m_left_leader; }
  void setLeftLeader(kernel::model::Vehicle *leader) { m_left_leader = leader; }

  kernel::model::Vehicle *getLeftFollower() const { return m_left_follower; }
  void setLeftFollower(kernel::model::Vehicle *follower) {
    m_left_follower = follower;
  }

  double getGapToLeftLeader() const { return m_gap_to_left_leader; }
  void setGapToLeftLeader(double gap) { m_gap_to_left_leader = gap; }

  double getGapToLeftFollower() const { return m_gap_to_left_follower; }
  void setGapToLeftFollower(double gap) { m_gap_to_left_follower = gap; }

  // Right lane information
  kernel::model::Lane *getRightLane() const { return m_right_lane; }
  void setRightLane(kernel::model::Lane *lane) { m_right_lane = lane; }

  kernel::model::Vehicle *getRightLeader() const { return m_right_leader; }
  void setRightLeader(kernel::model::Vehicle *leader) {
    m_right_leader = leader;
  }

  kernel::model::Vehicle *getRightFollower() const { return m_right_follower; }
  void setRightFollower(kernel::model::Vehicle *follower) {
    m_right_follower = follower;
  }

  double getGapToRightLeader() const { return m_gap_to_right_leader; }
  void setGapToRightLeader(double gap) { m_gap_to_right_leader = gap; }

  double getGapToRightFollower() const { return m_gap_to_right_follower; }
  void setGapToRightFollower(double gap) { m_gap_to_right_follower = gap; }

  // Lane end information
  bool isApproachingLaneEnd() const { return m_approaching_lane_end; }
  void setApproachingLaneEnd(bool approaching) {
    m_approaching_lane_end = approaching;
  }

  double getDistanceToLaneEnd() const { return m_distance_to_lane_end; }
  void setDistanceToLaneEnd(double distance) {
    m_distance_to_lane_end = distance;
  }

  // Next road information (for routing)
  const std::string &getNextRoadId() const { return m_next_road_id; }
  void setNextRoadId(const std::string &roadId) { m_next_road_id = roadId; }

  int getTargetLaneIndex() const { return m_target_lane_index; }
  void setTargetLaneIndex(int index) { m_target_lane_index = index; }

  // Speed limit
  double getCurrentSpeedLimit() const { return m_current_speed_limit; }
  void setCurrentSpeedLimit(double limit) { m_current_speed_limit = limit; }

  // Clear all perceived data
  void clear();

private:
  kernel::agents::SimulationTimeStamp m_transitory_min;
  kernel::agents::SimulationTimeStamp m_transitory_max;

  // Leader/follower in current lane
  kernel::model::Vehicle *m_leader;
  double m_gap_to_leader;
  double m_leader_speed;

  kernel::model::Vehicle *m_follower;
  double m_gap_to_follower;
  double m_follower_speed;

  // Left lane
  kernel::model::Lane *m_left_lane;
  kernel::model::Vehicle *m_left_leader;
  kernel::model::Vehicle *m_left_follower;
  double m_gap_to_left_leader;
  double m_gap_to_left_follower;

  // Right lane
  kernel::model::Lane *m_right_lane;
  kernel::model::Vehicle *m_right_leader;
  kernel::model::Vehicle *m_right_follower;
  double m_gap_to_right_leader;
  double m_gap_to_right_follower;

  // Lane end
  bool m_approaching_lane_end;
  double m_distance_to_lane_end;

  // Routing
  std::string m_next_road_id;
  int m_target_lane_index;

  // Speed limit
  double m_current_speed_limit;
};

} // namespace agents
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PERCEIVED_DATA_MICRO_H
