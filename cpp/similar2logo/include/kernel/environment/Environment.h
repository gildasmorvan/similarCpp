#pragma once
#include "kernel/model/environment/Mark.h"
#include "kernel/model/environment/TurtlePLSInLogo.h"
#include "kernel/tools/Point2D.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::
    environment {
class Pheromone;
class TurtlePLSInLogo;
} // namespace
  // fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::environment

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

  // mark handling ------------------------------------------------------
  void add_mark(int x, int y,
                std::shared_ptr<model::environment::SimpleMark> mark);
  void remove_mark(int x, int y,
                   std::shared_ptr<model::environment::SimpleMark> mark);

  // turtle access ------------------------------------------------------
  const std::vector<std::shared_ptr<model::environment::TurtlePLSInLogo>> &
  get_turtles() const;
  void add_turtle(std::shared_ptr<model::environment::TurtlePLSInLogo> turtle);
  void
  remove_turtle(std::shared_ptr<model::environment::TurtlePLSInLogo> turtle);

  // Spatial indexing for efficient turtle queries
  const std::unordered_set<
      std::shared_ptr<model::environment::TurtlePLSInLogo>> &
  get_turtles_at(int x, int y) const;

  void update_turtle_patch(
      std::shared_ptr<model::environment::TurtlePLSInLogo> turtle, int old_x,
      int old_y, int new_x, int new_y);

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

  // marks grid: marks[x][y] = set of marks at that location
  std::vector<std::vector<
      std::unordered_set<std::shared_ptr<model::environment::SimpleMark>>>>
      m_marks;

  // turtles list
  std::vector<std::shared_ptr<model::environment::TurtlePLSInLogo>> m_turtles;

  // Spatial index: turtles_in_patches[x][y] = set of turtles at that patch
  std::vector<std::vector<
      std::unordered_set<std::shared_ptr<model::environment::TurtlePLSInLogo>>>>
      m_turtles_in_patches;

  // Empty set for out-of-bounds queries
  static const std::unordered_set<
      std::shared_ptr<model::environment::TurtlePLSInLogo>>
      s_empty_turtle_set;
};
} // namespace
  // fr::univ_artois::lgi2a::similar::similar2logo::kernel::environment
