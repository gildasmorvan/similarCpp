#ifndef JAMFREE_MACROSCOPIC_AGENTS_VEHICLE_PUBLIC_LOCAL_STATE_MACRO_H
#define JAMFREE_MACROSCOPIC_AGENTS_VEHICLE_PUBLIC_LOCAL_STATE_MACRO_H

#include "../../../kernel/include/agents/Interfaces.h"
#include "../../../kernel/include/model/Lane.h"
#include "../../../kernel/include/model/Point2D.h"
#include <memory>

namespace jamfree {
namespace macroscopic {
namespace agents {

/**
 * @brief Public local state of a vehicle in the macroscopic level.
 *
 * At the macroscopic level, vehicles are represented as part of
 * traffic flow rather than individual entities. This state captures
 * aggregate properties.
 */
class VehiclePublicLocalStateMacro : public kernel::agents::ILocalState {
public:
  /**
   * @brief Constructor.
   * @param ownerId The identifier of the agent owning this state.
   */
  explicit VehiclePublicLocalStateMacro(const std::string &ownerId);

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

  // Flow properties
  double getDensity() const { return m_density; }
  void setDensity(double density) { m_density = density; }

  double getFlow() const { return m_flow; }
  void setFlow(double flow) { m_flow = flow; }

  double getAverageSpeed() const { return m_average_speed; }
  void setAverageSpeed(double speed) { m_average_speed = speed; }

  // Spatial properties
  kernel::model::Lane *getCurrentLane() const { return m_current_lane; }
  void setCurrentLane(kernel::model::Lane *lane) { m_current_lane = lane; }

  double getCellPosition() const { return m_cell_position; }
  void setCellPosition(double position) { m_cell_position = position; }

  int getCellIndex() const { return m_cell_index; }
  void setCellIndex(int index) { m_cell_index = index; }

  // Status
  bool isActive() const { return m_active; }
  void setActive(bool active) { m_active = active; }

private:
  std::string m_ownerId;

  // Flow properties
  double m_density;       // vehicles/m
  double m_flow;          // vehicles/s
  double m_average_speed; // m/s

  // Spatial properties
  kernel::model::Lane *m_current_lane;
  double m_cell_position; // position of cell center (m)
  int m_cell_index;       // cell index within lane

  // Status
  bool m_active;
};

} // namespace agents
} // namespace macroscopic
} // namespace jamfree

#endif // JAMFREE_MACROSCOPIC_AGENTS_VEHICLE_PUBLIC_LOCAL_STATE_MACRO_H
