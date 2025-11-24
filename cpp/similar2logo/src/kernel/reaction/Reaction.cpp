#include "kernel/reaction/Reaction.h"
#include "kernel/influences/ChangeDirection.h"
#include "kernel/influences/ChangePosition.h"
#include "kernel/influences/ChangeSpeed.h"
#include "kernel/influences/EmitPheromone.h"
#include "kernel/influences/Stop.h"
#include "kernel/model/environment/TurtlePLSInLogo.h"
#include "kernel/tools/MathUtil.h"
#include <algorithm>
#include <cmath>

namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::reaction {

using namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::
    influences;
using namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::
    environment;
using namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools;

void Reaction::apply(const std::vector<std::shared_ptr<IInfluence>> &influences,
                     Environment &env, double dt) {

  for (const auto &influence : influences) {
    if (!influence)
      continue;

    // ChangePosition
    if (auto cp = std::dynamic_pointer_cast<ChangePosition>(influence)) {
      auto target = cp->getTarget();
      if (target) {
        double newX = target->getLocation().x + cp->getDx();
        double newY = target->getLocation().y + cp->getDy();

        if (env.toroidal()) {
          newX = std::fmod(newX, env.width());
          if (newX < 0)
            newX += env.width();
          newY = std::fmod(newY, env.height());
          if (newY < 0)
            newY += env.height();
        } else {
          newX = std::max(0.0, std::min(newX, (double)env.width() - 1));
          newY = std::max(0.0, std::min(newY, (double)env.height() - 1));
        }
        target->setLocation(Point2D(newX, newY));
      }
    }
    // ChangeDirection
    else if (auto cd = std::dynamic_pointer_cast<ChangeDirection>(influence)) {
      auto target = cd->getTarget();
      if (target) {
        double newHeading = target->getHeading() + cd->getDd();
        target->setHeading(MathUtil::normalizeAngle(newHeading));
      }
    }
    // ChangeSpeed
    else if (auto cs = std::dynamic_pointer_cast<ChangeSpeed>(influence)) {
      auto target = cs->getTarget();
      if (target) {
        double newSpeed = target->getSpeed() + cs->getDs();
        if (newSpeed < 0)
          newSpeed = 0;
        target->setSpeed(newSpeed);
      }
    }
    // Stop
    else if (auto stop = std::dynamic_pointer_cast<Stop>(influence)) {
      auto target = stop->getTarget();
      if (target) {
        target->setSpeed(0);
      }
    }
    // EmitPheromone
    else if (auto ep = std::dynamic_pointer_cast<EmitPheromone>(influence)) {
      double x = ep->getLocation().x;
      double y = ep->getLocation().y;
      // Clamp to grid
      if (env.toroidal()) {
        x = std::fmod(x, env.width());
        if (x < 0)
          x += env.width();
        y = std::fmod(y, env.height());
        if (y < 0)
          y += env.height();
      } else {
        x = std::max(0.0, std::min(x, (double)env.width() - 1));
        y = std::max(0.0, std::min(y, (double)env.height() - 1));
      }

      double current =
          env.get_pheromone_value(x, y, ep->getPheromoneIdentifier());
      env.set_pheromone(x, y, ep->getPheromoneIdentifier(),
                        current + ep->getValue());
    }
  }

  env.diffuse_and_evaporate(dt);
}

} // namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::reaction
