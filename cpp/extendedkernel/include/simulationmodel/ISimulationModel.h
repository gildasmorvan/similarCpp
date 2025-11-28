#ifndef EXTENDEDKERNEL_ISIMULATIONMODEL_H
#define EXTENDEDKERNEL_ISIMULATIONMODEL_H

#include "../../../microkernel/include/ISimulationModel.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace simulationmodel {

// Forward declaration
class ISimulationParameters;

/**
 * Stub interface for simulation model (minimal for web interface compilation)
 */
class ISimulationModel
    : public ::fr::univ_artois::lgi2a::similar::microkernel::ISimulationModel {
public:
  virtual ~ISimulationModel() = default;

  /**
   * Gets the simulation parameters.
   * @return The simulation parameters
   */
  virtual ISimulationParameters *getSimulationParameters() = 0;
};

} // namespace simulationmodel
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // EXTENDEDKERNEL_ISIMULATIONMODEL_H
