#ifndef JAMFREE_KERNEL_MODEL_POINT2D_H
#define JAMFREE_KERNEL_MODEL_POINT2D_H

#include <cmath>

namespace jamfree {
namespace kernel {
namespace model {

/**
 * @brief Represents a 2D point in space.
 *
 * Used for positions, coordinates, and vectors in traffic simulations.
 * Coordinates are in meters.
 */
class Point2D {
public:
  double x; ///< X coordinate (meters)
  double y; ///< Y coordinate (meters)

  /**
   * @brief Default constructor - creates point at origin.
   */
  Point2D() : x(0.0), y(0.0) {}

  /**
   * @brief Constructor with coordinates.
   *
   * @param x_coord X coordinate
   * @param y_coord Y coordinate
   */
  Point2D(double x_coord, double y_coord) : x(x_coord), y(y_coord) {}

  /**
   * @brief Calculate distance to another point.
   *
   * @param other Other point
   * @return Distance in meters
   */
  double distanceTo(const Point2D &other) const {
    double dx = other.x - x;
    double dy = other.y - y;
    return std::sqrt(dx * dx + dy * dy);
  }

  /**
   * @brief Calculate angle to another point.
   *
   * @param other Other point
   * @return Angle in radians [-π, π]
   */
  double angleTo(const Point2D &other) const {
    return std::atan2(other.y - y, other.x - x);
  }

  /**
   * @brief Vector addition.
   */
  Point2D operator+(const Point2D &other) const {
    return Point2D(x + other.x, y + other.y);
  }

  /**
   * @brief Vector subtraction.
   */
  Point2D operator-(const Point2D &other) const {
    return Point2D(x - other.x, y - other.y);
  }

  /**
   * @brief Scalar multiplication.
   */
  Point2D operator*(double scalar) const {
    return Point2D(x * scalar, y * scalar);
  }

  /**
   * @brief Scalar division.
   */
  Point2D operator/(double scalar) const {
    return Point2D(x / scalar, y / scalar);
  }

  /**
   * @brief In-place addition.
   */
  Point2D &operator+=(const Point2D &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  /**
   * @brief In-place subtraction.
   */
  Point2D &operator-=(const Point2D &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  /**
   * @brief Calculate magnitude (length) of vector.
   */
  double magnitude() const { return std::sqrt(x * x + y * y); }

  /**
   * @brief Normalize vector to unit length.
   */
  Point2D normalized() const {
    double mag = magnitude();
    if (mag < 1e-10) {
      return Point2D(0, 0);
    }
    return Point2D(x / mag, y / mag);
  }

  /**
   * @brief Dot product with another vector.
   */
  double dot(const Point2D &other) const { return x * other.x + y * other.y; }
};

} // namespace model
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_MODEL_POINT2D_H
