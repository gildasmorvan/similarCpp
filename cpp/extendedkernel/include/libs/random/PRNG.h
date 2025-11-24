#ifndef PRNG_H
#define PRNG_H

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>
#include <vector>

#include "Xoshiro256PlusPlus.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace random {

/**
 * Pseudo-Random Number Generator (PRNG) utility class.
 *
 * Provides convenient static methods for generating random values.
 * Uses Xoshiro256++ as the default generator for high performance.
 *
 * Thread-safe: Each thread should have its own generator or use mutex
 * protection.
 */
class PRNG {
private:
  static Xoshiro256PlusPlus generator;
  static std::uniform_real_distribution<double> uniformDist;
  static std::normal_distribution<double> normalDist;

  // Private constructor to prevent instantiation
  PRNG() = delete;

public:
  /**
   * Sets the seed for the random number generator.
   * @param seed The seed value
   */
  static void setSeed(uint64_t seed);

  /**
   * Gets a random double between 0 (included) and 1 (excluded).
   * @return A random double in [0, 1)
   */
  static double randomDouble();

  /**
   * Generates a random double within a range.
   * @param lowerBound The lower bound (included)
   * @param higherBound The higher bound (excluded)
   * @return A random double in [lowerBound, higherBound)
   */
  static double randomDouble(double lowerBound, double higherBound);

  /**
   * Gets a random angle between -π (included) and π (excluded).
   * @return A random angle in [-π, π)
   */
  static double randomAngle();

  /**
   * Gets a random boolean.
   * @return A random boolean value
   */
  static bool randomBoolean();

  /**
   * Gets a random integer from 0 to bound - 1.
   * @param bound The (excluded) upper bound
   * @return A random integer in [0, bound)
   */
  static int randomInt(int bound);

  /**
   * Gets -1 or +1 randomly.
   * @return -1 or +1
   */
  static int randomSign();

  /**
   * Gets a Gaussian (normally) distributed double value with mean 0.0 and
   * standard deviation 1.0.
   * @return A Gaussian distributed value
   */
  static double randomGaussian();

  /**
   * Gets a Gaussian (normally) distributed double value with given mean and
   * standard deviation.
   * @param mean The mean
   * @param sd The standard deviation
   * @return A Gaussian distributed value
   */
  static double randomGaussian(double mean, double sd);

  /**
   * Shuffles the given vector.
   * @param vec The vector to shuffle
   */
  template <typename T> static void shuffle(std::vector<T> &vec) {
    std::shuffle(vec.begin(), vec.end(), generator);
  }

  /**
   * Gets the name of the random generator implementation.
   * @return The generator name
   */
  static std::string getImplementationName();
};

} // namespace random
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // PRNG_H
