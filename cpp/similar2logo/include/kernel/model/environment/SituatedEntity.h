#ifndef SIMILAR2LOGO_SITUATEDENTITY_H
#define SIMILAR2LOGO_SITUATEDENTITY_H

#include "../../tools/Point2D.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {
namespace environment {

/**
 * An interface representing a situated entity in the Logo environment.
 * Any entity that has a position in the 2D space implements this interface.
 */
class SituatedEntity {
public:
  virtual ~SituatedEntity() = default;

  /**
   * Gets the location of the entity.
   * @return The location of the entity
   */
  virtual ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D
  getLocation() const = 0;
};

} // namespace environment
} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_SITUATEDENTITY_H
