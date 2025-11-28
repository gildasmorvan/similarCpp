#ifndef SIMILAR2LOGO_TURTLEPLSINLOGO_H
#define SIMILAR2LOGO_TURTLEPLSINLOGO_H

#include "../../tools/Point2D.h"
#include "SituatedEntity.h"
#include <memory>
#include <string>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {
namespace environment {

/**
 * The public local state of a turtle in the Logo environment.
 *
 * This mirrors the Java TurtlePLSInLogo public state closely:
 * - position (location)
 * - speed and acceleration
 * - heading/direction
 */
class TurtlePLSInLogo : public SituatedEntity {
private:
  ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D
      location;
  double heading;
  double speed;
  double acceleration;
  bool penDown;
  ::std::string color;

public:
  /**
   * Creates a new TurtlePLSInLogo.
   * @param location The initial location
   * @param heading The initial heading in radians
   * @param speed The initial speed
   * @param acceleration The initial acceleration
   * @param penDown Whether the pen is down
   * @param color The color of the turtle
   */
  TurtlePLSInLogo(const ::fr::univ_artois::lgi2a::similar::similar2logo::
                      kernel::tools::Point2D &location,
                  double heading, double speed, double acceleration,
                  bool penDown, const ::std::string &color)
      : location(location), heading(heading), speed(speed),
        acceleration(acceleration), penDown(penDown), color(color) {}

  virtual ~TurtlePLSInLogo() = default;

  // SituatedEntity interface
  ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D
  getLocation() const override {
    return location;
  }

  void setLocation(const ::fr::univ_artois::lgi2a::similar::similar2logo::
                       kernel::tools::Point2D &newLocation) {
    location = newLocation;
  }

  double getHeading() const { return heading; }
  void setHeading(double newHeading) { heading = newHeading; }

  double getSpeed() const { return speed; }
  void setSpeed(double newSpeed) { speed = newSpeed; }

  double getAcceleration() const { return acceleration; }
  void setAcceleration(double newAcceleration) {
    acceleration = newAcceleration;
  }

  bool isPenDown() const { return penDown; }
  void setPenDown(bool isDown) { penDown = isDown; }

  const ::std::string &getColor() const { return color; }
  void setColor(const ::std::string &newColor) { color = newColor; }

  // Clone method for deep copying
  std::shared_ptr<TurtlePLSInLogo> clone() const {
    return std::make_shared<TurtlePLSInLogo>(*this);
  }
};

} // namespace environment
} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

#endif // SIMILAR2LOGO_TURTLEPLSINLOGO_H
