#ifndef ABSTRACTINFLUENCE_H
#define ABSTRACTINFLUENCE_H

#include "IInfluence.h"
#include <stdexcept>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace microkernel {
namespace influences {

/**
 * Provides a default behavior of the generic methods of the IInfluence class.
 */
class AbstractInfluence : public ::fr::univ_artois::lgi2a::similar::
                              microkernel::influences::IInfluence {
private:
  ::std::string category;
  ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier level;
  ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
      timeLowerBound;
  ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
      timeUpperBound;

public:
  /**
   * Builds an influence having a specific category and a specific target level.
   */
  AbstractInfluence(
      const ::std::string &category,
      const ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &level,
      const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeLowerBound,
      const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &timeUpperBound)
      : category(category), level(level), timeLowerBound(timeLowerBound),
        timeUpperBound(timeUpperBound) {}

  virtual ~AbstractInfluence() = default;

  // IInfluence interface
  ::std::string getCategory() const override { return category; }

  ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
  getTargetLevel() const override {
    return level;
  }

  ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
  getLevel() const {
    return level;
  }

  ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
  getTimeLowerBound() const override {
    return timeLowerBound;
  }

  ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
  getTimeUpperBound() const override {
    return timeUpperBound;
  }
};

} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTINFLUENCE_H
