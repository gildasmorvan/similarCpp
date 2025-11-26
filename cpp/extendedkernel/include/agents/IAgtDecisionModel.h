#ifndef IAGTDECISIONMODEL_H
#define IAGTDECISIONMODEL_H

#include "../../../microkernel/include/LevelIdentifier.h"
#include "../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../microkernel/include/agents/IGlobalState.h"
#include "../../../microkernel/include/agents/ILocalStateOfAgent.h"
#include "../../../microkernel/include/agents/IPerceivedData.h"
#include "../../../microkernel/include/influences/InfluencesMap.h"
#include <memory>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace extendedkernel {
namespace agents {

/**
 * Models the decision process used by an agent to make decision from a specific
 * level.
 */
class IAgtDecisionModel {
public:
  virtual ~IAgtDecisionModel() = default;

  /**
   * Gets the level from which the decision is made.
   */
  virtual microkernel::LevelIdentifier getLevel() const = 0;

  /**
   * Produces the influences resulting from the decisions of an agent.
   * Default implementation does nothing.
   */
  virtual void decide(
      const microkernel::SimulationTimeStamp &timeLowerBound,
      const microkernel::SimulationTimeStamp &timeUpperBound,
      std::shared_ptr<microkernel::agents::IGlobalState> globalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent> publicLocalState,
      std::shared_ptr<microkernel::agents::ILocalStateOfAgent>
          privateLocalState,
      std::shared_ptr<microkernel::agents::IPerceivedData> perceivedData,
      std::shared_ptr<microkernel::influences::InfluencesMap>
          producedInfluences) {
    // Does nothing by default
  }
};

} // namespace agents
} // namespace extendedkernel
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // IAGTDECISIONMODEL_H
