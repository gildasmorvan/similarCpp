#ifndef SIMILAR2LOGO_TURTLEPLSINLOGO_H
#define SIMILAR2LOGO_TURTLEPLSINLOGO_H

#include "../../tools/Point2D.h"
#include "SituatedEntity.h"
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
 * Contains public information about the turtle such as its position and
 * heading.
 */
class TurtlePLSInLogo : public SituatedEntity {
private:
  ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::Point2D
      location;
  double heading;
  double speed;
  bool penDown;
  std::string color;

public:
  /**
   * Creates a new TurtlePLSInLogo.
   * @param location The initial location
   * @param heading The initial heading in radians
   * @param penDown Whether the pen is down
   * @param color The color of the turtle
   */
  TurtlePLSInLogo(const ::fr::univ_artois::lgi2a::similar::similar2logo::
                      kernel::tools::Point2D &location,
                  double heading, bool penDown, const std::string &color)
      : location(location), heading(heading), penDown(penDown), color(color) {}

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

  bool isPenDown() const { return penDown; }
  void setPenDown(bool isDown) { penDown = isDown; }

  const std::string &getColor() const { return color; }
  void setColor(const std::string &newColor) { color = newColor; }
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
