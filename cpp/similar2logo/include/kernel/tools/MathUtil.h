#ifndef SIMILAR2LOGO_MATHUTIL_H
#define SIMILAR2LOGO_MATHUTIL_H

#include "Point2D.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace tools {

/**
 * Mathematical utilities for Logo simulations.
 * Provides angle normalization, distance calculations, and other geometric
 * functions.
 */
class MathUtil {
public:
  // Constants
  static constexpr double PI = M_PI;
  static constexpr double TWO_PI = 2.0 * M_PI;
  static constexpr double HALF_PI = M_PI / 2.0;
  static constexpr double DEG_TO_RAD = M_PI / 180.0;
  static constexpr double RAD_TO_DEG = 180.0 / M_PI;

  /**
   * Normalizes an angle to the range [-π, π].
   */
  static double normalizeAngle(double angle) {
    while (angle > PI)
      angle -= TWO_PI;
    while (angle < -PI)
      angle += TWO_PI;
    return angle;
  }

  /**
   * Normalizes an angle to the range [0, 2π].
   */
  static double normalizeAnglePositive(double angle) {
    while (angle < 0)
      angle += TWO_PI;
    while (angle >= TWO_PI)
      angle -= TWO_PI;
    return angle;
  }

  /**
   * Computes the shortest angular difference between two angles.
   * Result is in range [-π, π].
   */
  static double angleDifference(double angle1, double angle2) {
    double diff = angle2 - angle1;
    return normalizeAngle(diff);
  }

  /**
   * Converts degrees to radians.
   */
  static double degreesToRadians(double degrees) {
    return degrees * DEG_TO_RAD;
  }

  /**
   * Converts radians to degrees.
   */
  static double radiansToDegrees(double radians) {
    return radians * RAD_TO_DEG;
  }

  /**
   * Computes the Euclidean distance between two points.
   */
  /**
   * Computes the Euclidean distance between two points.
   */
  static double distance(const ::fr::univ_artois::lgi2a::similar::similar2logo::
                             kernel::tools::Point2D &p1,
                         const ::fr::univ_artois::lgi2a::similar::similar2logo::
                             kernel::tools::Point2D &p2) {
    return p1.distanceTo(p2);
  }

  /**
   * Computes the squared distance between two points.
   */
  static double distanceSquared(const ::fr::univ_artois::lgi2a::similar::
                                    similar2logo::kernel::tools::Point2D &p1,
                                const ::fr::univ_artois::lgi2a::similar::
                                    similar2logo::kernel::tools::Point2D &p2) {
    return p1.distanceSquaredTo(p2);
  }

  /**
   * Computes the Manhattan distance between two points.
   */
  static double
  manhattanDistance(const ::fr::univ_artois::lgi2a::similar::similar2logo::
                        kernel::tools::Point2D &p1,
                    const ::fr::univ_artois::lgi2a::similar::similar2logo::
                        kernel::tools::Point2D &p2) {
    return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
  }

  /**
   * Clamps a value between min and max.
   */
  static double clamp(double value, double min, double max) {
    if (value < min)
      return min;
    if (value > max)
      return max;
    return value;
  }

  /**
   * Linear interpolation between two values.
   */
  static double lerp(double a, double b, double t) { return a + (b - a) * t; }

  /**
   * Checks if a value is approximately equal to another within epsilon.
   */
  static bool approximately(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
  }

  /**
   * Computes the sign of a value (-1, 0, or 1).
   */
  static int sign(double value) {
    if (value > 0)
      return 1;
    if (value < 0)
      return -1;
    return 0;
  }

  /**
   * Wraps a value to stay within [min, max) range (toroidal wrapping).
   */
  static double wrap(double value, double min, double max) {
    double range = max - min;
    while (value < min)
      value += range;
    while (value >= max)
      value -= range;
    return value;
  }

  /**
   * Computes the toroidal distance between two points on a grid.
   * @param p1 First point
   * @param p2 Second point
   * @param width Grid width
   * @param height Grid height
   * @param xTorus Whether x-axis wraps
   * @param yTorus Whether y-axis wraps
   */
  static double toroidalDistance(const ::fr::univ_artois::lgi2a::similar::
                                     similar2logo::kernel::tools::Point2D &p1,
                                 const ::fr::univ_artois::lgi2a::similar::
                                     similar2logo::kernel::tools::Point2D &p2,
                                 double width, double height, bool xTorus,
                                 bool yTorus) {
    double dx = std::abs(p1.x - p2.x);
    double dy = std::abs(p1.y - p2.y);

    if (xTorus) {
      dx = std::min(dx, width - dx);
    }
    if (yTorus) {
      dy = std::min(dy, height - dy);
    }

    return std::sqrt(dx * dx + dy * dy);
  }

  /**
   * Computes the toroidal displacement vector from p1 to p2.
   */
  static ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D
  toroidalDisplacement(const ::fr::univ_artois::lgi2a::similar::similar2logo::
                           kernel::tools::Point2D &p1,
                       const ::fr::univ_artois::lgi2a::similar::similar2logo::
                           kernel::tools::Point2D &p2,
                       double width, double height, bool xTorus, bool yTorus) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    if (xTorus) {
      if (dx > width / 2)
        dx -= width;
      else if (dx < -width / 2)
        dx += width;
    }
    if (yTorus) {
      if (dy > height / 2)
        dy -= height;
      else if (dy < -height / 2)
        dy += height;
    }

    return ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::
        Point2D(dx, dy);
  }
};

} // namespace tools
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_MATHUTIL_H
