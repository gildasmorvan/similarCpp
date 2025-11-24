#ifndef JAMFREE_KERNEL_TOOLS_GEOMETRY_TOOLS_H
#define JAMFREE_KERNEL_TOOLS_GEOMETRY_TOOLS_H

#include <cmath>
#include <utility>

namespace jamfree {
namespace kernel {
namespace tools {

/**
 * @brief Utility class for geometric calculations in traffic simulations.
 *
 * Provides methods for distance calculations, angle computations,
 * and other geometric operations needed for traffic modeling.
 */
class GeometryTools {
public:
  /**
   * @brief Calculate Euclidean distance between two 2D points.
   *
   * @param x1 X coordinate of first point
   * @param y1 Y coordinate of first point
   * @param x2 X coordinate of second point
   * @param y2 Y coordinate of second point
   * @return Distance in meters
   */
  static double distance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return std::sqrt(dx * dx + dy * dy);
  }

  /**
   * @brief Calculate angle from point 1 to point 2.
   *
   * @param x1 X coordinate of first point
   * @param y1 Y coordinate of first point
   * @param x2 X coordinate of second point
   * @param y2 Y coordinate of second point
   * @return Angle in radians [-π, π]
   */
  static double angle(double x1, double y1, double x2, double y2) {
    return std::atan2(y2 - y1, x2 - x1);
  }

  /**
   * @brief Normalize angle to range [-π, π].
   *
   * @param angle Angle in radians
   * @return Normalized angle
   */
  static double normalizeAngle(double angle) {
    while (angle > M_PI)
      angle -= 2.0 * M_PI;
    while (angle < -M_PI)
      angle += 2.0 * M_PI;
    return angle;
  }

  /**
   * @brief Calculate shortest angular difference between two angles.
   *
   * @param angle1 First angle in radians
   * @param angle2 Second angle in radians
   * @return Angular difference in range [-π, π]
   */
  static double angleDifference(double angle1, double angle2) {
    return normalizeAngle(angle2 - angle1);
  }

  /**
   * @brief Project point onto line segment.
   *
   * @param px Point X coordinate
   * @param py Point Y coordinate
   * @param x1 Line start X
   * @param y1 Line start Y
   * @param x2 Line end X
   * @param y2 Line end Y
   * @return Pair of (projected X, projected Y)
   */
  static std::pair<double, double> projectPointOnSegment(double px, double py,
                                                         double x1, double y1,
                                                         double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double length_sq = dx * dx + dy * dy;

    if (length_sq == 0.0) {
      return {x1, y1};
    }

    // Calculate projection parameter
    double t = ((px - x1) * dx + (py - y1) * dy) / length_sq;

    // Clamp to segment
    t = std::max(0.0, std::min(1.0, t));

    return {x1 + t * dx, y1 + t * dy};
  }

  /**
   * @brief Calculate perpendicular distance from point to line segment.
   *
   * @param px Point X coordinate
   * @param py Point Y coordinate
   * @param x1 Line start X
   * @param y1 Line start Y
   * @param x2 Line end X
   * @param y2 Line end Y
   * @return Perpendicular distance in meters
   */
  static double distanceToSegment(double px, double py, double x1, double y1,
                                  double x2, double y2) {
    auto [proj_x, proj_y] = projectPointOnSegment(px, py, x1, y1, x2, y2);
    return distance(px, py, proj_x, proj_y);
  }
};

} // namespace tools
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_TOOLS_GEOMETRY_TOOLS_H
