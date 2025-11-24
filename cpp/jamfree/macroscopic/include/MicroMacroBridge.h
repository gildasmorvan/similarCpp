#ifndef JAMFREE_MACROSCOPIC_MODELS_MICRO_MACRO_BRIDGE_H
#define JAMFREE_MACROSCOPIC_MODELS_MICRO_MACRO_BRIDGE_H

#include "../../kernel/include/model/Lane.h"
#include "../../kernel/include/model/Vehicle.h"
#include "CTM.h"
#include "LWR.h"
#include <memory>
#include <vector>

namespace jamfree {
namespace macroscopic {
namespace models {

/**
 * @brief Bridge between microscopic and macroscopic traffic models.
 *
 * This class provides methods to:
 * 1. Extract macroscopic state from microscopic simulation
 * 2. Initialize microscopic simulation from macroscopic state
 * 3. Calibrate macroscopic parameters from microscopic data
 *
 * Philosophy: Seamless integration between scales, allowing hybrid simulation
 */
class MicroMacroBridge {
public:
  /**
   * @brief Extract density profile from microscopic lane.
   *
   * Converts vehicle positions to spatial density distribution.
   *
   * @param lane Lane with vehicles
   * @param num_cells Number of cells for discretization
   * @return Vector of densities (vehicles/m)
   */
  static std::vector<double>
  extractDensityProfile(const std::shared_ptr<kernel::model::Lane> &lane,
                        int num_cells) {
    std::vector<double> density(num_cells, 0.0);

    if (!lane) {
      return density;
    }

    double lane_length = lane->getLength();
    double cell_length = lane_length / num_cells;

    // Count vehicles in each cell
    const auto &vehicles = lane->getVehicles();
    for (const auto &vehicle : vehicles) {
      double pos = vehicle->getLanePosition();
      int cell_index = static_cast<int>(pos / cell_length);

      if (cell_index >= 0 && cell_index < num_cells) {
        density[cell_index] += 1.0 / cell_length;
      }
    }

    return density;
  }

  /**
   * @brief Extract flow profile from microscopic lane.
   *
   * @param lane Lane with vehicles
   * @param num_cells Number of cells
   * @return Vector of flows (vehicles/s)
   */
  static std::vector<double>
  extractFlowProfile(const std::shared_ptr<kernel::model::Lane> &lane,
                     int num_cells) {
    std::vector<double> flow(num_cells, 0.0);

    if (!lane) {
      return flow;
    }

    double lane_length = lane->getLength();
    double cell_length = lane_length / num_cells;

    // Calculate flow as density * speed in each cell
    const auto &vehicles = lane->getVehicles();
    std::vector<double> total_speed(num_cells, 0.0);
    std::vector<int> count(num_cells, 0);

    for (const auto &vehicle : vehicles) {
      double pos = vehicle->getLanePosition();
      int cell_index = static_cast<int>(pos / cell_length);

      if (cell_index >= 0 && cell_index < num_cells) {
        total_speed[cell_index] += vehicle->getSpeed();
        count[cell_index]++;
      }
    }

    // Calculate flow = density * average_speed
    for (int i = 0; i < num_cells; ++i) {
      if (count[i] > 0) {
        double density = count[i] / cell_length;
        double avg_speed = total_speed[i] / count[i];
        flow[i] = density * avg_speed;
      }
    }

    return flow;
  }

  /**
   * @brief Extract speed profile from microscopic lane.
   *
   * @param lane Lane with vehicles
   * @param num_cells Number of cells
   * @return Vector of average speeds (m/s)
   */
  static std::vector<double>
  extractSpeedProfile(const std::shared_ptr<kernel::model::Lane> &lane,
                      int num_cells) {
    std::vector<double> speed(num_cells, 0.0);

    if (!lane) {
      return speed;
    }

    double lane_length = lane->getLength();
    double cell_length = lane_length / num_cells;

    const auto &vehicles = lane->getVehicles();
    std::vector<double> total_speed(num_cells, 0.0);
    std::vector<int> count(num_cells, 0);

    for (const auto &vehicle : vehicles) {
      double pos = vehicle->getLanePosition();
      int cell_index = static_cast<int>(pos / cell_length);

      if (cell_index >= 0 && cell_index < num_cells) {
        total_speed[cell_index] += vehicle->getSpeed();
        count[cell_index]++;
      }
    }

    for (int i = 0; i < num_cells; ++i) {
      if (count[i] > 0) {
        speed[i] = total_speed[i] / count[i];
      }
    }

    return speed;
  }

