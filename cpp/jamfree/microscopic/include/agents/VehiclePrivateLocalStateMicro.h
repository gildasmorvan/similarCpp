#ifndef JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PRIVATE_LOCAL_STATE_MICRO_H
#define JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PRIVATE_LOCAL_STATE_MICRO_H

#include "../../../kernel/include/agents/Interfaces.h"
#include "../../include/IDM.h"
#include "../../include/MOBIL.h"
#include <memory>
#include <string>
#include <vector>

namespace jamfree {
namespace microscopic {
namespace agents {

/**
 * @brief Private local state of a vehicle in the microscopic level.
 *
 * This represents the hidden state of the vehicle that only the
 * vehicle itself can access (decision-making parameters, route, etc.).
 */
class VehiclePrivateLocalStateMicro : public kernel::agents::ILocalState {
public:
  /**
   * @brief Constructor with default parameters.
   * @param ownerId The identifier of the agent owning this state.
   */
  explicit VehiclePrivateLocalStateMicro(const std::string &ownerId);

  /**
   * @brief Clone this state.
   * @return Cloned state
   */
  std::shared_ptr<kernel::agents::ILocalState> clone() const override;

  /**
   * @brief Gets the category of the agent owning this local state.
   */
  kernel::AgentCategory getCategoryOfAgent() const override;

  /**
   * @brief Checks if an agent is the owner of a local state.
   */
  bool isOwnedBy(const kernel::agents::IAgent &agent) const override;

  // IDM parameters
  double getDesiredSpeed() const { return m_desired_speed; }
  void setDesiredSpeed(double speed) { m_desired_speed = speed; }

  double getTimeHeadway() const { return m_time_headway; }
  void setTimeHeadway(double headway) { m_time_headway = headway; }

  double getMinGap() const { return m_min_gap; }
  void setMinGap(double gap) { m_min_gap = gap; }

  double getMaxAcceleration() const { return m_max_acceleration; }
  void setMaxAcceleration(double accel) { m_max_acceleration = accel; }

  double getComfortableDeceleration() const {
    return m_comfortable_deceleration;
  }
  void setComfortableDeceleration(double decel) {
    m_comfortable_deceleration = decel;
  }

  double getAccelerationExponent() const { return m_acceleration_exponent; }
  void setAccelerationExponent(double exp) { m_acceleration_exponent = exp; }

  // MOBIL parameters
  double getPoliteness() const { return m_politeness; }
  void setPoliteness(double politeness) { m_politeness = politeness; }

  double getLaneChangeThreshold() const { return m_lane_change_threshold; }
  void setLaneChangeThreshold(double threshold) {
    m_lane_change_threshold = threshold;
  }

  double getMaxSafeDeceleration() const { return m_max_safe_deceleration; }
  void setMaxSafeDeceleration(double decel) { m_max_safe_deceleration = decel; }

  double getRightLaneBias() const { return m_right_lane_bias; }
  void setRightLaneBias(double bias) { m_right_lane_bias = bias; }

  // Route information
  const std::vector<std::string> &getRoute() const { return m_route; }
  void setRoute(const std::vector<std::string> &route) { m_route = route; }

  int getCurrentRouteIndex() const { return m_current_route_index; }
  void setCurrentRouteIndex(int index) { m_current_route_index = index; }

  const std::string &getDestination() const { return m_destination; }
  void setDestination(const std::string &dest) { m_destination = dest; }

  // Driver characteristics
  double getReactionTime() const { return m_reaction_time; }
  void setReactionTime(double time) { m_reaction_time = time; }

  double getAggressiveness() const { return m_aggressiveness; }
  void setAggressiveness(double aggressiveness) {
    m_aggressiveness = aggressiveness;
  }

private:
  std::string m_ownerId;

  // IDM parameters
  double m_desired_speed;            // m/s
  double m_time_headway;             // s
  double m_min_gap;                  // m
  double m_max_acceleration;         // m/s²
  double m_comfortable_deceleration; // m/s²
  double m_acceleration_exponent;    // dimensionless (usually 4)

  // MOBIL parameters
  double m_politeness;            // 0 = selfish, 1 = altruistic
  double m_lane_change_threshold; // m/s²
  double m_max_safe_deceleration; // m/s²
  double m_right_lane_bias;       // m/s² (keep-right rule)

  // Route information
  std::vector<std::string> m_route; // List of road IDs
  int m_current_route_index;        // Current position in route
  std::string m_destination;        // Final destination road ID

  // Driver characteristics
  double m_reaction_time;  // s
  double m_aggressiveness; // 0-1 (affects IDM/MOBIL params)
};

} // namespace agents
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PRIVATE_LOCAL_STATE_MICRO_H
