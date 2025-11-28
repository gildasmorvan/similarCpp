#include "../../include/agents/VehiclePerceivedDataMicro.h"
#include <limits>

namespace jamfree {
namespace microscopic {
namespace agents {

VehiclePerceivedDataMicro::VehiclePerceivedDataMicro()
    : m_transitory_min(0.0), m_transitory_max(0.0), m_leader(nullptr),
      m_gap_to_leader(std::numeric_limits<double>::infinity()),
      m_leader_speed(0.0), m_follower(nullptr),
      m_gap_to_follower(std::numeric_limits<double>::infinity()),
      m_follower_speed(0.0), m_left_lane(nullptr), m_left_leader(nullptr),
      m_left_follower(nullptr),
      m_gap_to_left_leader(std::numeric_limits<double>::infinity()),
      m_gap_to_left_follower(std::numeric_limits<double>::infinity()),
      m_right_lane(nullptr), m_right_leader(nullptr), m_right_follower(nullptr),
      m_gap_to_right_leader(std::numeric_limits<double>::infinity()),
      m_gap_to_right_follower(std::numeric_limits<double>::infinity()),
      m_approaching_lane_end(false),
      m_distance_to_lane_end(std::numeric_limits<double>::infinity()),
      m_next_road_id(""), m_target_lane_index(-1),
      m_current_speed_limit(33.3) { // Default 120 km/h
}

std::shared_ptr<
    fr::univ_artois::lgi2a::similar::microkernel::agents::IPerceivedData>
VehiclePerceivedDataMicro::clone() const {
  auto cloned = std::make_shared<VehiclePerceivedDataMicro>();

  cloned->m_transitory_min = m_transitory_min;
  cloned->m_transitory_max = m_transitory_max;

  // Current lane
  cloned->m_leader = m_leader;
  cloned->m_gap_to_leader = m_gap_to_leader;
  cloned->m_leader_speed = m_leader_speed;
  cloned->m_follower = m_follower;
  cloned->m_gap_to_follower = m_gap_to_follower;
  cloned->m_follower_speed = m_follower_speed;

  // Left lane
  cloned->m_left_lane = m_left_lane;
  cloned->m_left_leader = m_left_leader;
  cloned->m_left_follower = m_left_follower;
  cloned->m_gap_to_left_leader = m_gap_to_left_leader;
  cloned->m_gap_to_left_follower = m_gap_to_left_follower;

  // Right lane
  cloned->m_right_lane = m_right_lane;
  cloned->m_right_leader = m_right_leader;
  cloned->m_right_follower = m_right_follower;
  cloned->m_gap_to_right_leader = m_gap_to_right_leader;
  cloned->m_gap_to_right_follower = m_gap_to_right_follower;

  // Lane end
  cloned->m_approaching_lane_end = m_approaching_lane_end;
  cloned->m_distance_to_lane_end = m_distance_to_lane_end;

  // Routing
  cloned->m_next_road_id = m_next_road_id;
  cloned->m_target_lane_index = m_target_lane_index;

  // Speed limit
  cloned->m_current_speed_limit = m_current_speed_limit;

  return cloned;
}

kernel::agents::LevelIdentifier VehiclePerceivedDataMicro::getLevel() const {
  return kernel::agents::LevelIdentifier("Microscopic");
}

kernel::agents::SimulationTimeStamp
VehiclePerceivedDataMicro::getTransitoryPeriodMin() const {
  return m_transitory_min;
}

kernel::agents::SimulationTimeStamp
VehiclePerceivedDataMicro::getTransitoryPeriodMax() const {
  return m_transitory_max;
}

void VehiclePerceivedDataMicro::clear() {
  m_leader = nullptr;
  m_gap_to_leader = std::numeric_limits<double>::infinity();
  m_leader_speed = 0.0;

  m_follower = nullptr;
  m_gap_to_follower = std::numeric_limits<double>::infinity();
  m_follower_speed = 0.0;

  m_left_lane = nullptr;
  m_left_leader = nullptr;
  m_left_follower = nullptr;
  m_gap_to_left_leader = std::numeric_limits<double>::infinity();
  m_gap_to_left_follower = std::numeric_limits<double>::infinity();

  m_right_lane = nullptr;
  m_right_leader = nullptr;
  m_right_follower = nullptr;
  m_gap_to_right_leader = std::numeric_limits<double>::infinity();
  m_gap_to_right_follower = std::numeric_limits<double>::infinity();

  m_approaching_lane_end = false;
  m_distance_to_lane_end = std::numeric_limits<double>::infinity();

  m_next_road_id = "";
  m_target_lane_index = -1;
}

} // namespace agents
} // namespace microscopic
} // namespace jamfree
