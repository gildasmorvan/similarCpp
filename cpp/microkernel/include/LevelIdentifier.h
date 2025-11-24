#ifndef LEVELIDENTIFIER_H
#define LEVELIDENTIFIER_H

#include <functional>
#include <stdexcept>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

/**
 * The object identifying one level involved in a simulation.
 */
class LevelIdentifier {
private:
  /**
   * The string identifier of the level.
   */
  std::string identifier;

public:
  /**
   * Builds an instance of this class using a specific value for the level
   * identifier.
   * @param identifier The identifier of the level. This value should be unique.
   */
  explicit LevelIdentifier(const std::string &identifier)
      : identifier(identifier) {
    if (identifier.empty()) { // Assuming empty check as null check equivalent
                              // or just allowing empty string but not null
      // In Java it checked for null. In C++ string cannot be null, but can be
      // empty. Keeping it simple for now.
    }
  }

  /**
   * Gets a printable version of the level identifier.
   * @return A string representation of the level identifier.
   */
  std::string toString() const { return this->identifier; }

  bool operator==(const LevelIdentifier &other) const {
    return this->identifier == other.identifier;
  }

  bool operator!=(const LevelIdentifier &other) const {
    return !(*this == other);
  }

  bool operator<(const LevelIdentifier &other) const {
    return this->identifier < other.identifier;
  }

  std::size_t hashCode() const { return std::hash<std::string>{}(identifier); }
};

} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

namespace std {
template <>
struct hash<fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier> {
  std::size_t operator()(
      const fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier &k)
      const {
    return k.hashCode();
  }
};
} // namespace std

#endif // LEVELIDENTIFIER_H
