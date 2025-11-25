#ifndef JAMFREE_KERNEL_AGENTS_INTERFACES_H
#define JAMFREE_KERNEL_AGENTS_INTERFACES_H

#include "../../../../extendedkernel/include/agents/IAgtDecisionModel.h"
#include "../../../../extendedkernel/include/agents/IAgtPerceptionModel.h"
#include "../../../../microkernel/include/LevelIdentifier.h"
#include "../../../../microkernel/include/SimulationTimeStamp.h"
#include "../../../../microkernel/include/agents/IGlobalState.h"
#include "../../../../microkernel/include/agents/ILocalStateOfAgent.h"
#include "../../../../microkernel/include/agents/IPerceivedData.h"
#include "../../../../microkernel/include/dynamicstate/IPublicDynamicStateMap.h"
#include "../../../../microkernel/include/influences/IInfluence.h"
#include "../../../../microkernel/include/influences/InfluencesMap.h"

namespace jamfree {
namespace kernel {
namespace agents {

// Use SIMILAR types
using LevelIdentifier =
    fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier;
using SimulationTimeStamp =
    fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp;
using ILocalState =
    fr::univ_artois::lgi2a::similar::microkernel::agents::ILocalStateOfAgent;
using GlobalState =
    fr::univ_artois::lgi2a::similar::microkernel::agents::IGlobalState;
using IPerceivedData =
    fr::univ_artois::lgi2a::similar::microkernel::agents::IPerceivedData;
using IInfluence =
    fr::univ_artois::lgi2a::similar::microkernel::influences::IInfluence;
using InfluencesMap =
    fr::univ_artois::lgi2a::similar::microkernel::influences::InfluencesMap;
using IPublicDynamicStateMap = fr::univ_artois::lgi2a::similar::microkernel::
    dynamicstate::IPublicDynamicStateMap;

using IPerceptionModel = fr::univ_artois::lgi2a::similar::extendedkernel::
    agents::IAgtPerceptionModel;
using IDecisionModel =
    fr::univ_artois::lgi2a::similar::extendedkernel::agents::IAgtDecisionModel;

/**
 * @brief Reaction model interface.
 * SIMILAR doesn't seem to have a generic IReactionModel in extendedkernel?
 * It usually handles reaction in the engine or environment.
 * But we defined one for jamfree. We can keep it or look for SIMILAR
 * equivalent.
 *
 * In SIMILAR, reaction is often part of the environment or a specific model.
 * Let's keep our IReactionModel for now as it fits our architecture,
 * but make it use SIMILAR types.
 */
class IReactionModel {
public:
  virtual ~IReactionModel() = default;

  /**
   * @brief React to influences and update states.
   *
   * @param timeLowerBound Lower bound of time interval
   * @param timeUpperBound Upper bound of time interval
   * @param influences Influences to react to
   */
  virtual void react(SimulationTimeStamp timeLowerBound,
                     SimulationTimeStamp timeUpperBound,
                     const InfluencesMap &influences) = 0;
};

} // namespace agents
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_AGENTS_INTERFACES_H
