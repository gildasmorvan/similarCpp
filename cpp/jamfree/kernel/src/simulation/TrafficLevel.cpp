#include "../../include/simulation/TrafficLevel.h"
#include "../../../../microkernel/include/LevelIdentifier.h"

namespace jamfree {
namespace kernel {
namespace simulation {

using namespace fr::univ_artois::lgi2a::similar::microkernel;

TrafficLevel::TrafficLevel(const SimulationTimeStamp &initialTime)
    : libs::abstractimpl::AbstractLevel(initialTime,
                                        LevelIdentifier("microscopic")) {}

SimulationTimeStamp
TrafficLevel::getNextTime(const SimulationTimeStamp &currentTime) {
  // Fixed time step of 1 second for now
  return SimulationTimeStamp(currentTime.getIdentifier() + 1);
}

void TrafficLevel::makeRegularReaction(
    const SimulationTimeStamp &transitoryTimeMin,
    const SimulationTimeStamp &transitoryTimeMax,
    std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
        consistentState,
    const std::set<std::shared_ptr<influences::IInfluence>>
        &regularInfluencesOftransitoryStateDynamics,
    std::shared_ptr<influences::InfluencesMap> remainingInfluences) {
  // Calculate dt
  double dt = (double)(transitoryTimeMax.compareToTimeStamp(transitoryTimeMin));

  // Apply influences using the reaction model
  m_reactionModel.applyInfluences(regularInfluencesOftransitoryStateDynamics,
                                  dt);

  // Update the consistent state time
  consistentState->setTime(transitoryTimeMax);
}

void TrafficLevel::makeSystemReaction(
    const SimulationTimeStamp &transitoryTimeMin,
    const SimulationTimeStamp &transitoryTimeMax,
    std::shared_ptr<dynamicstate::ConsistentPublicLocalDynamicState>
        consistentState,
    const std::vector<std::shared_ptr<influences::IInfluence>>
        &systemInfluencesToManage,
    bool happensBeforeRegularReaction,
    std::shared_ptr<influences::InfluencesMap> newInfluencesToProcess) {
  // Handle system influences (e.g. adding/removing agents)
  // For now, we rely on the default behavior or just ignore if not needed.
  // But usually system influences are handled by the engine or specific
  // handlers. If we need to handle agent addition/removal, we should do it
  // here.

  // For now, empty implementation.
}

std::shared_ptr<levels::ILevel> TrafficLevel::clone() const {
  return std::make_shared<TrafficLevel>(*this);
}

} // namespace simulation
} // namespace kernel
} // namespace jamfree
