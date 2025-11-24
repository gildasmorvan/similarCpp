#ifndef SIMULATIONTIMESTAMP_H
#define SIMULATIONTIMESTAMP_H

#include <cstdint>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {

/**
 * Models a time stamp (i.e. the beginning of a time step) of the simulation.
 */
class SimulationTimeStamp {
private:
  /**
   * A value used in the computation of the hashcode.
   */
  static const int SHIFT_VALUE = 32;

  /**
   * The unique identifier of this time stamp.
   */
  long identifier;

public:
  /**
   * Builds a time stamp having a specific identifier.
   * @param identifier The identifier of the time stamp.
   */
  explicit SimulationTimeStamp(long identifier) : identifier(identifier) {}

  /**
   * Builds a time stamp as a copy of another time stamp.
   * @param toCopy The time stamp to copy.
   */
  SimulationTimeStamp(const SimulationTimeStamp &toCopy)
      : identifier(toCopy.identifier) {}

  /**
   * Builds a time stamp which identifier is defined relatively to the
   * identifier of another time stamp.
   * @param reference The time stamp used as a reference to create the
   * identifier of the new time stamp.
   * @param shift The value added to the identifier of the reference to obtain
   * the identifier of the new time stamp.
   * @throws std::invalid_argument If the reference is null (not applicable in
   * C++ ref).
   * @throws std::overflow_error If the identifier of the new time stamp exceeds
   * the capacity of the long type.
   */
  SimulationTimeStamp(const SimulationTimeStamp &reference, int shift) {
    if ((shift > 0 &&
         std::numeric_limits<long>::max() - reference.identifier < shift) ||
        (shift < 0 &&
         reference.identifier - std::numeric_limits<long>::min() > -shift)) {
      throw std::overflow_error("The identifier of the new time stamp exceeds "
                                "the capacity of long values.");
    } else {
      this->identifier = reference.identifier + shift;
    }
  }

  /**
   * Gets the unique identifier of this time stamp.
   * @return The unique identifier of this time stamp.
   */
  long getIdentifier() const { return this->identifier; }

  /**
   * Compares the identifier of this time stamp to the identifier of another
   * time stamp.
   * @param otherTimeStamp The other time stamp used in the comparison.
   * @return the difference between the identifier of this time stamp and the
   * identifier of the parameter.
   */
  long compareToTimeStamp(const SimulationTimeStamp &otherTimeStamp) const {
    return this->identifier - otherTimeStamp.identifier;
  }

  bool operator==(const SimulationTimeStamp &other) const {
    return this->identifier == other.identifier;
  }

  bool operator!=(const SimulationTimeStamp &other) const {
    return !(*this == other);
  }

  bool operator<(const SimulationTimeStamp &other) const {
    return this->identifier < other.identifier;
  }

  /**
   * Returns a hash code value for this object.
   */
  std::size_t hashCode() const {
    // Simple hash implementation similar to Java's
    return std::hash<long>{}(identifier);
  }

  std::string toString() const {
    return "t(" + std::to_string(this->identifier) + ")";
  }
};

} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

namespace std {
template <>
struct hash<fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp> {
  std::size_t operator()(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &k) const {
    return k.hashCode();
  }
};
} // namespace std

#endif // SIMULATIONTIMESTAMP_H
