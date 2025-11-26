#include "../../include/reaction/TrafficReactionModel.h"
#include "../../../kernel/include/model/Vehicle.h"
#include "../../include/influences/AccelerationInfluence.h"
#include "../../include/influences/LaneChangeInfluence.h"
#include "../../include/influences/PositionUpdateInfluence.h"
#include "../../include/model/Road.h"
#include <iostream>

namespace jamfree {
namespace kernel {
namespace reaction {

using namespace fr::univ_artois::lgi2a::similar::microkernel::influences;
using namespace jamfree::kernel::influences;

void TrafficReactionModel::applyInfluences(
    const std::set<std::shared_ptr<IInfluence>> &influences, double dt) {
  for (const auto &influence : influences) {
    std::string category = influence->getCategory();

    if (category == AccelerationInfluence::CATEGORY) {
      auto accInf = std::dynamic_pointer_cast<AccelerationInfluence>(influence);
      if (accInf) {
        auto target = accInf->getTarget();
        if (target) {
          double acc = accInf->getAcceleration();

          // Update speed: v = v0 + a * dt
          double currentSpeed = target->getSpeed();
          double newSpeed = currentSpeed + acc * dt;
          if (newSpeed < 0)
            newSpeed = 0; // No reversing

          // Update position: x = x0 + v * dt + 0.5 * a * dt^2
          // Note: We update lane position. Global position (x,y) needs lane
          // geometry. For now, we update lane position and let the agent/level
          // handle geometry update if needed. Or we can update geometry here if
          // we have access to the lane.

          double currentLanePos = target->getLanePosition();
          double dist = currentSpeed * dt + 0.5 * acc * dt * dt;
          if (dist < 0 && newSpeed == 0)
            dist = 0; // Stopped

          target->setSpeed(newSpeed);
          target->setAcceleration(acc);
          target->setLanePosition(currentLanePos + dist);

          // Note: We don't handle lane end transitions here yet.
          // That requires access to the road network or routing engine.
          // This simple reaction model assumes the vehicle stays on the lane
          // for this step. Transitions should be handled either here (if we
          // have network access) or in a separate phase. Given the
          // architecture, the Level usually has access to the environment. But
          // here we are just updating the state based on influences.
        }
      }
    } else if (category == LaneChangeInfluence::CATEGORY) {
      auto lcInf = std::dynamic_pointer_cast<LaneChangeInfluence>(influence);
      if (lcInf) {
        auto target = lcInf->getTarget();
        if (target) {
          LaneChangeInfluence::Direction dir = lcInf->getDirection();
          // Perform lane change logic
          // This requires access to adjacent lanes.
          // The target state has current lane.
          // We need to get the adjacent lane from the current lane.

          auto currentLane = target->getCurrentLane();
          if (currentLane) {
            auto road = currentLane->getParentRoad();
            if (road) {
              int currentIndex = currentLane->getIndex();
              int targetIndex = currentIndex;

              if (dir == LaneChangeInfluence::Direction::LEFT) {
                targetIndex++;
              } else if (dir == LaneChangeInfluence::Direction::RIGHT) {
                targetIndex--;
              }

              if (targetIndex >= 0 && targetIndex < road->getNumLanes() &&
                  targetIndex != currentIndex) {
                auto targetLane = road->getLane(targetIndex);
                if (targetLane) {
                  // Move vehicle
                  auto pls = std::dynamic_pointer_cast<
                      microscopic::agents::VehiclePublicLocalStateMicro>(
                      target);
                  if (pls) {
                    std::string vehicleId = pls->getOwnerId();
                    std::shared_ptr<model::Vehicle> vehicleToMove = nullptr;

                    // Find vehicle in current lane
                    for (const auto &v : currentLane->getVehicles()) {
                      if (v->getId() == vehicleId) {
                        vehicleToMove = v;
                        break;
                      }
                    }

                    if (vehicleToMove) {
                      currentLane->removeVehicle(vehicleToMove);
                      targetLane->addVehicle(vehicleToMove);

                      // Update PLS
                      target->setCurrentLane(targetLane.get());
                      target->setLaneIndex(targetIndex);
                    }
                  }
                }
              }
            }
          }
        }
      }
    } else if (category == PositionUpdateInfluence::CATEGORY) {
      auto posInf =
          std::dynamic_pointer_cast<PositionUpdateInfluence>(influence);
      if (posInf) {
        auto target = posInf->getTarget();
        if (target) {
          // Update global position directly (e.g. for teleport)
          // Note: This doesn't update lane info automatically.
          // Usually used for initialization.
          // target->setPosition(Point2D(posInf->getX(), posInf->getY())); //
          // Need Point2D target->setHeading(posInf->getHeading());
        }
      }
    }
  }
}

} // namespace reaction
} // namespace kernel
} // namespace jamfree
