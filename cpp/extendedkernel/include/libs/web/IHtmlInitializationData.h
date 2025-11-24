#ifndef IHTMLINITIALIZATIONDATA_H
#define IHTMLINITIALIZATIONDATA_H

#include <memory>

namespace fr::univ_artois::lgi2a::similar::extendedkernel::libs::web {
class SimilarWebConfig;
}

namespace fr::univ_artois::lgi2a::similar::extendedkernel::simulationmodel {
class ISimulationParameters;
}

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace libs {
namespace web {

/**
 * Interface for classes providing initialization data to the HTML web view.
 */
class IHtmlInitializationData {
public:
  virtual ~IHtmlInitializationData() = default;

  /**
   * Gets the configuration for the runner and view.
   * @return The configuration
   */
  virtual SimilarWebConfig *getConfig() = 0;

  /**
   * Gets the simulation parameters.
   * @return The simulation parameters
   */
  virtual simulationmodel::ISimulationParameters *getSimulationParameters() = 0;
};

} // namespace web
} // namespace libs
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IHTMLINITIALIZATIONDATA_H
