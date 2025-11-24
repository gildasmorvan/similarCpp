#ifndef JAMFREE_HYBRID_ADAPTIVE_SIMULATOR_H
#define JAMFREE_HYBRID_ADAPTIVE_SIMULATOR_H

#include "../../kernel/include/model/Lane.h"
#include "../../kernel/include/model/Road.h"
#include "../../kernel/include/model/Vehicle.h"
#include "../../macroscopic/include/LWR.h"
#include "../../macroscopic/include/MicroMacroBridge.h"
#include "../../microscopic/include/IDM.h"
#include <memory>
#include <unordered_map>
#include <vector>

namespace jamfree {
namespace hybrid {

/**
 * @brief Simulation mode for a lane.
 */
enum class SimulationMode {
  MICROSCOPIC,  ///< Individual vehicle tracking
  MACROSCOPIC,  ///< Continuum flow model
  TRANSITIONING ///< Switching between modes
};

/**
 * @brief Adaptive hybrid simulator.
 *
 * Automatically switches between microscopic and macroscopic simulation
 * based on vehicle density and computational load.
 *
 * Philosophy:
 * - Use microscopic where detail matters (low density, critical areas)
 * - Use macroscopic where flow matters (high density, highways)
 * - Seamless transitions preserve traffic state
 */
class AdaptiveSimulator {
public:
  /**
   * @brief Configuration for adaptive simulation.
   */
  struct Config {
    // Density thresholds
    double micro_to_macro_density = 0.08; ///< Switch to macro (80 veh/km)
    double macro_to_micro_density = 0.04; ///< Switch to micro (40 veh/km)

    // Vehicle count thresholds
    int micro_to_macro_count = 50; ///< Switch to macro at N vehicles
    int macro_to_micro_count = 20; ///< Switch to micro at N vehicles

    // Computational thresholds
    double max_micro_time_ms = 10.0; ///< Max time for micro update

    // Macroscopic discretization
    int macro_num_cells = 50; ///< Cells for macroscopic model

    // Hysteresis to prevent oscillation
    double hysteresis_factor = 1.2; ///< Prevent rapid switching

    // Critical areas (always microscopic)
    bool force_micro_intersections = true;
    bool force_micro_ramps = true;

    // Explicit default constructor
    Config() = default;
  };

  /**
   * @brief Stored vehicle data for micro-macro transitions.
   *
   * Preserves individual vehicle properties during macroscopic simulation
   * to allow perfect reconstruction when switching back to microscopic.
   */
  struct VehicleData {
    std::string id;
    double position;     // Lane position (m)
    double speed;        // Speed (m/s)
    double acceleration; // Acceleration (m/s²)
    double length;       // Vehicle length (m)
    // Can add more: route, destination, driver characteristics, etc.
  };

  /**
   * @brief Lane simulation state.
   */
  struct LaneState {
    std::shared_ptr<kernel::model::Lane> lane;
    SimulationMode mode;

    // Microscopic state
    std::vector<std::shared_ptr<kernel::model::Vehicle>> vehicles;

    // Macroscopic state
    std::unique_ptr<macroscopic::models::LWR> lwr_model;

    // Vehicle data preservation (stored during macro mode)
    std::vector<VehicleData> stored_vehicle_data;

    // Metrics
    double current_density;
    double avg_speed;
    double flow;
    int vehicle_count;
    double last_update_time_ms;

    // Transition state
    bool is_critical_area;
    int frames_since_transition;
    bool force_mode; // If true, don't auto-switch
  };

  /**
   * @brief Default constructor with default configuration.
   */
  AdaptiveSimulator() : m_config() {}

  /**
   * @brief Constructor with custom configuration.
   *
   * @param config Configuration parameters
   */
  explicit AdaptiveSimulator(const Config &config) : m_config(config) {}

  /**
   * @brief Register a lane for adaptive simulation.
   *
   * @param lane Lane to simulate
   * @param is_critical True if lane is in critical area (intersection, ramp)
   */
  void registerLane(const std::shared_ptr<kernel::model::Lane> &lane,
                    bool is_critical = false);

  /**
   * @brief Update all lanes for one time step.
   *
   * Automatically switches between micro/macro as needed.
   *
   * @param dt Time step (seconds)
   * @param idm IDM model for microscopic simulation
   */
  void update(double dt, const microscopic::models::IDM &idm);

  /**
   * @brief Get current mode for a lane.
   *
   * @param lane_id Lane identifier
   * @return Current simulation mode
   */
  SimulationMode getMode(const std::string &lane_id) const;

  /**
   * @brief Get statistics for a lane.
   *
   * @param lane_id Lane identifier
   * @return Lane state with metrics
   */
  const LaneState *getLaneState(const std::string &lane_id) const;

  /**
   * @brief Get overall statistics.
   *
   * @return Struct with aggregate statistics
   */
  struct Statistics {
    int total_lanes;
    int micro_lanes;
    int macro_lanes;
    int transitioning_lanes;
    int total_vehicles;
    double avg_density;
    double total_update_time_ms;
    double speedup_factor;
  };

  Statistics getStatistics() const;

  /**
   * @brief Force a lane to microscopic mode.
   *
   * @param lane_id Lane identifier
   */
  void forceMicroscopic(const std::string &lane_id);

  /**
   * @brief Force a lane to macroscopic mode.
   *
   * @param lane_id Lane identifier
   */
  void forceMacroscopic(const std::string &lane_id);

  /**
   * @brief Allow automatic mode switching.
   *
   * @param lane_id Lane identifier
   */
  void allowAutomatic(const std::string &lane_id);

private:
  Config m_config;
  std::unordered_map<std::string, LaneState> m_lane_states;

  /**
   * @brief Evaluate if lane should switch modes.
   *
   * @param state Lane state
   * @return True if mode switch recommended
   */
  bool shouldSwitchMode(LaneState &state);

  /**
   * @brief Transition from microscopic to macroscopic.
   *
   * @param state Lane state
   */
  void transitionToMacro(LaneState &state);

  /**
   * @brief Transition from macroscopic to microscopic.
   *
   * @param state Lane state
   */
  void transitionToMicro(LaneState &state);

  /**
   * @brief Update lane in microscopic mode.
   *
   * @param state Lane state
   * @param dt Time step
   * @param idm IDM model
   */
  void updateMicroscopic(LaneState &state, double dt,
                         const microscopic::models::IDM &idm);

  /**
   * @brief Update lane in macroscopic mode.
   *
   * @param state Lane state
   * @param dt Time step
   */
  void updateMacroscopic(LaneState &state, double dt);

  /**
   * @brief Update lane metrics.
   *
   * @param state Lane state
   */
  void updateMetrics(LaneState &state);

  /**
   * @brief Check if lane is in critical area.
   *
   * @param lane Lane to check
   * @return True if critical
   */
  bool isCriticalArea(const std::shared_ptr<kernel::model::Lane> &lane) const;
};

} // namespace hybrid
} // namespace jamfree

#endif // JAMFREE_HYBRID_ADAPTIVE_SIMULATOR_H
