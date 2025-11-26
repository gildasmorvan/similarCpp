#ifndef IAGTGLOBALSTATEREVISIONMODEL_H
#define IAGTGLOBALSTATEREVISIONMODEL_H

#include "../../../microkernel/include/LevelIdentifier.h"
#include "../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../microkernel/include/agents/IGlobalState.h"
#include "../../../microkernel/include/agents/IPerceivedData.h"
#include <map>
#include <memory>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace agents {

/**
 * Models the global state revision process used by an agent to update its
 * global state.
 */
class IAgtGlobalStateRevisionModel {
public:
  virtual ~IAgtGlobalStateRevisionModel() = default;

  /**
   * Revises the content of the global state of the agent.
   * Default implementation does nothing.
   */
  virtual void reviseGlobalState(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::IPerceivedData>>
          &perceivedData,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState) {
    // Does nothing by default
  }
};

} // namespace agents
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IAGTGLOBALSTATEREVISIONMODEL_H
