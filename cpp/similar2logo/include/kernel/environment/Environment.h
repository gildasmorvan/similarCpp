#pragma once
#include "kernel/tools/Point2D.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::
    environment {
class Pheromone;
}

namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::environment {

namespace tools = fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools;
using Pheromone = fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::
    environment::Pheromone;

class Environment {
public:
  Environment(int width, int height, bool toroidal = false);

  // pheromone handling ---------------------------------------------------
  Pheromone &add_pheromone(const std::string &identifier,
                           double diffusion_coef = 0.0,
                           double evaporation_coef = 0.0,
                           double default_value = 0.0, double min_value = 0.0);
  void set_pheromone(double x, double y, const std::string &identifier,
                     double value);
  double get_pheromone_value(double x, double y,
                             const std::string &identifier) const;

  void diffuse_and_evaporate(double dt);

  // random helpers ------------------------------------------------------
  tools::Point2D random_position() const;
  double random_heading() const;

  // geometry ------------------------------------------------------------
  double get_distance(const tools::Point2D &a, const tools::Point2D &b) const;
  double get_direction(const tools::Point2D &from,
                       const tools::Point2D &to) const;

  // public getters ------------------------------------------------------
  int width() const { return m_width; }
  int height() const { return m_height; }
  bool toroidal() const { return m_toroidal; }

private:
  int m_width, m_height;
  bool m_toroidal;
  std::unordered_map<std::string, std::vector<std::vector<double>>>
      m_pheromone_grids;
  std::unordered_map<std::string, Pheromone> m_pheromones;
};
} // namespace
  // fr::univ_artois::lgi2a::similar::similar2logo::kernel::environment
