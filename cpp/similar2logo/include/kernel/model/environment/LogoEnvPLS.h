#ifndef SIMILAR2LOGO_LOGOENVPLS_H
#define SIMILAR2LOGO_LOGOENVPLS_H

#include "../../../../../microkernel/include/LevelIdentifier.h"
#include "../../../../../microkernel/include/libs/abstractimpl/AbstractLocalStateOfEnvironment.h"
#include "../../tools/MathUtil.h"
#include "../../tools/Point2D.h"
#include "Mark.h"
#include "Pheromone.h"
#include "TurtlePLSInLogo.h"
#include <cmath>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fr {
namespace univ_artois {
namespace lgi2a {
namespace similar {
namespace similar2logo {
namespace kernel {
namespace model {
namespace environment {

// Forward declaration for turtle (will be implemented later)
class TurtlePLSInLogo;

/**
 * Models the public local state of the Logo environment.
 * Contains:
 * - Grid structure with patches
 * - Pheromone fields
 * - Marks dropped by agents
 * - Turtle positions
 */
class LogoEnvPLS : public ::fr::univ_artois::lgi2a::similar::microkernel::libs::
                       abstractimpl::AbstractLocalStateOfEnvironment {
public:
  // Direction constants (in radians)
  static constexpr double NORTH = 0.0;
  static constexpr double NORTH_EAST =
      -::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::
          MathUtil::PI /
      4.0;
  static constexpr double EAST = -::fr::univ_artois::lgi2a::similar::
                                     similar2logo::kernel::tools::MathUtil::PI /
                                 2.0;
  static constexpr double SOUTH_EAST =
      -3.0 *
      ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::MathUtil::
          PI /
      4.0;
  static constexpr double SOUTH = -::fr::univ_artois::lgi2a::similar::
                                      similar2logo::kernel::tools::MathUtil::PI;
  static constexpr double SOUTH_WEST =
      3.0 *
      ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::MathUtil::
          PI /
      4.0;
  static constexpr double WEST = ::fr::univ_artois::lgi2a::similar::
                                     similar2logo::kernel::tools::MathUtil::PI /
                                 2.0;
  static constexpr double NORTH_WEST =
      ::fr::univ_artois::lgi2a::similar::similar2logo::kernel::tools::MathUtil::
          PI /
      4.0;

private:
  int width;
  int height;
  bool xAxisTorus;
  bool yAxisTorus;

  // Pheromone field: map from pheromone to 2D grid of values
  std::unordered_map<Pheromone, std::vector<std::vector<double>>>
      pheromoneField;

  // Marks in each patch
  std::vector<std::vector<std::unordered_set<std::shared_ptr<SimpleMark>>>>
      marks;

  // Turtles in each patch
  std::vector<std::vector<std::unordered_set<std::shared_ptr<TurtlePLSInLogo>>>>
      turtlesInPatches;

public:
  /**
   * Builds an initialized instance with pheromones.
   * @param levelIdentifier The identifier of the level
   * @param gridWidth The width of the grid
   * @param gridHeight The height of the grid
   * @param xAxisTorus True if environment is toroidal along x axis
   * @param yAxisTorus True if environment is toroidal along y axis
   * @param pheromones The set of pheromone fields
   */
  LogoEnvPLS(
      const ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &levelIdentifier,
      int gridWidth, int gridHeight, bool xAxisTorus, bool yAxisTorus,
      const std::unordered_set<Pheromone> &pheromones)
      : ::fr::univ_artois::lgi2a::similar::microkernel::libs::abstractimpl::
            AbstractLocalStateOfEnvironment(levelIdentifier),
        width(gridWidth), height(gridHeight), xAxisTorus(xAxisTorus),
        yAxisTorus(yAxisTorus) {

    // Initialize pheromone fields
    for (const auto &pheromone : pheromones) {
      std::vector<std::vector<double>> field(
          width, std::vector<double>(height, pheromone.getDefaultValue()));
      pheromoneField[pheromone] = field;
    }

    // Initialize marks grid
    marks.resize(width);
    for (int x = 0; x < width; x++) {
      marks[x].resize(height);
    }

    // Initialize turtles grid
    turtlesInPatches.resize(width);
    for (int x = 0; x < width; x++) {
      turtlesInPatches[x].resize(height);
    }
  }

  virtual ~LogoEnvPLS() = default;

  // Grid properties
  int getWidth() const { return width; }
  int getHeight() const { return height; }
  bool isXAxisTorus() const { return xAxisTorus; }
  bool isYAxisTorus() const { return yAxisTorus; }

  /**
   * Computes the positions of the neighbors of a patch.
   * @param x The x coordinate of the patch
   * @param y The y coordinate of the patch
   * @param distance The maximal distance of neighbors
   * @return The positions of the patch neighbors
   */
  std::vector<kernel::tools::Point2D> getNeighbors(int x, int y,
                                                   int distance) const {
    std::vector<kernel::tools::Point2D> neighbors;

    for (int dx = -distance; dx <= distance; dx++) {
      for (int dy = -distance; dy <= distance; dy++) {
        int nx = x + dx;
        int ny = y + dy;

        // Handle toroidal wrapping
        if (xAxisTorus) {
          nx = ((nx % width) + width) % width;
        }
        if (yAxisTorus) {
          ny = ((ny % height) + height) % height;
        }

        // Check if within bounds
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
          neighbors.push_back(kernel::tools::Point2D(nx, ny));
        }
      }
    }

    return neighbors;
  }

