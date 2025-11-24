#ifndef JAMFREE_KERNEL_MODEL_ROAD_H
#define JAMFREE_KERNEL_MODEL_ROAD_H

#include "Lane.h"
#include "Point2D.h"
#include <memory>
#include <string>
#include <vector>

namespace jamfree {
namespace kernel {
namespace model {

/**
 * @brief Represents a road segment in the network.
 *
 * A road is a directed path between two points, containing one or more lanes.
 * It has geometry (start/end points, possibly intermediate waypoints),
 * and manages the lanes within it.
 */
class Road {
public:
  /**
   * @brief Constructor for straight road.
   *
   * @param id Unique identifier
   * @param start Start point
   * @param end End point
   * @param num_lanes Number of lanes
   * @param lane_width Width of each lane (meters)
   */
  Road(const std::string &id, const Point2D &start, const Point2D &end,
       int num_lanes = 1, double lane_width = 3.5)
      : m_id(id), m_start(start), m_end(end), m_lane_width(lane_width) {
    // Create lanes
    double length = start.distanceTo(end);
    for (int i = 0; i < num_lanes; ++i) {
      std::string lane_id = id + "_lane_" + std::to_string(i);
      auto lane = std::make_shared<Lane>(lane_id, i, lane_width, length);
      lane->setParentRoad(this);
      m_lanes.push_back(lane);
    }
  }

  /**
   * @brief Constructor with waypoints for curved roads.
   *
   * @param id Unique identifier
   * @param waypoints List of points defining road geometry
   * @param num_lanes Number of lanes
   * @param lane_width Width of each lane (meters)
   */
  Road(const std::string &id, const std::vector<Point2D> &waypoints,
       int num_lanes = 1, double lane_width = 3.5)
      : m_id(id), m_waypoints(waypoints), m_lane_width(lane_width) {
    if (waypoints.size() >= 2) {
      m_start = waypoints.front();
      m_end = waypoints.back();

      // Calculate total length
      double length = 0.0;
      for (size_t i = 1; i < waypoints.size(); ++i) {
        length += waypoints[i - 1].distanceTo(waypoints[i]);
      }

      // Create lanes
      for (int i = 0; i < num_lanes; ++i) {
        std::string lane_id = id + "_lane_" + std::to_string(i);
        auto lane = std::make_shared<Lane>(lane_id, i, lane_width, length);
        lane->setParentRoad(this);
        m_lanes.push_back(lane);
      }
    }
  }

  // Getters
  const std::string &getId() const { return m_id; }
  const Point2D &getStart() const { return m_start; }
  const Point2D &getEnd() const { return m_end; }
  double getLaneWidth() const { return m_lane_width; }
  int getNumLanes() const { return m_lanes.size(); }

  /**
   * @brief Get lane by index.
   *
   * @param index Lane index (0 = rightmost)
   * @return Lane pointer, or nullptr if invalid index
   */
  std::shared_ptr<Lane> getLane(int index) const {
    if (index >= 0 && index < static_cast<int>(m_lanes.size())) {
      return m_lanes[index];
    }
    return nullptr;
  }

  /**
   * @brief Get all lanes.
   *
   * @return Vector of lanes
   */
  const std::vector<std::shared_ptr<Lane>> &getLanes() const { return m_lanes; }

  /**
   * @brief Get total length of road.
   *
   * @return Length in meters
   */
  double getLength() const {
    if (!m_lanes.empty()) {
      return m_lanes[0]->getLength();
    }
    return 0.0;
  }

  /**
   * @brief Get position at distance along road centerline.
   *
   * @param distance Distance from start (meters)
   * @return 2D position
   */
  Point2D getPositionAt(double distance) const {
    if (m_waypoints.empty()) {
      // Straight road
      double t = distance / m_start.distanceTo(m_end);
      t = std::max(0.0, std::min(1.0, t));
      return m_start + (m_end - m_start) * t;
    } else {
      // Curved road - interpolate along waypoints
      return interpolateAlongWaypoints(distance);
    }
  }

  /**
   * @brief Get heading at distance along road.
   *
   * @param distance Distance from start (meters)
   * @return Heading in radians
   */
  double getHeadingAt(double distance) const {
    if (m_waypoints.empty()) {
      return m_start.angleTo(m_end);
    } else {
      return getHeadingAlongWaypoints(distance);
    }
  }

  /**
   * @brief Check if road has waypoints (is curved).
   *
   * @return True if road has intermediate waypoints
   */
  bool isCurved() const {
    return !m_waypoints.empty() && m_waypoints.size() > 2;
  }

private:
  std::string m_id;
  Point2D m_start;
  Point2D m_end;
  std::vector<Point2D> m_waypoints;
  double m_lane_width;
  std::vector<std::shared_ptr<Lane>> m_lanes;

  /**
   * @brief Interpolate position along waypoints.
   */
  Point2D interpolateAlongWaypoints(double distance) const {
    double accumulated = 0.0;
    for (size_t i = 1; i < m_waypoints.size(); ++i) {
      double segment_length = m_waypoints[i - 1].distanceTo(m_waypoints[i]);
      if (accumulated + segment_length >= distance) {
        // Position is in this segment
        double t = (distance - accumulated) / segment_length;
        return m_waypoints[i - 1] + (m_waypoints[i] - m_waypoints[i - 1]) * t;
      }
      accumulated += segment_length;
    }
    // Beyond end
    return m_waypoints.back();
  }

  /**
   * @brief Get heading along waypoints.
   */
  double getHeadingAlongWaypoints(double distance) const {
    double accumulated = 0.0;
    for (size_t i = 1; i < m_waypoints.size(); ++i) {
      double segment_length = m_waypoints[i - 1].distanceTo(m_waypoints[i]);
      if (accumulated + segment_length >= distance) {
        // Heading is direction of this segment
        return m_waypoints[i - 1].angleTo(m_waypoints[i]);
      }
      accumulated += segment_length;
    }
    // Use last segment
    if (m_waypoints.size() >= 2) {
      return m_waypoints[m_waypoints.size() - 2].angleTo(m_waypoints.back());
    }
    return 0.0;
  }
};

} // namespace model
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_MODEL_ROAD_H
