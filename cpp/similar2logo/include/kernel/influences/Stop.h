#ifndef SIMILAR2LOGO_STOP_H
#define SIMILAR2LOGO_STOP_H

#include "../../../../microkernel/include/LevelIdentifier.h"
#include "../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../microkernel/include/influences/RegularInfluence.h"
#include "../model/environment/TurtlePLSInLogo.h"
#include <memory>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace influences {

/**
 * Models an influence that aims at stopping a turtle.
 * Matches the Java implementation exactly.
 *
 * @author Gildas Morvan
 * @author Yoann Kubera
 */
class Stop : public microkernel::influences::RegularInfluence {
public:
  /**
   * The category of the influence, used as a unique identifier in
   * the reaction of the target level to determine the nature of the influence.
   */
  static constexpr const char *CATEGORY = "stop";

private:
  /**
   * The turtle's public local state that is going to change.
   */
  std::shared_ptr<model::environment::TurtlePLSInLogo> target;

public:
  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in the LOGO level.
   *
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param target The turtle's public local state that is going to change
   */
  Stop(const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
           &timeLowerBound,
       const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
           &timeUpperBound,
       std::shared_ptr<model::environment::TurtlePLSInLogo> target);

  /**
   * Builds an instance of this influence created during the transitory
   * period ] timeLowerBound, timeUpperBound [, in a given level.
   *
   * @param levelIdentifier The level in which the influence is emitted
   * @param timeLowerBound The lower bound of the transitory period
   * @param timeUpperBound The upper bound of the transitory period
   * @param target The turtle's public local state that is going to change
   */
  Stop(const ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
           &levelIdentifier,
       const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
           &timeLowerBound,
       const ::fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
           &timeUpperBound,
       std::shared_ptr<model::environment::TurtlePLSInLogo> target);

  /**
   * @return The turtle's public local state that is going to change.
   */
  std::shared_ptr<model::environment::TurtlePLSInLogo> getTarget() const {
    return target;
  }
};

} // namespace influences
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_STOP_H