  /**
   * Computes the direction from one point to another, considering toroidal
   * topology.
   * @param from The starting point
   * @param to The target point
   * @return The direction in radians
   */
  double getDirection(const kernel::tools::Point2D &from,
                      const kernel::tools::Point2D &to) const {
    double xtarget = to.x;
    double ytarget = to.y;

    // Handle toroidal wrapping for shortest path
    if (xAxisTorus && std::abs(xtarget - from.x) * 2 > width) {
      if (from.x > xtarget) {
        xtarget += width;
      } else {
        xtarget -= width;
      }
    }

    if (yAxisTorus && std::abs(ytarget - from.y) * 2 > height) {
      if (from.y > ytarget) {
        ytarget += height;
      } else {
        ytarget -= height;
      }
    }

    return -std::atan2(xtarget - from.x, ytarget - from.y);
  }

  /**
   * Computes the distance between two points, considering toroidal topology.
   * @param loc1 The first point
   * @param loc2 The second point
   * @return The distance
   */
  double getDistance(const kernel::tools::Point2D &loc1,
                     const kernel::tools::Point2D &loc2) const {
    double dx = std::abs(loc1.x - loc2.x);
    double dy = std::abs(loc1.y - loc2.y);

    // Handle toroidal wrapping for shortest distance
    if (xAxisTorus && dx * 2 > width) {
      dx = width - dx;
    }
    if (yAxisTorus && dy * 2 > height) {
      dy = height - dy;
    }

    return std::sqrt(dx * dx + dy * dy);
  }

  // Pheromone field access
  double getPheromoneValueAt(const Pheromone &pheromone, int x, int y) const {
    auto it = pheromoneField.find(pheromone);
    if (it != pheromoneField.end()) {
      return it->second[x][y];
    }
    return 0.0;
  }

  double getPheromoneValueAt(const Pheromone &pheromone, double x,
                             double y) const {
    return getPheromoneValueAt(pheromone, static_cast<int>(x),
                               static_cast<int>(y));
  }

  void setPheromoneValueAt(const Pheromone &pheromone, int x, int y,
                           double value) {
    auto it = pheromoneField.find(pheromone);
    if (it != pheromoneField.end()) {
      it->second[x][y] = value;
    }
  }

  const std::vector<std::vector<double>> &
  getPheromoneValues(const Pheromone &pheromone) const {
    static const std::vector<std::vector<double>> empty;
    auto it = pheromoneField.find(pheromone);
    if (it != pheromoneField.end()) {
      return it->second;
    }
    return empty;
  }

  std::vector<std::vector<double>> &
  getPheromoneValues(const Pheromone &pheromone) {
    return pheromoneField[pheromone];
  }

  const std::unordered_map<Pheromone, std::vector<std::vector<double>>> &
  getPheromoneField() const {
    return pheromoneField;
  }

  std::unordered_map<Pheromone, std::vector<std::vector<double>>> &
  getPheromoneField() {
    return pheromoneField;
  }

  // Mark access
  const std::unordered_set<std::shared_ptr<SimpleMark>> &
  getMarksAt(int x, int y) const {
    return marks[x][y];
  }

  const std::unordered_set<std::shared_ptr<SimpleMark>> &
  getMarksAt(const kernel::tools::Point2D &position) const {
    return marks[static_cast<int>(position.x)][static_cast<int>(position.y)];
  }

  std::unordered_set<std::shared_ptr<SimpleMark>> getMarksAt(double x,
                                                             double y) const {
    return marks[static_cast<int>(x)][static_cast<int>(y)];
  }

  std::unordered_set<std::shared_ptr<SimpleMark>> getAllMarks() const {
    std::unordered_set<std::shared_ptr<SimpleMark>> allMarks;
    for (const auto &row : marks) {
      for (const auto &cell : row) {
        allMarks.insert(cell.begin(), cell.end());
      }
    }
    return allMarks;
  }

