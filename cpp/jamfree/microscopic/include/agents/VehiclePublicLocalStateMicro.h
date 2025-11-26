#ifndef JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PUBLIC_LOCAL_STATE_MICRO_H
#define JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PUBLIC_LOCAL_STATE_MICRO_H

#include "../../../kernel/include/agents/Interfaces.h"
#include "../../../kernel/include/model/Lane.h"
#include "../../../kernel/include/model/Point2D.h"
#include <memory>

namespace jamfree {
namespace microscopic {
namespace agents {

/**
 * @brief Public local state of a vehicle in the microscopic level.
 *
 * This represents the observable state of the vehicle that other
 * agents can perceive.
 */
class VehiclePublicLocalStateMicro : public kernel::agents::ILocalState {
public:
  /**
   * @brief Constructor.
   * @param ownerId The identifier of the agent owning this state.
   */
  explicit VehiclePublicLocalStateMicro(const std::string &ownerId);

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

  // Position and orientation
  const kernel::model::Point2D &getPosition() const { return m_position; }
  void setPosition(const kernel::model::Point2D &position) {
    m_position = position;
  }

  double getHeading() const { return m_heading; }
  void setHeading(double heading) { m_heading = heading; }

  // Kinematics
  double getSpeed() const { return m_speed; }
  void setSpeed(double speed) { m_speed = speed; }

  double getAcceleration() const { return m_acceleration; }
  void setAcceleration(double acceleration) { m_acceleration = acceleration; }

  // Lane information
  kernel::model::Lane *getCurrentLane() const { return m_current_lane; }
  void setCurrentLane(kernel::model::Lane *lane) { m_current_lane = lane; }

  double getLanePosition() const { return m_lane_position; }
  void setLanePosition(double position) { m_lane_position = position; }

  int getLaneIndex() const { return m_lane_index; }
  void setLaneIndex(int index) { m_lane_index = index; }

  // Vehicle properties
  double getLength() const { return m_length; }
  void setLength(double length) { m_length = length; }

  double getWidth() const { return m_width; }
  void setWidth(double width) { m_width = width; }

  double getHeight() const { return m_height; }
  void setHeight(double height) { m_height = height; }

  // Status
  bool isActive() const { return m_active; }
  void setActive(bool active) { m_active = active; }

private:
  std::string m_ownerId;

  // Position and orientation
  kernel::model::Point2D m_position;
  double m_heading; // radians

  // Kinematics
  double m_speed;        // m/s
  double m_acceleration; // m/s²

  // Lane information
  kernel::model::Lane *m_current_lane;
  double m_lane_position; // position along lane (m)
  int m_lane_index;       // index within road

  // Vehicle dimensions
  double m_length; // m
  double m_width;  // m
  double m_height; // m

  // Status
  bool m_active; // Is vehicle active in simulation
};

} // namespace agents
} // namespace microscopic
} // namespace jamfree

#endif // JAMFREE_MICROSCOPIC_AGENTS_VEHICLE_PUBLIC_LOCAL_STATE_MICRO_H
