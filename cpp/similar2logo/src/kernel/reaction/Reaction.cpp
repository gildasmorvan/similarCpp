// C++ Reaction implementation for Similar2Logo.
// This implementation is intentionally simpler than the full Java
// LogoDefaultReactionModel and focuses on:
// - Basic turtle kinematics (position, heading, speed)
// - Pheromone emission and field dynamics
// Higher-level behaviors (marks, natural influences, system influences) are
// handled in the extended kernel / Java-compatible layer.

#include "kernel/reaction/Reaction.h"
// C++ Reaction implementation for Similar2Logo.
// This implementation focuses on:
// - Basic turtle kinematics (position, heading, speed)
// - Pheromone emission and field dynamics
//
// Higher-level behaviors (marks, full natural/system influences, Java
// multi-level coordination) are handled in the full extended kernel.

#include "kernel/reaction/Reaction.h"
#include "kernel/influences/ChangeDirection.h"
#include "kernel/influences/ChangePosition.h"
#include "kernel/influences/ChangeSpeed.h"
#include "kernel/influences/ChangeAcceleration.h"
#include "kernel/influences/EmitPheromone.h"
#include "kernel/influences/Stop.h"
#include "kernel/influences/DropMark.h"
#include "kernel/influences/RemoveMark.h"
#include "kernel/influences/RemoveMarks.h"
#include "kernel/influences/AgentPositionUpdate.h"
#include "kernel/influences/PheromoneFieldUpdate.h"
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
    // ChangeAcceleration
    else if (auto ca = std::dynamic_pointer_cast<ChangeAcceleration>(influence)) {
      auto target = ca->getTarget();
      if (target) {
        double newAccel = target->getAcceleration() + ca->getDa();
        target->setAcceleration(newAccel);
      }
    }
    // DropMark
    else if (auto dm = std::dynamic_pointer_cast<DropMark>(influence)) {
      auto mark = dm->getMark();
      if (mark) {
        int x = static_cast<int>(std::floor(mark->getLocation().x));
        int y = static_cast<int>(std::floor(mark->getLocation().y));
        env.add_mark(x, y, mark);
      }
    }
    // RemoveMark
    else if (auto rm = std::dynamic_pointer_cast<RemoveMark>(influence)) {
      auto mark = rm->getMark();
      if (mark) {
        int x = static_cast<int>(std::floor(mark->getLocation().x));
        int y = static_cast<int>(std::floor(mark->getLocation().y));
        env.remove_mark(x, y, mark);
      }
    }
    // RemoveMarks
    else if (auto rms = std::dynamic_pointer_cast<RemoveMarks>(influence)) {
      for (const auto &mark : rms->getMarks()) {
        int x = static_cast<int>(std::floor(mark->getLocation().x));
        int y = static_cast<int>(std::floor(mark->getLocation().y));
        env.remove_mark(x, y, mark);
      }
    }
    // AgentPositionUpdate (natural influence)
    else if (auto apu = std::dynamic_pointer_cast<AgentPositionUpdate>(influence)) {
      // Update all turtles based on their speed and acceleration
      // This is a system-level influence that affects all turtles
      for (auto &turtle : env.get_turtles()) {
        // Update speed based on acceleration
        double newSpeed = turtle->getSpeed() + turtle->getAcceleration() * dt;
        if (newSpeed < 0) newSpeed = 0;  // No negative speed
        turtle->setSpeed(newSpeed);

        // Calculate movement
        double dx = std::cos(turtle->getHeading()) * newSpeed * dt;
        double dy = std::sin(turtle->getHeading()) * newSpeed * dt;

        double newX = turtle->getLocation().x + dx;
        double newY = turtle->getLocation().y + dy;

        // Apply toroidal wrapping if enabled
        if (env.toroidal()) {
          newX = std::fmod(newX, env.width());
          if (newX < 0) newX += env.width();
          newY = std::fmod(newY, env.height());
          if (newY < 0) newY += env.height();
        }

        turtle->setLocation(Point2D(newX, newY));
      }
    }
    // PheromoneFieldUpdate (natural influence)
    else if (auto pfu = std::dynamic_pointer_cast<PheromoneFieldUpdate>(influence)) {
      // Run pheromone diffusion and evaporation
      env.diffuse_and_evaporate(dt);
    }
  }

  // Natural pheromone dynamics
  env.diffuse_and_evaporate(dt);
}

} // namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::reaction
