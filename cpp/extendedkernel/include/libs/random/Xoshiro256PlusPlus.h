#ifndef XOSHIRO256PLUSPLUS_H
#define XOSHIRO256PLUSPLUS_H

#include <cstdint>
#include <limits>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace random {

/**
 * Xoshiro256++ pseudo-random number generator.
 *
 * This is a fast, high-quality PRNG that is significantly faster than
 * std::mt19937. It is suitable for large-scale parallel simulations.
 *
 * Based on the reference implementation by David Blackman and Sebastiano Vigna.
 * http://prng.di.unimi.it/
 */
class Xoshiro256PlusPlus {
public:
  using result_type = uint64_t;

  static constexpr result_type min() {
    return std::numeric_limits<result_type>::min();
  }
  static constexpr result_type max() {
    return std::numeric_limits<result_type>::max();
  }

  explicit Xoshiro256PlusPlus(uint64_t seed_val = 0) { seed(seed_val); }

  void seed(uint64_t seed_val) {
    // Initialize state with SplitMix64
    uint64_t z = seed_val + 0x9e3779b97f4a7c15;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    s[0] = z ^ (z >> 31);

    z = s[0] + 0x9e3779b97f4a7c15;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    s[1] = z ^ (z >> 31);

    z = s[1] + 0x9e3779b97f4a7c15;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    s[2] = z ^ (z >> 31);

    z = s[2] + 0x9e3779b97f4a7c15;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    s[3] = z ^ (z >> 31);
  }

  result_type operator()() {
    const uint64_t result = rotl(s[0] + s[3], 23) + s[0];

    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = rotl(s[3], 45);

    return result;
  }

  /**
   * Jump function to advance the state by 2^128 calls.
   * Useful for parallel streams.
   */
  void jump() {
    static const uint64_t JUMP[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c,
                                    0xa9582618e03fc9aa, 0x39abdc4529b1661c};

    uint64_t s0 = 0;
    uint64_t s1 = 0;
    uint64_t s2 = 0;
    uint64_t s3 = 0;
    for (int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
      for (int b = 0; b < 64; b++) {
        if (JUMP[i] & (uint64_t(1) << b)) {
          s0 ^= s[0];
          s1 ^= s[1];
          s2 ^= s[2];
          s3 ^= s[3];
        }
        operator()();
      }

    s[0] = s0;
    s[1] = s1;
    s[2] = s2;
    s[3] = s3;
  }

private:
  uint64_t s[4];

  static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
  }
};

} // namespace random
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // XOSHIRO256PLUSPLUS_H
