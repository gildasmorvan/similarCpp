#ifndef IAGTPERCEPTIONMODEL_H
#define IAGTPERCEPTIONMODEL_H

#include "../../microkernel/include/LevelIdentifier.h"
#include "../../microkernel/include/SimulationTimeStamp.h"
#include "../../microkernel/include/agents/ILocalStateOfAgent.h"
#include "../../microkernel/include/agents/IPerceivedData.h"
#include "../../microkernel/include/dynamicstate/IPublicDynamicStateMap.h"
#include <map>
#include <memory>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace agents {

/**
 * Models the perception process used by an agent to produce perceived data from
 * a specific level.
 */
class IAgtPerceptionModel {
public:
  virtual ~IAgtPerceptionModel() = default;

  /**
   * Gets the level from which the perception is made.
   */
  virtual microkernel::LevelIdentifier getLevel() const = 0;

  /**
   * Creates the data perceived by an agent located in a specific level.
   */
  virtual std::shared_ptr<microkernel::agents::IPerceivedData> perceive(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      const std::map<microkernel::LevelIdentifier,
                     std::shared_ptr<microkernel::agents::ILocalStateOfAgent>>
          &publicLocalStates,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::dynamicstate::IPublicDynamicStateMap>
          dynamicStates) = 0;
};

} // namespace agents
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IAGTPERCEPTIONMODEL_H
