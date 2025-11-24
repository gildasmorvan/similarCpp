#ifndef ABSTRACTSIMULATIONPARAMETERS_H
#define ABSTRACTSIMULATIONPARAMETERS_H

#include "../../simulationmodel/ISimulationParameters.h"

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace abstractimpl {

/**
 * An abstract implementation of the ISimulationParameters interface.
 */
class AbstractSimulationParameters
    : public simulationmodel::ISimulationParameters {
public:
  virtual ~AbstractSimulationParameters() = default;

protected:
  AbstractSimulationParameters() = default;
};

} // namespace abstractimpl
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // ABSTRACTSIMULATIONPARAMETERS_H
