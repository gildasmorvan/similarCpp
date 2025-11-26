#ifndef JAMFREE_KERNEL_REACTION_TRAFFIC_REACTION_MODEL_H
#define JAMFREE_KERNEL_REACTION_TRAFFIC_REACTION_MODEL_H

#include "../../../../microkernel/include/influences/IInfluence.h"
#include <memory>
#include <set>

namespace jamfree {
namespace kernel {
namespace reaction {

/**
 * @brief Reaction model for traffic simulation.
 *
 * Handles the resolution of influences and updates vehicle states.
 */
class TrafficReactionModel {
public:
  /**
   * @brief Apply influences to vehicle states.
   *
   * @param influences Set of influences to process
   * @param dt Time step duration (seconds)
   */
  void applyInfluences(
      const std::set<std::shared_ptr<
          fr::univ_artois::lgi2a::similar::microkernel::influences::IInfluence>>
          &influences,
      double dt);
};

} // namespace reaction
} // namespace kernel
} // namespace jamfree

#endif // JAMFREE_KERNEL_REACTION_TRAFFIC_REACTION_MODEL_H
