#ifndef JAMFREE_KERNEL_TOOLS_MATH_TOOLS_H
#define JAMFREE_KERNEL_TOOLS_MATH_TOOLS_H

#include <algorithm>
#include <cmath>

namespace jamfree {
namespace kernel {
namespace tools {

/**
 * @brief Mathematical utility functions for traffic simulations.
 *
 * Provides common mathematical operations and constraints
 * used in traffic modeling.
 */
class MathTools {
public:
  /**
   * @brief Clamp value between minimum and maximum.
   *
   * @param value Value to clamp
   * @param min_val Minimum allowed value
   * @param max_val Maximum allowed value
   * @return Clamped value
   */
  static double clamp(double value, double min_val, double max_val) {
    return std::max(min_val, std::min(max_val, value));
  }

  /**
   * @brief Linear interpolation between two values.
   *
   * @param a Start value
   * @param b End value
   * @param t Interpolation parameter [0, 1]
   * @return Interpolated value
   */
  static double lerp(double a, double b, double t) { return a + t * (b - a); }

  /**
   * @brief Check if two floating-point values are approximately equal.
   *
   * @param a First value
   * @param b Second value
   * @param epsilon Tolerance (default: 1e-9)
   * @return True if values are within epsilon
   */
  static bool approxEqual(double a, double b, double epsilon = 1e-9) {
    return std::abs(a - b) < epsilon;
  }

  /**
   * @brief Safe division avoiding division by zero.
   *
   * @param numerator Numerator
   * @param denominator Denominator
   * @param default_value Value to return if denominator is zero
   * @return Result of division or default value
   */
  static double safeDivide(double numerator, double denominator,
                           double default_value = 0.0) {
    if (std::abs(denominator) < 1e-10) {
      return default_value;
    }
    return numerator / denominator;
  }

  /**
   * @brief Convert km/h to m/s.
   *
   * @param kmh Speed in km/h
   * @return Speed in m/s
   */
  static double kmhToMs(double kmh) { return kmh / 3.6; }

  /**
   * @brief Convert m/s to km/h.
   *
   * @param ms Speed in m/s
   * @return Speed in km/h
   */
  static double msToKmh(double ms) { return ms * 3.6; }

  /**
   * @brief Calculate sign of a number.
   *
   * @param value Input value
   * @return -1, 0, or 1
   */
  static int sign(double value) {
    if (value > 0.0)
      return 1;
    if (value < 0.0)
      return -1;
    return 0;
  }

  /**
   * @brief Square a number.
   *
   * @param value Input value
   * @return value²
   */
  static double square(double value) { return value * value; }
};

} // namespace tools
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_TOOLS_MATH_TOOLS_H