  void addMark(std::shared_ptr<SimpleMark> mark) {
    auto loc = mark->getLocation();
    int x = static_cast<int>(loc.x);
    int y = static_cast<int>(loc.y);
    if (x >= 0 && x < width && y >= 0 && y < height) {
      marks[x][y].insert(mark);
    }
  }

  void removeMark(std::shared_ptr<SimpleMark> mark) {
    auto loc = mark->getLocation();
    int x = static_cast<int>(loc.x);
    int y = static_cast<int>(loc.y);
    if (x >= 0 && x < width && y >= 0 && y < height) {
      marks[x][y].erase(mark);
    }
  }

  /**
   * Private constructor for cloning.
   */
  LogoEnvPLS(
      const ::fr::univ_artois::lgi2a::similar::microkernel::LevelIdentifier
          &levelIdentifier,
      int gridWidth, int gridHeight, bool xAxisTorus, bool yAxisTorus,
      const std::unordered_map<Pheromone, std::vector<std::vector<double>>>
          &pheromoneField,
      const std::vector<
          std::vector<std::unordered_set<std::shared_ptr<SimpleMark>>>> &marks,
      const std::vector<
          std::vector<std::unordered_set<std::shared_ptr<TurtlePLSInLogo>>>>
          &turtlesInPatches)
      : ::fr::univ_artois::lgi2a::similar::microkernel::libs::abstractimpl::
            AbstractLocalStateOfEnvironment(levelIdentifier),
        width(gridWidth), height(gridHeight), xAxisTorus(xAxisTorus),
        yAxisTorus(yAxisTorus), pheromoneField(pheromoneField), marks(marks),
        turtlesInPatches(turtlesInPatches) {}

public:
  /**
   * Creates a deep copy of this environment state.
   * @return A new LogoEnvPLS with copied data
   */
  std::shared_ptr<LogoEnvPLS> clone() const {
    // Deep copy marks
    std::vector<std::vector<std::unordered_set<std::shared_ptr<SimpleMark>>>>
        marksCopy = marks;
    for (int x = 0; x < width; x++) {
      for (int y = 0; y < height; y++) {
        marksCopy[x][y].clear();
        for (const auto &mark : marks[x][y]) {
          marksCopy[x][y].insert(mark->clone());
        }
      }
    }

    // Turtles are shared pointers, shallow copy of the set is fine if we want
    // to share the same turtle PLS objects But if we want a snapshot, we might
    // need deep copy? Usually PLS are snapshots. But TurtlePLSInLogo is a state
    // object. For now, let's keep it as shared pointers to the same objects
    // (shallow copy of the list). Actually, if LogoEnvPLS is cloned for a new
    // time step or for an agent's perception, it should probably reference the
    // SAME turtle states if they haven't changed, or new ones if they have. But
    // here we are just cloning the environment state.

    return std::shared_ptr<LogoEnvPLS>(
        new LogoEnvPLS(getLevel(), width, height, xAxisTorus, yAxisTorus,
                       pheromoneField, marksCopy, turtlesInPatches));
  }

  // Turtle access
  const std::vector<
      std::vector<std::unordered_set<std::shared_ptr<TurtlePLSInLogo>>>> &
  getTurtlesInPatches() const {
    return turtlesInPatches;
  }

  std::vector<
      std::vector<std::unordered_set<std::shared_ptr<TurtlePLSInLogo>>>> &
  getTurtlesInPatches() {
    return turtlesInPatches;
  }

  std::unordered_set<std::shared_ptr<TurtlePLSInLogo>>
  getTurtlesAt(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
      return turtlesInPatches[x][y];
    }
    return {};
  }

  std::unordered_set<std::shared_ptr<TurtlePLSInLogo>>
  getTurtlesAt(const kernel::tools::Point2D &position) const {
    return getTurtlesAt(static_cast<int>(position.x),
                        static_cast<int>(position.y));
  }

  std::unordered_set<std::shared_ptr<TurtlePLSInLogo>> getAllTurtles() const {
    std::unordered_set<std::shared_ptr<TurtlePLSInLogo>> allTurtles;
    for (const auto &row : turtlesInPatches) {
      for (const auto &cell : row) {
        allTurtles.insert(cell.begin(), cell.end());
      }
    }
    return allTurtles;
  }

  // Mark access (direct grid access)
  const std::vector<
      std::vector<std::unordered_set<std::shared_ptr<SimpleMark>>>> &
  getMarks() const {
    return marks;
  }

  std::vector<std::vector<std::unordered_set<std::shared_ptr<SimpleMark>>>> &
  getMarks() {
    return marks;
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

#endif // SIMILAR2LOGO_LOGOENVPLS_H
