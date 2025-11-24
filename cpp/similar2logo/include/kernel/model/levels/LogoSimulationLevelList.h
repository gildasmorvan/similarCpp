#ifndef SIMILAR2LOGO_LOGOSIMULATIONLEVELLIST_H
#define SIMILAR2LOGO_LOGOSIMULATIONLEVELLIST_H

#include "../../../../../microkernel/include/LevelIdentifier.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {
namespace levels {

/**
 * Contains the level identifiers used in Logo simulations.
 */
class LogoSimulationLevelList {
public:
  /**
   * The identifier of the "Logo" level.
   * This is the main level used in Logo simulations.
   */
  static const microkernel::LevelIdentifier LOGO;

protected:
  /**
   * Protected constructor to prevent instantiation.
   * This class only defines static values.
   */
  LogoSimulationLevelList() = default;
};

// Define the static member
inline const microkernel::LevelIdentifier LogoSimulationLevelList::LOGO("logo");

} // namespace levels
} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_LOGOSIMULATIONLEVELLIST_H