  /**
   * @brief Initialize LWR model from microscopic lane state.
   *
   * @param lane Lane with vehicles
   * @param lwr LWR model to initialize
   */
  static void
  initializeLWRFromLane(const std::shared_ptr<kernel::model::Lane> &lane,
                        LWR &lwr) {
    int num_cells = lwr.getNumCells();
    auto density_profile = extractDensityProfile(lane, num_cells);

    for (int i = 0; i < num_cells; ++i) {
      lwr.setDensity(i, density_profile[i]);
    }
  }

  /**
   * @brief Initialize CTM model from microscopic lane state.
   *
   * @param lane Lane with vehicles
   * @param ctm CTM model to initialize
   */
  static void
  initializeCTMFromLane(const std::shared_ptr<kernel::model::Lane> &lane,
                        CTM &ctm) {
    int num_cells = ctm.getNumCells();
    double cell_length = ctm.getCellLength();

    auto density_profile = extractDensityProfile(lane, num_cells);

    for (int i = 0; i < num_cells; ++i) {
      double num_vehicles = density_profile[i] * cell_length;
      ctm.setNumVehicles(i, num_vehicles);
    }
  }

  /**
   * @brief Calibrate fundamental diagram from microscopic data.
   *
   * Extracts speed-density relationship from microscopic simulation.
   *
   * @param lane Lane with vehicles
   * @param num_samples Number of samples
   * @return Pair of (densities, speeds) vectors
   */
  static std::pair<std::vector<double>, std::vector<double>>
  calibrateFundamentalDiagram(const std::shared_ptr<kernel::model::Lane> &lane,
                              int num_samples = 50) {
    std::vector<double> densities;
    std::vector<double> speeds;

    if (!lane) {
      return {densities, speeds};
    }

    int num_cells = num_samples;
    auto density_profile = extractDensityProfile(lane, num_cells);
    auto speed_profile = extractSpeedProfile(lane, num_cells);

    for (int i = 0; i < num_cells; ++i) {
      if (density_profile[i] > 0.0) {
        densities.push_back(density_profile[i]);
        speeds.push_back(speed_profile[i]);
      }
    }

    return {densities, speeds};
  }

  /**
   * @brief Estimate free-flow speed from microscopic data.
   *
   * @param lane Lane with vehicles
   * @return Estimated free-flow speed (m/s)
   */
  static double
  estimateFreeFlowSpeed(const std::shared_ptr<kernel::model::Lane> &lane) {
    if (!lane) {
      return 0.0;
    }

    const auto &vehicles = lane->getVehicles();
    if (vehicles.empty()) {
      return lane->getSpeedLimit();
    }

    // Find maximum speed (approximation of free-flow)
    double max_speed = 0.0;
    for (const auto &vehicle : vehicles) {
      max_speed = std::max(max_speed, vehicle->getSpeed());
    }

    return max_speed;
  }

  /**
   * @brief Estimate jam density from microscopic data.
   *
   * @param lane Lane with vehicles
   * @param vehicle_length Average vehicle length (m)
   * @return Estimated jam density (vehicles/m)
   */
  static double
  estimateJamDensity(const std::shared_ptr<kernel::model::Lane> &lane,
                     double vehicle_length = 5.0) {
    // Theoretical jam density: 1 / (vehicle_length + min_gap)
    double min_gap = 2.0; // Typical minimum gap
    return 1.0 / (vehicle_length + min_gap);
  }

  /**
   * @brief Calculate aggregate statistics from microscopic simulation.
   *
   * @param lane Lane with vehicles
   * @return Struct with average density, flow, and speed
   */
  struct AggregateStats {
    double avg_density; // vehicles/m
    double avg_flow;    // vehicles/s
    double avg_speed;   // m/s
    int num_vehicles;   // total vehicles
  };

  static AggregateStats
  calculateAggregateStats(const std::shared_ptr<kernel::model::Lane> &lane) {
    AggregateStats stats{0.0, 0.0, 0.0, 0};

    if (!lane) {
      return stats;
    }

    const auto &vehicles = lane->getVehicles();
    stats.num_vehicles = vehicles.size();

    if (vehicles.empty()) {
      return stats;
    }

    // Calculate average density
    double lane_length = lane->getLength();
    stats.avg_density = stats.num_vehicles / lane_length;

    // Calculate average speed
    double total_speed = 0.0;
    for (const auto &vehicle : vehicles) {
      total_speed += vehicle->getSpeed();
    }
    stats.avg_speed = total_speed / stats.num_vehicles;

    // Calculate flow
    stats.avg_flow = stats.avg_density * stats.avg_speed;

    return stats;
  }
};

} // namespace models
} // namespace macroscopic
} // namespace jamfree

#endif // JAMFREE_MACROSCOPIC_MODELS_MICRO_MACRO_BRIDGE_H
