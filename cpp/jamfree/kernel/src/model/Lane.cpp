#include "kernel/include/model/Lane.h"
#include "kernel/include/model/Road.h"
#include "kernel/include/model/Vehicle.h"
#include "kernel/include/tools/GeometryTools.h"
#include <algorithm>
#include <limits>

namespace jamfree {
namespace kernel {
namespace model {

Point2D Lane::getPositionAt(double distance) const {
  if (m_parent_road) {
    // Get position from parent road and offset by lane index
    Point2D center_pos = m_parent_road->getPositionAt(distance);
    double heading = m_parent_road->getHeadingAt(distance);

    // Offset perpendicular to road direction
    // Lane 0 is rightmost, so offset to the right
    double offset = (m_index + 0.5) * m_width;
    double perp_heading = heading - M_PI / 2.0; // 90 degrees right

    return Point2D(center_pos.x + offset * std::cos(perp_heading),
                   center_pos.y + offset * std::sin(perp_heading));
  }
  return Point2D(distance, 0);
}

double Lane::getHeadingAt(double distance) const {
  if (m_parent_road) {
    return m_parent_road->getHeadingAt(distance);
  }
  return 0.0;
}

bool Lane::containsPosition(const Point2D &position, double tolerance) const {
  // Simple implementation - check distance to lane centerline
  double dist = getDistanceAlong(position);
  Point2D lane_pos = getPositionAt(dist);
  return position.distanceTo(lane_pos) < tolerance;
}

double Lane::getDistanceAlong(const Point2D &position) const {
  // Project position onto lane
  // For now, simple implementation assuming straight lane
  if (m_parent_road) {
    Point2D start = m_parent_road->getStart();
    Point2D end = m_parent_road->getEnd();

    // Project onto road direction
    Point2D dir = (end - start).normalized();
    Point2D to_pos = position - start;
    double dist = to_pos.dot(dir);

    return std::max(0.0, std::min(m_length, dist));
  }
  return 0.0;
}

void Lane::addVehicle(std::shared_ptr<Vehicle> vehicle) {
  m_vehicles.push_back(vehicle);

  // Sort by position (for efficient leader/follower queries)
  std::sort(
      m_vehicles.begin(), m_vehicles.end(),
      [](const std::shared_ptr<Vehicle> &a, const std::shared_ptr<Vehicle> &b) {
        return a->getLanePosition() < b->getLanePosition();
      });
}

void Lane::removeVehicle(std::shared_ptr<Vehicle> vehicle) {
  auto it = std::find(m_vehicles.begin(), m_vehicles.end(), vehicle);
  if (it != m_vehicles.end()) {
    m_vehicles.erase(it);
  }
}

std::shared_ptr<Vehicle> Lane::getVehicleAhead(double position) const {
  // Vehicles are sorted by position
  for (const auto &vehicle : m_vehicles) {
    if (vehicle->getLanePosition() > position) {
      return vehicle;
    }
  }
  return nullptr;
}

std::shared_ptr<Vehicle> Lane::getVehicleBehind(double position) const {
  // Vehicles are sorted by position
  std::shared_ptr<Vehicle> behind = nullptr;
  for (const auto &vehicle : m_vehicles) {
    if (vehicle->getLanePosition() < position) {
      behind = vehicle;
    } else {
      break;
    }
  }
  return behind;
}

double Lane::getGapAhead(double position) const {
  auto ahead = getVehicleAhead(position);
  if (ahead) {
    return ahead->getLanePosition() - position;
  }
  return std::numeric_limits<double>::infinity();
}

Vehicle *Lane::getLeader(const Vehicle &vehicle) const {
  double pos = vehicle.getLanePosition();
  auto ahead = getVehicleAhead(pos);
  return ahead ? ahead.get() : nullptr;
}

Vehicle *Lane::getFollower(const Vehicle &vehicle) const {
  double pos = vehicle.getLanePosition();
  auto behind = getVehicleBehind(pos);
  return behind ? behind.get() : nullptr;
}

} // namespace model
} // namespace kernel
} // namespace jamfree
