#ifndef SIMILAR2LOGO_POINT2D_H
#define SIMILAR2LOGO_POINT2D_H

#include "FastMath.h"
#include <cmath>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace tools {

/**
 * A 2D point/vector class for Logo simulations.
 * Provides basic vector operations and geometric utilities.
 */
class Point2D {
public:
  double x;
  double y;

  // Constructors
  Point2D() : x(0.0), y(0.0) {}
  Point2D(double x, double y) : x(x), y(y) {}

  // Vector operations
  Point2D operator+(const Point2D &other) const {
    return Point2D(x + other.x, y + other.y);
  }

  Point2D operator-(const Point2D &other) const {
    return Point2D(x - other.x, y - other.y);
  }

  Point2D operator*(double scalar) const {
    return Point2D(x * scalar, y * scalar);
  }

  Point2D operator/(double scalar) const {
    return Point2D(x / scalar, y / scalar);
  }

  Point2D &operator+=(const Point2D &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Point2D &operator-=(const Point2D &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Point2D &operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  Point2D &operator/=(double scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
  }

  // Comparison
  bool operator==(const Point2D &other) const {
    return std::abs(x - other.x) < 1e-9 && std::abs(y - other.y) < 1e-9;
  }

  bool operator!=(const Point2D &other) const { return !(*this == other); }

  // Geometric operations
  /**
   * Computes the Euclidean distance to another point.
   */
  double distanceTo(const Point2D &other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
  }

  /**
   * Computes the squared distance to another point (faster than distanceTo).
   */
  double distanceSquaredTo(const Point2D &other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return dx * dx + dy * dy;
  }

  /**
   * Computes the magnitude (length) of this vector.
   */
  double magnitude() const { return std::sqrt(x * x + y * y); }

  /**
   * Computes the squared magnitude of this vector.
   */
  double magnitudeSquared() const { return x * x + y * y; }

  /**
   * Returns a normalized version of this vector (unit vector).
   */
  Point2D normalized() const {
    double mag = magnitude();
    if (mag < 1e-9)
      return Point2D(0, 0);
    return Point2D(x / mag, y / mag);
  }

  /**
   * Computes the dot product with another vector.
   */
  double dot(const Point2D &other) const { return x * other.x + y * other.y; }

  /**
   * Computes the cross product (z-component) with another vector.
   */
  double cross(const Point2D &other) const { return x * other.y - y * other.x; }

  /**
   * Computes the angle of this vector in radians.
   * Returns value in range [-π, π].
   */
  double angle() const { return std::atan2(y, x); }

  /**
   * Computes the angle to another point in radians.
   */
  double angleTo(const Point2D &other) const {
    return std::atan2(other.y - y, other.x - x);
  }

  /**
   * Rotates this vector by the given angle (in radians).
   */
  Point2D rotated(double angleRad) const {
    double cosA = microkernel::tools::FastMath::cos(angleRad);
    double sinA = microkernel::tools::FastMath::sin(angleRad);
    return Point2D(x * cosA - y * sinA, x * sinA + y * cosA);
  }

  /**
   * Creates a point from polar coordinates.
   * @param magnitude The distance from origin
   * @param angleRad The angle in radians
   */
  static Point2D fromPolar(double magnitude, double angleRad) {
    return Point2D(magnitude * microkernel::tools::FastMath::cos(angleRad),
                   magnitude * microkernel::tools::FastMath::sin(angleRad));
  }

  /**
   * Linear interpolation between this point and another.
   * @param other The target point
   * @param t Interpolation factor (0 = this, 1 = other)
   */
  Point2D lerp(const Point2D &other, double t) const {
    return Point2D(x + (other.x - x) * t, y + (other.y - y) * t);
  }

  /**
   * Returns a string representation of this point.
   */
  std::string toString() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
  }
};

} // namespace tools
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_POINT2D_H
