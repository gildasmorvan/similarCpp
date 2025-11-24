#include "kernel/environment/Environment.h"
#include "kernel/model/environment/Pheromone.h"
#include "kernel/tools/MathUtil.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::environment {

using namespace fr::univ_artois::lgi2a::similar::similar2logo::kernel::model::
    environment; // for Pheromone
namespace tools = fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools;

static std::mt19937 rng{std::random_device{}()};

Environment::Environment(int w, int h, bool tor)
    : m_width(w), m_height(h), m_toroidal(tor) {}

Pheromone &Environment::add_pheromone(const std::string &id, double diffusion,
                                      double evaporation, double default_val,
                                      double min_val) {
  Pheromone pher(id, diffusion, evaporation, default_val, min_val);
  m_pheromones.insert_or_assign(id, pher);

  m_pheromone_grids[id] = std::vector<std::vector<double>>(
      m_height, std::vector<double>(m_width, default_val));

  return m_pheromones.at(id);
}

void Environment::set_pheromone(double x, double y, const std::string &id,
                                double value) {
  int ix = static_cast<int>(std::round(x));
  int iy = static_cast<int>(std::round(y));
  if (m_toroidal) {
    ix = (ix % m_width + m_width) % m_width;
    iy = (iy % m_height + m_height) % m_height;
  } else {
    ix = std::clamp(ix, 0, m_width - 1);
    iy = std::clamp(iy, 0, m_height - 1);
  }

  if (m_pheromone_grids.find(id) != m_pheromone_grids.end()) {
    m_pheromone_grids.at(id)[iy][ix] = value;
  }
}

double Environment::get_pheromone_value(double x, double y,
                                        const std::string &id) const {
  int ix = static_cast<int>(std::round(x));
  int iy = static_cast<int>(std::round(y));
  if (m_toroidal) {
    ix = (ix % m_width + m_width) % m_width;
    iy = (iy % m_height + m_height) % m_height;
  } else {
    ix = std::clamp(ix, 0, m_width - 1);
    iy = std::clamp(iy, 0, m_height - 1);
  }
  auto it = m_pheromone_grids.find(id);
  if (it != m_pheromone_grids.end())
    return it->second[iy][ix];
  return 0.0;
}

void Environment::diffuse_and_evaporate(double dt) {
  for (auto &[id, pheromone] : m_pheromones) {
    auto &grid = m_pheromone_grids[id];

    // Diffusion
    if (pheromone.getDiffusionCoef() > 0) {
      std::vector<std::vector<double>> new_grid = grid; // Copy

      for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
          double current_value = grid[y][x];
          if (current_value <= 0)
            continue;

          // Get neighbors (8-connected)
          std::vector<std::pair<int, int>> neighbors;
          for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
              if (dx == 0 && dy == 0)
                continue;

              int nx = x + dx;
              int ny = y + dy;

              if (m_toroidal) {
                nx = (nx % m_width + m_width) % m_width;
                ny = (ny % m_height + m_height) % m_height;
              } else {
                if (nx < 0 || nx >= m_width || ny < 0 || ny >= m_height)
                  continue;
              }
              neighbors.push_back({nx, ny});
            }
          }

          if (neighbors.empty())
            continue;

          double diffusion_amount =
              pheromone.getDiffusionCoef() * current_value * dt;
          double amount_per_neighbor = diffusion_amount / neighbors.size();

          for (const auto &n : neighbors) {
            new_grid[n.second][n.first] += amount_per_neighbor;
          }
          new_grid[y][x] -= diffusion_amount;
        }
      }
      grid = new_grid;
    }

    // Evaporation
    if (pheromone.getEvaporationCoef() > 0) {
      for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
          double current_value = grid[y][x];
          double evaporation =
              pheromone.getEvaporationCoef() * current_value * dt;
          double new_value = current_value - evaporation;

          if (new_value < pheromone.getMinValue()) {
            new_value = 0.0;
          }
          grid[y][x] = new_value;
        }
      }
    }
  }
}

tools::Point2D Environment::random_position() const {
  std::uniform_real_distribution<double> dx(0.0, static_cast<double>(m_width));
  std::uniform_real_distribution<double> dy(0.0, static_cast<double>(m_height));
  return tools::Point2D(dx(rng), dy(rng));
}

double Environment::random_heading() const {
  std::uniform_real_distribution<double> dh(0.0, tools::MathUtil::TWO_PI);
  return dh(rng);
}

double Environment::get_distance(const tools::Point2D &a,
                                 const tools::Point2D &b) const {
  return tools::MathUtil::toroidalDistance(a, b, static_cast<double>(m_width),
                                           static_cast<double>(m_height),
                                           m_toroidal, m_toroidal);
}

double Environment::get_direction(const tools::Point2D &from,
                                  const tools::Point2D &to) const {
  double dx = to.x - from.x;
  double dy = to.y - from.y;
  if (m_toroidal) {
    if (std::abs(dx) > m_width / 2.0)
      dx = dx > 0 ? dx - m_width : dx + m_width;
    if (std::abs(dy) > m_height / 2.0)
      dy = dy > 0 ? dy - m_height : dy + m_height;
  }
  return std::atan2(dx, -dy);
}

} // namespace
  // fr::univ_artois::lgi2a::similar::similar2logo::kernel::environment
