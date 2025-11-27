#include "libs/random/PRNG.h"

// Define M_PI for Windows MSVC
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace random {

// Initialize static members
Xoshiro256PlusPlus PRNG::generator(std::random_device{}());
std::uniform_real_distribution<double> PRNG::uniformDist(0.0, 1.0);
std::normal_distribution<double> PRNG::normalDist(0.0, 1.0);

void PRNG::setSeed(uint64_t seed) { generator.seed(seed); }

double PRNG::randomDouble() { return uniformDist(generator); }

double PRNG::randomDouble(double lowerBound, double higherBound) {
  return lowerBound + (higherBound - lowerBound) * randomDouble();
}

double PRNG::randomAngle() { return randomDouble(-M_PI, M_PI); }

bool PRNG::randomBoolean() { return randomDouble() < 0.5; }

int PRNG::randomInt(int bound) {
  std::uniform_int_distribution<int> dist(0, bound - 1);
  return dist(generator);
}

int PRNG::randomSign() { return randomBoolean() ? 1 : -1; }

double PRNG::randomGaussian() { return normalDist(generator); }

double PRNG::randomGaussian(double mean, double sd) {
  return mean + sd * randomGaussian();
}

std::string PRNG::getImplementationName() { return "Xoshiro256++"; }

} // namespace random
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr
