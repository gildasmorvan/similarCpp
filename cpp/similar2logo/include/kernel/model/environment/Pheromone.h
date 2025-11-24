#ifndef SIMILAR2LOGO_PHEROMONE_H
#define SIMILAR2LOGO_PHEROMONE_H

#include <stdexcept>
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
 * Represents a pheromone in the Logo environment.
 * Pheromones have diffusion and evaporation properties.
 */
class Pheromone {
private:
  std::string identifier;
  double diffusionCoef;
  double evaporationCoef;
  double defaultValue;
  double minValue;

public:
  /**
   * Builds a pheromone with all parameters.
   * @param identifier Unique identifier for the pheromone
   * @param diffusionCoef Diffusion coefficient (>= 0)
   * @param evaporationCoef Evaporation coefficient (>= 0)
   * @param defaultValue Initial amount in environment
   * @param minValue Minimum value the pheromone can take
   */
  Pheromone(const std::string &identifier, double diffusionCoef,
            double evaporationCoef, double defaultValue, double minValue)
      : identifier(identifier), diffusionCoef(diffusionCoef),
        evaporationCoef(evaporationCoef), defaultValue(defaultValue),
        minValue(minValue) {
    if (identifier.empty()) {
      throw std::invalid_argument("Pheromone identifier cannot be empty");
    }
    if (diffusionCoef < 0 || evaporationCoef < 0 || defaultValue < 0) {
      throw std::invalid_argument(
          "Diffusion, evaporation coefficients and default value must be >= 0");
    }
  }

  /**
   * Builds a pheromone with default and min values set to 0.
   */
  Pheromone(const std::string &identifier, double diffusionCoef,
            double evaporationCoef)
      : Pheromone(identifier, diffusionCoef, evaporationCoef, 0.0, 0.0) {}

  /**
   * Builds a pheromone with min value set to 0.
   */
  Pheromone(const std::string &identifier, double diffusionCoef,
            double evaporationCoef, double defaultValue)
      : Pheromone(identifier, diffusionCoef, evaporationCoef, defaultValue,
                  0.0) {}

  // Getters
  const std::string &getIdentifier() const { return identifier; }
  double getDiffusionCoef() const { return diffusionCoef; }
  double getEvaporationCoef() const { return evaporationCoef; }
  double getDefaultValue() const { return defaultValue; }
  double getMinValue() const { return minValue; }

  // Comparison operators
  bool operator==(const Pheromone &other) const {
    return identifier == other.identifier;
  }

  bool operator!=(const Pheromone &other) const { return !(*this == other); }

  bool operator<(const Pheromone &other) const {
    return identifier < other.identifier;
  }

  // String representation
  std::string toString() const { return identifier; }
};

} // namespace environment
} // namespace model
} // namespace kernel
} // namespace similar2logo
} // namespace similar
} // namespace lgi2a
} // namespace univ_artois
} // namespace fr

// Hash function for use in unordered containers
namespace std {
template <>
struct hash<fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::
                environment::Pheromone> {
  size_t operator()(const fr::univ_artois::lgi2a::similar::similar2logo::
                        kernel::model::environment::Pheromone &p) const {
    return std::hash<std::string>{}(p.getIdentifier());
  }
};
} // namespace std

#endif // SIMILAR2LOGO_PHEROMONE_H
