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
class AbstractInfluence : public IInfluence {
private:
  const std::string category;
  const LevelIdentifier targetLevel;
  const SimulationTimeStamp timeLowerBound;
  const SimulationTimeStamp timeUpperBound;

public:
  /**
   * Builds an influence having a specific category and a specific target level.
   */
  AbstractInfluence(const std::string &category,
                    const LevelIdentifier &targetLevel,
                    const SimulationTimeStamp &timeLowerBound,
                    const SimulationTimeStamp &timeUpperBound)
      : category(category), targetLevel(targetLevel),
        timeLowerBound(timeLowerBound), timeUpperBound(timeUpperBound) {
    if (category.empty()) {
      // In Java it checked for null.
    }
  }

  std::string getCategory() const override { return this->category; }

  LevelIdentifier getTargetLevel() const override { return this->targetLevel; }

  SimulationTimeStamp getTimeLowerBound() const override {
    return this->timeLowerBound;
  }

  SimulationTimeStamp getTimeUpperBound() const override {
    return this->timeUpperBound;
  }
};

} // namespace influences
} // namespace microkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTINFLUENCE_H
