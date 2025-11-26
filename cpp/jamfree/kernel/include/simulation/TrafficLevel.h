#ifndef JAMFREE_KERNEL_SIMULATION_TRAFFIC_LEVEL_H
#define JAMFREE_KERNEL_SIMULATION_TRAFFIC_LEVEL_H

#include "../../../../microkernel/include/libs/abstractimpl/AbstractLevel.h"
#include "../reaction/TrafficReactionModel.h"
#include <memory>

namespace jamfree {
namespace kernel {
namespace simulation {

/**
 * @brief Traffic simulation level (microscopic).
 */
class TrafficLevel : public fr::univ_artois::lgi2a::similar::microkernel::libs::
                         abstractimpl::AbstractLevel {
public:
  /**
   * @brief Constructor.
   * @param initialTime Initial simulation time
   */
  TrafficLevel(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &initialTime);

  /**
   * @brief Destructor.
   */
  virtual ~TrafficLevel() = default;

  // ILevel implementation
  fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp getNextTime(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &currentTime) override;

  void makeRegularReaction(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &transitoryTimeMin,
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &transitoryTimeMax,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::
                          dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::set<std::shared_ptr<
          fr::univ_artois::lgi2a::similar::microkernel::influences::IInfluence>>
          &regularInfluencesOftransitoryStateDynamics,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::influences::
                          InfluencesMap>
          remainingInfluences) override;

  void makeSystemReaction(
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &transitoryTimeMin,
      const fr::univ_artois::lgi2a::similar::microkernel::SimulationTimeStamp
          &transitoryTimeMax,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::
                          dynamicstate::ConsistentPublicLocalDynamicState>
          consistentState,
      const std::vector<std::shared_ptr<
          fr::univ_artois::lgi2a::similar::microkernel::influences::IInfluence>>
          &systemInfluencesToManage,
      bool happensBeforeRegularReaction,
      std::shared_ptr<fr::univ_artois::lgi2a::similar::microkernel::influences::
                          InfluencesMap>
          newInfluencesToProcess) override;

private:
  reaction::TrafficReactionModel m_reactionModel;
};

} // namespace simulation
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_SIMULATION_TRAFFIC_LEVEL_H
