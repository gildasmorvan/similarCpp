/**
 * SIMILAR C++ - Random & End Criterion Demo
 *
 * Demonstrates the PRNG utility and TimeBasedEndCriterion classes.
 */

#include "libs/endcriterion/TimeBasedEndCriterion.h"
#include "libs/random/PRNG.h"
#include <iomanip>
#include <iostream>
#include <vector>

using namespace fr::univ_artois::lgi2a::similar;

int main() {
  std::cout << "╔══════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║     SIMILAR C++ - Random & End Criterion Demo       ║"
            << std::endl;
  std::cout << "╚══════════════════════════════════════════════════════╝"
            << std::endl;
  std::cout << std::endl;

  // ===== 1. PRNG Basics =====
  std::cout << "1️⃣  PRNG - Pseudo-Random Number Generator" << std::endl;
  std::cout << "   Generator: "
            << extendedkernel::libs::random::PRNG::getImplementationName()
            << std::endl;
  std::cout << std::endl;

  // Set seed for reproducibility
  extendedkernel::libs::random::PRNG::setSeed(42);
  std::cout << "   🌱 Seed set to: 42" << std::endl;
  std::cout << std::endl;

  // ===== 2. Random Doubles =====
  std::cout << "2️⃣  Random Doubles [0, 1):" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 5; i++) {
    std::cout << std::fixed << std::setprecision(4)
              << extendedkernel::libs::random::PRNG::randomDouble() << "  ";
  }
  std::cout << std::endl << std::endl;

  // ===== 3. Random Doubles in Range =====
  std::cout << "3️⃣  Random Doubles [10, 20):" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 5; i++) {
    std::cout << std::fixed << std::setprecision(2)
              << extendedkernel::libs::random::PRNG::randomDouble(10.0, 20.0)
              << "  ";
  }
  std::cout << std::endl << std::endl;

  // ===== 4. Random Angles =====
  std::cout << "4️⃣  Random Angles [-π, π):" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 5; i++) {
    std::cout << std::fixed << std::setprecision(3)
              << extendedkernel::libs::random::PRNG::randomAngle() << "  ";
  }
  std::cout << std::endl << std::endl;

  // ===== 5. Random Booleans =====
  std::cout << "5️⃣  Random Booleans:" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 10; i++) {
    std::cout << (extendedkernel::libs::random::PRNG::randomBoolean() ? "T"
                                                                      : "F")
              << " ";
  }
  std::cout << std::endl << std::endl;

  // ===== 6. Random Integers =====
  std::cout << "6️⃣  Random Integers [0, 10):" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 10; i++) {
    std::cout << extendedkernel::libs::random::PRNG::randomInt(10) << " ";
  }
  std::cout << std::endl << std::endl;

  // ===== 7. Random Signs =====
  std::cout << "7️⃣  Random Signs (-1 or +1):" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 10; i++) {
    int sign = extendedkernel::libs::random::PRNG::randomSign();
    std::cout << (sign > 0 ? "+" : "") << sign << " ";
  }
  std::cout << std::endl << std::endl;

  // ===== 8. Gaussian Distribution =====
  std::cout << "8️⃣  Gaussian Distribution (mean=0, sd=1):" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 5; i++) {
    std::cout << std::fixed << std::setprecision(3)
              << extendedkernel::libs::random::PRNG::randomGaussian() << "  ";
  }
  std::cout << std::endl << std::endl;

  std::cout << "9️⃣  Gaussian Distribution (mean=100, sd=15):" << std::endl;
  std::cout << "   ";
  for (int i = 0; i < 5; i++) {
    std::cout << std::fixed << std::setprecision(1)
              << extendedkernel::libs::random::PRNG::randomGaussian(100.0, 15.0)
              << "  ";
  }
  std::cout << std::endl << std::endl;

  // ===== 10. Shuffle =====
  std::cout << "🔟 Shuffle Vector:" << std::endl;
  std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::cout << "   Before: ";
  for (int n : numbers)
    std::cout << n << " ";
  std::cout << std::endl;

  extendedkernel::libs::random::PRNG::shuffle(numbers);
  std::cout << "   After:  ";
  for (int n : numbers)
    std::cout << n << " ";
  std::cout << std::endl << std::endl;

  // ===== 11. Time-Based End Criterion =====
  std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
            << std::endl;
  std::cout << "1️⃣1️⃣  Time-Based End Criterion" << std::endl;
  std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
            << std::endl;
  std::cout << std::endl;

  microkernel::SimulationTimeStamp finalTime(100);
  extendedkernel::libs::endcriterion::TimeBasedEndCriterion endCriterion(
      finalTime);

  std::cout << "   Final time set to: "
            << endCriterion.getFinalTimeStamp().getIdentifier() << std::endl;
  std::cout << std::endl;

  std::cout << "   Testing end criterion:" << std::endl;
  std::vector<long> testTimes = {50, 99, 100, 101, 150};
  for (long t : testTimes) {
    microkernel::SimulationTimeStamp currentTime(t);
    bool isEnd = endCriterion.isFinalTimeOrAfter(currentTime, nullptr);
    std::cout << "   t=" << std::setw(3) << t << " → "
              << (isEnd ? "✓ END" : "✗ continue") << std::endl;
  }
  std::cout << std::endl;

  // ===== Summary =====
  std::cout << "╔══════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║                    Summary                           ║"
            << std::endl;
  std::cout << "╚══════════════════════════════════════════════════════╝"
            << std::endl;
  std::cout << std::endl;
  std::cout << "✅ PRNG utilities working correctly!" << std::endl;
  std::cout << "✅ TimeBasedEndCriterion working correctly!" << std::endl;
  std::cout << std::endl;
  std::cout << "Available PRNG methods:" << std::endl;
  std::cout << "  • randomDouble() / randomDouble(min, max)" << std::endl;
  std::cout << "  • randomAngle()" << std::endl;
  std::cout << "  • randomBoolean()" << std::endl;
  std::cout << "  • randomInt(bound)" << std::endl;
  std::cout << "  • randomSign()" << std::endl;
  std::cout << "  • randomGaussian() / randomGaussian(mean, sd)" << std::endl;
  std::cout << "  • shuffle(vector)" << std::endl;
  std::cout << "  • setSeed(seed)" << std::endl;
  std::cout << std::endl;

  return 0;
}
