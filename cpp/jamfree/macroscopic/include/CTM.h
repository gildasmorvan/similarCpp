#ifndef JAMFREE_MACROSCOPIC_MODELS_CTM_H
#define JAMFREE_MACROSCOPIC_MODELS_CTM_H

#include "../../kernel/include/tools/MathTools.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace jamfree {
namespace macroscopic {
namespace models {

/**
 * @brief Cell Transmission Model (CTM).
 *
 * The CTM is a discrete version of the LWR model that explicitly represents
 * traffic flow between cells. It's particularly useful for network simulation
 * and captures shockwaves and rarefaction waves naturally.
 *
 * Reference: Daganzo, C. F. (1994).
 * "The cell transmission model: A dynamic representation of highway traffic
 * consistent with the hydrodynamic theory."
 */
class CTM {
public:
  /**
   * @brief Constructor with parameters.
   *
   * @param free_flow_speed Maximum speed (m/s)
   * @param wave_speed Backward wave speed (m/s)
   * @param jam_density Maximum density (vehicles/m)
   * @param road_length Length of road segment (m)
   * @param num_cells Number of cells
   */
  CTM(double free_flow_speed = 33.3, // 120 km/h
      double wave_speed = 5.56,      // 20 km/h
      double jam_density = 0.15,     // 150 vehicles/km
      double road_length = 1000.0,   // 1 km
      int num_cells = 100)
      : m_free_flow_speed(free_flow_speed), m_wave_speed(wave_speed),
        m_jam_density(jam_density), m_road_length(road_length),
        m_num_cells(num_cells), m_cell_length(road_length / num_cells) {
    // Initialize state
    m_num_vehicles.resize(num_cells, 0.0);
    m_num_vehicles_new.resize(num_cells, 0.0);

    // Calculate derived parameters
    m_critical_density = calculateCriticalDensity();
    m_max_flow = m_critical_density * m_free_flow_speed;
  }

  /**
   * @brief Calculate sending flow from a cell.
   *
   * S(n) = min(n/Δt, Q_max)
   *
   * @param num_vehicles Number of vehicles in cell
   * @param dt Time step
   * @return Sending flow (vehicles/time step)
   */
  double sendingFlow(double num_vehicles, double dt) const {
    double flow_capacity = m_max_flow * dt;
    double available = num_vehicles;
    return std::min(available, flow_capacity);
  }

  /**
   * @brief Calculate receiving flow of a cell.
   *
   * R(n) = min(N_max - n, Q_max)
   *
   * @param num_vehicles Number of vehicles in cell
   * @param dt Time step
   * @return Receiving flow (vehicles/time step)
   */
  double receivingFlow(double num_vehicles, double dt) const {
    double max_vehicles = m_jam_density * m_cell_length;
    double flow_capacity = m_max_flow * dt;
    double space_available = max_vehicles - num_vehicles;
    return std::min(space_available, flow_capacity);
  }

  /**
   * @brief Update traffic state using CTM.
   *
   * @param dt Time step (seconds)
   */
  void update(double dt) {
    // Calculate flows between cells
    std::vector<double> flows(m_num_cells + 1, 0.0);

    for (int i = 0; i < m_num_cells; ++i) {
      int i_next = (i + 1) % m_num_cells; // Periodic BC

      // Flow from cell i to cell i+1
      double send = sendingFlow(m_num_vehicles[i], dt);
      double receive = receivingFlow(m_num_vehicles[i_next], dt);
      flows[i + 1] = std::min(send, receive);
    }

    // Update number of vehicles in each cell
    for (int i = 0; i < m_num_cells; ++i) {
      m_num_vehicles_new[i] = m_num_vehicles[i] + flows[i] - flows[i + 1];

      // Ensure physical bounds
      double max_vehicles = m_jam_density * m_cell_length;
      m_num_vehicles_new[i] =
          std::max(0.0, std::min(max_vehicles, m_num_vehicles_new[i]));
    }

    // Swap buffers
    m_num_vehicles.swap(m_num_vehicles_new);
  }

  /**
   * @brief Set number of vehicles in a cell.
   *
   * @param cell_index Cell index
   * @param num_vehicles Number of vehicles
   */
  void setNumVehicles(int cell_index, double num_vehicles) {
    if (cell_index >= 0 && cell_index < m_num_cells) {
      double max_vehicles = m_jam_density * m_cell_length;
      m_num_vehicles[cell_index] =
          std::max(0.0, std::min(max_vehicles, num_vehicles));
    }
  }

  /**
   * @brief Get number of vehicles in a cell.
   *
   * @param cell_index Cell index
   * @return Number of vehicles
   */
  double getNumVehicles(int cell_index) const {
    if (cell_index >= 0 && cell_index < m_num_cells) {
      return m_num_vehicles[cell_index];
    }
    return 0.0;
  }

  /**
   * @brief Get density in a cell.
   *
   * @param cell_index Cell index
   * @return Density (vehicles/m)
   */
  double getDensity(int cell_index) const {
    return getNumVehicles(cell_index) / m_cell_length;
  }

  /**
   * @brief Get speed in a cell.
   *
   * @param cell_index Cell index
   * @return Speed (m/s)
   */
  double getSpeed(int cell_index) const {
    double density = getDensity(cell_index);
    if (density < m_critical_density) {
      return m_free_flow_speed;
    } else {
      // Congested regime
      return m_wave_speed * (m_jam_density - density) /
             (m_jam_density - m_critical_density);
    }
  }

  /**
   * @brief Get flow in a cell.
   *
   * @param cell_index Cell index
   * @return Flow (vehicles/s)
   */
  double getFlow(int cell_index) const {
    return getDensity(cell_index) * getSpeed(cell_index);
  }

  // Getters
  int getNumCells() const { return m_num_cells; }
  double getRoadLength() const { return m_road_length; }
  double getCellLength() const { return m_cell_length; }
  double getFreeFlowSpeed() const { return m_free_flow_speed; }
  double getWaveSpeed() const { return m_wave_speed; }
  double getJamDensity() const { return m_jam_density; }
  double getCriticalDensity() const { return m_critical_density; }
  double getMaxFlow() const { return m_max_flow; }

  /**
   * @brief Get all vehicle counts.
   *
   * @return Vector of vehicle counts
   */
  const std::vector<double> &getNumVehiclesArray() const {
    return m_num_vehicles;
  }

private:
  double m_free_flow_speed; ///< Free-flow speed (m/s)
  double m_wave_speed;      ///< Backward wave speed (m/s)
  double m_jam_density;     ///< Jam density (vehicles/m)
  double m_road_length;     ///< Road length (m)
  int m_num_cells;          ///< Number of cells
  double m_cell_length;     ///< Cell length (m)

  double m_critical_density; ///< Critical density
  double m_max_flow;         ///< Maximum flow

  std::vector<double> m_num_vehicles;     ///< Current state
  std::vector<double> m_num_vehicles_new; ///< Next state

  /**
   * @brief Calculate critical density.
   *
   * @return Critical density (vehicles/m)
   */
  double calculateCriticalDensity() const {
    // From triangular fundamental diagram
    return m_jam_density * m_wave_speed / (m_free_flow_speed + m_wave_speed);
  }
};

} // namespace models
} // namespace macroscopic
} // namespace jamfree

#endif // JAMFREE_MACROSCOPIC_MODELS_CTM_H
