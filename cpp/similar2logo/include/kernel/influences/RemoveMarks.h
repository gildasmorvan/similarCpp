#ifndef SIMILAR2LOGO_REMOVEMARKS_H
#define SIMILAR2LOGO_REMOVEMARKS_H

#include "../../../../microkernel/include/LevelIdentifier.h"
#include "../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../model/environment/Mark.h"
#include <memory>
#include <string>
#include <unordered_set>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

/**
 * Models an influence that aims at removing marks from the environment.
 * Matches the Java implementation exactly.
 *
 * @author Gildas Morvan
 * @author Yoann Kubera
 */
class RemoveMarks : public microkernel::influences::RegularInfluence {
public:
  /**
   * The category of the influence, used as a unique identifier in
   * the reaction of the target level to determine the nature of the influence.
   */
  static constexpr const char *CATEGORY = "remove marks";

private:
  /**
   * The marks to remove.
   */
  std::unordered_set<std::shared_ptr<model::environment::SimpleMark>> marks;

public:
  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in the LOGO level.
   *
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param marks The marks to remove
   */
  RemoveMarks(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::unordered_set<std::shared_ptr<model::environment::SimpleMark>>
          &marks);

  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in a given level.
   *
   * @param levelIdentifier The level in which the influence is emitted
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param marks The marks to remove
   */
  RemoveMarks(
      const microkernel::LevelIdentifier &levelIdentifier,
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::unordered_set<std::shared_ptr<model::environment::SimpleMark>>
          &marks);

  /**
   * @return The marks to remove.
   */
  const std::unordered_set<std::shared_ptr<model::environment::SimpleMark>> &
  getMarks() const {
    return marks;
  }
};

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_REMOVEMARKS_H
