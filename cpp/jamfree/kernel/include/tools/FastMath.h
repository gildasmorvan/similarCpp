#ifndef JAMFREE_KERNEL_TOOLS_FAST_MATH_H
#define JAMFREE_KERNEL_TOOLS_FAST_MATH_H

#include <cmath>
#include <cstdint>

namespace jamfree {
namespace kernel {
namespace tools {

/**
 * @brief Fast mathematical approximations for performance-critical code.
 *
 * Provides optimized implementations of common math functions with
 * acceptable accuracy trade-offs for traffic simulation.
 */
class FastMath {
public:
  /**
   * @brief Fast square root using bit manipulation.
   *
   * Accuracy: ~1% error
   * Speedup: ~2-3x vs std::sqrt
   *
   * @param x Input value (must be positive)
   * @return Approximate square root
   */
  static inline float fastSqrt(float x) {
    // Quake III fast inverse square root algorithm (adapted)
    if (x <= 0.0f)
      return 0.0f;

    float xhalf = 0.5f * x;
    int32_t i = *(int32_t *)&x;
    i = 0x5f3759df - (i >> 1); // Magic constant
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x); // Newton iteration
    return 1.0f / x;                // Invert to get sqrt
  }

  /**
   * @brief Fast inverse square root.
   *
   * @param x Input value
   * @return Approximate 1/sqrt(x)
   */
  static inline float fastInvSqrt(float x) {
    if (x <= 0.0f)
      return 0.0f;

    float xhalf = 0.5f * x;
    int32_t i = *(int32_t *)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float *)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
  }

  /**
   * @brief Fast power function for small integer exponents.
   *
   * @param base Base value
   * @param exp Exponent (should be small integer)
   * @return base^exp
   */
  static inline double fastPow(double base, int exp) {
    if (exp == 0)
      return 1.0;
    if (exp == 1)
      return base;
    if (exp == 2)
      return base * base;
    if (exp == 3)
      return base * base * base;
    if (exp == 4) {
      double sq = base * base;
      return sq * sq;
    }

    // Fallback for larger exponents
    double result = 1.0;
    double current = base;
    int n = exp;

    if (n < 0) {
      current = 1.0 / current;
      n = -n;
    }

    while (n > 0) {
      if (n & 1)
        result *= current;
      current *= current;
      n >>= 1;
    }

    return result;
  }

  /**
   * @brief Fast absolute value.
   *
   * @param x Input value
   * @return |x|
   */
  static inline double fastAbs(double x) {
    // Bit manipulation for double
    uint64_t bits = *(uint64_t *)&x;
    bits &= 0x7FFFFFFFFFFFFFFF; // Clear sign bit
    return *(double *)&bits;
  }

  /**
   * @brief Fast minimum of two values.
   *
   * @param a First value
   * @param b Second value
   * @return min(a, b)
   */
  static inline double fastMin(double a, double b) { return (a < b) ? a : b; }

  /**
   * @brief Fast maximum of two values.
   *
   * @param a First value
   * @param b Second value
   * @return max(a, b)
   */
  static inline double fastMax(double a, double b) { return (a > b) ? a : b; }

  /**
   * @brief Fast clamp function.
   *
   * @param value Value to clamp
   * @param min_val Minimum
   * @param max_val Maximum
   * @return Clamped value
   */
  static inline double fastClamp(double value, double min_val, double max_val) {
    return fastMax(min_val, fastMin(max_val, value));
  }

  /**
   * @brief Fast exponential approximation.
   *
   * Uses Padé approximation for exp(x) near 0.
   * Accurate for |x| < 1.
   *
   * @param x Exponent
   * @return Approximate e^x
   */
  static inline double fastExp(double x) {
    // For traffic simulation, we rarely need full range
    // Padé [2/2] approximation
    if (x > 10.0)
      return std::exp(x); // Fallback for large values
    if (x < -10.0)
      return 0.0;

    double x2 = x * x;
    double numerator = 2.0 + x + x2 / 6.0;
    double denominator = 2.0 - x + x2 / 6.0;
    return numerator / denominator;
  }

  /**
   * @brief Fast natural logarithm approximation.
   *
   * @param x Input (must be positive)
   * @return Approximate ln(x)
   */
  static inline double fastLog(double x) {
    if (x <= 0.0)
      return -INFINITY;
    if (x == 1.0)
      return 0.0;

    // Use bit manipulation for fast approximation
    uint64_t bits = *(uint64_t *)&x;
    int exponent = ((bits >> 52) & 0x7FF) - 1023;
    bits &= 0x000FFFFFFFFFFFFF;
    bits |= 0x3FF0000000000000; // Set exponent to 0
    double mantissa = *(double *)&bits;

    // Approximate log(mantissa) using polynomial
    double m = mantissa - 1.0;
    double log_mantissa = m * (1.0 - 0.5 * m + 0.333333 * m * m);

    return exponent * 0.693147180559945 + log_mantissa; // ln(2) = 0.693...
  }
};

} // namespace tools
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_TOOLS_FAST_MATH_H
